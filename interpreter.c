#pragma once
#include "chip-8.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const uint8_t CLC = 0x00E0;
const uint8_t RET = 0x00EE;

void clear_screen(Display display) { memset(&display, 0, sizeof(display)); }

uint8_t return_code(ChipContext chip_context, Display display, FILE *ROM) {
  // fetching instruction
  uint8_t buf;
  size_t elements_read = fread(&buf, sizeof(uint8_t), 1, ROM);

  if (elements_read != 1) {
    printf("Error reading file or reached End-of-File.\n");
  }

  if (buf == CLC) { // CLC - Clear the display
    clear_screen(display);
  } else if (buf == RET) {
    chip_context.program_counter = chip_context.stack[--chip_context.stack_pointer];
  }

  // update display
  printf("\u250C");
  for (int i = 0; i < 64; i++) printf("\u2500");
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
  for (int i = 0; i < 64; i++) printf("\u2500");
  printf("\u2518\n");

  return 0;
}
