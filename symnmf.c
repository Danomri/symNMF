#include "symnmf.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <assert.h> 
#include <math.h>

#define DELIMETER ","
#define BETHA 0.5
#define EPSILON 0.0001
#define MAX_ITER 300
/* EPSILON 1e-4=0.0001*/ 


/*freeing matrix*/
void free_matrix(double** matrix) {
    
       free(*matrix);
    free(matrix);
       
    
}


/*find num of rows */
int num_rows(const char *filename) {
    int rows = 0;
    char line[100];

    FILE *file = fopen(filename, "r");

    while (fscanf(file, "%s", line)!=EOF) {
        rows++;
    }
    rewind(file);
    fclose(file);
    return rows;
}

 /*find num of cols */
int num_cols(char *line)
{
    int i;
    int num_commas = 0;
    int len = strlen(line);

    for (i = 0; i < len; i++)
    {
        if (line[i] == ',')
        {
            num_commas++;
        }
    }
    return num_commas + 1;
}

/*allocate empty template matrix */
double **to_table(int rows, int cols) 
{
    double **matrix;
    int i;
    double *data;
    matrix = (double **)malloc(rows * sizeof(double *));
    data = (double *)calloc(rows * cols, sizeof(double));
    if (matrix == NULL || data == NULL) {
        fprintf(stderr, "memory allocation failed - to_table\n");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < rows; i++) {
        matrix[i] = data + i * cols;
    }
    return matrix;
}


/*make regular matrix from file*/ 
double **to_reg_matrix( const char *file_name, int rows, int cols){
    FILE *file=fopen(file_name, "r");
    double **matrix;
    char *sval, line[2000];
    double dval;
    int i=0, j=0;
    if (file == NULL) {
        fprintf(stderr, "can't opening file: %s\n", file_name);
        exit(EXIT_FAILURE);
    }
    matrix= to_table(rows, cols);
    assert(matrix != NULL);
    while(fscanf(file, "%s", line) !=EOF)
    {
        sval= strtok(line, DELIMETER);
        while(sval != NULL){
            dval=atof(sval);
            matrix[i][j]= dval;
            j++;
            sval=strtok(NULL, DELIMETER);
        }
        j=0;
        i++;
    }
    rewind(file);
    fclose(file);
    return matrix;

}


/*calculate the norm*/ 
double calc_norma(double *point1, double *point2,  int cols)
{
    
    double res = 0.0;
    int i;
        for (i = 0; i < cols; i++)
    {
        double dis=(point1[i] - point2[i]);
        res += dis*dis;
    }
    return res;
    
}


/*convert regular matrix into similarity matrix*/ 
 double **to_similarity_matrix(double **matrix, int rows, int cols){
    int i;
    double **similarity_matrix = to_table(rows, rows);
    for (i = 0; i < rows; i++) {
        int j;
        for (j = 0; j < rows; j++) {
            similarity_matrix[i][j] = i == j ? 0.0 : exp(-1*((calc_norma(matrix[i], matrix[j], cols)) / 2.0));
        }
    }
    /*print_matrix(similarity_matrix, rows, rows);*/ 
    return similarity_matrix;
}

/*summing all points in line*/
double sum_of_coordinates_in_line(double **matrix, int line, int line_length){
    int i;
    double sum_of_line=0;
    for(i=0; i<line_length; i++){
        sum_of_line+= matrix[line][i];
    }
    return sum_of_line;
}

/*preforming diagonal degree matrix algorithm*/
double **to_ddg_matrix(double **matrix, int rows, int cols){
    int i, j;
    double **sym_matrix= to_similarity_matrix(matrix, rows, cols);
    double **degree_matrix= to_table(rows,rows); /*nxn matrix*/
    /*put each sum of the lines in his place at the diagonal of the matrix*/ 
    for (i = 0; i < rows; i++) {
        for(j = 0; j < rows; j++){
            if(i!=j){
                degree_matrix[i][j]=0;
            }
            else{
                degree_matrix[i][j]= sum_of_coordinates_in_line(sym_matrix, i, rows);
            }   
        }
    }
    free_matrix(sym_matrix);
    return degree_matrix;
}

/*finding the invertible matrix*/
double **invers_sqrt_diag(double **diag, int rows, int cols){
    int i;
    int j;
    double **invers_sqrt= to_table(rows, cols);
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
                invers_sqrt[i][j] = (i == j) ? (1.0 / sqrt(diag[i][j])) : 0.0;
        }
    }
   return invers_sqrt;
}

/*calculate multiply of 2 matrix (A*B) */
double **multiply_matrix(double **a,  int a_rows, int a_cols, double **b, int b_cols) {
    int i, j, k;
    double **multiply_res;
    multiply_res = to_table(a_rows, b_cols); /*A->nxn , B->nxk, so A*B-> nxk*/
    /*(A rows)*(B cols)*/
    for (i = 0; i < a_rows; i++) {
        for (j = 0; j < b_cols; j++) {
            multiply_res[i][j] = 0;
            /*sum the multiply of each element that we ;ut in multiply res matrix*/
            for (k = 0; k < a_cols; k++) {
                multiply_res[i][j] += a[i][k] *b[k][j];
            }
        }
    }
    return multiply_res;
}

/*preforming norm matrix*/
double **to_norm_matrix(double **matrix, int rows, int cols){
    double **a;
    double **diag;
    double **inv_sqrt_d;
    double **mult_matrix;
    double **norm_matrix;

    a = to_similarity_matrix(matrix, rows, cols);
    if (!a) {
    printf("Handle error: failed to allocate a");
}
    diag= to_ddg_matrix(matrix, rows, cols);
    inv_sqrt_d = invers_sqrt_diag(diag, rows, rows);
    /*D^(-1/2)*A*/
    mult_matrix=multiply_matrix(inv_sqrt_d, rows, rows, a, rows);
    /*D^(-1/2)*A*D^(-1/2)*/
    norm_matrix = multiply_matrix(mult_matrix, rows, rows, inv_sqrt_d, rows);

    free_matrix(a);
    free_matrix(inv_sqrt_d);
    free_matrix(diag);
    free_matrix(mult_matrix);
       
    
    
    
    return norm_matrix;
}
/*doing transpose to matrix*/
double **transpose(double **h, int rows, int cols) {
    /*h->nxk, so trans_h->kxn*/
    double **trans_h = to_table(cols, rows);
    int i,j;
    /*flipping the order such that h[i][j] to h[j][i]*/
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            trans_h[j][i] = h[i][j];
        }
    }
    
    return trans_h;
}
/*TO DO- marge both function update_h and optimize_h together, I thoght to do here only one update each time */
double **update_h(double **optimized_h, double **w, int rows, int cols) {
    double **updated_h = to_table(rows, cols);
    double **w_h;
    double old_val;
    double update_factor;
    double dist;
    int i;
    int j;
    int iter;
    
    double sum;
    dist= EPSILON;
    old_val=0.0;
    iter=0;
    fflush(stdout);
    /*update h using above previous calculations and check convergence*/ 
    while(dist>=EPSILON && iter<MAX_ITER){
        double **h_transpose;
        double **h_ht;
        double **h_ht_h;
        
        /* gettin the variables to calculat (w*h)/(h*h^t*h)*/
        /* calculate h^t dim= kxn*/ 
        h_transpose= transpose(optimized_h, rows, cols);
        /*calculate h*htrans dim=nxk */ 
        h_ht= multiply_matrix(optimized_h, rows, cols, h_transpose, rows);
        /*calculate h* h^t * h,    dim(h*h^t)= nxn ----> dim(h* h^t *h)= nxk */ 
        h_ht_h =multiply_matrix(h_ht, rows, rows, optimized_h, cols);
        /*calculate w*h dim=nxn*/
        w_h= multiply_matrix(w, rows, rows, optimized_h, cols);
         
        sum=0.0;
        for (i = 0; i < rows; i++) {
            for (j = 0; j <cols; j++) {
                old_val= optimized_h[i][j]; /* prev h val forc checking convergence */
                update_factor= ((1 - BETHA) + (BETHA * (w_h[i][j] / h_ht_h[i][j])));
                optimized_h[i][j] = optimized_h[i][j] * update_factor;
                sum += pow((old_val - optimized_h[i][j]),2);
                updated_h[i][j]= optimized_h[i][j];
            }
        }

        dist=sum;
        iter++;


       free_matrix(h_transpose);
       free_matrix(h_ht);
       free_matrix(h_ht_h);
       free_matrix(w_h);
        
        
        
    }
    return updated_h;
}

/*preforming optimization of matrix h*/
    double **optimize_h(double **h, double **w, int rows, int cols){
        double **new_h = to_table(rows, cols);
        new_h = update_h(h, w, rows, cols);
        return new_h;
    }

void print_point(double *point, int size)
{
    int i;
    for (i = 0; i < size - 1; i++)
    {
        printf("%.4f,", point[i]);
    }
    printf("%.4f\n", point[size - 1]); /*last without comma*/
}
void print_matrix(double **matrix, int rows, int cols){
    int i;
    for (i = 0; i < rows; i++)
    {
        print_point(matrix[i], cols);
    }
}


/*----------------main----------------*/
int main(int argc, char **argv)
{
    double **res_matrix;
    double **reg_matrix;
    const char *goal;
    char line[100], *file_name;
    int rows, cols;
    FILE* file;

    assert(argc == 3); /*Assert that the correct number of arguments is provided*/ 
    file_name= argv[2];
    /*get the original matrix*/ 
    goal = argv[1];
    assert(strcmp(goal,"sym") || strcmp(goal,"ddg") || strcmp(goal,"norm")  );
    rows= num_rows(file_name);
    file = fopen(file_name, "r");
     if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", file_name);
        return EXIT_FAILURE;
    }
    fscanf(file, "%s", line);
    rewind(file);
    cols=num_cols(line);
    reg_matrix= to_reg_matrix(file_name, rows , cols);
    if(reg_matrix == NULL){
         fprintf(stderr, "Failed to allocate memory for reg_matrix\n");
         return EXIT_FAILURE;
    }
    
    /*check wich matrix is the desire */
    if(strcmp(goal,"sym") == 0){
       res_matrix = to_similarity_matrix(reg_matrix, rows, cols);
    }
    else if(strcmp(goal,"ddg") == 0){
       res_matrix = to_ddg_matrix(reg_matrix, rows, cols);
    }
    else {
       res_matrix = to_norm_matrix(reg_matrix, rows, cols);
       /*reg_matrix= invers_sqrt_diag(reg_matrix, rows, rows); debug mult*/
    }
    if (res_matrix == NULL) {
        fprintf(stderr, "Failed to generate the result matrix\n");
        return EXIT_FAILURE;
    }
    else{
        
       
        print_matrix(res_matrix, rows, rows);
        free_matrix(res_matrix);
        free_matrix(reg_matrix);

        
    }
    
    return 0;
}
