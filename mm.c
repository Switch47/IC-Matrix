#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include "mm.h"


// Task 1: Flush the cache so that we can do our measurement :)
// Reference : https://stackoverflow.com/questions/11277984/how-to-flush-the-cpu-cache-in-linux-from-a-c-program
void flush_all_caches() {
	long i = 0;
	for (i = 0; i < SIZEX*SIZEY; i++) {
		asm volatile("clflush (%0)\n\t" 
			:
			: "r"(huge_matrixA+i) 
			: "memory"
		);
		asm volatile("clflush (%0)\n\t" 
			:
			: "r"(huge_matrixB+i) 
			: "memory"
		);
		asm volatile("clflush (%0)\n\t" 
			:
			: "r"(huge_matrixC+i) 
			: "memory"
		);
	}
	asm volatile("sfence\n\t" ::: "memory");
}

void load_matrix_base() {
	long i;
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixB = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixC = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	// Load the input
	// Note: This is suboptimal because each of these loads can be done in parallel.
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i)); 		
		fscanf(fin2,"%ld", (huge_matrixB+i)); 		
		huge_matrixC[i] = 0;		
	}
}

void free_all()
{
	free(huge_matrixA);
	free(huge_matrixB);
	free(huge_matrixC);
}

void multiply_base()
{
	for (int i = 0; i < SIZEY; i++) {
		for (int j = 0; j < SIZEX; j++) {
			long sum = 0;
			for (int w = 0; w < SIZEX; w++) {
				// 	A =	| a b | , B = | e f |
				// 		| c d |		  | g h |
				//  A * B = | ae+bg		af+bh |
				//			| ce+dg		cf+dh |	
				sum = sum + (huge_matrixA[(SIZEY*i) + w]*huge_matrixB[(w * SIZEX) + j]);
			}	
			huge_matrixC[(i * SIZEY) + j] = sum;
		}
	}
}

void compare_results()
{
	fout = fopen("./out.in","r");
	ftest = fopen("./reference.in","r");
	long i;
	long temp1, temp2;
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fout, "%ld", &temp1);
		fscanf(ftest, "%ld", &temp2);
		if(temp1!=temp2)
		{
			printf("Wrong solution!\n");
			exit(1);
		}
		else {
			printf("Right solution!\n");
			exit(1);
		}
	}
	fclose(fout);
	fclose(ftest);
}

void write_results()
{
	// Your code here
	//
	// Basically, make sure the result is written on fout
	// Each line represent value in the X-dimension of your matrix
	fout = fopen("./out.in","w");
	char *output;
	output = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	for (int i = 0; i < SIZEY; i++) {
		for (int j = 0; j < SIZEX; j++) {
			sprintf(output, "%ld ", huge_matrixC[i*SIZEX+j]); // read in huge_matrixC then,
			fwrite(output, sizeof(char), strlen(output), fout); // write to out.in
		}
	}
	free(output);
}

void load_matrix()
{
	long i;
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixB = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixC = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	// Load the input
	// Note: This is suboptimal because each of these loads can be done in parallel.
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i)); 		
		fscanf(fin2,"%ld", (huge_matrixB+i)); 		
		huge_matrixC[i] = 0;		
	}
}


// reference : http://csapp.cs.cmu.edu/3e/waside/waside-blocking.pdf
void multiply() {
	long i, j, k, kk, jj;
	long sum = 0;
	long bsize = SIZEX; //less block, more speed // In my case if the bsize less than the SIZEX,
						//the result will wrong

	for (kk = 0; kk < SIZEY; kk += bsize) {
 		for (jj = 0; jj < SIZEX; jj += bsize) {
 			for (i = 0; i < SIZEX; i++) {
 				for (j = 0; j < bsize; j++) {
 					sum = huge_matrixC[(i*SIZEY)+j];
 					for (k = kk; k < bsize; k++) {
 						sum += huge_matrixA[i*SIZEX+k]*huge_matrixB[k*SIZEX+j];	
 					}
 					huge_matrixC[(i*SIZEY)+j] = sum;
 				}
 			}
 		}
 	}
}

int main()
{
	
	clock_t s,t;
	double total_in_base = 0.0;
	double total_in_your = 0.0;
	double total_mul_base = 0.0;
	double total_mul_your = 0.0;
	fin1 = fopen("./input1.in","r");
	fin2 = fopen("./input2.in","r");
	fout = fopen("./out.in","w");
	ftest = fopen("./reference.in","r");
	

	// flush_all_caches();

	s = clock();
	load_matrix_base();
	t = clock();
	total_in_base += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("[Baseline] Total time taken during the load = %f seconds\n", total_in_base);

	s = clock();
	multiply_base();
	t = clock();
	total_mul_base += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("[Baseline] Total time taken during the multiply = %f seconds\n", total_mul_base);
	// write_results();
	fclose(fin1);
	fclose(fin2);
	fclose(fout);
	free_all();
	// compare_results();

	// flush_all_caches();

	fin1 = fopen("./input1.in","r");
	fin2 = fopen("./input2.in","r");
	fout = fopen("./out.in","w");
	ftest = fopen("./reference.in","r");
	

	s = clock();
	load_matrix();
	t = clock();
	total_in_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the load = %f seconds\n", total_in_your);

	s = clock();
	multiply();
	t = clock();
	total_mul_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the multiply = %f seconds\n", total_mul_your);
	write_results();
	fclose(fin1);
	fclose(fin2);
	fclose(fout);
	flush_all_caches();
	free_all();
	compare_results();

	return 0;
}
