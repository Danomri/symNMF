#ifndef SYM_NMF_H
#define SYM_NMF_H

void free_matrix(double** matrix);
double **to_table(int rows, int cols);
double **to_similarity_matrix(double **matrix, int rows, int cols);
double **to_ddg_matrix(double **matrix, int rows, int cols);
double **to_norm_matrix(double **matrix, int rows, int cols);
double **optimize_h(double **h, double **w, int rows, int cols);
void print_matrix(double **matrix, int rows, int cols);

#endif 
