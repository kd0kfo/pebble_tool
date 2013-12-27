#include "morse.h"
#include <stdio.h>

int main(int argc, char **argv) {
  int word_counter = 1;

  if(argc > 1) {
    for(;word_counter < argc;word_counter++) {
      char *morse = argv[word_counter];
      printf("%c", morse2char(morse));
    }
    printf("\n");
  }

  return 0;
}
