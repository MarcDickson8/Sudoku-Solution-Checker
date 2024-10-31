# Suduko Solution Validator

The program will use the suduko solution inputted to validate that all rows, subgrids and columns
only contain numbers 1-9 with one of each number in a 9x9 grid. This uses a multithreaded solution
to check and validate the suduko.


### COMPILATION

Ensure that your command prompt is in the correct folder where the mssv.c file is located and
run the command 'gcc -pthread -o mssv mssv.c' to create a compiled file called mssv.

### RUNNING

The format for running the program includes ./mssv <suduko_solution.txt> <Thread_Delay_Time>.
Ensure the <suduko_solution.txt> file is within the same folder as the compiled program to ensure
it can find and run the program.

### OUTPUT

The output should show the 4 threads responsible for subgrids, rows and columns. If all threads are
valid then the solution is valid and has no problems. If one of the threads declares a row/col/subgrid
then these are the locations where the error/s have occured. It will also state that the 
solution is invalid and the total count of valid rows/cols/subgrids.
