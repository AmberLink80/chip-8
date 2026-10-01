#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// probably unneeded but leaving it for now so its in my mind
typedef struct Display {
  bool display[64][32];
} Display;

typedef struct Keypad {
  // probablly some sort of pipe indicating "up <key>" and "down <key>"
} Keypad;

typedef struct ChipContext {
  // memory
  uint16_t *memory;

  // registers
  // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#:~:text=12%20bits%20are%20usually%20used

  uint8_t v0;
  uint8_t v1;
  uint8_t v2;
  uint8_t v3;
  uint8_t v4;
  uint8_t v5;
  uint8_t v6;
  uint8_t v7;
  uint8_t v8;
  uint8_t v9;
  uint8_t v10;
  uint8_t v11;
  uint8_t v12;
  uint8_t v13;
  uint8_t v14;
  uint8_t v15;

  // lowest (rightmost) 12 bits are usually used
  uint8_t v_addr; // vi
  uint8_t v_flag; // vf

  uint8_t v_sound; // vs
  uint8_t v_delay; // vd

  uint16_t program_counter; // pc
  uint8_t stack_pointer;    // sp

  uint16_t *stack;

  Display display;
  Keypad keypad;
} ChipContext;

typedef void (*Instruction)(ChipContext *chip_context,
                            uint16_t raw_instruction);

void print_display(Display display);

uint16_t fetch(ChipContext *chip_context);
Instruction decode(uint16_t raw_instruction);

// raw_instruction (uint16_t) will be held in the method call instruction
// (Instruction)

// op codes
void SYS_addr(ChipContext *chip_context, uint16_t raw_instruction);
void CLS(ChipContext *chip_context, uint16_t raw_instruction);
void RET(ChipContext *chip_context, uint16_t raw_instruction);
void JP_addr(ChipContext *chip_context, uint16_t raw_instruction);
void CALL_addr(ChipContext *chip_context, uint16_t raw_instruction);
void SE_Vx_byte(ChipContext *chip_context, uint16_t raw_instruction);
void SNE_Vx_byte(ChipContext *chip_context, uint16_t raw_instruction);
void SE_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction);
void LD_Vx_byte(ChipContext *chip_context, uint16_t raw_instruction);
void ADD_Vx_byte(ChipContext *chip_context, uint16_t raw_instruction);
void LD_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction);
void OR_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction);
void AND_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction);
void XOR_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction);
void ADD_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction);
void SUB_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction);
void SHR_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction);
void SUBN_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction);
void SUBN_Vx_VY(ChipContext *chip_context, uint16_t raw_instruction);
void SHL_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction);
void SNE_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction);
void LD_I_addr(ChipContext *chip_context, uint16_t raw_instruction);
void JP_V0_addr(ChipContext *chip_context, uint16_t raw_instruction);
void RND_Vx_byte(ChipContext *chip_context, uint16_t raw_instruction);
void DRW_Vx_nibble(ChipContext *chip_context, uint16_t raw_instruction);
void SKP_Vx(ChipContext *chip_context, uint16_t raw_instruction);
void SKNP_Vx(ChipContext *chip_context, uint16_t raw_instruction);
void LD_Vx_DT(ChipContext *chip_context, uint16_t raw_instruction);
void LD_Vx_K(ChipContext *chip_context, uint16_t raw_instruction);
void LD_DT_Vx(ChipContext *chip_context, uint16_t raw_instruction);
void LD_ST_Vx(ChipContext *chip_context, uint16_t raw_instruction);
void ADD_I_Vx(ChipContext *chip_context, uint16_t raw_instruction);
void LD_F_Vx(ChipContext *chip_context, uint16_t raw_instruction);
void LD_B_Vx(ChipContext *chip_context, uint16_t raw_instruction);
void LD_I_Vx(ChipContext *chip_context, uint16_t raw_instruction);
void LD_Vx_I(ChipContext *chip_context, uint16_t raw_instruction);
void ignore(ChipContext *chip_context, uint16_t raw_instruction);
