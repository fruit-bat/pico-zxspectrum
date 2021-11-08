/*
 Z80 emulator code derived from Lin Ke-Fong source. Copyright says:

 Copyright (c) 2016, 2017 Lin Ke-Fong

 This code is free, do whatever you want with it.

 2020 adapted by Fabrizio Di Vittorio for fabgl ESP32 library
 2021 adapted for Sorcerer2 emulator
 */


#pragma once


/**
 * @file
 *
 * @brief This file contains fabgl::Z80 definition.
 */



#include <stdint.h>
#include <pico/printf.h>





/* Define this macro if the host processor is big endian. */

/* #define Z80_BIG_ENDIAN */

/* Emulation can be speed up a little bit by emulating only the documented
 * flags.
 */

/* #define Z80_DOCUMENTED_FLAGS_ONLY */

/* HALT, DI, EI, RETI, and RETN instructions can be catched. When such an
 * instruction is catched, the emulator is stopped and the PC register points
 * at the opcode to be executed next. The catched instruction can be determined
 * from the Z80_STATE's status value. Keep in mind that no interrupt can be
 * accepted at the instruction right after a DI or EI on an actual processor.
 */

#define Z80_CATCH_HALT
/*
 #define Z80_CATCH_DI
 #define Z80_CATCH_EI
 #define Z80_CATCH_RETI
 #define Z80_CATCH_RETN
 */

/* Undefined 0xed prefixed opcodes may be catched, otherwise they are treated
 * like NOP instructions. When one is catched, Z80_STATUS_ED_UNDEFINED is set
 * in Z80_STATE's status member and the PC register points at the 0xed prefix
 * before the undefined opcode.
 */

/* #define Z80_CATCH_ED_UNDEFINED */

/* By defining this macro, the emulator will always fetch the displacement or
 * address of a conditionnal jump or call instruction, even if the condition
 * is false and the fetch can be avoided. Define this macro if you need to
 * account for memory wait states on code read.
 */

/* #define Z80_FALSE_CONDITION_FETCH */

/* It may be possible to overwrite the opcode of the currently executing LDIR,
 * LDDR, INIR, or OTDR instruction. Define this macro if you need to handle
 * these pathological cases.
 */

/* #define Z80_HANDLE_SELF_MODIFYING_CODE */

/* For interrupt mode 2, bit 0 of the 16-bit address to the interrupt vector
 * can be masked to zero. Some documentation states that this bit is forced to
 * zero. For instance, Zilog's application note about interrupts, states that
 * "only 7 bits are required" and "the least significant bit is zero". Yet,
 * this is quite unclear, even from Zilog's manuals. So this is left as an
 * option.
 */

#define Z80_MASK_IM2_VECTOR_ADDRESS






/* If Z80_STATE's status is non-zero, the emulation has been stopped for some
 * reason other than emulating the requested number of cycles.
 */

enum {

  Z80_STATUS_FLAG_HALT = 1,
  Z80_STATUS_FLAG_DI = 1<<1,
  Z80_STATUS_FLAG_EI = 1<<2,
  Z80_STATUS_FLAG_RETI = 1<<3,
  Z80_STATUS_FLAG_RETN = 1<<4,
  Z80_STATUS_FLAG_ED_UNDEFINED = 1<<5,
  Z80_STATUS_FLAG_PREFIX = 1<<6

};



/* The main registers are stored inside Z80_STATE as an union of arrays named
 * registers. They are referenced using indexes. Words are stored in the
 * endianness of the host processor. The alternate set of word registers AF',
 * BC', DE', and HL' is stored in the alternates member of Z80_STATE, as an
 * array using the same ordering.
 */

#ifdef Z80_BIG_ENDIAN

#       define Z80_B            0
#       define Z80_C            1
#       define Z80_D            2
#       define Z80_E            3
#       define Z80_H            4
#       define Z80_L            5
#       define Z80_A            6
#       define Z80_F            7

#       define Z80_IXH          8
#       define Z80_IXL          9
#       define Z80_IYH          10
#       define Z80_IYL          11
#       define Z80_SPH          12
#       define Z80_SPL          13

#else

#       define Z80_B            1
#       define Z80_C            0
#       define Z80_D            3
#       define Z80_E            2
#       define Z80_H            5
#       define Z80_L            4
#       define Z80_A            7
#       define Z80_F            6

#       define Z80_IXH          9
#       define Z80_IXL          8
#       define Z80_IYH          11
#       define Z80_IYL          10
#       define Z80_SPH          13
#       define Z80_SPL          12

#endif

#define Z80_BC                  0
#define Z80_DE                  1
#define Z80_HL                  2
#define Z80_AF                  3

#define Z80_IX                  4
#define Z80_IY                  5
#define Z80_SP                  6



/* Z80's flags. */

#define Z80_S_FLAG_SHIFT        7
#define Z80_Z_FLAG_SHIFT        6
#define Z80_Y_FLAG_SHIFT        5
#define Z80_H_FLAG_SHIFT        4
#define Z80_X_FLAG_SHIFT        3
#define Z80_PV_FLAG_SHIFT       2
#define Z80_N_FLAG_SHIFT        1
#define Z80_C_FLAG_SHIFT        0

#define Z80_S_FLAG              (1 << Z80_S_FLAG_SHIFT)
#define Z80_Z_FLAG              (1 << Z80_Z_FLAG_SHIFT)
#define Z80_Y_FLAG              (1 << Z80_Y_FLAG_SHIFT)
#define Z80_H_FLAG              (1 << Z80_H_FLAG_SHIFT)
#define Z80_X_FLAG              (1 << Z80_X_FLAG_SHIFT)
#define Z80_PV_FLAG             (1 << Z80_PV_FLAG_SHIFT)
#define Z80_N_FLAG              (1 << Z80_N_FLAG_SHIFT)
#define Z80_C_FLAG              (1 << Z80_C_FLAG_SHIFT)

#define Z80_P_FLAG_SHIFT        Z80_PV_FLAG_SHIFT
#define Z80_V_FLAG_SHIFT        Z80_PV_FLAG_SHIFT
#define Z80_P_FLAG              Z80_PV_FLAG
#define Z80_V_FLAG              Z80_PV_FLAG



/* Z80's three interrupt modes. */

enum {
  Z80_INTERRUPT_MODE_0,
  Z80_INTERRUPT_MODE_1,
  Z80_INTERRUPT_MODE_2
};



struct Z80_STATE {
  int               status;

  union {
    unsigned char   byte[14];
    unsigned short  word[7];
  } registers;

  unsigned short    alternates[4];

  int               i, r, pc, iff1, iff2, im;

  /* Register decoding tables. */

  void              * register_table[16];
  void              * dd_register_table[16];
  void              * fd_register_table[16];
};


/**
 * @brief Zilog Z80 CPU emulator
 */
class Z80 {

public:

  // callbacks
  typedef int  (*ReadByteCallback)(void * context, int addr);
  typedef void (*WriteByteCallback)(void * context, int addr, int value);
  typedef int  (*ReadWordCallback)(void * context, int addr);
  typedef void (*WriteWordCallback)(void * context, int addr, int value);
  typedef int  (*ReadIOCallback)(void * context, int addr);
  typedef void (*WriteIOCallback)(void * context, int addr, int value);

  void setCallbacks(void * context, ReadByteCallback readByte, WriteByteCallback writeByte, ReadWordCallback readWord, WriteWordCallback writeWord, ReadIOCallback readIO, WriteIOCallback writeIO) {
    m_context   = context;
    m_readByte  = readByte;
    m_writeByte = writeByte;
    m_readWord  = readWord;
    m_writeWord = writeWord;
    m_readIO    = readIO;
    m_writeIO   = writeIO;
  }

  /* Initialize processor's state to power-on default. */
  void reset();

  /* Trigger an interrupt according to the current interrupt mode and return the
  * number of cycles elapsed to accept it. If maskable interrupts are disabled,
  * this will return zero. In interrupt mode 0, data_on_bus must be a single
  * byte opcode.
  */
  int __not_in_flash_func(IRQ)(int data_on_bus);

  /* Trigger a non maskable interrupt, then return the number of cycles elapsed
   * to accept it.
   */
  int NMI();

  int __not_in_flash_func(step)();


  // CPU registers access

  uint8_t readRegByte(int reg)                { return state.registers.byte[reg]; }
  void writeRegByte(int reg, uint8_t value)   { state.registers.byte[reg] = value; }

  uint16_t readRegWord(int reg)               { return state.registers.word[reg]; }
  void writeRegWord(int reg, uint16_t value)  { state.registers.word[reg] = value; }
  
  void setPC(uint16_t v)                      { state.pc = v; }
  void setA(uint8_t v)                        { state.registers.byte[Z80_A] = v; }
  void setF(uint8_t v)                        { state.registers.byte[Z80_F] = v; }
  void setB(uint8_t v)                        { state.registers.byte[Z80_B] = v; }
  void setC(uint8_t v)                        { state.registers.byte[Z80_C] = v; }
  void setD(uint8_t v)                        { state.registers.byte[Z80_D] = v; }
  void setE(uint8_t v)                        { state.registers.byte[Z80_E] = v; }
  void setH(uint8_t v)                        { state.registers.byte[Z80_H] = v; }
  void setL(uint8_t v)                        { state.registers.byte[Z80_L] = v; }
  void setIXH(uint8_t v)                      { state.registers.byte[Z80_IXH] = v; }
  void setIXL(uint8_t v)                      { state.registers.byte[Z80_IXL] = v; }
  void setIYH(uint8_t v)                      { state.registers.byte[Z80_IYH] = v; }
  void setIYL(uint8_t v)                      { state.registers.byte[Z80_IYL] = v; }
  void setIFF1(uint8_t v)                     { state.iff1 = v; }
  void setIFF2(uint8_t v)                     { state.iff2 = v; }
  void setIM(uint8_t v)                       { state.im = v; }
  void setI(uint8_t v)                        { state.i = v; }
  void setR(uint8_t v)                        { state.r = v; }
  void setSPH(uint8_t v)                      { state.registers.byte[Z80_SPH] = v; }
  void setSPL(uint8_t v)                      { state.registers.byte[Z80_SPL] = v; }
 
  uint16_t getPC()                      { return state.pc; }
  uint8_t getA()                        { return state.registers.byte[Z80_A]; }
  uint8_t getF()                        { return state.registers.byte[Z80_F]; }
  uint8_t getB()                        { return state.registers.byte[Z80_B]; }
  uint8_t getC()                        { return state.registers.byte[Z80_C]; }
  uint8_t getD()                        { return state.registers.byte[Z80_D]; }
  uint8_t getE()                        { return state.registers.byte[Z80_E]; }
  uint8_t getH()                        { return state.registers.byte[Z80_H]; }
  uint8_t getL()                        { return state.registers.byte[Z80_L]; }
  uint8_t getIXH()                      { return state.registers.byte[Z80_IXH]; }
  uint8_t getIXL()                      { return state.registers.byte[Z80_IXL]; }
  uint8_t getIYH()                      { return state.registers.byte[Z80_IYH]; }
  uint8_t getIYL()                      { return state.registers.byte[Z80_IYL]; }
  uint8_t getIFF1()                     { return state.iff1; }
  uint8_t getIFF2()                     { return state.iff2; }
  uint8_t getIM()                       { return state.im; }
  uint8_t getI()                        { return state.i; }
  uint8_t getR()                        { return state.r; }
  uint8_t getSPH()                      { return state.registers.byte[Z80_SPH]; }
  uint8_t getSPL()                      { return state.registers.byte[Z80_SPL]; } 
 

  void swap() {
      for(int i=0; i <4; ++i) {
        unsigned short t = state.alternates[i];
        state.alternates[i] = state.registers.word[i];
        state.registers.word[i] = t;
      }
  }
  
private:

  int __not_in_flash_func(intemulate)(int opcode, int elapsed_cycles);


  Z80_STATE         state;

  // callbacks

  void *            m_context;

  ReadByteCallback  m_readByte;
  WriteByteCallback m_writeByte;
  ReadWordCallback  m_readWord;
  WriteWordCallback m_writeWord;
  ReadIOCallback    m_readIO;
  WriteIOCallback   m_writeIO;

};









