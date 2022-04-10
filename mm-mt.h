//22000 do not forget to change**

#define SIZEX 22000
#define SIZEY 22000

long * huge_matrixA;
long * huge_matrixB;
long * huge_matrixC;

FILE *fin1, *fin2, *fout, *ftest;

void flush_all_caches();
void load_matrix_base();
void free_all();
void multiply_base();
void compare_results();

// Your job is to finish these three functions in the way that it is fast
void write_results();
void load_matrix();
void multiply();

// void *multiply(void *arg);

// Any additional variables needed go here

// Any helper functions go here

