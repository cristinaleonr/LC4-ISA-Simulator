/*
 * CIS 240 HW 10: LC4 Simulator
 * lc4.h - defines lc4_state struct and prototypes for related functions
 */
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Struct representing all mux control signals
 */
typedef struct{
    unsigned char pc_mux_ctl;
    unsigned char rs_mux_ctl;
    unsigned char rt_mux_ctl;
    unsigned char rd_mux_ctl;
    unsigned char reg_input_mux_ctl;
    unsigned char arith_mux_ctl;
    unsigned char logic_mux_ctl;
    unsigned char alu_mux_ctl;
} mux_ctl;


/*
 * Struct representing alu control signals
 */
typedef struct {
    unsigned char arith_ctl;
    unsigned char logic_ctl;
    unsigned char shift_ctl;
    unsigned char const_ctl;
    unsigned char cmp_ctl;

} alu_ctrl;


/*
 * Struct representing write-enable control signals
 */
typedef struct {
  unsigned char reg_file_we;
  unsigned char nzp_we;
  unsigned char data_we;

} reg_ctrl;


/*
 * Struct representing all control signals, holds structs representing the three sub-structs
 */
typedef struct {

  mux_ctl mux_ctrls;
  alu_ctrl alu_ctrls;
  reg_ctrl reg_ctrls;

} ctrl;


/*
 * Struct representing entire memory of the machine.
 */
typedef struct {

  // Machine registers - all 8
  unsigned short int R[8];

  // Machine memory - all rows
  /* HINT: You will have to dynamically create this array of 65536 short, unsigned ints*/
  unsigned short int* memory_array;


} total_memory;


/*
 * Struct representing the complete state of the machine.
 */
typedef struct {

  // Current value of the program counter register
  unsigned short int PC;

  // Processor status register; bit[2]=N, [1]=Z, [0]=P, bit[15]=privilege bit
  unsigned short int PSR;

  // machine memory
  total_memory memory;

  // Value of the current unsigned immediate
  unsigned short int uimm;

  // Value of the current signed immediate
  short int imm;

  // Current rs address
  unsigned short int rs_addr;

  // Current rt address
  unsigned short int rt_addr;

  // Current rd address
  unsigned short int rd_addr;

} lc4_state;



/*
 * Function headers -- to be implemented in lc4.c
 */
void set_fp(FILE* fp);

void clear_control_signals(ctrl* control);

void reset_lc4(lc4_state* state, ctrl* control);

void decode_instruction(ctrl* control, unsigned short int I);

short int sext(unsigned int length, unsigned short int input);

int check_exceptions(lc4_state* state);

void set_registers(lc4_state* state, ctrl* control, unsigned short int I);

int update_lc4_state(lc4_state* state, ctrl* control);

unsigned short int rs_mux(lc4_state* state, ctrl* control);

unsigned short int rt_mux(lc4_state* state, ctrl* control);

unsigned short int alu_mux(lc4_state* state,
                           ctrl* control,
                           unsigned short int rs_out,
                           unsigned short int rt_out);

unsigned short int reg_input_mux(lc4_state* state,
                                 ctrl* control,
                                 unsigned short int alu_out);

unsigned short int pc_mux(lc4_state* state, ctrl* control, unsigned short int rs_out);

void print_lc4_state(lc4_state* state, ctrl* control);

void print_operation(lc4_state* state, FILE* fp, ctrl* control);








