#pragma once

#include <pico/stdlib.h>
#include "InputStream.h"

#define MAX_PULSE_INSTRUCTIONS 20

enum PulseCommand {
  PC_SET_R,        // Set the register R
  PC_SET_ISRBH_R,  // Set the register R if the input shift register bit is set
  PC_SET_ISRBL_R,  // Set the register R if the input shift register bit is not set
  PC_PUSH_R,   // Push register R onto the stack
  PC_POP_R,    // Pop register R from the stack
  PC_WAIT,     // Wait until the T-state counter is exhausted
  PC_DJNZ_R,   // Decrement the register R and jump if not zero
  PC_JRNZ_R,   // Jump if the register is non zero
  PC_IN_R,     // Read a byte into register R
  PC_OUT_H,    // Set the output high
  PC_OUT_L,    // Set the output low
  PC_OUT_T,    // Toggle the output
  PC_SRR_R,    // Shift the register to the right
  PC_SRL_R,    // Shift the register to the right
  PC_MOV_R_R,  // Copy value from one register to another
  PC_JR,       // Jump relative
  PC_END       // Finished
};

enum PulseRegister {
  PR_IP = 0,   // Instruction pointer
  PR_SP,       // Stack pointer
  PR_ISR,      // Input shift register
  PR_X,        // X register
  PR_Y,        // Y register
  PR_T,        // T state counter
  PR_L,        // L register
  PR_COUNT
};

enum PulseCpuState {
  PS_START,
  PS_WAIT,
  PS_END,
  PS_ERR,
  PS_EOF
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
  PulseCpuState _state;
  InputStream* _is;

  PulseInstruction* _instructions;
  
public:

  PulseCpu();
  
  void reset(
    InputStream* is,
    PulseInstruction* instructions
  );

  void __not_in_flash_func(execute)();
  
  void __not_in_flash_func(advance)(int *tstates, bool *pstate) {

  }

  inline bool end() {
    return true;
  }

  
};

