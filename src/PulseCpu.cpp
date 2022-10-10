#include "PulseCpu.h"

#define P_OUT_L { PC_OUT_L, 0, 0, 0 }
#define P_OUT_H { PC_OUT_H, 0, 0, 0 }
#define P_OUT_T { PC_OUT_T, 0, 0, 0 }
#define P_DJNZ(R, D) { PC_DJNZ_R, D, R, 0 }
#define P_SET(R, V) { PC_SET_R, V, R, 0 }
#define P_SET_ISRBH(R, V) { PC_SET_ISRBH_R, V, R, 0 }
#define P_SET_ISRBL(R, V) { PC_SET_ISRBL_R, V, R, 0 }
#define P_JRNZ_R(R, D) { PC_JRNZ_R, D, R, 0 }
#define P_WAIT P_JRNZ_R(PR_T, 0)
#define P_MOV(R1, R2) { PC_MOV_R_R, 0, R1, R2 }
#define P_SRR(R, V) { PC_SRR_R, V, R, 0 }
#define P_SRL(R, V) { PC_SRL_R, V, R, 0 }
#define P_IN(R) { PC_IN_R, 0, R, 0 }
#define P_JR(D) { PC_JR, D, 0, 0 }
#define P_END { PC_END, 0, 0, 0 }

// TAP loader program
//  X = marker byte
//  L = length in bytes
PulseInstruction TAP_LOADER[] {
  // Standard pre-amble
  P_OUT_L,
  P_SET(PR_Y, 8063), // Header data pulse count
  P_JRNZ_R(PR_X, 2), // If the marker is zero, this is a program block
  P_SET(PR_Y, 3223), // Program data pulse count
  P_SET(PR_T, 2168), // Header pulse length in T states
  P_WAIT,
  P_OUT_T,
  P_DJNZ(PR_Y, -3),
  P_SET(PR_T, 667),
  P_WAIT,
  P_OUT_T,
  P_SET(PR_T, 735),
  P_WAIT,
  P_OUT_T,
  P_MOV(PR_ISR, PR_X),     // Move the marker to the input shift register 
  P_SET(PR_Y, 8),          // 8 bits in the marker
  P_SET_ISRBH(PR_X, 1710),
  P_SET_ISRBL(PR_X, 855),
  P_MOV(PR_T, PR_X),       // Pulse 1
  P_WAIT,
  P_OUT_T,
  P_MOV(PR_T, PR_X),       // Pulse 2
  P_WAIT,
  P_OUT_T,  
  P_SRL(PR_ISR, 1),
  P_DJNZ(PR_Y, -9),
  // Read and send the data bit by bit
  P_IN(PR_ISR),
  P_DJNZ(PR_L, -12),
  P_END
};

PulseCpu::PulseCpu()  : 
  _is(0),
  _instructions(0)
{
}

void PulseCpu::reset(
  InputStream* is,
  PulseInstruction* instructions
) {
  _is = is;
  _instructions = instructions;
  _state = PS_START;
  for (int i = 0; i < PR_COUNT; ++i) _registers[i] = 0;
}

void PulseCpu::execute() {
  _state = PS_START;
  while(true) {
    PulseInstruction *instruction = &_instructions[_registers[PR_IP]++];
    switch(instruction->command) {
      case PC_OUT_L: *_out = false; break;
      case PC_OUT_H: *_out = true; break;
      case PC_OUT_T: *_out = !*_out; break;
      case PC_DJNZ_R: if(--_registers[instruction->r1] != 0) _registers[PR_IP] += instruction->x - 1; break;
      case PC_SET_R: _registers[instruction->r1] = instruction->x; break;
      case PC_JRNZ_R: if(_registers[instruction->r1] != 0) _registers[PR_IP] += instruction->x - 1; break;
      case PC_WAIT: _state = PS_WAIT; return;
      case PC_MOV_R_R: _registers[instruction->r1] = _registers[instruction->r2]; break;
      case PC_SET_ISRBH_R: (_registers[PR_ISR] & 0x80) && (_registers[instruction->r1] = instruction->x); break;
      case PC_SET_ISRBL_R: (_registers[PR_ISR] & 0x80) || (_registers[instruction->r1] = instruction->x); break;
      case PC_SRR_R: _registers[instruction->r1] >>= instruction->x; break;
      case PC_SRL_R: _registers[instruction->r1] <<= instruction->x; break;
      case PC_IN_R: { 
        int b = _is->readByte();
        if (b < 0) { 
           if (b == -1) _state = PS_EOF;
           else _state = PS_ERR;
           return;
        }
        _registers[instruction->r1] = b; break; 
      }
      case PC_JR: _registers[PR_IP] += instruction->x - 1; break;
      case PC_END: _state = PS_END; _registers[PR_IP]--; return;
    }
  }
}

