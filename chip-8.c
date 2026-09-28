#include "chip-8.h"
#include <stdint.h>

const uint8_t CLC = 0x00E0;
const uint8_t RET = 0x00EE;

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

int8_t return_code(ChipContext *chip_context, FILE *ROM) {
  return 0;
}

Instruction fetch(ChipContext *chip_context, FILE *ROM) {
  uint16_t raw_instruction =
      chip_context->memory[chip_context->program_counter];

  if (raw_instruction == 0x00E0) {
    return NULL;
  } else if (raw_instruction == 0x00EE) {
    return NULL;
  }

  return NULL;
}
