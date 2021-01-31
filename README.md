<h1>LC4-ISA-Simulator</h1>

Assembly-level similator of the LC4 Instruction Set Architecture (ISA)

Execution
./trace –m 0x---- output_filename.txt first.obj 

Arguments
  1. Optional -m flag to indicate at what address the program should stop executing
  2. The name of the file to write the output to
  3. One or more LC4 object files that have been assembled to be loaded into the simulator
