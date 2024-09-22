#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"



    /*convert python matrix into c*/
double **parse_py_to_matrix_c(PyObject *matrix, int rows, int cols)
{
    double **c_matrix= to_table(rows,cols);
    int i, j;
    for(i=0; i <rows ; i++)
    {
        for(j=0; j < cols; j++)
        {
            PyObject *tmp=PyList_GetItem(matrix, i);
           c_matrix[i][j]= PyFloat_AsDouble(PyList_GetItem(tmp, j));
           /*printf("c_matrix[%d][%d] = %f\n", i, j, c_matrix[i][j]);*/

        }
    }
    return c_matrix;
   
}

/*convert c matrix into python*/
PyObject *parse_matrix_to_py(double **matrix, int rows, int cols)
{
    PyObject *list = PyList_New(rows);
    int i, j;
    for(i=0; i <rows ; i++)
    {
        PyObject *point = PyList_New(cols);
        for(j=0; j < cols; j++)
        {
            PyList_SetItem(point, j, PyFloat_FromDouble(matrix[i][j]));
            /*printf("Returning matrix[%d][%d] = %f\n", i, j, matrix[i][j]);*/

        }
        PyList_SetItem(list, i, point);
    }
    return list;
}

/*convert regular matrix into similarity matrix*/ 
static PyObject *sym(PyObject *self, PyObject *args)
{
    PyObject *py_matrix;
    double **c_matrix;
    double **sym_matrix;

    if(!PyArg_ParseTuple(args, "O!" , &PyList_Type, &py_matrix))
    {
        return NULL;

    }
    if(!PyList_Check(py_matrix))
    {
        return NULL;

    }
    if(!PyList_Check(PyList_GetItem(py_matrix, 0)))
    {
        return NULL;

    }
    int rows = (int)PyList_Size(py_matrix);
    int cols = (int)PyList_Size(PyList_GetItem(py_matrix, 0));
    c_matrix= parse_py_to_matrix_c(py_matrix, rows, cols);
    sym_matrix= to_similarity_matrix(c_matrix, rows, cols);
    PyObject *matrix= parse_matrix_to_py(sym_matrix, rows, rows);
    free_matrix(c_matrix);
    free_matrix(sym_matrix);
    
    
    return matrix;

}
/*convert regular matrix into ddg matrix*/ 
static PyObject *ddg(PyObject *self, PyObject *args)
{
    PyObject *py_matrix;
    double **c_matrix;
    double **ddg_matrix;

    if(!PyArg_ParseTuple(args, "O!" , &PyList_Type, &py_matrix))
    {
        return NULL;

    }
    if(!PyList_Check(py_matrix))
    {
        return NULL;

    }
    if(!PyList_Check(PyList_GetItem(py_matrix, 0)))
    {
        return NULL;

    }
    int rows = (int)PyList_Size(py_matrix);
    int cols = (int)PyList_Size(PyList_GetItem(py_matrix, 0));
    c_matrix= parse_py_to_matrix_c(py_matrix, rows, cols);
    ddg_matrix= to_ddg_matrix(c_matrix, rows, cols);
    PyObject *matrix= parse_matrix_to_py(ddg_matrix, rows, rows);
    free_matrix(c_matrix);
    free_matrix(ddg_matrix);
    
    return matrix;

}
/*convert regular matrix into norm matrix*/ 
static PyObject *norm(PyObject *self, PyObject *args)
{
    PyObject *py_matrix;
    double **c_matrix;
    double **norm_matrix;

    if(!PyArg_ParseTuple(args, "O!" , &PyList_Type, &py_matrix))
    {
        return NULL;

    }
    if(!PyList_Check(py_matrix))
    {
        return NULL;

    }
    if(!PyList_Check(PyList_GetItem(py_matrix, 0)))
    {
        return NULL;

    }
    int rows = (int)PyList_Size(py_matrix);
    int cols = (int)PyList_Size(PyList_GetItem(py_matrix, 0));
    c_matrix= parse_py_to_matrix_c(py_matrix, rows, cols);
    norm_matrix= to_norm_matrix(c_matrix, rows, cols);
    PyObject *matrix= parse_matrix_to_py(norm_matrix, rows, rows);
    free_matrix(c_matrix);
    free_matrix(norm_matrix);
    
    return matrix;

}
/*preform symnmf*/ 
static PyObject *symnmf(PyObject *self, PyObject *args)
{
    PyObject *h, *w;
    double **symnmf_matrix;
    double **c_w_matrix;
    double **c_h_matrix;
    int rows;
    int cols;

    if(!PyArg_ParseTuple(args, "O!O!" , &PyList_Type, &h , &PyList_Type, &w))
    {
        return NULL;

    }
    if(!PyList_Check(h) || !PyList_Check(w))
    {
        return NULL;

    }
    if(!PyList_Check(PyList_GetItem(h, 0)) || !PyList_Check(PyList_GetItem(w, 0)))
    {
        return NULL;

    }
    rows = (int)PyList_Size(h);
    cols = (int)PyList_Size(PyList_GetItem(h, 0));
    c_h_matrix= parse_py_to_matrix_c(h, rows, cols);
    if (c_h_matrix == NULL) {
        return PyErr_NoMemory();
    }
    c_w_matrix= parse_py_to_matrix_c(w, rows, rows);
    if (c_w_matrix == NULL) {
          free_matrix(c_h_matrix); 
        return PyErr_NoMemory();
    }
    symnmf_matrix= optimize_h(c_h_matrix, c_w_matrix, rows, cols);
    if (symnmf_matrix == NULL) {
        free_matrix(c_h_matrix);
        free_matrix(c_w_matrix);  
        return PyErr_NoMemory();
    }
    PyObject *matrix= parse_matrix_to_py(symnmf_matrix, rows, cols);
    if (matrix == NULL) {

        free_matrix(c_h_matrix);
        free_matrix(c_w_matrix);
        free_matrix(symnmf_matrix);  
        return PyErr_NoMemory();
    }
    free_matrix(c_w_matrix);
    free_matrix(c_h_matrix);
    free_matrix(symnmf_matrix);
    return matrix;
    
    

}


static PyMethodDef symnmfMethods[] = {
    {"sym", (PyCFunction)sym, METH_VARARGS, PyDoc_STR("Perform sym algorithm")},
    {"ddg", (PyCFunction)ddg, METH_VARARGS, PyDoc_STR("Perform ddg algorithm")},
    {"norm", (PyCFunction)norm, METH_VARARGS, PyDoc_STR("Perform norm algorithm")},
    {"symnmf", (PyCFunction)symnmf, METH_VARARGS, PyDoc_STR("Perform symnmf algorithm")},
    {NULL, NULL, 0, NULL}
    };

    static struct PyModuleDef symnmfmodule = {
        PyModuleDef_HEAD_INIT,
        "mysymnmf",
        NULL,
        -1,
        symnmfMethods
        };

PyMODINIT_FUNC PyInit_mysymnmf(void)
{
    PyObject *m;
    m= PyModule_Create(&symnmfmodule);
    if(!m)
    {
        return NULL;
    }
    return m;
}