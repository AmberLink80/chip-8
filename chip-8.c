/**
 * @file chip-8.c
 * @brief fetch, decode, execute, and cycle the Chip-8 interpreter
 *
 * fetch:
 * ROM is read from Chip-8 Memory indexed by the Program Counter.
 *
 * decode:
 * Masks are applied to break up execution easier.
 * A switch statement then differentiates the type of command followed
 * by if/else blocks to differentiate the specific instruction.
 *
 * will likely change to a pure switch statement setup.
 *
 * execute:
 * Call a function pointer associated with the decoded instruction.
 *
 * cycle:
 * Tick the CPU Program Counter forward.
 *
 * @author Amber Link
 * @version 0
 */

#include "chip-8.h"
#include <SDL2/SDL_version.h>
#include <stdint.h>
#include <string.h>

const uint8_t SCREEN_WIDTH = 64;
const uint8_t SCREEN_HEIGHT = 32;

// masks for decoding
#define mask_0x000F(a) (a & 0x000F)
#define mask_0x00F0(a) (a & 0x00F0)
#define mask_0x00FF(a) (a & 0x00FF)
#define mask_0x0F00(a) (a & 0x0F00)
#define mask_0x0FFF(a) (a & 0x0FFF)
#define mask_0xF000(a) (a & 0xF000)
#define mask_0xF00F(a) (a & 0xF00F)

#define PC(chip8) (chip8->PC)
#define n0(chip8) (chip8->memory[chip8->PC])
#define n1(chip8) (chip8->memory[chip8->PC + 1])

// instructions

// definition of instructions VVV
// https://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf

/**
 * TODO:
 * 0nnn - SYS addr
 * Jump to a machine code routine at nnn. This instruction is only used on the
 * old computers on which Chip-8 was originally implemented. It is ignored by
 * modern interpreters. This will not be implemented.
 */
void SYS_addr(ChipContext *chip8) { printf("SYS_addr\n"); }

/**
 * 00E0 - CLC
 * Clear the display.
 */
void CLS(ChipContext *chip8) {
  memset(&(chip8->display), 0, sizeof(chip8->display));
}

/**
 * TODO:
 * 00E0 - CLC
 * Return from a subroutine.The interpreter sets the program counter to the
 * address at the top of the stack, then subtracts 1 from the stack pointer.
 */
void RET(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 1nnn - JP addr
 * Jump to location nnn. The interpreter sets the program counter to nnn.
 */
void JP_addr(ChipContext *chip8) {}

/**
 * TODO:
 * 2nnn - CALL addr
 * Call subroutine at nnn. The interpreter increments the stack pointer, then
 * puts the current PC on the top of the stack. The PC is then set to nnn.
 */

void CALL_addr(ChipContext *chip8) { printf("CALL_addr\n"); }

/**
 * TODO:
 * 3xkk - SE Vx, byte
 * Skip next instruction if Vx = kk. The interpreter compares register Vx to
 * kk, and if they are equal, increments the program counter by 2.
 */
void SE_Vx_byte(ChipContext *chip8) { printf("SE_Vx_byte\n"); }

/**
 * TODO:
 * 4xkk - SNE Vx, byte
 * Skip next instruction if Vx != kk. The interpreter compares register Vx to
 * kk, and if they are not equal, increments the program counter by 2.
 */
void SNE_Vx_byte(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 5xy0 - SE Vx, Vy
 * Skip next instruction if Vx = Vy. The interpreter compares register Vx to
 * register Vy, and if they are equal, increments the program counter by 2.
 */
void SE_Vx_Vy(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 6xkk - LD Vx, byte
 * Set Vx = kk. The interpreter puts the value kk into register Vx.
 */
void LD_Vx_byte(ChipContext *chip8) {
  uint16_t byte = chip8->memory[chip8->PC + 1];
  chip8->v[chip8->memory[chip8->PC] & 0x0F] = byte;
}

/**
 * TODO:
 * 7xkk - ADD Vx, byte
 * Set Vx = Vx + kk. Adds the value kk to the value of register Vx, then
 * stores the result in Vx.
 */
void ADD_Vx_byte(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 8xy0 - LD Vx, Vy
 * Set Vx = Vy. Stores the value of register Vy in register Vx.
 */
void LD_Vx_Vy(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 8xy1 - OR Vx, Vy
 * Set Vx = Vx OR Vy. Performs a bitwise OR on the values of Vx and Vy, then
 * stores the result in Vx. A bitwise OR compares the corresponding bits from
 * two values, and if either bit is 1, then the same bit in the result is
 * also 1. Otherwise, it is 0.
 */
void OR_Vx_Vy(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 8xy2 - AND Vx, Vy
 * Set Vx = Vx AND Vy. Performs a bitwise AND on the values of Vx and Vy, then
 * stores the result in Vx. A bitwise AND compares the corresponding bits from
 * two values, and if both bits are 1, then the same bit in the result is
 * also 1. Otherwise, it is 0.
 */
void AND_Vx_Vy(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 8xy3 - XOR Vx, Vy
 * Set Vx = Vx XOR Vy. Performs a bitwise exclusive OR on the values of Vx and
 * Vy, then stores the result in Vx. An exclusive OR compares the
 * corresponding bits from two values, and if the bits are not both the same,
 * then the corresponding bit in the result is set to 1. Otherwise, it is 0.
 */
void XOR_Vx_Vy(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 8xy4 - ADD Vx, Vy
 * Set Vx = Vx + Vy, set VF = carry. The values of Vx and Vy are added
 * together. If the result is greater than 8 bits (i.e., ¿ 255,) VF is set to
 * 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored
 * in Vx.
 */
void ADD_Vx_Vy(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 8xy5 - SUB Vx, Vy
 * Set Vx = Vx - Vy, set VF = NOT borrow. If Vx ¿ Vy, then VF is set to 1,
 * otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
 */
void SUB_Vx_Vy(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 8xy6 - SHR Vx {, Vy}
 * Set Vx = Vx SHR 1. If the least-significant bit of Vx is 1, then VF is set
 * to 1, otherwise 0. Then Vx is divided by 2.
 */
void SHR_Vx_Vy(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 8xy7 - SUBN Vx, Vy
 * Set Vx = Vy - Vx, set VF = NOT borrow. If Vy ¿ Vx, then VF is set to 1,
 * otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
 */
void SUBN_Vx_Vy(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 8xyE - SHL Vx {, Vy}
 * Set Vx = Vx SHL 1. If the most-significant bit of Vx is 1, then VF is set
 * to 1, otherwise to 0. Then Vx is multiplied by 2.
 */
void SHL_Vx_Vy(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * 9xy0 - SNE Vx, Vy
 * Skip next instruction if Vx != Vy. The values of Vx and Vy are compared,
 * and if they are not equal, the program counter is increased by 2.
 */
void SNE_Vx_Vy(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * Annn - LD I, addr
 * Set I = nnn. The value of register I is set to nnn.
 */
void LD_I_addr(ChipContext *chip8) {
  uint16_t processed_instruction =
      ((chip8->memory[chip8->PC] + 1) << 8) + chip8->memory[chip8->PC];
  chip8->v_addr = mask_0x0FFF(processed_instruction);
}

void JP_V0_addr(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * Cxkk - RND Vx, byte
 * Set Vx = random byte AND kk. The interpreter generates a random number from
 * 0 to 255, which is then ANDed with the value kk. The results are stored in
 * Vx. See instruction 8xy2 for more information on AND.
 */
void RND_Vx_byte(ChipContext *chip8) { printf("void\n"); }

/**
 * Dxyn - DRW Vx, Vy, nibble
 * Display n-byte sprite starting at memory location I at (Vx, Vy), set VF =
 * collision. The interpreter reads n bytes from memory, starting at the
 * address stored in I. These bytes are then displayed as sprites on screen at
 * coordinates (Vx, Vy). Sprites are XOR’d onto the existing screen. If this
 * causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
 * If the sprite is positioned so part of it is outside the coordinates of the
 * display, it wraps around to the opposite side of the screen.
 */
void DRW_Vx_nibble(ChipContext *chip8) {
  uint8_t x = n0(chip8) & 0x0F;
  uint8_t y = n1(chip8) >> 4;

  size_t size = n0(chip8) & 0x0F;

  for (int i = chip8->v_addr; i < size; i++) {
    chip8->display[x][y] ^= chip8->memory[i];
  }
}

/**
 * TODO:
 * Ex9E - SKP Vx
 * Skip next instruction if key with the value of Vx is pressed. Checks the
 * keyboard, and if the key corresponding to the value of Vx is currently in
 * the down position, PC is increased by 2.
 */
void SKP_Vx(ChipContext *chip8) { printf("SKP_Vx\n"); }

/**
 * TODO:
 * ExA1 - SKNP Vx
 * Skip next instruction if key with the value of Vx is not pressed. Checks
 * the keyboard, and if the key corresponding to the value of Vx is currently
 * in the up position, PC is increased by 2.
 */
void SKNP_Vx(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * Fx07 - LD Vx, DT
 * Set Vx = delay timer value. The value of DT is placed into Vx.
 */
void LD_Vx_DT(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * Fx0A - LD Vx, K
 * Wait for a key press, store the value of the key in Vx. All execution stops
 * until a key is pressed, then the value of that key is stored in Vx.
 */
void LD_Vx_K(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * Fx15 - LD DT, Vx
 * Set delay timer = Vx. Delay Timer is set equal to the value of Vx.
 */
void LD_DT_Vx(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * Fx18 - LD ST, Vx
 * Set sound timer = Vx. Sound Timer is set equal to the value of Vx.
 */
void LD_ST_Vx(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * Fx1E - ADD I, Vx
 * Set I = I + Vx. The values of I and Vx are added, and the results are
 * stored in I.
 */
void ADD_I_Vx(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * Fx29 - LD F, Vx
 * Set I = location of sprite for digit Vx. The value of I is set to the
 * location for the hexadecimal sprite corresponding to the value of Vx. See
 * section 2.4, Display, for more information on the Chip-8 hexadecimal font.
 * To obtain this value, multiply VX by 5 (all font data stored in first 80
 * bytes of memory).
 */
void LD_F_Vx(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * Fx33 - LD B, Vx
 * Store BCD representation of Vx in memory locations I, I+1, and I+2. The
 * interpreter takes the decimal value of Vx, and places the hundreds digit in
 * memory at location in I, the tens digit at location I+1, and the ones digit
 * at location I+2.
 */
void LD_B_Vx(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * Fx55 - LD [I], Vx
 * Stores V0 to VX in memory starting at address I. I is then set to I + x
 * + 1.
 */
void LD_I_Vx(ChipContext *chip8) { printf("void\n"); }

/**
 * TODO:
 * Fx65 - LD Vx, [I]
 * Fills V0 to VX with values from memory starting at address I. I is then set
 * to I + x + 1.
 */
void LD_Vx_I(ChipContext *chip8) { printf("LD_Vx_I\n"); }

/**
 * bad instruction
 */
void ignore(ChipContext *chip8) { printf("ignore\n"); }

/**
 * @brief Print current state of display to terminal
 *
 * @param display Current state of display
 */
void print_display(bool display[64][32]) {
  // update display
  printf("\u250C");
  for (int i = 0; i < 64; i++)
    printf("\u2500");
  printf("\u2510\n");

  for (int i = 0; i < 32; i++) {
    printf("\u2502");
    for (int k = 0; k < 64; k++) {
      if (display[k][i]) {
        printf("\U000025A0");
      } else {
        printf(" ");
      }
    }
    printf("\u2502\n");
  }

  printf("\u2514");
  for (int i = 0; i < 64; i++)
    printf("\u2500");
  printf("\u2518\n");
}

/**
 * @brief Fetch 16 bit instruction from ROM
 *
 * @param chip8 Chip-8 context
 * @return uint16_t Raw instruction pointed to by
 * chip8->PC
 */
uint16_t fetch(ChipContext *chip8) {
  printf("PC[0x%04x]", chip8->PC);
  fflush(stdout);
  printf(" -> 0x%04x\n", chip8->memory[chip8->PC]);

  return chip8->memory[chip8->PC];
}

/**
 * @brief Decode a uint16_t raw_instruction
 *
 * @param raw_instruction Instruction fetched from ROM
 * @return Instruction function pointer to intended instruction from ROM
 */
Instruction decode(ChipContext *chip8) {
  switch (n0(chip8) >> 4) {
  case 0x0:
    if (n1(chip8) == 0xE0) {
      return CLS;
    } else if (n1(chip8) == 0xEE) {
      return RET;
    } else if (n1(chip8) == 0x00) {
      return SYS_addr;
    }
  case 0x1:
    return JP_addr;
  case 0x2:
    return CALL_addr;
  case 0x3:
    return SE_Vx_byte;
  case 0x4:
    return SNE_Vx_byte;
  case 0x5:
    return SE_Vx_Vy;
  case 0x6:
    return LD_Vx_byte;
  case 0x7:
    return ADD_Vx_byte;
  case 0x8:
    if (n1(chip8) == 0x00) {
      return LD_Vx_Vy;
    } else if (n1(chip8) == 0x01) {
      return OR_Vx_Vy;
    } else if (n1(chip8) == 0x02) {
      return AND_Vx_Vy;
    } else if (n1(chip8) == 0x03) {
      return XOR_Vx_Vy;
    } else if (n1(chip8) == 0x04) {
      return ADD_Vx_Vy;
    } else if (n1(chip8) == 0x05) {
      return SUB_Vx_Vy;
    } else if (n1(chip8) == 0x06) {
      return SHR_Vx_Vy;
    } else if (n1(chip8) == 0x07) {
      return SUBN_Vx_Vy;
    } else if (n1(chip8) == 0x0E) {
      return SHL_Vx_Vy;
    }
  case 0x9:
    return SHL_Vx_Vy;
  case 0xA:
    return LD_I_addr;
  case 0xB:
    return JP_V0_addr;
  case 0xC:
    return RND_Vx_byte;
  case 0xD:
    return DRW_Vx_nibble;
  case 0xF:
    if (n1(chip8) == 0x07) {
      return LD_Vx_DT;
    } else if (n1(chip8) == 0x0A) {
      return LD_Vx_K;
    } else if (n1(chip8) == 0x15) {
      return LD_DT_Vx;
    } else if (n1(chip8) == 0x18) {
      return LD_ST_Vx;
    } else if (n1(chip8) == 0x1E) {
      return ADD_I_Vx;
    } else if (n1(chip8) == 0x29) {
      return LD_F_Vx;
    } else if (n1(chip8) == 0x33) {
      return LD_B_Vx;
    } else if (n1(chip8) == 0x55) {
      return LD_I_Vx;
    } else if (n1(chip8) == 0x65) {
      return &LD_Vx_I;
    }
  default:
    if ((n0(chip8) << 8) + n1(chip8) == 0xE09E) {
      return SKP_Vx;
    } else if ((n0(chip8) << 8) + n1(chip8) == 0xE0A1) {
      return SKNP_Vx;
    }
  }

  printf("ERROR: unknown instruction 0x%04x\n", (n0(chip8) << 8) + n1(chip8));
  exit(1);
}
