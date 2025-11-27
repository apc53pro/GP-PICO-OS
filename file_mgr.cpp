#include "draw_funcs.h"
#include "globals.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"
#include "PS2_KB.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"
#include "func_exports.h"
#include "terminal.h"
#include "LEXER.h"

int file_mgr_cursorY = 0;

static String currentFolder;
static String entries[100];
static bool isFolder[100];
static uint16_t entryAmount = 0;
static int page = 0;
static int pageAmount = 0;
static int pageSize = 15;
static int entriesInPage = 0;
static int lastPage = 0;
bool returnS = false;

void file_mgr_open_controls()
{
  for(int i = 0; i < 20; i++)
  {

    if(KEY_EVENT[i].key == 20001 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(file_mgr_cursorY > 0)
      {
        file_mgr_cursorY--;
      }
      KEY_EVENT[i].processed = true;
    }
    
    if(KEY_EVENT[i].key == 20002 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(file_mgr_cursorY < entriesInPage-1)
      {
        file_mgr_cursorY++;
      }
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20003 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(page > 0 && lastPage )
      {
        page--;
        memset(vga_data_array, 0x4f, sizeof(vga_data_array));
      }

      if(lastPage != page)
      {
        lastPage = page;
        file_mgr_cursorY = 0;
      }
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20004 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(page < pageAmount-1)
      {
        page++;
        memset(vga_data_array, 0x4f, sizeof(vga_data_array));
      }

      if(lastPage != page)
      {
        lastPage = page;
        file_mgr_cursorY = 0;
      }
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 13 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(isFolder[file_mgr_cursorY + (page*pageSize)] == true)
      {
        currentFolder.concat(entries[file_mgr_cursorY]);
        currentFolder.concat("/");
        memset(vga_data_array, 0x4f, sizeof(vga_data_array));
        file_mgr_cursorY = 0;
        page = 0;
      }else if(isFolder[file_mgr_cursorY + (page*pageSize)] == false)
      {
        currentFolder.concat(entries[file_mgr_cursorY]);
        returnS = true;
      }
      
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20013 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(currentFolder != "/")
      {
        int counter = currentFolder.length() - 1;
        currentFolder.remove(currentFolder.length() - 1, 1);
        while(currentFolder.charAt(currentFolder.length() - 1) != '/')
        {
          currentFolder.remove(currentFolder.length() - 1, 1);
        }
        memset(vga_data_array, 0x4f, sizeof(vga_data_array));
      }
      
      file_mgr_cursorY = 0;
      KEY_EVENT[i].processed = true;
    }
    
    if(KEY_EVENT[i].key == 20011 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      numLock = !numLock;
      KEY_EVENT[i].processed = true;
    }
    
    if(KEY_EVENT[i].processed == false && KEY_EVENT[i].released == false)
    {
      KEY_EVENT[i].processed = true;
    }

    //Serial.print("\nCURSOR Y: ");
    //Serial.print(file_mgr_cursorY);
  }
  
}

static String fileName;
static int fileNamePtr = 0;
void file_mgr_save_controls()
{
  for(int i = 0; i < 20; i++)
  {

    if(KEY_EVENT[i].key == 20012 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      shift = true;
      KEY_EVENT[i].processed = true;
    }else if(KEY_EVENT[i].key == 20012 && KEY_EVENT[i].released == true && KEY_EVENT[i].processed == false)
    {
      shift = false;
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].processed == false && KEY_EVENT[i].printable == true && KEY_EVENT[i].released == false)
    {
      if(file_mgr_cursorY == entriesInPage)
      {
        if(fileNamePtr < 35)
        {
          fileName.setCharAt(fileNamePtr, KEY_EVENT[i].key);
          fileNamePtr++;
          memset(vga_data_array, 0x4f, sizeof(vga_data_array));
        }
      }

      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20001 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(file_mgr_cursorY > 0)
      {
        file_mgr_cursorY--;
      }
      KEY_EVENT[i].processed = true;
    }
    
    if(KEY_EVENT[i].key == 20002 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(file_mgr_cursorY < entriesInPage+1)
      {
        file_mgr_cursorY++;
      }
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20003 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(page > 0 && lastPage )
      {
        page--;
        memset(vga_data_array, 0x4f, sizeof(vga_data_array));
      }

      if(lastPage != page)
      {
        lastPage = page;
        file_mgr_cursorY = 0;
      }
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20004 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(page < pageAmount-1)
      {
        page++;
        memset(vga_data_array, 0x4f, sizeof(vga_data_array));
      }

      if(lastPage != page)
      {
        lastPage = page;
        file_mgr_cursorY = 0;
      }
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 13 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(isFolder[file_mgr_cursorY + (page*pageSize)] == true)
      {
        currentFolder.concat(entries[file_mgr_cursorY]);
        currentFolder.concat("/");
        memset(vga_data_array, 0x4f, sizeof(vga_data_array));
        file_mgr_cursorY = 0;
        page = 0;
      }else if(isFolder[file_mgr_cursorY + (page*pageSize)] == false && file_mgr_cursorY < entriesInPage)
      {
        memset(vga_data_array, 0x4f, sizeof(vga_data_array));
        for(int i = 0; i < fileName.length(); i++)
        {
          fileName.setCharAt(i, ' ');
        }

        for(int i = 0; i < entries[file_mgr_cursorY].length(); i++)
        {
          fileName.setCharAt(i, entries[file_mgr_cursorY].charAt(i));
        }
        fileNamePtr = entries[file_mgr_cursorY].length();
      }else if(file_mgr_cursorY == entriesInPage+1)
      {
        fileName.trim();
        currentFolder.concat(fileName);
        returnS = true;
      }
      
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20000 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      caps = !caps;
      KEY_EVENT[i].processed = true;
    }

    if(KEY_EVENT[i].key == 20013 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      if(currentFolder != "/" && file_mgr_cursorY < entriesInPage)
      {
        int counter = currentFolder.length() - 1;
        currentFolder.remove(currentFolder.length() - 1, 1);
        while(currentFolder.charAt(currentFolder.length() - 1) != '/')
        {
          currentFolder.remove(currentFolder.length() - 1, 1);
        }
        memset(vga_data_array, 0x4f, sizeof(vga_data_array));
        file_mgr_cursorY = 0;
      }else if(file_mgr_cursorY >= entriesInPage && fileNamePtr > 0)
      {
        fileNamePtr--;
        fileName.setCharAt(fileNamePtr, ' '); 
        memset(vga_data_array, 0x4f, sizeof(vga_data_array));
      }
      
      KEY_EVENT[i].processed = true;
    }
    
    if(KEY_EVENT[i].key == 20011 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
    {
      numLock = !numLock;
      KEY_EVENT[i].processed = true;
    }
    
    if(KEY_EVENT[i].processed == false && KEY_EVENT[i].released == false)
    {
      KEY_EVENT[i].processed = true;
    }

    //Serial.print("\nCURSOR Y: ");
    //Serial.print(file_mgr_cursorY);
  }
  
}

int minZero2(int n) //had to give a dumb name because of dumb compiler
{
  if(n == 0)
  {
    return 1;
  }else
  {
    return n;
  }
}

int entryDrawVOffset = 48;

void countEntries()
{
  entryAmount = 0;
  File dir = SD.open(currentFolder);
  
  while(true)
  {
    File entry = dir.openNextFile();

    if(entry == false)
    {
      entry.close();
      break;
    }

    entryAmount++;
  }
  dir.close();

  pageAmount = 1 + (entryAmount/pageSize);
}

String file_mgr_open()
{
  file_mgr_cursorY = 0;
  currentFolder = "/";
  entries[100];
  isFolder[100];
  entryAmount = 0;
  page = 0;
  pageAmount = 1;
  pageSize = 15;
  entriesInPage = 0;
  lastPage = 0;
  returnS = false;
  memset(vga_data_array, 0x4f, sizeof(vga_data_array));

  while(true)
  {
    countEntries();
    drawString_free(16, 0, 12, "DIRECTORY:" + currentFolder, 255, 2, false);
    drawString_free(16, 16, 12, "                                       ", 255, 2, false);
    drawString_free(16, 16, 12, "PAGE " + String(page+1) + "/" + String(minZero2(pageAmount)), 255, 2, false);
    
    File dir = SD.open(currentFolder);
    int entryCounter = 0;

    bool offsetSet = false;
    entriesInPage = 0;
    while(true)
    {
      File entry;

      if(offsetSet == false)
      {
        while(entryCounter <= (page*pageSize))
        {
          entry = dir.openNextFile();
          entryCounter++;
        }
        entryCounter = 0;
        offsetSet = true;
      }else
      {
        entry = dir.openNextFile();
      }

      if(entry == false || entryCounter >= pageSize)
      {
        break;
      }

      entries[entryCounter] = entry.name();
      
      if(entry.isDirectory() == false)
      {
        isFolder[entryCounter + (page*pageSize)] = false;

        if(file_mgr_cursorY == entryCounter)
        {
          drawString_free(16, (entryCounter*16) + entryDrawVOffset, 12, String(entry.name()), 0b00011100, 2, false);
        }else
        {
          drawString_free(16, (entryCounter*16) + entryDrawVOffset, 12, String(entry.name()), 255, 2, false);
        }
      }else
      {
        isFolder[entryCounter + (page*pageSize)] = true;

        if(file_mgr_cursorY == entryCounter)
        {
          drawString_free(16, (entryCounter*16) + entryDrawVOffset, 16, String(entry.name()), 0b00011100, 2, false);
        }else
        {
          drawString_free(16, (entryCounter*16) + entryDrawVOffset, 16, String(entry.name()), 0b11100000, 2, false);
        }
      }
    
      
      entryCounter++;
      entriesInPage++;
      
      entry.close();
    }
    
    dir.close();

    for(int i = 0; i < 20; i++)
    {
      if(KEY_EVENT[i].key == 20014 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
      {
        KEY_EVENT[i].processed = true;
        return "";
      }
    }

    file_mgr_open_controls();

    if(returnS == true)
    {
      returnS = false;
      return currentFolder;
    }
  }
  
}

String file_mgr_save()
{
  file_mgr_cursorY = 0;
  currentFolder = "/";
  entries[100];
  isFolder[100];
  entryAmount = 0;
  page = 0;
  pageAmount = 0;
  pageSize = 15;
  entriesInPage = 0;
  lastPage = 0;
  returnS = false;
  memset(vga_data_array, 0x4f, sizeof(vga_data_array));
  fileName = "                                   ";
  fileNamePtr = 0;

  while(true)
  {
    
    drawString_free(16, 0, 12, "DIRECTORY:" + currentFolder, 255, 2, false);
    drawString_free(16, 16, 12, "PAGE " + String(page+1) + "/" + String(minZero2(pageAmount)), 255, 2, false);
    if(file_mgr_cursorY == entriesInPage)
    {
      drawString_free(16, 448, 12, "FILE NAME: " + fileName, 0b00011100, 2, false);
    }else
    {
      drawString_free(16, 448, 12, "FILE NAME: " + fileName, 255, 2, false);
    }

    if(file_mgr_cursorY == entriesInPage+1)
    {
      drawString_free(16, 448+16, 12, "SAVE", 0b00011100, 2, false);
    }else
    {
      drawString_free(16, 448+16, 12, "SAVE", 255, 2, false);
    }
    
    
    File dir = SD.open(currentFolder);
    int entryCounter = 0;
    countEntries();

    bool offsetSet = false;
    entriesInPage = 0;
    while(true)
    {
      File entry;

      if(offsetSet == false)
      {
        while(entryCounter <= (page*pageSize))
        {
          entry = dir.openNextFile();
          entryCounter++;
        }
        entryCounter = 0;
        offsetSet = true;
      }else
      {
        entry = dir.openNextFile();
      }

      if(entry == false || entryCounter >= pageSize)
      {
        break;
      }

      entries[entryCounter] = entry.name();
      
      if(entry.isDirectory() == false)
      {
        isFolder[entryCounter + (page*pageSize)] = false;

        if(file_mgr_cursorY == entryCounter)
        {
          drawString_free(16, (entryCounter*16) + entryDrawVOffset, 12, String(entry.name()), 0b00011100, 2, false);
        }else
        {
          drawString_free(16, (entryCounter*16) + entryDrawVOffset, 12, String(entry.name()), 255, 2, false);
        }
      }else
      {
        isFolder[entryCounter + (page*pageSize)] = true;

        if(file_mgr_cursorY == entryCounter)
        {
          drawString_free(16, (entryCounter*16) + entryDrawVOffset, 16, String(entry.name()), 0b00011100, 2, false);
        }else
        {
          drawString_free(16, (entryCounter*16) + entryDrawVOffset, 16, String(entry.name()), 0b11100000, 2, false);
        }
      }
    
      
      entryCounter++;
      entriesInPage++;
      
      entry.close();
    }

    dir.close();

    for(int i = 0; i < 20; i++)
    {
      if(KEY_EVENT[i].key == 20014 && KEY_EVENT[i].released == false && KEY_EVENT[i].processed == false)
      {
        KEY_EVENT[i].processed = true;
        return "";
      }
    }
    
    file_mgr_save_controls();

    if(returnS == true)
    {

      returnS = false;
      return currentFolder;
    }
  }
  
}



