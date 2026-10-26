#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chip-8.h"

#define PC (chip8->program_counter)

void help() {
  printf("chip8 interpreter.\n");
  printf("Usage: chip8 [OPTION]\n");
  printf("  -f,         specify input file\n");
  printf("  -c,         stop at cycle n\n");
  printf("  -s,         set start of program (default 0x200)\n");
  printf("  -h,         print this help message\n");
}

// void draw(ChipContext *chip8) {
//   erase();
//   for (int y = 0; y < SCREEN_HEIGHT; y++) {
//     move(y, 0);
//     for (int x = 0; x < SCREEN_WIDTH; x++) {
//       addch(chip8->display[y * SCREEN_WIDTH + x] ? ((char) 9632) : ' ');
//     }
//   }
//   refresh();
// }
void draw(ChipContext *chip8) {
  // erase();
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    move(y, 0);
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      int idx = y * (SCREEN_WIDTH / 8) + x / 8;
      uint8_t mask = 0x80 >> (x % 8);
      addch((chip8->display[idx] & mask) ? ((char)9632) : ' ');
    }
  }
  refresh();
}

int main(int argc, char *argv[]) {
  bool verbose = false;

  FILE *ROM = NULL;
  ChipContext *chip8 = calloc(1, sizeof(ChipContext));
  // chip8->memory = calloc(4096 / 2, sizeof(uint16_t));
  chip8->stack = calloc(16, sizeof(uint16_t));
  chip8->stack_pointer = -1;
  PC = 0x200;

  uint32_t cycles_original = -1;

  // no file specified
  if (argc < 2) {
    printf("Error: No file specified\n");
    return 1;
  }

  // update flags
  int opt;
  while ((opt = getopt(argc, argv, "hf:c:vs:")) != -1) {
    switch (opt) {
    case 'f':
      if (verbose)
        printf("opening file...\n");

      ROM = fopen(optarg, "rb");
      break;
    case 'v':
      verbose = true;
      break;
    case 'c':
      cycles_original = strtol(optarg, NULL, 0);
      break;
    case 's':
      PC = atoi(optarg);
      break;
    case 'h':
      help();
      exit(0);
    }
  }

  // file failed to open
  size_t new_len = 0;
  if (ROM != NULL) {
    if (verbose)
      printf("file openned succesfully\n");
    new_len =
        fread(&(chip8->memory[0x200]), sizeof(uint8_t), 4096 - 0x200, ROM);
    if (verbose)
      printf("Loaded %zu bytes into memory\n", new_len);
    fclose(ROM);
  } else {
    printf("Error: Could not open file <%s>", argv[1]);
    return 1;
  }

  // load ROM & cleanup
  uint8_t default_sprites[80] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };
  memcpy(chip8->memory, default_sprites, sizeof default_sprites);

  // initialize curses
  initscr();
  cbreak();
  curs_set(0);
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE); // getch() returns ERR instead of blocking
  setlocale(LC_ALL, "");

  // initialize cycles
  int32_t cycles = cycles_original;
  while (cycles > 0 || cycles == -1) {
    int ch = getch();
    if (ch == 'q')
      break;

    // fetch->decode->execute instruction
    uint16_t raw_instruction = fetch(chip8);
    Instruction instruction = decode(raw_instruction);
    instruction(chip8);

    // update cycle
    if (cycles != -1) {
      cycles--;
    }

    // update display
    if (chip8->v_draw) {
      chip8->v_draw = 0;
      draw(chip8);
      napms(16); // ~60Hz
    }
  }

  // free(chip8->memory);
  endwin();
  free(chip8->stack);
  free(chip8);

  return 0;
}
