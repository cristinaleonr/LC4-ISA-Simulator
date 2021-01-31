#include <stdio.h>
#include "lc4.h"

int generate_milestone_file(lc4_state* state) {
  // make output file
  FILE *w = fopen("milestone.txt","w");
  // make pointer to memory array
  total_memory* mem = &state->memory;
  unsigned short* i = mem->memory_array;
  int j = 0;
  int boolCode = 0;
  int boolData = 0;

  while (j < 65536) {
    if (*i != 0) {
      if (j < 0x2000) {
	if (boolCode == 0) {
	  boolCode = 1;
	  fprintf(w, "CODE:\n");
	}
	fprintf(w,"address: %05d contents: 0x%04X\n",j,*i);
      }
      else {
	if (boolData == 0) {
	  boolData = 1;
	  fprintf(w,"DATA:\n");
	}
	fprintf(w,"address: %05d contents: 0x%04X\n",j,*i);
      }
    }
    i++;
    j++;
  }
  

  return 0;
}
