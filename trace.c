#include "object_files.h"
#include <string.h>
#include "milestone_file.h"
#include <stdlib.h>

int main (int argc, char *argv[]) {

  int flag;
  // check whether flag was passed
  if (strncmp(argv[1],"-m",2) == 0) {
    flag = 1;
  }
  else {
    flag = 0;
  }
  

  // allocate memory for state
  lc4_state* state = malloc(sizeof(lc4_state));
  if (state == NULL) {
    free(state);
    return 1;
  }
  // make pointer to memory array
  total_memory* mem = &state->memory;
  // initialize R
  for (int i = 0; i < 8; i++) {
    mem->R[i] = 0;
  }
  // make memory for memory array
  mem->memory_array = malloc(65536*sizeof(unsigned short int));
  if (mem->memory_array == NULL) {
    free(mem->memory_array);
    free(state);
    return 1;
  }
  unsigned short int *arr_ptr = mem->memory_array;
  // initialize memory array
  for (int i = 0; i < 65536; i++) {
    *arr_ptr = 0;
    arr_ptr++;
  }
  
  // make control signals
  ctrl* signals = malloc(sizeof(ctrl));

  if (flag == 1) {
    // read files
    for (int i = 5; i <= argc; i++) {
	if (argv[i - 1] != NULL) {
	  int error = read_object_file(argv[i-1],state);
	  if (error) {
	    free(mem->memory_array);
	    free(state);
	    free(signals);
	    return 1;
	  }
	}
    }
    
    generate_milestone_file(state);
    // make output files
    FILE* out = fopen(argv[3],"w");
    set_fp(out);
    reset_lc4(state,signals);
    
    while(1) {
      if (state->PC == strtol(argv[2],NULL,16)) {
	break;
      }
      if (state->PC == 0x80FF) {
	break;
      }
      if (update_lc4_state(state, signals) == 1) {
	break;
      }
    }

    fclose(out);
    
  }
  else {
    // read files if argument was passed in
    for (int i = 4; i <= argc; i++) {
      if (argv[i - 1] != NULL) {
	if (read_object_file(argv[i-1],state)) {
	  free(mem->memory_array);
	  free(state);
	  free(signals);
	  return 1;
	}
      }
    }
    // make output file
    FILE* out = fopen(argv[2],"w");
    set_fp(out);
    reset_lc4(state,signals);
    while(1) {
      if (state->PC == strtol(argv[1],NULL,16)) {
	break;
      }
      if (state->PC == 0x80FF) {
	break;
      }
      if (update_lc4_state(state,signals) == 1) {
	break;
      }
    }

    fclose(out);
  }

  // free allocated memory
  free(mem->memory_array);
  free(state);
  free(signals);
  return 0;
}
