// TODO:
// [ ] fix semantics that i missed bc tired TvT
// [ ] catch up on documentation

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chip-8.h"

void help() {
  printf("Chip-8 interpreter.\n");
  printf("Usage: chip-8 [FILENAME] [OPTION]\n");
  printf("  -c,         stop at cycle n\n");
  printf("  -s,         set start of program (default 0x200)\n");
}

int main(int argc, char *argv[]) {
  bool verbose = false;

  FILE *ROM;
  ChipContext *chip_context = calloc(1, sizeof(ChipContext));
  chip_context->memory = calloc(4096 / 2, sizeof(uint16_t));
  chip_context->stack = calloc(16, sizeof(uint16_t));
  chip_context->program_counter = 0x200;

  int32_t cycles = -1;

  // no file specified
  if (argc < 2) {
    printf("Error: No file specified\n");
    return 1;
  }

  // update flags
  int opt;
  while ((opt = getopt(argc, argv, "f:c:v")) != -1) {
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
      cycles = atoi(optarg);
      break;
    case 's':
      chip_context->program_counter = atoi(optarg);
      break;
    }
  }

  // file failed to open
  if (ROM) {
    if (verbose)
      printf("file openned succesfully\n");
    size_t new_len =
        fread(&(chip_context->memory[0x0]), sizeof(uint8_t), 3584, ROM);
    if (verbose)
      printf("Loaded %zu bytes into memory\n", new_len);
    fclose(ROM);
  } else {
    printf("Error: Could not open file <%s>", argv[1]);
    return 1;
  }

  // load ROM & cleanup
  uint16_t default_sprites[] = {
      0xF090, 0x9090, 0xF020, 0x6020, 0x2070, 0xF010, 0xF080, 0xF0F0,
      0x10F0, 0x10F0, 0xF080, 0xF010, 0xF0F0, 0x80F0, 0x10F0, 0xF080,
      0xF090, 0xF0F0, 0x1020, 0x4040, 0xF090, 0xF090, 0xF0F0, 0x90F0,
      0x10F0, 0xF090, 0xF090, 0x90E0, 0x90E0, 0x90E0, 0xF080, 0x8080,
      0xF0E0, 0x9090, 0x90E0, 0xF080, 0xF080, 0xF0F0, 0x80F0, 0x8080};
  for (int i = 0; i < 40; i++) {
    // chip_context->memory[i] = default_sprites[i];
  }

  // stupid and messy but i don't really care right now
  if (verbose) {
    printf("ROM closed\n");
    printf("hex dump of initial memory\n");

    for (int i = 0x0; i < 0xFFF; i++) {
      printf("%04x ", chip_context->memory[i]);
      if ((i + 1) % 8 == 0) {
        printf("\n");
      }
    }
    if (0xFF % 8 != 0) {
      printf("\n");
    }
  }

  // run interpreter
  // if (verbose)
  //   printf("initializing timer...\n");
  // if (verbose)
  //   printf("timer initialized");
  uint64_t last_tick = SDL_GetTicks64();

  printf("cycles: %d\n", cycles);
  while (cycles > 0 || cycles == -1) {
    // wait for 60hz
    while ((SDL_GetTicks64() - last_tick) < 16) {
      SDL_Delay(1); // Frees the CPU to prevent 100% core usage
    }
    last_tick = SDL_GetTicks64();

    // fetch instruction
    if (verbose)
      printf("fetching instruction...\n");
    uint16_t raw_instruction = fetch(chip_context);
    if (verbose)
      printf("fetched [0x%x] at PC [0x%x]\n", raw_instruction,
             chip_context->program_counter);

    if (verbose)
      printf("decoding instruction...\n");
    Instruction instruction = decode(raw_instruction);
    if (verbose)
      printf("decoded\n");

    // execute instruction
    if (verbose)
      printf("executing instruction <>\n");
    instruction(chip_context, raw_instruction);
    if (verbose)
      printf("executed instruction <>\n");

    // update display
    if (verbose)
      printf("printing screen...\n");
    print_display(chip_context->display);
    if (verbose)
      printf("printed\n");

    // update cycle
    if (cycles != -1) {
      cycles--;
    }

    chip_context->program_counter++;

    printf("cycle complete\n\n");
  }

  // free(&(chip_context->display));
  free(chip_context->memory);
  free(chip_context->stack);
  free(chip_context);

  return 0;
}
