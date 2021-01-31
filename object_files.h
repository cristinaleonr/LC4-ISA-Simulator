/*
 * CIS 240 HW 10: LC4 Simulator
 * object_files.h - declares functions for object_files.c
 */
#include "lc4.h"


/* Function Headers -- to be implemented in object_files.c */
int read_object_file (char *filename, lc4_state* state);

int read_instruction(FILE* fp);

void check_file (char *filename, FILE *file);

/* flip_endian is a function you must implement only if you run
 * into an endianness issue when reading in the .OBJ file
 */
short int flip_endian (short int word);
