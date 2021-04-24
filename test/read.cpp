#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>

#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define NUM_FRAMES 128

int main(void) {
  FILE* _file = fopen("BACKING_STORE.bin", "rb");
  std::vector<signed char> container;
  int page_number = 0;
  char memory[FRAME_SIZE][PAGE_SIZE];

  if(fseek(_file, page_number * PAGE_SIZE, SEEK_SET) != 0) {
    std::cerr << "[FATAL] Error seeking backing store" << std::endl;
    exit(EXIT_FAILURE);
  }

  if(std::fread(memory[page_number], sizeof(char), FRAME_SIZE, _file) == 0) {
    std::cerr << "[FATAL] Error reading backing store" << std::endl;
  }
  for(int i = 0; i < FRAME_SIZE; ++i){
    std::cout << (signed char)memory[page_number][i] << " ";
  }
  std::cout << std::endl;
  return 0;
}
