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
#include <iso646.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "debug.h"

// masks for decoding
#define mask_0x000F(a) (a & 0x000F)
#define mask_0x00F0(a) (a & 0x00F0)
#define mask_0x00FF(a) (a & 0x00FF)
#define mask_0x0F00(a) (a & 0x0F00)
#define mask_0x0FFF(a) (a & 0x0FFF)
#define mask_0xF000(a) (a & 0xF000)
#define mask_0xF00F(a) (a & 0xF00F)

#define PC (chip8->program_counter)

// .xy.
// ..kk
// .nnn

#define OP ((chip8->memory[PC - 2] << 8) | chip8->memory[PC - 1])
#define fetch_nnn (OP & 0x0FFF)
#define fetch_kk (OP & 0x00FF)
#define fetch_x ((OP >> 8) & 0xF)
#define fetch_y ((OP >> 4) & 0xF)
#define fetch_nibble (OP & 0xF)

#define v(n) (chip8->v[n])

// instructions

// definition of instructions VVV
// https://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf

#define CHIP_MEMCHECK(chip8)                                                   \
  {                                                                            \
    if (chip8 == NULL) {                                                       \
      LOG("Error: ChipContext is null");                                       \
      exit(1);                                                                 \
    }                                                                          \
  }

/**
 * 0nnn - SYS addr
 * Jump to a machine code routine at nnn. This instruction is only used on the
 * old computers on which Chip-8 was originally implemented. It is ignored by
 * modern interpreters. This will not be implemented.
 */
void SYS_addr(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  PC += 2; // skip instruction
}

/**
 * 00E0 - CLC
 * Clear the display.
 */
void CLS(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  // clear the display by setting all elements to 0
  if (memset(&(chip8->display), 0, sizeof(chip8->display)) == NULL) {
    LOG("Error: memset failed");
    return;
  }

  LOG("Display cleared");
}

/**
 * 00EE - RET
 * Return from a subroutine.The interpreter sets the program counter to the
 * address at the top of the stack, then subtracts 1 from the stack pointer.
 */
void RET(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  if (chip8->stack_pointer < 0) {
    LOG("stack underflow");
    return;
  }

  PC = chip8->stack[chip8->stack_pointer];
  chip8->stack_pointer--;
}

/**
 * 1nnn - JP addr
 * Jump to location nnn. The interpreter sets the program counter to nnn.
 */
void JP_addr(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing JP_addr");
  PC = fetch_nnn;
}

/**
 * 2nnn - CALL addr
 * Call subroutine at nnn. The interpreter increments the stack pointer, then
 * puts the current PC on the top of the stack. The PC is then set to nnn.
 */

void CALL_addr(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing CALL_addr");

  uint16_t nnn = fetch_nnn;

  if (chip8->stack_pointer >= 15) {
    LOG("stack overflow");
    return;
  }

  chip8->stack_pointer++;
  chip8->stack[chip8->stack_pointer] = PC;
  PC = nnn;

  LOG("calling subroutine at %d", PC);
}

/**
 * 3xkk - SE Vx, byte
 * Skip next instruction if Vx = kk. The interpreter compares register Vx to
 * kk, and if they are equal, increments the program counter by 2.
 */
void SE_Vx_byte(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing SE_Vx_byte");

  uint8_t x = fetch_x;
  uint8_t kk = fetch_kk;

  LOG("SE Vx %d kk %d", x, kk);

  if (chip8->v[x] == kk) {
    chip8->program_counter += 2;
  }
}

/**
 * 4xkk - SNE Vx, byte
 * Skip next instruction if Vx != kk. The interpreter compares register Vx to
 * kk, and if they are not equal, increments the program counter by 2.
 */
void SNE_Vx_byte(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing SNE_Vx_byte");

  uint8_t x = fetch_x;
  uint8_t kk = fetch_kk;

  if (chip8->v[x] != kk) {
    chip8->program_counter += 2;
    LOG("SNE Vx %d kk %d", x, kk);
  }
}

/**
 * 5xy0 - SE Vx, Vy
 * Skip next instruction if Vx = Vy. The interpreter compares register Vx to
 * register Vy, and if they are equal, increments the program counter by 2.
 */
void SE_Vx_Vy(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing SE_Vx_Vy");

  uint8_t x = fetch_x;
  uint8_t y = fetch_y;

  LOG("SE Vx %d Vy %d", x, y);

  if (v(x) == v(y))
    PC += 2;
}

/**
 * 6xkk - LD Vx, byte
 * Set Vx = kk. The interpreter puts the value kk into register Vx.
 */
void LD_Vx_byte(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing LD_Vx_byte");

  uint8_t x = fetch_x;
  uint8_t kk = fetch_kk;

  v(x) = kk;
}

/**
 * 7xkk - ADD Vx, byte
 * Set Vx = Vx + kk. Adds the value kk to the value of register Vx, then
 * stores the result in Vx.
 */
void ADD_Vx_byte(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing ADD_Vx_byte");

  uint8_t x = fetch_x;
  uint8_t vx = v(x);
  uint8_t kk = fetch_kk;

  v(x) = vx + kk;
}

/**
 * 8xy0 - LD Vx, Vy
 * Set Vx = Vy. Stores the value of register Vy in register Vx.
 */
void LD_Vx_Vy(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing LD_Vx_Vy");

  uint8_t x = fetch_x;
  uint8_t y = fetch_y;

  v(x) = v(y);
}

/**
 * 8xy1 - OR Vx, Vy
 * Set Vx = Vx OR Vy. Performs a bitwise OR on the values of Vx and Vy, then
 * stores the result in Vx. A bitwise OR compares the corresponding bits from
 * two values, and if either bit is 1, then the same bit in the result is
 * also 1. Otherwise, it is 0.
 */
void OR_Vx_Vy(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing OR_Vx_Vy");

  uint8_t x = fetch_x;
  uint8_t y = fetch_y;

  v(x) |= v(y);
}

/**
 * 8xy2 - AND Vx, Vy
 * Set Vx = Vx AND Vy. Performs a bitwise AND on the values of Vx and Vy, then
 * stores the result in Vx. A bitwise AND compares the corresponding bits from
 * two values, and if both bits are 1, then the same bit in the result is
 * also 1. Otherwise, it is 0.
 */
void AND_Vx_Vy(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing AND_Vx_Vy");

  uint8_t x = fetch_x;
  uint8_t y = fetch_y;

  v(x) &= v(y);
}

/**
 * 8xy3 - XOR Vx, Vy
 * Set Vx = Vx XOR Vy. Performs a bitwise exclusive OR on the values of Vx and
 * Vy, then stores the result in Vx. An exclusive OR compares the
 * corresponding bits from two values, and if the bits are not both the same,
 * then the corresponding bit in the result is set to 1. Otherwise, it is 0.
 */
void XOR_Vx_Vy(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing XOR_Vx_Vy");

  uint8_t x = fetch_x;
  uint8_t y = fetch_y;

  v(x) ^= v(y);
}

/**
 * 8xy4 - ADD Vx, Vy
 * Set Vx = Vx + Vy, set VF = carry. The values of Vx and Vy are added
 * together. If the result is greater than 8 bits (i.e., ¿ 255,) VF is set to
 * 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored
 * in Vx.
 */
void ADD_Vx_Vy(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing ADD_Vx_Vy");

  uint8_t x = fetch_x;
  uint8_t y = fetch_y;

  if (v(y) > (0xFF - v(x)))
    chip8->v_flag = 1;
  else
    chip8->v_flag = 0;

  v(x) += v(y);
}

/**
 * 8xy5 - SUB Vx, Vy
 * Set Vx = Vx - Vy, set VF = NOT borrow. If Vx ¿ Vy, then VF is set to 1,
 * otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
 */
void SUB_Vx_Vy(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing SUB_Vx_Vy");

  uint8_t x = fetch_x;
  uint8_t y = fetch_y;

  if (v(x) > v(y))
    chip8->v_flag = 1;
  else
    chip8->v_flag = 0;

  v(x) -= v(y);
}

/**
 * 8xy6 - SHR Vx {, Vy}
 * Set Vx = Vx SHR 1. If the least-significant bit of Vx is 1, then VF is set
 * to 1, otherwise 0. Then Vx is divided by 2.
 */
void SHR_Vx_Vy(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing SHR_Vx_Vy");

  uint8_t x = fetch_x;
  // uint8_t y = fetch_y;

  if (v(x) & 0x0001)
    chip8->v_flag = 1;
  else
    chip8->v_flag = 0;

  v(x) /= 2;
}

/**
 * 8xy7 - SUBN Vx, Vy
 * Set Vx = Vy - Vx, set VF = NOT borrow. If Vy ¿ Vx, then VF is set to 1,
 * otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
 */
void SUBN_Vx_Vy(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing SUBN_Vx_Vy");

  uint8_t x = fetch_x;
  uint8_t y = fetch_y;

  if (v(y) > v(x))
    chip8->v_flag = 1;
  else
    chip8->v_flag = 0;

  v(x) = v(y) - v(x);
}

/**
 * 8xyE - SHL Vx {, Vy}
 * Set Vx = Vx SHL 1. If the most-significant bit of Vx is 1, then VF is set
 * to 1, otherwise to 0. Then Vx is multiplied by 2.
 */
void SHL_Vx_Vy(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing SHL_Vx_Vy");

  uint8_t x = fetch_x;
  // uint8_t y = fetch_y;

  if (v(x) & 0x40)
    chip8->v_flag = 1;
  else
    chip8->v_flag = 0;

  v(x) *= 2;
}

/**
 * 9xy0 - SNE Vx, Vy
 * Skip next instruction if Vx != Vy. The values of Vx and Vy are compared,
 * and if they are not equal, the program counter is increased by 2.
 */
void SNE_Vx_Vy(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing SNE_Vx_Vy");

  uint8_t x = fetch_x;
  uint8_t y = fetch_y;

  if (v(x) != v(y))
    PC += 2;
}

/**
 * Annn - LD I, addr
 * Set I = nnn. The value of register I is set to nnn.
 */
void LD_I_addr(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing LD_I_addr");

  chip8->v_addr = fetch_nnn;
}

/**
 * Bnnn - JP V0, addr
 * Jump to location nnn + V0. The program counter is set to nnn plus the value
 * of V0.
 */
void JP_V0_addr(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("execuing JP_V0_addr");

  uint16_t nnn = fetch_nnn;
  PC = v(0) + nnn;
}

/**
 * Cxkk - RND Vx, byte
 * Set Vx = random byte AND kk. The interpreter generates a random number from
 * 0 to 255, which is then ANDed with the value kk. The results are stored in
 * Vx. See instruction 8xy2 for more information on AND.
 */
void RND_Vx_byte(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing RND_Vx_byte");

  uint8_t x = fetch_x;

  uint16_t kk = fetch_kk;

  v(x) = (rand() % 256) & kk;
}

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
  CHIP_MEMCHECK(chip8);

  LOG("executing DRW_Vx_nibble");

  size_t nbytes = fetch_nibble;

  uint8_t x = v(fetch_x);
  uint8_t y = v(fetch_y);

  chip8->v_flag = 0;

  for (size_t r = 0; r < nbytes; r++) {
    uint8_t sprite = chip8->memory[chip8->v_addr + r];
    int py = (y + r) % SCREEN_HEIGHT;

    for (int bit = 0; bit < 8; bit++) {
      if (!((sprite >> (7 - bit)) & 1))
        continue;

      int px = (x + bit) % SCREEN_WIDTH;
      int idx = py * (SCREEN_WIDTH / 8) + px / 8;
      uint8_t mask = 0x80 >> (px % 8);

      if (chip8->display[idx] & mask)
        chip8->v_flag = 1; // pixel erased -> collision

      chip8->display[idx] ^= mask;
    }
  }

  LOG("sprite drawn");

  chip8->v_draw = 1;
}

/**
 * Ex9E - SKP Vx
 * Skip next instruction if key with the value of Vx is pressed. Checks the
 * keyboard, and if the key corresponding to the value of Vx is currently in
 * the down position, PC is increased by 2.
 */
void SKP_Vx(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing SKP_Vx");

  uint8_t x = fetch_x;

  if (chip8->keypad[v(x)])
    PC += 2;
}

/**
 * ExA1 - SKNP Vx
 * Skip next instruction if key with the value of Vx is not pressed. Checks
 * the keyboard, and if the key corresponding to the value of Vx is currently
 * in the up position, PC is increased by 2.
 */
void SKNP_Vx(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing SKNP_Vx");

  uint8_t x = fetch_x;

  if (!chip8->keypad[v(x)])
    PC += 2;
}

/**
 * Fx07 - LD Vx, DT
 * Set Vx = delay timer value. The value of DT is placed into Vx.
 */
void LD_Vx_DT(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing LD_Vx_DT");

  uint8_t x = fetch_x;

  v(x) = chip8->v_delay;
}

/**
 * Fx0A - LD Vx, K
 * Wait for a key press, store the value of the key in Vx. All execution stops
 * until a key is pressed, then the value of that key is stored in Vx.
 */
void LD_Vx_K(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing LD_Vx_K");

  uint8_t x = fetch_x;

  LOG("waiting for a key press");

  while (true) {
    for (int i = 0; i < 16; i++) {
      if (chip8->keypad[i]) {
        v(x) = i + 1;
        return;
      }
    }
  }
}

/**
 * Fx15 - LD DT, Vx
 * Set delay timer = Vx. Delay Timer is set equal to the value of Vx.
 */
void LD_DT_Vx(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing LD_DT_Vx");

  uint8_t x = fetch_x;

  chip8->v_delay = v(x);
}

/**
 * Fx18 - LD ST, Vx
 * Set sound timer = Vx. Sound Timer is set equal to the value of Vx.
 */
void LD_ST_Vx(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing LD_ST_Vx");

  uint8_t x = fetch_x;

  chip8->v_sound = x;
}

/**
 * Fx1E - ADD I, Vx
 * Set I = I + Vx. The values of I and Vx are added, and the results are
 * stored in I.
 */
void ADD_I_Vx(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing ADD_I_Vx");

  uint8_t x = fetch_x;

  chip8->v_addr += v(x);
}

/**
 * TODO: aint no way this works
 * Fx29 - LD F, Vx
 * Set I = location of sprite for digit Vx. The value of I is set to the
 * location for the hexadecimal sprite corresponding to the value of Vx. See
 * section 2.4, Display, for more information on the Chip-8 hexadecimal font.
 * To obtain this value, multiply VX by 5 (all font data stored in first 80
 * bytes of memory).
 */
void LD_F_Vx(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing LD_F_Vx");
  uint8_t x = fetch_x;
  chip8->v_addr = v(x) * 5;
}

/**
 * Fx33 - LD B, Vx
 * Store BCD representation of Vx in memory locations I, I+1, and I+2. The
 * interpreter takes the decimal value of Vx, and places the hundreds digit in
 * memory at location in I, the tens digit at location I+1, and the ones digit
 * at location I+2.
 */
void LD_B_Vx(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing LD_B_Vx");

  uint8_t vx = v(fetch_x);

  chip8->memory[chip8->v_addr] = vx / 100;
  chip8->memory[chip8->v_addr + 1] = (vx / 10) % 10;
  chip8->memory[chip8->v_addr + 2] = vx % 10;
}

/**
 * Fx55 - LD [I], Vx
 * Stores V0 to VX in memory starting at address I. I is then set to I + x
 * + 1.
 */
void LD_I_Vx(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing LD_I_Vx");

  uint8_t vx = v(fetch_x);

  for (int i = 0; i < vx + 1; i++) {
    chip8->memory[chip8->v_addr + i] = v(i) % MEMORY_SIZE;
  }

  chip8->v_addr += vx;
}

/**
 * Fx65 - LD Vx, [I]
 * Fills V0 to VX with values from memory starting at address I. I is then set
 * to I + x + 1.
 */
void LD_Vx_I(ChipContext *chip8) {
  CHIP_MEMCHECK(chip8);

  LOG("executing LD_Vx_I");

  uint8_t vx = v(fetch_x);

  for (int i = 0; i < vx + 1; i++) {
    chip8->v[i] = chip8->memory[chip8->v_addr + i] % MEMORY_SIZE;
  }

  chip8->v_addr += vx + 1;
}

/**
 * bad instruction
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"
void ignore(ChipContext *chip8) { LOG("ignoring instruction"); }
// Source - https://stackoverflow.com/a/7776146
// Posted by paxdiablo, modified by community. See post 'Timeline' for change
// history Retrieved 2026-10-07, License - CC BY-SA 4.0

/**
 * @brief Fetch 16 bit instruction from ROM
 *
 * @param chip8 Chip-8 context
 * @return uint16_t Raw instruction pointed to by
 * chip8->PC
 */
uint16_t fetch(ChipContext *chip8) {
  uint16_t op = (chip8->memory[PC] << 8) | chip8->memory[PC + 1];
  PC += 2;
  return op;
}

/**
 * @brief Decode a uint16_t raw_instruction
 *
 * @param raw_instruction Instruction fetched from ROM
 * @return Instruction function pointer to intended instruction from ROM
 */
Instruction decode(uint16_t op) {
  switch (op >> 12) {
  case 0x0:
    if (op == 0x00E0)
      return CLS;
    if (op == 0x00EE)
      return RET;
    return SYS_addr;
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
    switch (op & 0xF) {
    case 0x0:
      return LD_Vx_Vy;
    case 0x1:
      return OR_Vx_Vy;
    case 0x2:
      return AND_Vx_Vy;
    case 0x3:
      return XOR_Vx_Vy;
    case 0x4:
      return ADD_Vx_Vy;
    case 0x5:
      return SUB_Vx_Vy;
    case 0x6:
      return SHR_Vx_Vy;
    case 0x7:
      return SUBN_Vx_Vy;
    case 0xE:
      return SHL_Vx_Vy;
    default:
      return ignore;
    }
  case 0x9:
    return SNE_Vx_Vy;
  case 0xA:
    return LD_I_addr;
  case 0xB:
    return JP_V0_addr;
  case 0xC:
    return RND_Vx_byte;
  case 0xD:
    return DRW_Vx_nibble;
  case 0xE:
    if ((op & 0xFF) == 0x9E)
      return SKP_Vx;
    if ((op & 0xFF) == 0xA1)
      return SKNP_Vx;
    return ignore;
  case 0xF:
    switch (op & 0xFF) {
    case 0x07:
      return LD_Vx_DT;
    case 0x0A:
      return LD_Vx_K;
    case 0x15:
      return LD_DT_Vx;
    case 0x18:
      return LD_ST_Vx;
    case 0x1E:
      return ADD_I_Vx;
    case 0x29:
      return LD_F_Vx;
    case 0x33:
      return LD_B_Vx;
    case 0x55:
      return LD_I_Vx;
    case 0x65:
      return LD_Vx_I;
    default:
      return ignore;
    }
  }
  return ignore;
}
