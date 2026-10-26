#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE (4096)
#define SCREEN_HEIGHT (32)
#define SCREEN_WIDTH (64)

typedef struct Keypad {
  // probablly some sort of pipe indicating "up <key>" and "down <key>"
} Keypad;

typedef struct ChipContext {
  // memory
  uint8_t memory[4096];

  // registers
  // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#:~:text=12%20bits%20are%20usually%20used

  // registers
  uint16_t v[16];

  // lowest (rightmost) 12 bits are usually used
  uint16_t v_addr; // vi
  uint16_t v_flag; // vf

  uint8_t v_draw;  // for drawing in main
  uint8_t v_sound; // vs
  uint8_t v_delay; // vd

  uint16_t program_counter; // program counter
  int8_t stack_pointer;    // stack pointer

  uint16_t *stack;

  uint8_t display[SCREEN_HEIGHT * SCREEN_WIDTH];
  uint8_t keypad[16]; // 1 is down 0 is up
} ChipContext;

typedef void (*Instruction)(ChipContext *chip_context);

uint16_t fetch(ChipContext *chip_context);
Instruction decode(uint16_t raw_instruction);

// raw_instruction (uint16_t) will be held in the method call instruction
// (Instruction)

// op codes
void SYS_addr(ChipContext *chip_context);
void CLS(ChipContext *chip_context);
void RET(ChipContext *chip_context);
void JP_addr(ChipContext *chip_context);
void CALL_addr(ChipContext *chip_context);
void SE_Vx_byte(ChipContext *chip_context);
void SNE_Vx_byte(ChipContext *chip_context);
void SE_Vx_Vy(ChipContext *chip_context);
void LD_Vx_byte(ChipContext *chip_context);
void ADD_Vx_byte(ChipContext *chip_context);
void LD_Vx_Vy(ChipContext *chip_context);
void OR_Vx_Vy(ChipContext *chip_context);
void AND_Vx_Vy(ChipContext *chip_context);
void XOR_Vx_Vy(ChipContext *chip_context);
void ADD_Vx_Vy(ChipContext *chip_context);
void SUB_Vx_Vy(ChipContext *chip_context);
void SHR_Vx_Vy(ChipContext *chip_context);
void SUBN_Vx_Vy(ChipContext *chip_context);
void SUBN_Vx_VY(ChipContext *chip_context);
void SHL_Vx_Vy(ChipContext *chip_context);
void SNE_Vx_Vy(ChipContext *chip_context);
void LD_I_addr(ChipContext *chip_context);
void JP_V0_addr(ChipContext *chip_context);
void RND_Vx_byte(ChipContext *chip_context);
void DRW_Vx_nibble(ChipContext *chip_context);
void SKP_Vx(ChipContext *chip_context);
void SKNP_Vx(ChipContext *chip_context);
void LD_Vx_DT(ChipContext *chip_context);
void LD_Vx_K(ChipContext *chip_context);
void LD_DT_Vx(ChipContext *chip_context);
void LD_ST_Vx(ChipContext *chip_context);
void ADD_I_Vx(ChipContext *chip_context);
void LD_F_Vx(ChipContext *chip_context);
void LD_B_Vx(ChipContext *chip_context);
void LD_I_Vx(ChipContext *chip_context);
void LD_Vx_I(ChipContext *chip_context);
void ignore(ChipContext *chip_context);
