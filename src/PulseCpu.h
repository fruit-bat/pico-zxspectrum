#pragma once

#include <pico/stdlib.h>

#define MAX_PULSE_INSTRUCTIONS 20

enum PulseCommand {
  PC_SET_R,    // Set the register R
  PC_PUSH_R,   // Push register R onto the stack
  PC_POP_R,    // Pop register R from the stack
  PC_WAIT,     // Wait until the T-state counter is exhausted
  PC_DJNZ_R,   // Decrement the register R and jump if not zero
  PC_JRNZ_R,   // Jump if the register is non zero
  PC_IN,       // Load the input shift register
  PC_JR_ISRBH, // Jump if the current bit is set
  PC_JR_ISRHL, // Jump if the current bit is not set
  PC_OUT_H,    // Set the output high
  PC_OUT_L,    // Set the output low
  PC_OUT_T,    // Toggle the output
  PC_SRR_R,    // Shift the register to the right
  PC_SRL_R,    // Shift the register to the right
  PC_MOV_R_R , // Copy value from one register to another
};

enum PulseRegister {
  PR_IP = 0,   // Instruction pointer
  PR_SP,       // Stack pointer
  PR_ISR,      // Input shift register
  PR_X,        // X register
  PR_Y,        // Y register
  PR_T,        // T state counter
  PR_COUNT
};

typedef struct  {
  PulseCommand command;
  int32_t x;
  uint8_t r1;
  uint8_t r2;
} PulseInstruction;

class PulseCpu {
private:  
  int32_t _stack[20];
  int32_t _registers[PR_COUNT];
  bool *_out;
  bool _wait;
  
  PulseInstruction _instructions[MAX_PULSE_INSTRUCTIONS];
  
public:

  void __not_in_flash_func(execute)();
  
  void __not_in_flash_func(advance)(int *tstates, bool *pstate) {

  }

  inline bool end() {
    return true;
  }

  
};

