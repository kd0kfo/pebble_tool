#include "morse.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void print(const char *list, bool prefix) {
  static char buffer[16];

  while(list != NULL && *list != 0) {
    char2morse(*list, buffer);
    if(prefix)
      printf("%c: ", *list);
    printf("%s ", buffer);
    if(prefix)
      printf("\n");
    *buffer = 0;
    list++;
  }
  printf("\n");
}

void list_all() {
  print("abcdefghijklmnopqrstuvwxyz 0123456789!", true);
}

int main(int argc, char **argv) {
  if(argc == 1) {
    list_all();
  }
  else {
    int i = 1;
    for(;i < argc;i++)
      print(argv[i], false);
  }

  return 0;
}
