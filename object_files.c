/*
 * CIS 240 HW 10: LC4 Simulator
 * object_files.c
 */
#include "object_files.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
/*
 * Read an object file and modify the machine state accordingly.
 * Return a zero if successful and a non-zero error code if you encounter a
 * problem. For the Milestone you can simply set machine_state to 0.
 * Params: pointer to filename, pointer to current machine state.
 * If you have to deal with endianness, call flip_endian here.
 */
int read_object_file (char *filename, lc4_state* state) {
  // try to open file
  
  FILE *file = fopen(filename, "rb");
  check_file(filename,file);
  if (file == NULL) {
    return 1;
  }
  
  // make pointer to memory array
  total_memory* memory = &state->memory;
  unsigned short int* i = memory->memory_array;
  
  // parse file
  while(!feof(file)) {
    // get 2 bytes
    unsigned short int s1 = fgetc(file);
    unsigned short int s2 = fgetc(file);
    int inst = s1 << 8 | s2;
    
    // code
    if (inst == 51934) {
      // address
      s1 = fgetc(file);
      s2 = fgetc(file);
      int address = s1 << 8 | s2;

      // n
      s1 = fgetc(file);
      s2 = fgetc(file);
      // body
      int n = s1 << 8 | s2;
      for (int j = 0; j < n; j++) {
	s1 = fgetc(file);
	s2 = fgetc(file);
	*(i + address + j) = s1 << 8 | s2;
      }
    }
    // data
    if (inst == 56026) {
      // address
      s1 = fgetc(file);
      s2 = fgetc(file);
      int address = s1 << 8 | s2;
      // n
      s1 = fgetc(file);
      s2 = fgetc(file);
      int n = s1 << 8 | s2;
      // body
      for (int j = 0; j < n; j++) {
        s1 = fgetc(file);
        s2 = fgetc(file);
        *(i + address + j) = s1 << 8 | s2;
      }
    }

    // symbol
    if (inst == 50103) {
      // address
      s1 = fgetc(file);
      s2 = fgetc(file);
      int address = s1 << 8 | s2;
      printf("Label found at address x%0.4X: ",address);
      // n
      s1 = fgetc(file);
      s2 = fgetc(file);
      // body
      int n = s1 << 8 | s2;
      for (int j = 0; j < n; j++) {
	s1 = fgetc(file);
	printf("%c",s1);
      }
      printf("\n");
    }

    // file name
    if (inst == 61822) {
      // n
      s1 = fgetc(file);
      s2 = fgetc(file);
      int n = s1 << 8 | s2;
      // body
      for (int j = 0; j < n; j++) {
	s1 = fgetc(file);
      }
    }
    // line number
    if (inst == 29022) {
      // address
      s1 = fgetc(file);
      s1 = fgetc(file);
      // line
      s1 = fgetc(file);
      fgetc(file);
      // file index
      s1 = fgetc(file);
      s1 = fgetc(file);
    }
    
  }
  
  fclose(file);
  return 0;
}



/*
* Read in a 16-bit instruction from the current object file and flip its endianness.
* Return the correct endianness value of the instruction.
* Param: pointer to FILE.
*/
int read_instruction(FILE* fp) {
  int instruction;
  // get 2 bytes
  unsigned int s1;
  unsigned int s2;
  s1 = fgetc(fp);
  s2 = fgetc(fp);
  // get word
  instruction = s1 << 8 | s2;
  
  return instruction;
}



/*
 * Check that the given file exists.
 * Params: pointer to file, pointer to filename.
 */
void check_file (char *filename, FILE *file) {
  // print error if file is null
  if (file == NULL) {
    printf("Could not open %s\n", filename);
  }
  
}



/*
 * Changes a 4-byte word's endian. ex: xDECA -> xCADE
 * This is an *optional* function.
 * If you do run into issues with endianness, implement flip_endian.
 * Params: int to convert.
 */
short int flip_endian (short int word) {

  return 0;
}





