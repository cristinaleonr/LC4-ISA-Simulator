all: trace

lc4.o: lc4.h lc4.c
	clang -c lc4.c

object_files.o: object_files.h object_files.c
	clang -c object_files.c

milestone_file.o: milestone_file.h milestone_file.c
	clang -c milestone_file.c

trace: trace.c object_files.o lc4.o milestone_file.o
	clang trace.c object_files.o lc4.o milestone_file.o -o trace

clean_exe:
	rm -rf *.o

clean: clean_exe
	rm -rf trace
