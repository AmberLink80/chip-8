// debug.h
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG
  #define LOG(fmt, ...) \
      fprintf(stderr, "[DEBUG] %s:%d:%s(): " fmt "\n", \
              __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
  #define LOG(fmt, ...) ((void)0)
#endif

#endif
