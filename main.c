#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "chip-8.h"

void help() {
  printf("Chip-8 interpreter.\n");
  printf("Usage: chip-8 [FILENAME] [OPTION]\n");
  printf("  -c,         stop at cycle n\n");
}

int main(int argc, char *argv[]) {
  bool verbose = false;

  FILE *ROM;
  ChipContext *chip_context = calloc(1, sizeof(ChipContext));
  Display *display = calloc(1, sizeof(Display));

  int32_t cycles = -1;

  // no file specified
  if (argc < 2) {
    printf("Error: No file specified\n");
    return 1;
  }

  // update flags
  int opt;
  while ((opt = getopt(argc, argv, "c:v")) != -1) {
    switch (opt) {
    case 'v':
      verbose = true;
      break;
    case 'c':
      cycles = atoi(optarg);
      break;
    }
  }

  // open file
  if (verbose)
    printf("opening file...\n");
  ROM = fopen(argv[0], "rw");

  // file failed to open
  if (ROM == NULL) {
    printf("Error: Could not open file <%s>", argv[1]);
    return 1;
  } else if (verbose) {
    printf("file openned succesfully\n");
  }

  // run interpreter
  if (verbose)
    printf("initializing timer...\n");
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

    // == run state ==

    // fetch instruction
    if (verbose)
      printf("fetching instruction...\n");
    Instruction instruction = fetch(chip_context, ROM);
    if (verbose)
      printf("instruction <> fetched\n");

    // execute instruction

    // update display

    // update cycle
    if (cycles != -1) {
      cycles--;
    }

    printf("executing\n");
  }

  // cleanup & exit
  fclose(ROM);

  free(chip_context);
  free(display);

  return 0;
}
