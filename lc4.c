/*
 * CIS 240 HW 10: LC4 Simulator
 * lc4.c
 */

#include "lc4.h"
#define INSN_OP(I) ((I) >> 12)
#define INSN_11_9(I) (((I) >> 9) & 0x7)
#define INSN_7_0(I) ((I) & 0xFF)
#define INSN_5(I) (((I) >> 5) & 0x1) 
#define INSN_5_3(I) (((I) >> 3) & 0x7)
#define INSN_8_7(I) (((I) >> 7) & 0x3)
#define INSN_11(I) (((I) >> 11) & 0x1)
#define INSN_5_4(I) (((I) >> 4) & 0x3)
#define INSN_2_0(I) ((I) & 0x7)
#define INSN_8_6(I) (((I) >> 6) & 0x7)
#define INSN_8_0(I) ((I) & 0x1FF)
#define INSN_4_0(I) ((I) & 0x1F)
#define INSN_6_0(I) ((I) & 0x7F)
#define INSN_10_0(I) ((I) & 0x3FF)
#define INSN_5_0(I) ((I) & 0x3F)
#define INSN_3_0(I) ((I) & 0xF)
#define INSN_11_0(I) ((I) & 0x7FF)

FILE* file = 0;

/*
 * Resets all control signals to 0.
 * Param: pointer to control signals struct
 */
void clear_control_signals(ctrl* control) {

  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 0;
  mux->rs_mux_ctl = 0;
  mux->rt_mux_ctl = 0;
  mux->rd_mux_ctl = 0;
  mux->reg_input_mux_ctl = 0;
  mux->arith_mux_ctl = 0;
  mux->logic_mux_ctl = 0;
  mux->alu_mux_ctl = 0;

  alu_ctrl* alu = &control->alu_ctrls;
  alu->arith_ctl = 0;
  alu->logic_ctl = 0;
  alu->shift_ctl = 0;
  alu->const_ctl = 0;
  alu->cmp_ctl = 0;

  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 0;
  reg->nzp_we = 0;
  reg->data_we = 0;

}



/*
 * Resets the machine state as PennSim would do.
 * Param: pointer to current machine state
 * Make sure to reset the entire lc4 machine, including the control signals!
 */
void reset_lc4(lc4_state* state, ctrl* control) {

  state->PC = 33280;
  state->PSR = 32770;
  state->uimm = 0;
  state->imm = 0;
  state->rs_addr = 0;
  state->rt_addr = 0;
  state->rd_addr = 0;

  total_memory* mem = &state->memory;

  for (int i = 0; i < 8; i++) {
    mem->R[i] = 0;
  }

  clear_control_signals(control);
}

void decode_br(ctrl* control, unsigned short int I) {

  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 0;
  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 0;
  reg->nzp_we = 0;
  reg->data_we = 0;
  
  int sub_op = INSN_11_9(I);
  if (sub_op == 0) {
    mux->pc_mux_ctl = 1;
  }
}

void decode_arith(ctrl* control, unsigned short int I) {

  int sub_op_imm = INSN_5(I);

  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 1;
  mux->rs_mux_ctl = 0;
  mux->rt_mux_ctl = 0;
  mux->rd_mux_ctl = 0;
  mux->reg_input_mux_ctl = 0;
  mux->alu_mux_ctl = 0;
  mux->arith_mux_ctl = 0;
  alu_ctrl* alu = &control->alu_ctrls;
  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 1;
  reg->nzp_we = 1;
  reg->data_we = 0;

  int sub_op = INSN_5_3(I);
  
  switch(sub_op_imm) {
  // add immediate
  case 1:
    alu->arith_ctl = 0;
    mux->arith_mux_ctl = 1;
    break;
  // not immediate
  case 0:
    switch(sub_op) {
    // addition
    case 0:
      alu->arith_ctl = 0;
      break;
    // multiplication
    case 1:
      alu->arith_ctl = 1;
      break;
    // subtraction
    case 2:
      alu->arith_ctl = 2;
      break;
    // division
    case 3:
      alu->arith_ctl = 3;
      break;
    default:
      break;
    }
  default:
    break;
  }
  
}

void decode_cmp(ctrl* control, unsigned short int I) {
  int sub_op = INSN_8_7(I);

  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 1;
  mux->rs_mux_ctl = 2;
  mux->rt_mux_ctl = 0;
  mux->reg_input_mux_ctl = 0;
  mux->alu_mux_ctl = 4;
  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 0;
  reg->nzp_we = 1;
  reg->data_we = 0;

  switch(sub_op) {
    alu_ctrl* alu = &control->alu_ctrls;
  case 0:
    alu->cmp_ctl = 0;
    break;
  case 1:
    alu->cmp_ctl = 1;
    break;
  case 2:
    alu->cmp_ctl = 2;
    break;
  case 3:
    alu->cmp_ctl = 3;
    break;
  default:
    break;
  }
}

void decode_jsr(ctrl* control, unsigned short int I) {
  
  mux_ctl* mux = &control->mux_ctrls;
  mux->rd_mux_ctl = 1;
  mux->reg_input_mux_ctl = 2;
  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 1;
  reg->nzp_we = 1;
  reg->data_we = 0;

  unsigned int sub_op = INSN_11(I);

  if (sub_op == 0) {
    mux->pc_mux_ctl = 5;
  }
  else {
    mux->pc_mux_ctl = 3;
  }
}

void decode_logic(ctrl* control, unsigned short int I) {
  int sub_op_imm = INSN_5(I);
  int sub_op = INSN_5_3(I);

  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 1;
  mux->rs_mux_ctl = 0;
  mux->rt_mux_ctl = 0;
  mux->rd_mux_ctl = 0;
  mux->reg_input_mux_ctl = 0;
  mux->logic_mux_ctl = 0;
  mux->alu_mux_ctl = 1;

  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 1;
  reg->nzp_we = 1;
  reg->data_we = 0;

  alu_ctrl* alu = &control->alu_ctrls;

  switch(sub_op_imm) {
  // immediate
  case 1:
    alu->logic_ctl = 0;
    mux->logic_mux_ctl = 1;
  // non-immediate
  case 0:
    switch(sub_op) {
      // and
    case 0:
      alu->logic_ctl = 0;
    case 1:
      // not
      alu->logic_ctl = 1;
    case 2:
      // or
      alu->logic_ctl = 2;
    case 3:
      // xor
      alu->logic_ctl = 3;
    default:
      break;
    }
  default:
    break;
  }
}

void decode_ldr(ctrl* control, unsigned short int I) {
  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 1;
  mux->rs_mux_ctl = 0;
  mux->rd_mux_ctl = 0;
  mux->reg_input_mux_ctl = 1;
  mux->arith_mux_ctl = 2;
  mux->alu_mux_ctl = 0;

  alu_ctrl* alu = &control->alu_ctrls;
  alu->arith_ctl = 0;

  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 1;
  reg->nzp_we = 1;
  reg->data_we = 0;
}

void decode_str(ctrl* control, unsigned short int I) {
  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 1;
  mux->rs_mux_ctl = 0;
  mux->rt_mux_ctl = 1;
  mux->arith_mux_ctl = 2;
  mux->alu_mux_ctl = 0;

  alu_ctrl* alu = &control->alu_ctrls;
  alu->arith_ctl = 0;

  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 0;
  reg->nzp_we = 0;
  reg->data_we = 1;
  
}

void decode_rti(ctrl* control, unsigned short int I) {
  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 3;
  mux->rs_mux_ctl = 1;
  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 0;
  reg->nzp_we = 0;
  reg->data_we = 0;
}


void decode_const(ctrl* control, unsigned short int I) {
  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 1;
  mux->rd_mux_ctl = 0;
  mux->reg_input_mux_ctl = 0;
  mux->alu_mux_ctl = 3; 

  alu_ctrl* alu = &control->alu_ctrls;
  alu->const_ctl = 0;

  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 1;
  reg->nzp_we = 1;
  reg->data_we = 0;
}

void decode_shift(ctrl* control, unsigned short int I) {
  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 1;
  mux->rs_mux_ctl = 0;
  mux->rd_mux_ctl = 0;
  mux->reg_input_mux_ctl = 0;
  mux->alu_mux_ctl = 2;
  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 1;
  reg->nzp_we = 1;
  reg->data_we = 0;
  alu_ctrl* alu = &control->alu_ctrls;

  int sub_op = INSN_5_4(I);
  
  switch(sub_op) {
  case 0:
    alu->shift_ctl = 0;
    break;
  case 1:
    alu->shift_ctl = 1;
    break;
  case 2:
    alu->shift_ctl = 2;
    break;
  case 3:
    mux->alu_mux_ctl = 0;
    mux->rt_mux_ctl = 0;
    mux->arith_mux_ctl = 0;
    alu->arith_ctl = 4;
  default:
    break;
  }
}

void decode_jmp(ctrl* control, unsigned short int I) {
  int sub_op = INSN_11(I);
  
  mux_ctl* mux = &control->mux_ctrls;
  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 0;
  reg->nzp_we = 0;
  reg->data_we = 0;

  if (sub_op == 0) {
    mux->pc_mux_ctl = 3;
    mux->rs_mux_ctl = 0;
  }
  else {
    mux->pc_mux_ctl = 2;
  }
}

void decode_hiconst(ctrl* control, unsigned short int I) {
  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 1;
  mux->rs_mux_ctl = 2;
  mux->rd_mux_ctl = 0;
  mux->reg_input_mux_ctl = 0;
  mux->alu_mux_ctl = 3;
  
  alu_ctrl* alu = &control->alu_ctrls;
  alu->const_ctl = 1;

  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 1;
  reg->nzp_we = 1;
  reg->data_we = 0;
  
}

void decode_trap(ctrl* control, unsigned short int I) {
  mux_ctl* mux = &control->mux_ctrls;
  mux->pc_mux_ctl = 4;
  mux->rd_mux_ctl = 1;
  mux->reg_input_mux_ctl = 2;
  mux->rd_mux_ctl = 1;

  reg_ctrl* reg = &control->reg_ctrls;
  reg->reg_file_we = 1;
  reg->nzp_we = 1;
  reg->data_we = 0;
}

/*
 * Decodes instruction and sets control signals accordingly.
 * Params: pointer to control signals struct, instruction to decode
 * HINT: Split up the individual decoding into multiple functions
 * Here is a great place to use switch statements to call a specific decoding function
 */
void decode_instruction(ctrl* control, unsigned short int I) {

  unsigned int op = INSN_OP(I);

  switch(op) {
  // BR
  case 0:
    decode_br(control,I);
    break;
  // arithmetic 
  case 1:
    decode_arith(control, I);
    break;
  // CMP
  case 2:
    decode_cmp(control, I);
    break;
  // JSR
  case 4:
    decode_jsr(control, I);
    break;
  // logic
  case 5:
    decode_logic(control,I);
    break;
  // load
  case 6:
    decode_ldr(control,I);
    break;
  // store
  case 7:
    decode_str(control,I);
    break;
  // rti
  case 8:
    decode_rti(control,I);
    break;
  // const
  case 9:
    decode_const(control,I);
    break;
  // shift
  case 10:
    decode_shift(control,I);
    break;
  // jmpr/jmp
  case 12:
    decode_jmp(control,I);
    break;
  // hiconst
  case 13:
    decode_hiconst(control,I);
    break;
  // trap
  case 15:
    decode_trap(control,I);
    break;
  // default
  default:
    break;
  }
  
}


/*
 * Updates the machine state, simulating a single clock cycle.
 * Check for exceptions here.
 * Set privileged bit.
 * Set registers.
 * Returns 0 if update successful, a nonzero error code if unsuccessful.
 * Params: pointer to current machine state, pointer to control signals struct
 */
int update_lc4_state(lc4_state* state, ctrl* control) {
  
  reg_ctrl* reg = &control->reg_ctrls;

  total_memory* mem = &state->memory;
  unsigned short int I = mem->memory_array[state->PC];

  decode_instruction(control,mem->memory_array[state->PC]);
  set_registers(state,control,mem->memory_array[state->PC]);

  if (check_exceptions(state) != 0) {
    return 1;
  }

  unsigned short int rs_out = rs_mux(state,control);
  unsigned short int rt_out = rt_mux(state,control);
  unsigned short int alu_out = alu_mux(state,control,rs_out,rt_out);
  unsigned short int reg_out = reg_input_mux(state,control,alu_out);
  short int signed_reg_out = reg_out;

  // write to register
  if (reg->reg_file_we == 1) {
    mem->R[state->rd_addr] = signed_reg_out;
  }
  // write to data
  if (reg->data_we == 1) {
    mem->memory_array[alu_out] = rt_out;
  }
  // write nzp
  if (reg->nzp_we == 1) {
    // nzp = z
    if (signed_reg_out == 0) {
      state->PSR = state->PSR & 0xFFF8;
      state->PSR = state->PSR | 2;
    } // nzp = p
    else if (signed_reg_out > 0) {
      state->PSR = state->PSR & 0xFFF8;
      state->PSR = state->PSR | 1;
    } // nzp = n
    else {
      state->PSR = state->PSR & 0xFFF8;
      state->PSR = state->PSR | 4;
    }
  }

  // check PSR[15]
  if (INSN_OP(I) == 15) {
    // set privilege bit to 1
    state->PSR = state->PSR | 32768;
  }
  else if (INSN_OP(I) == 8) {
    // set privilege bit to 0
    state->PSR = state->PSR & 32767;
  }
  
  // print
  print_operation(state,file,control);
  // update state
  state->PC = pc_mux(state,control,rs_out);

  return 0;
}



/*
* Check if any exceptions have happened in the current state.
* Returns int corresponding to error code that occured, or 0 if no errors
* Param: pointer to current machine state
*/
int check_exceptions(lc4_state* state) {
  
  total_memory* mem = &state->memory;
  unsigned short int I = mem->memory_array[state->PC];
  unsigned short int op = INSN_OP(I);

  // accessing data as code
  if ((state->PC >= 0x2000 && state->PC < 0x8000) || (state->PC >= 0xA000)) {
    return 1;
  }

  // trying to read code as data
  if (op == 6 || op == 7) {
    unsigned short int add = mem->R[state->rs_addr] + 
      (signed)sext(6, INSN_5_0(I)); 
    if ((add < 0x2000) || (add >= 0x8000 && add < 0xA000)) {
      return 2;
    }
    if (add >= 0xA000) {
      
      // trying to store data in OS without permission
      unsigned short int psr_15 = (state->PSR & 0x8000) >> 15;
      if (psr_15 != 1) {
	return 3;
      }
    }
  }

  // trying to access OS when not in OS mode
  if (state->PC >= 0x8000) {
    if (((state->PSR & 0x8000) >> 15) != 1) {
      return 3;
    }
  }

  return 0;
}



/*
 * Sets file pointer.
 * Param: pointer to the file
 */
void set_fp(FILE* fp) {

  file = fp;
}



/*
 * Sets the values for rs_addr, rt_addr, and rd_addr fields in machine_state based
 * on current instruction.
 * Param: pointer to machine_state, current instruction
 */
void set_registers(lc4_state* state, ctrl* control, unsigned short int I) {

  int op = INSN_OP(I);

  // BR
  if (op == 0) {
    int sub_op = INSN_11_9(I);
    if (sub_op != 0) {
      state->imm = sext(9, INSN_8_0(I));
    }
  }
  // arithmetic
  else if (op == 1) {
    int sub_op = INSN_5(I);
    if (sub_op != 1) {
      state->rt_addr = INSN_2_0(I);
    }
    else {
      state->imm = sext(5, INSN_4_0(I));
    }
    state->rs_addr = INSN_8_6(I);
    state->rd_addr = INSN_11_9(I);
  }
  // comparison
  else if (op == 2) {
    state->rs_addr = INSN_11_9(I);
    int sub_op = INSN_8_7(I);
    if (sub_op < 2) {
      state->rt_addr = INSN_2_0(I);
    }
    else if (sub_op == 2) {
      state->imm = sext(7, INSN_6_0(I));
    }
    else if (sub_op == 3) {
      state->uimm = INSN_6_0(I);
    }
  }
  // JSR
  else if (op == 4) {
    int sub_op = INSN_11(I);
    if (sub_op == 0) {
      state->rs_addr = INSN_8_7(I);
    }
    else {
      state->imm = sext(11, INSN_10_0(I));
    }
    state->rd_addr = 7;
  }
  // logic
  else if (op == 5) {
    int sub_op = INSN_5(I);
    if (sub_op != 1) {
      sub_op = INSN_5_3(I);
      if (sub_op != 1) {
	state->rt_addr = INSN_2_0(I);
      }
    }
    else {
      state->imm = sext(5, INSN_4_0(I));
    }
    state->rd_addr = INSN_11_9(I);
    state->rs_addr = INSN_8_6(I);
  }
  // ldr
  else if (op == 6) {
    state->rd_addr = INSN_11_9(I);
    state->rs_addr = INSN_8_6(I);
    state->imm = sext(6, INSN_5_0(I));
  }
  // str
  else if (op == 7) {
    state->rt_addr = INSN_11_9(I);
    state->rs_addr = INSN_8_6(I);
    state->imm = sext(6, INSN_5_0(I));
  }
  // rti
  else if (op == 8) {
    state->rs_addr = 7;
  }
  // const
  else if (op == 9) {
    state->rd_addr = INSN_11_9(I);
    state->imm = sext(9, INSN_8_0(I));
  }
  // shift
  else if (op == 10) {
    state->rd_addr = INSN_11_9(I);
    state->rs_addr = INSN_8_6(I);
    int sub_op = INSN_5_4(I);
    if (sub_op == 3) {
      state->rt_addr = INSN_2_0(I);
    }
    else {
      state->uimm = INSN_3_0(I);
    }
  }
  // JMP
  else if (op == 12) {
    int sub_op = INSN_11(I);
    if (sub_op == 0) {
      state->rs_addr = INSN_8_6(I);
    }
    else {
      state->imm = sext(11, INSN_10_0(I));
    } 
  }
  // hiconst
  else if (op == 13) {
    state->rd_addr = INSN_11_9(I);
    state->uimm = INSN_7_0(I);
  }
  // trap
  else if (op == 15) {
    state->rd_addr = 7;
    state->uimm = INSN_7_0(I);
  }
}



/*
 * Returns the current output of the RS mux.
 * Param: pointer to current machine state, pointer to control signals struct
 */
unsigned short int rs_mux(lc4_state* state, ctrl* control) {
  total_memory* mem = &state->memory;
  return mem->R[state->rs_addr];
}



/*
 * Returns the current output of the RT mux.
 * Param: pointer to current machine state, pointer to control signals struct
 */
unsigned short int rt_mux(lc4_state* state, ctrl* control) {
  total_memory* mem = &state->memory;
  return mem->R[state->rt_addr];
}



/*
 * Returns the current output of the ALU mux.
 * Params: pointer to current machine state, RS mux output, RT mux output, pointer to control signals struct
 */
unsigned short int alu_mux(lc4_state* state, ctrl* control, unsigned short int rs_out, unsigned short int rt_out) {

  mux_ctl* mux = &control->mux_ctrls;
  alu_ctrl* alu = &control->alu_ctrls;
  total_memory* mem = &state->memory;
  unsigned short int I = mem->memory_array[state->PC];

  // arithmetic ops
  if (mux->alu_mux_ctl == 0) {
    // register arithmetic
    if (mux->arith_mux_ctl == 0) {
      // addition
      if (alu->arith_ctl == 0) {
	return rs_out + rt_out;
      }
      // multiplication
      else if (alu->arith_ctl == 1) {
	return rs_out * rt_out;
      }
      // subtraction
      else if (alu->arith_ctl == 2) {
	return rs_out - rt_out;
      }
      // division
      else if (alu->arith_ctl == 3) {
	if (rt_out == 0) {
	  return 0;
	}
	else {
	  return rs_out/rt_out;
	}
      }
      // mod
      else if (alu->arith_ctl == 4) {
	return rs_out%rt_out;
      }
    }
    // immediate
    else if (mux->arith_mux_ctl == 1 || mux->arith_mux_ctl == 2) {
      if (alu->arith_ctl == 0) {
	return rs_out + sext(5,INSN_4_0(I));
      }
    }
  }
  // logical ops
  else if (mux->alu_mux_ctl == 1) {
    // register logical
    if (mux->logic_mux_ctl == 0) {
      // and 
      if (alu->logic_ctl == 0) {
	return rs_out & rt_out;
      }
      // not
      else if (alu->logic_ctl == 1) {
	return ~rs_out;
      }
      // or
      else if (alu->logic_ctl == 2) {
	return rs_out | rt_out;
      }
      // xor
      else if (alu->logic_ctl == 3) {
	return rs_out ^ rt_out;
      }				     
    }
    // immediate logical
    else if (mux->logic_mux_ctl == 1) {
      // and
      if (alu->logic_ctl == 0) {
	return rs_out & state->imm;
      }
    }
  }
  // shifter
  else if (mux->alu_mux_ctl == 2) {
    // shift left logical
    if (alu->shift_ctl == 0) {
      return rs_out << state->uimm;
    }
    // shift right arithmetic
    else if (alu->shift_ctl == 1) {
      return (signed)rs_out >> state->uimm;
    }
    // shift right logical
    else if (alu->shift_ctl == 2) {
      return (unsigned)rs_out >> state->uimm;
    }
  }
  // constants
  else if (mux->alu_mux_ctl == 3) {
    if (alu->const_ctl == 0) {
      return state->imm;
    }
    else if (alu->const_ctl == 1) {
      return ((rs_out & 0xFF) | (state->uimm << 8));
    }
  }
  // comparator
  else if (mux->alu_mux_ctl == 4) {
    // signed register comparison
    if (alu->cmp_ctl == 0) {
      if (rs_out - rt_out == 0) {
	return 0;
      }
      else if (rs_out - rt_out > 0) {
	return 1;
      }
      else {
	return -1;
      }
    }
    // unsigned register comparison
    else if (alu->cmp_ctl == 1) {
      if (rs_out - (unsigned)rt_out == 0) {
	return 0;
      }
      else if (rs_out - (unsigned)rt_out > 0) {
	return 1;
      }
      else {
	return -1;
      }
    }
    // signed immediate comparison
    else if (alu->cmp_ctl == 2) {
      if (rs_out - state->imm == 0) {
	return 0;
      }
      else if (rs_out - state->imm > 0) {
	return 1;
      }
      else {
	return -1;
      }
    }
    // unsigned immediate comparison
    else if (alu->cmp_ctl == 3) {
      if (rs_out - state->uimm == 0) {
	return 0;
      }
      else if (rs_out - state->uimm > 0) {
	return 1;
      }
      else {
	return -1;
      }
    }
  }
   
  return 0;
}



/*
 * Returns the current output of the register input mux.
 * Params: pointer to current machine state, current ALU mux output, pointer to control signals struct
 */
unsigned short int reg_input_mux(lc4_state* state, ctrl* control, unsigned short int alu_out) {
  mux_ctl* mux = &control->mux_ctrls;

  // alu
  if (mux->reg_input_mux_ctl == 0) {
    return alu_out;
  }
  // data
  else if (mux->reg_input_mux_ctl == 1) {
    total_memory* mem = &state->memory;
    unsigned short int* mem_array = mem->memory_array;
    return mem_array[alu_out];
  }
  // pc
  else if (mux->reg_input_mux_ctl == 2) {
    return state->PC + 1;
  }

  return 0;
}



/*
 * Returns the current output of the PC mux.
 * Params: current RS mux output, pointer to current machine state, pointer to control signals struct
 */
unsigned short int pc_mux(lc4_state* state, ctrl* control, unsigned short int rs_out) {
 
  mux_ctl* mux = &control->mux_ctrls;
  total_memory* mem = &state->memory;
  unsigned short int I = mem->memory_array[state->PC];
  
  // value of nzp register compared to bits I[11:9]
  if (mux->pc_mux_ctl == 0) {
    unsigned short int nzp = state->PSR & 7;
    if ((nzp & INSN_11_9(I)) > 0) {
      return state->PC + 1 + sext(9, INSN_8_0(I));
    }
    else {
      return state->PC + 1;
    }
  }
  // pc + 1
  else if (mux->pc_mux_ctl == 1) {
    return state->PC + 1;
  }
  // pc + 1 + sext(IMM11)
  else if (mux->pc_mux_ctl == 2) {
    return (state->PC + 1 + sext(12, INSN_11_0(I)));
  }
  // rs 
  else if (mux->pc_mux_ctl == 3) {
    return mem->R[state->rs_addr];
  }
  // (0x8000 | UIMM8)
  else if (mux->pc_mux_ctl == 4) {
    return (0x8000 | INSN_8_0(I));
  }
  // pc & 0x8000 | IMM11 << 4
  else if (mux->pc_mux_ctl == 5) {
    return ((state->PC & 0x8000) | (INSN_11_0(I) << 4));
  }
  return 0;
}



/*
* Sign extends the given input unsigned short int to the inputted length
* Returns the sign extended value.
* Params: length to sign extend to, value to sign extend
*/
short int sext(unsigned int length, unsigned short int input) {
  int sign = (input >> (length - 1)) & 0x1;
  int mask = 0xFFFF;
  if (sign == 0) {
    return input;
  }
  else {
    mask = mask << length;
    return input | mask;
  }
  return 0;
}


/*
* Prints information about the current state
* Very helpful in debugging!
* Implementing this will also help with receiving partial credit.
*/
void print_lc4_state(lc4_state* state, ctrl* control) {
  mux_ctl* mux = &control->mux_ctrls;
  alu_ctrl* alu = &control->alu_ctrls;
  reg_ctrl* reg = &control->reg_ctrls;

  // print control signals
  printf("Control Signals: \n");
  printf("MUX_CTL \n");
  printf("pc_mux_ctl %d\n", mux->pc_mux_ctl);
  printf("rs_mux_ctl %d\n", mux->rs_mux_ctl);
  printf("rt_mux_ctl %d\n", mux->rt_mux_ctl);
  printf("rd_mux_ctl %d\n", mux->rd_mux_ctl);
  printf("reg_input_mux_ctl %d\n", mux->reg_input_mux_ctl);
  printf("arith_mux_ctl %d\n", mux->arith_mux_ctl);
  printf("logic_mux_ctl %d\n", mux->logic_mux_ctl);
  printf("alu_mux_ctl %d\n", mux->alu_mux_ctl);
  printf("ALU_CTL \n");
  printf("arith_ctl %d\n", alu->arith_ctl);
  printf("logic_ctl %d\n", alu->logic_ctl);
  printf("shift_ctl %d\n", alu->shift_ctl);
  printf("const_ctl %d\n", alu->const_ctl);
  printf("cmp_ctl %d\n", alu->cmp_ctl);
  printf("REG_CTL \n");
  printf("reg_file_we %d\n", reg->reg_file_we);
  printf("nzp_we %d\n", reg->nzp_we);
  printf("data_we %d\n", reg->data_we);

  // print state
  printf("\nState: \n");
  printf("PC %d\n", state->PC);
  printf("PSR %d\n", state->PSR);
  printf("uimm %d\n", state->uimm);
  printf("imm %d\n", state->imm);
  printf("rs_addr %d\n", state->rs_addr);
  printf("rt_addr %d\n", state->rt_addr);
  printf("rd_addr %d\n", state->rd_addr);

  // print registers
  total_memory* mem = &state->memory;

  for (int i = 0; i < 8; i++) {
    printf("R[%d] %d\n",i,mem->R[i]);
  }

}



/*
* Prints current operation to stdout and the output file
*/
void print_operation(lc4_state* state, FILE* fp, ctrl* control) {

  reg_ctrl* reg = &control->reg_ctrls;
  total_memory* mem = &state->memory;
  unsigned short int I = mem->memory_array[state->PC];

  // print to file
  // PC
  fprintf(fp,"%0.4X ",state->PC);
  
  // instruction
  for (int i = 0; i < 16; i++) {
    if (((I << i) & 0x8000) == 0x8000) {
	fprintf(fp,"1");
    }
    else {
      fprintf(fp,"0");
    }
  }

  // reg_file.we
  fprintf(fp," %0.1X ",reg->reg_file_we);

  // if ref_file.we = 1
  if (reg->reg_file_we == 1) {
    fprintf(fp,"%0.1X ",state->rd_addr);
    fprintf(fp,"%0.4X ",mem->R[state->rd_addr]);

  }
  // if ref_file.we = 0
  else {
    fprintf(fp,"0 0000 ");
  }

  // nzp.we
  fprintf(fp,"%0.1X ",reg->nzp_we);

  // if nzp.we = 1
  if (reg->nzp_we == 1) {
    int nzp = state->PSR & 7;
    fprintf(fp,"%0.1X ",nzp);
  }
  // if nzp.we = 0
  else {
    fprintf(fp,"0 ");
  }

  // data.we
  fprintf(fp,"%0.1X ",reg->data_we);

  // if load
  if (INSN_OP(I) == 6) {
    fprintf(fp,"%0.4X ", mem->R[state->rs_addr] + INSN_5_0(I));
    fprintf(fp,"%0.4X\n", mem->R[state->rt_addr]);
  }
  // else if store
  else if (INSN_OP(I) == 7) {
    fprintf(fp,"%0.4X ", mem->R[state->rs_addr] + INSN_5_0(I));
    fprintf(fp,"%0.4X\n",mem->R[state->rd_addr]);
  }
  else {
    fprintf(fp,"0000 0000\n");
  }

  // print to stdout
  printf("PC:%0.4X ",state->PC);

  unsigned short int op = INSN_OP(I);

 
  //BR
  if (op == 0) {
    int sub_op = INSN_11_9(I);
    // NOP
    if (sub_op == 0) {
      printf("%s ","NOP");
    }
    // BRn
    else if (sub_op == 4) {
      printf("BRn %0.4X ", INSN_8_0(I));
    }
    // BRnz
    else if (sub_op == 6) {
      printf("BRnz %0.4X ", INSN_8_0(I));
    }
    // BRnp
    else if (sub_op == 5) {
      printf("BRnp %0.4X ", INSN_8_0(I));
    }
    // BRz
    else if (sub_op == 2) {
      printf("BRz %0.4X ", INSN_8_0(I));
    }
    // BRzp
    else if (sub_op == 3) {
      printf("BRzp %0.4X ", INSN_8_0(I));
    } 
    // BRp
    else if (sub_op == 1) {
      printf("BRp %0.4X ", INSN_8_0(I));
    }
    // BRnzp
    else if (sub_op == 7) {
      printf("BRnzp %0.4X ", INSN_8_0(I));
    }
  }
  // arithmetic
  if (op == 1) {
    int sub_op_imm = INSN_5(I);
    // add imm
    if (sub_op_imm == 1) {
      printf("ADD R%d, R%d, #%d ", state->rd_addr, state->rs_addr, sext(5,INSN_4_0(I)));
    }
    else {
      // add
      int sub_op = INSN_5_3(I);
      if (sub_op == 0) {
	printf("ADD R%d, R%d, R%d ", state->rd_addr, state->rs_addr, state->rt_addr);
      }
      // mul
      else if (sub_op == 1) {
	printf("MUL R%d, R%d, R%d ", state->rd_addr, state->rs_addr, state->rt_addr);
      }
      // sub
      else if (sub_op == 2) {
	printf("SUB R%d, R%d, R%d ", state->rd_addr, state->rs_addr, state->rt_addr);
      }
      // div
      else if (sub_op == 3) {
	printf("DIV R%d, R%d, R%d ", state->rd_addr, state->rs_addr, state->rt_addr);
      }
    }
  }
  // CMP
  if (op == 2) {
    int sub_op = INSN_8_7(I);
    // cmp
    if (sub_op == 0) {
      printf("CMP R%d, R%d ", state->rs_addr, state->rt_addr);
    }
    // cmpu
    else if (sub_op == 1) {
      printf("CMPU R%d, R%d ", state->rs_addr, state->rt_addr);
    }
    // cmpi
    else if (sub_op == 2) {
      printf("CMPI R%d, %0.4X ",state->rs_addr, (signed)INSN_6_0(I));
    }
    // cmpiu
    else if (sub_op == 3) {
      printf("CMPIU R%d, %0.4X ", state->rs_addr, INSN_6_0(I));
    }
  }
  // JSR
  if (op == 4) {
    int sub_op = INSN_11(I);
    // JSR
    if (sub_op == 1) {
      printf("JSR %0.4X ", (signed)INSN_11_0(I));
    }
    // JSSR
    else if (sub_op == 0) {
      printf("JSSR R%d ", state->rs_addr);
    }
  }
  // logic
  if (op == 5) {
    int sub_op_imm = INSN_5(I);
    // and imm
    if (sub_op_imm == 1) {
      printf("AND R%d, R%d, #%d ", state->rd_addr, state->rs_addr, (signed)INSN_4_0(I));
    }
    else {
      int sub_op = INSN_5_3(I);
      // and
      if (sub_op == 0) {
	printf("AND R%d, R%d, R%d ", state->rd_addr, state->rs_addr,state->rt_addr);
      }
      // not
      else if (sub_op == 1) {
	printf("NOT R%d, R%d, R%d ", state->rd_addr, state->rs_addr, state->rt_addr);
      }
      // or
      else if (sub_op == 2) {
	printf("OR R%d, R%d, R%d ", state->rd_addr, state->rs_addr, state->rt_addr);
      }
      // xor
      else if (sub_op == 3) {
	printf("XOR R%d, R%d, R%d ", state->rd_addr, state->rs_addr, state->rt_addr);
      }
    }
  }
  //load
  if (op == 6) {
    printf("LDR R%d, R%d, #%d ", state->rd_addr, state->rs_addr, (signed)INSN_5_0(I));
  }
  // store
  if (op == 7) {
    printf("STR R%d, R%d, #%d ", state->rt_addr, state->rs_addr, (signed)INSN_5_0(I));
  }
  // rti
  if (op == 8) {
    printf("RTI ");
  }
  //const
  if (op == 9) {
    printf("CONST R%d, #%d ", state->rd_addr, (signed)INSN_8_0(I));
  }
  // shift
  if (op == 10) {
    int sub_op = INSN_5_4(I);
    // SLL
    if (sub_op == 0) {
      printf("SLL R%d, R%d, #%d ", state->rd_addr, state->rs_addr, INSN_3_0(I));
    }
    // SRA
    else if (sub_op == 1) {
      printf("SRA R%d, R%d, #%d ", state->rd_addr, state->rs_addr, INSN_3_0(I));
    }
    // SRL
    else if (sub_op == 2) {
      printf("SRL R%d, R%d, #%d ", state->rd_addr, state->rs_addr, INSN_3_0(I));
    }
    // MOD
    else if (sub_op == 3) {
      printf("MOD R%d, R%d, R%d ",state->rd_addr, state->rs_addr, state->rt_addr);
    }
  }
  // jmpr/jmp
  if (op == 12) {
    int sub_op = INSN_11(I);
    if (sub_op == 0) {
      printf("JMPR R%d ", state->rs_addr);
    }
    else if (sub_op == 1) {
      printf("JMP %0.4X ", (signed)INSN_10_0(I));
    }
  }
  // hiconst
  if (op == 13) {
    printf("HICONST R%d, #%d ", state->rd_addr, INSN_7_0(I));
  }    
  // trap
  if (op == 15) {
    printf("TRAP #%d ", INSN_7_0(I));
  }

  for (int i = 0; i < 8; i++) {
    printf("R%d:%0.4X ",i,mem->R[i]);
  }

  printf("PSR[15]:%d ",(state->PSR & 0x8000) >> 15);

  printf("NZP:%d\n", state->PSR & 7);
}







