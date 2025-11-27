#include <SPI.h>
#include <SD.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "pico/platform.h"
#include "globals.h"
#include "draw_funcs.h"
#include "hsync.pio.h"
#include "vsync.pio.h"
#include "rgb.pio.h"
#include "mms.h"
#include "PS2_KB.h"
#include "terminal.h"
#include "text_editor.h"
#include "LEXER.h"
#include "file_mgr.h"
#include "font.h"

const int _MISO = 4;  // AKA SPI RX
const int _MOSI = 3;  // AKA SPI TX
const int _CS = 5;
const int _SCK = 2;

bool terminal_on = true;
bool sdInitialized = false;

void setup() 
{
  graphics_mode = false;
  SPI.begin();
  Serial.begin(115200);
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  memset(memory, 0b00011100000111000001110000011100, sizeof(memory));
  
  //SETUP PS/2 COMMUNICATION
  pinMode(20, OUTPUT);
  pinMode(27, INPUT_PULLUP); //PS2 CLOCK
  pinMode(28, INPUT_PULLUP); //PS2 DATA

  attachInterrupt(digitalPinToInterrupt(27), kb_ISR, FALLING);

  digitalWrite(20, HIGH);

  
  //SETUP SD CARD
  SPI.setRX(_MISO);
  SPI.setTX(_MOSI);
  SPI.setSCK(_SCK);
  sdInitialized = SD.begin(_CS);

  if(sdInitialized == false) 
  {
    Serial.print("\nSD card initialization failed.");
  } else 
  {
    Serial.println("Wiring is correct and a card is present.");
  }

  //memset(vga_data_array, 255, TXCOUNT);
  memset(gridBuffer_backColor, 255, sizeof(gridBuffer_backColor));
  memset(gridBuffer_char, ' ', sizeof(gridBuffer_char));
  memset(gridBuffer_charColor, 0b00011100, sizeof(gridBuffer_charColor));

  memset(gridBuffer_char_text_editor, ' ', sizeof(gridBuffer_char_text_editor));

  //memset(half_res_buffer, 0b11111111, sizeof(half_res_buffer));

  PIO pio = pio0;
  uint hsync_offset = pio_add_program(pio, &hsync_program);
  uint vsync_offset = pio_add_program(pio, &vsync_program);
  uint rgb_offset = pio_add_program(pio, &rgb_program);

  // Manually select a few state machines from pio instance pio0.
  uint hsync_sm = 0;
  uint vsync_sm = 1;
  uint rgb_sm = 2;

  // Call the initialization functions that are defined within each PIO file.
  // Why not create these programs here? By putting the initialization function in
  // the pio file, then all information about how to use/setup that state machine
  // is consolidated in one place. Here in the C, we then just import and use it.
  hsync_program_init(pio, hsync_sm, hsync_offset, HSYNC);
  vsync_program_init(pio, vsync_sm, vsync_offset, VSYNC);
  rgb_program_init(pio, rgb_sm, rgb_offset, 8);


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // ============================== PIO DMA Channels =================================================
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  // DMA channels - 0 sends color data, 1 reconfigures and restarts 0
  int rgb_chan_0 = dma_claim_unused_channel(true);
  int rgb_chan_1 = dma_claim_unused_channel(true);

  // Channel Zero (sends color data to PIO VGA machine)
  dma_channel_config c0 = dma_channel_get_default_config(rgb_chan_0);  // default configs
  channel_config_set_transfer_data_size(&c0, DMA_SIZE_8);              // 8-bit txfers
  channel_config_set_read_increment(&c0, true);                        // yes read incrementing
  channel_config_set_write_increment(&c0, false);                      // no write incrementing
  channel_config_set_dreq(&c0, DREQ_PIO0_TX2) ;                        // DREQ_PIO0_TX2 pacing (FIFO)
  channel_config_set_chain_to(&c0, rgb_chan_1);                        // chain to other channel

  dma_channel_configure(
      rgb_chan_0,                 // Channel to be configured
      &c0,                        // The configuration we just created
      &pio->txf[rgb_sm],          // write address (RGB PIO TX FIFO)
      &vga_data_array,            // The initial read address (pixel color array)
      TXCOUNT,                    // Number of transfers; in this case each is 1 byte.
      false                       // Don't start immediately.
  );

  // Channel One (reconfigures the first channel)
  dma_channel_config c1 = dma_channel_get_default_config(rgb_chan_1);   // default configs
  channel_config_set_transfer_data_size(&c1, DMA_SIZE_32);              // 32-bit txfers
  channel_config_set_read_increment(&c1, false);                        // no read incrementing
  channel_config_set_write_increment(&c1, false);                       // no write incrementing
  channel_config_set_chain_to(&c1, rgb_chan_0);                         // chain to other channel

  dma_channel_configure(
      rgb_chan_1,                         // Channel to be configured
      &c1,                                // The configuration we just created
      &dma_hw->ch[rgb_chan_0].read_addr,  // Write address (channel 0 read address)
      &address_pointer,                   // Read address (POINTER TO AN ADDRESS)
      1,                                  // Number of transfers, in this case each is 4 byte
      false                               // Don't start immediately.
  );

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  // Initialize PIO state machine counters. This passes the information to the state machines
  // that they retrieve in the first 'pull' instructions, before the .wrap_target directive
  // in the assembly. Each uses these values to initialize some counting registers.
  pio_sm_put_blocking(pio, hsync_sm, H_ACTIVE);
  pio_sm_put_blocking(pio, vsync_sm, V_ACTIVE);
  pio_sm_put_blocking(pio, rgb_sm, RGB_ACTIVE);


  // Start the two pio machine IN SYNC
  // Note that the RGB state machine is running at full speed,
  // so synchronization doesn't matter for that one. But, we'll
  // start them all simultaneously anyway.
  pio_enable_sm_mask_in_sync(pio, ((1u << hsync_sm) | (1u << vsync_sm) | (1u << rgb_sm)));

  // Start DMA channel 0. Once started, the contents of the pixel color array
  // will be continously DMA's to the PIO machines that are driving the screen.
  // To change the contents of the screen, we need only change the contents
  // of that array.
  dma_start_channel_mask((1u << rgb_chan_0));

  //WRSR(1); //FOR THE LOVE OF GOD REMEMBER TO WRITE 1 INSTEAD OF 0 OR ELSE THE RAM STATUS REGISTER WILL ACTIVATE THE RAM HOLD PIN FEATURE AND FUCK EVERYTHING MAKING FOR UNPREDICTABLE BEHAVIOUR
  //CLEAR_RAM();
  fillMemoryWithFont();
  //STARTUP TERMINAL MESSAGE
  echo_message("====== UNNAMED RP2350 COMPUTER");
  echo_message("====== VARIABLE MEMORY: " + String(memory_size*4) + " B");
  echo_message("====== BYTECODE PROG MEMORY: " + String(program_mem_size) + " B");
  echo_message("");
  
}

static void esc()
{
  for(int i = 0; i < 20; i++)
  {
    if(KEY_EVENT[i].key == 20014 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      graphics_mode = false;
      terminal_on = true;
      UI = 0;
      KEY_EVENT[i].processed = true;
    }
  }
}

bool run = false;
String ARG_STRUCTURE;
String returnType;
String returnContentField;

int height = 100;
int width = 70;

uint8_t UI = 0;
int rpt = 0;
uint8_t lastUI = 0;
void loop() 
{
  esc();
  if(graphics_mode == false && UI == 0)
  {
    terminal(true, true, true);

  }else if(graphics_mode == false && UI == 1)
  {
    text_editor2(true, true, true);
    
  }

  if(graphics_mode == true)
  {
    draw_half_res_buffer();
  }
  
}
