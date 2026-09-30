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
  uint8_t memory[4096];

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

  uint16_t stack[16];

  Display display;
  Keypad keypad;
} ChipContext;

typedef void (*Instruction)(ChipContext *chip_context, FILE *ROM);
Instruction fetch_decode(ChipContext *chip_context, FILE *ROM);

int8_t return_code(ChipContext *chip_context, FILE *ROM);

// op codes
void SYS_addr(ChipContext *chip_context, FILE *ROM);
void CLS(ChipContext *chip_context, FILE *ROM);
void RET(ChipContext *chip_context, FILE *ROM);
void JP_addr(ChipContext *chip_context, FILE *ROM);
void CALL_addr(ChipContext *chip_context, FILE *ROM);
void SE_Vx_byte(ChipContext *chip_context, FILE *ROM);
void SNE_Vx_byte(ChipContext *chip_context, FILE *ROM);
void SE_Vx_Vy(ChipContext *chip_context, FILE *ROM);
void LD_Vx_byte(ChipContext *chip_context, FILE *ROM);
void ADD_Vx_byte(ChipContext *chip_context, FILE *ROM);
void LD_Vx_Vy(ChipContext *chip_context, FILE *ROM);
void OR_Vx_Vy(ChipContext *chip_context, FILE *ROM);
void AND_Vx_Vy(ChipContext *chip_context, FILE *ROM);
void XOR_Vx_Vy(ChipContext *chip_context, FILE *ROM);
void ADD_Vx_Vy(ChipContext *chip_context, FILE *ROM);
void SUB_Vx_Vy(ChipContext *chip_context, FILE *ROM);
void SHR_Vx_Vy(ChipContext *chip_context, FILE *ROM);
void SUBN_Vx_Vy(ChipContext *chip_context, FILE *ROM);
void SUBN_Vx_VY(ChipContext *chip_context, FILE *ROM);
void SHL_Vx_Vy(ChipContext *chip_context, FILE *ROM);
void SNE_Vx_Vy(ChipContext *chip_context, FILE *ROM);
void LD_I_addr(ChipContext *chip_context, FILE *ROM);
void JP_V0_addr(ChipContext *chip_context, FILE *ROM);
void RND_Vx_byte(ChipContext *chip_context, FILE *ROM);
void DRW_Vx_nibble(ChipContext *chip_context, FILE *ROM);
void SKP_Vx(ChipContext *chip_context, FILE *ROM);
void SKNP_Vx(ChipContext *chip_context, FILE *ROM);
void LD_Vx_DT(ChipContext *chip_context, FILE *ROM);
void LD_Vx_K(ChipContext *chip_context, FILE *ROM);
void LD_DT_Vx(ChipContext *chip_context, FILE *ROM);
void LD_ST_Vx(ChipContext *chip_context, FILE *ROM);
void ADD_I_Vx(ChipContext *chip_context, FILE *ROM);
void LD_F_Vx(ChipContext *chip_context, FILE *ROM);
void LD_B_Vx(ChipContext *chip_context, FILE *ROM);
void LD_I_Vx(ChipContext *chip_context, FILE *ROM);
void LD_Vx_I(ChipContext *chip_context, FILE *ROM);
