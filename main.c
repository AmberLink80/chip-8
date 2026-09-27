#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

typedef struct {
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

} chip_context;

// probably unneeded but leaving it for now so its in my mind
typedef struct {
  bool display[64][32];
} display;

typedef struct {
  // probablly some sort of pipe indicating "up <key>" and "down <key>"
} keyboard;

int main(int argc, char *argv[]) {
  FILE *fptr;

  // no file specified
  if (argc < 2) {
    printf("no file specified");
    return 1;
  }

  // open file
  fptr = fopen(argv[1], "rw");

  // file failed to open
  if (fptr == NULL) {
    printf("Error: Could not open file <%s>", argv[1]);
    return 1;
  }

  // cleanup & exit
  fclose(fptr);
  return 0;
}
