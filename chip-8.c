#include "chip-8.h"
#include <stdint.h>

// masks for decoding
#define mask_0xF000(a) (a % 0xF000)
#define mask_0xF00F(a) (a % 0xF00F)
#define mask_0xF0FF(a) (a % 0xF00F)
#define mask_0xF0FF(a) (a % 0xF00F)


// instructions
void SYS_addr(ChipContext *chip_context, uint16_t raw_instruction) {printf("SYS_addr\n");}
void CLS(ChipContext *chip_context, uint16_t raw_instruction) {printf("CLS\n");}
void RET(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void JP_addr(ChipContext *chip_context, uint16_t raw_instruction) {printf("JP_addr\n");}
void CALL_addr(ChipContext *chip_context, uint16_t raw_instruction) {printf("CALL_addr\n");}
void SE_Vx_byte(ChipContext *chip_context, uint16_t raw_instruction) {printf("SE_Vx_byte\n");}
void SNE_Vx_byte(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void SE_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void LD_Vx_byte(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void ADD_Vx_byte(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void LD_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void OR_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void AND_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void XOR_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void ADD_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void SUB_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void SHR_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void SUBN_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void SUBN_Vx_VY(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void SHL_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void SNE_Vx_Vy(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void LD_I_addr(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void JP_V0_addr(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void RND_Vx_byte(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void DRW_Vx_nibble(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void SKP_Vx(ChipContext *chip_context, uint16_t raw_instruction) {printf("SKP_Vx\n");}
void SKNP_Vx(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void LD_Vx_DT(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void LD_Vx_K(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void LD_DT_Vx(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void LD_ST_Vx(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void ADD_I_Vx(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void LD_F_Vx(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void LD_B_Vx(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void LD_I_Vx(ChipContext *chip_context, uint16_t raw_instruction) {printf("void\n");}
void LD_Vx_I(ChipContext *chip_context, uint16_t raw_instruction) {printf("LD_Vx_I\n");}
void ignore(ChipContext *chip_context, uint16_t raw_instruction) {printf("ignore\n");}

void print_display(Display display) {
  // update display
  printf("\u250C");
  for (int i = 0; i < 64; i++)
    printf("\u2500");
  printf("\u2510\n");

  for (int i = 0; i < 32; i++) {
    printf("\u2502");
    for (int k = 0; k < 64; k++) {
      if (display.display[i][k]) {
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

int8_t return_code(ChipContext *chip_context, FILE *ROM) { return 0; }

uint16_t fetch(ChipContext *chip_context) {
  return chip_context->memory[chip_context->program_counter];
}

Instruction decode(uint16_t raw_instruction) {
  switch (mask_0xF000(raw_instruction)) {
  case 0x0000:
    if (raw_instruction == 0x00E0) {
      return CLS;
    } else if (raw_instruction == 0x00EE) {
      return RET;
    } else if (mask_0xF000(raw_instruction) == 0x0000) {
      return SYS_addr;
    }
  case 0x1000:
    return JP_addr;
  case 0x2000:
    return CALL_addr;
  case 0x3000:
    return SE_Vx_byte;
  case 0x4000:
    return SNE_Vx_byte;
  case 0x5000:
    return SE_Vx_Vy;
  case 0x6000:
    return LD_Vx_byte;
  case 0x7000:
    return ADD_Vx_byte;
  case 0x8000:
    if (mask_0xF00F(raw_instruction) == 0x8000) {
      return LD_Vx_Vy;
    } else if (mask_0xF00F(raw_instruction) == 0x8001) {
      return OR_Vx_Vy;
    } else if (mask_0xF00F(raw_instruction) == 0x8002) {
      return AND_Vx_Vy;
    } else if (mask_0xF00F(raw_instruction) == 0x8003) {
      return XOR_Vx_Vy;
    } else if (mask_0xF00F(raw_instruction) == 0x8004) {
      return ADD_Vx_Vy;
    } else if (mask_0xF00F(raw_instruction) == 0x8005) {
      return SUB_Vx_Vy;
    } else if (mask_0xF00F(raw_instruction) == 0x8006) {
      return SHR_Vx_Vy;
    } else if (mask_0xF00F(raw_instruction) == 0x8007) {
      return SUBN_Vx_Vy;
    } else if (mask_0xF00F(raw_instruction) == 0x800E) {
      return SHL_Vx_Vy;
    }
  case 0x9000:
    return SHL_Vx_Vy;
  case 0xA000:
    return LD_I_addr;
  case 0xB000:
    return JP_V0_addr;
  case 0xC000:
    return RND_Vx_byte;
  case 0xD000:
    return DRW_Vx_nibble;
  case 0xF000:
    if (mask_0xF0FF(raw_instruction) == 0xF007) {
      return LD_Vx_DT;
    } else if (mask_0xF0FF(raw_instruction) == 0xF00A) {
      return LD_Vx_K;
    } else if (mask_0xF0FF(raw_instruction) == 0xF015) {
      return LD_DT_Vx;
    } else if (mask_0xF0FF(raw_instruction) == 0xF018) {
      return LD_ST_Vx;
    } else if (mask_0xF0FF(raw_instruction) == 0xF01E) {
      return ADD_I_Vx;
    } else if (mask_0xF0FF(raw_instruction) == 0xF029) {
      return LD_F_Vx;
    } else if (mask_0xF0FF(raw_instruction) == 0xF033) {
      return LD_B_Vx;
    } else if (mask_0xF0FF(raw_instruction) == 0xF055) {
      return LD_I_Vx;
    } else if (mask_0xF0FF(raw_instruction) == 0xF065) {
      return &LD_Vx_I;
    }
  default:
    if (mask_0xF0FF(raw_instruction) == 0xE09E) {
      return SKP_Vx;
    } else if (mask_0xF0FF(raw_instruction) == 0xE0A1) {
      return SKNP_Vx;
    }
  }

  printf("ERROR: unknown instruction 0x%x\n", raw_instruction);
  exit(1);
}
