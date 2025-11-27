#ifndef BYTECODE_INTERPRETER_H
#define BYTECODE_INTERPRETER_H

extern struct key_event KB_RX;

void load_bytecode(String bin_file);

//void init_dispatch_table();

void run_bytecode(); 

#endif