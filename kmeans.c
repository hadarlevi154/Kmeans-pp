#define PY_SSIZE_T_CLEAN  
#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

static double** k_means(int K, int N, int d, int max_iter, double **data_points, double **init_centroids);
void free_memory_array(double ** arr, int len);
int is_same(double **cents1, double **cents2, int K, int d);
void divide_by_scalar(double *arr, int d, int size_s);
void copy_array_from_to(double **arr_from, double **arr_to, int K, int d);
void sum_by_coordinates(double *arr_sum, double *arr_to_add_to_sum, int d);
void fill_zeros(double **arr, int K, int d);
double distance(double *arr1, double *arr2, int d);
int find_min(double *x_i, double **centroids, int K, int d);


static double** k_means(int K, int N, int d, int max_iter, double **data_points, double **init_centroids) {  /*should we give something else? */
    
    int i,j, j_min;
    int curr_iter;
    int *counts;

    double **centroids_old, **centroids_new, **centroids_res;
    

    /* create arrays for centroids */
    centroids_old = (double**) calloc(K, sizeof(double));
    centroids_new = (double**) calloc(K, sizeof(double));
    centroids_res = (double**) calloc(K, sizeof(double));
    assert(centroids_old != NULL);
    assert(centroids_new != NULL);
    assert(centroids_res != NULL);

    /* create arrays for data points in each centroid */
    for (i = 0 ; i < K ; i++) {
        centroids_old[i] = (double*) calloc(d, sizeof(double));
        centroids_new[i] = (double*) calloc(d, sizeof(double));
        centroids_res[i] = (double*) calloc(d, sizeof(double));
        assert(centroids_old[i] != NULL);
        assert(centroids_new[i] != NULL);
        assert(centroids_res[i] != NULL);
    }

    /* initialize counts array - counts[i] is size of set S_j */
    counts = (int*) calloc(K, sizeof(int));
    assert(counts);

    /* step 1: initialize centroids */
    for (i = 0 ; i < K ; i++) {
        for (j = 0; j < d ; j++) {
            centroids_new[i][j] = init_centroids[i][j];
        }
    }
    curr_iter = 0;

    while (curr_iter <= max_iter && !is_same(centroids_new, centroids_old, K, d)) {

        copy_array_from_to(centroids_new, centroids_old, K, d);
        fill_zeros(centroids_new, K, d);

        /* step 3 */
        for (i = 0 ; i < N ; i++) {

            /* find index j, data_points[i] is closest to the cluster S_j */
            j_min = find_min(data_points[i], centroids_old, K, d);
            sum_by_coordinates(centroids_new[j_min], data_points[i], d);
            counts[j_min] = counts[j_min] + 1;
        }
        /* step 4 */
        for (i = 0 ; i < K ; i++) {
            divide_by_scalar(centroids_new[i], d, counts[i]);
        }

        for (i = 0 ; i < K ; i++) {
            counts[i] = 0;
        }
        curr_iter++;
    }

    /* copy last centroids to res centroids */
    for (i = 0 ; i < K ; i++) {
        for (j = 0; j < d ; j++) {
            centroids_res[i][j] = centroids_new[i][j];
        }
    }

    /* free memory */
    free_memory_array(centroids_new, K);
    free_memory_array(centroids_old, K);
    free(counts);
    return centroids_res;
}


void free_memory_array(double **arr, int len) {
    
    int i;
    for (i = 0; i < len; i++) {
        free(arr[i]);
    }
    free(arr);
}


int is_same(double **cents1, double **cents2, int K, int d) {

    int m,q;

    for (m = 0 ; m < K ; m++) {
        for (q = 0 ; q < d  ; q++) {
            if (cents1[m][q] != cents2[m][q]) {
                return 0;
            }
        }
    }
    return 1;
}


void divide_by_scalar(double *arr, int d, int size_s) {

    int m;

    for (m = 0 ; m < d ; m++) {
        arr[m] = (double)(arr[m] / size_s);
    }
}


void copy_array_from_to(double **arr_from, double **arr_to, int K, int d) {

    int m,q;

    for (m = 0 ; m < K ; m++) {
        for (q = 0 ; q < d  ; q++) {
            arr_to[m][q] = arr_from[m][q];
        }
    }
}

void sum_by_coordinates(double *arr_sum, double *arr_to_add_to_sum, int d) {

    int m;

    for (m = 0 ; m < d ; m++) {
        arr_sum[m] += arr_to_add_to_sum[m];
    }

}

void fill_zeros(double **arr, int K, int d) {

    int m,q;

    for (m = 0 ; m < K ; m++) {
        for (q = 0 ; q < d  ; q++) {
            arr[m][q] = 0;
        }
    }
}

double distance(double *arr1, double *arr2, int d) {

    int m;
    double distance;

    distance = 0;
    for (m = 0 ; m < d ; m++) {
        distance += (double) ((arr1[m] - arr2[m]) * (arr1[m] - arr2[m]));
    }
    return distance;
}

int find_min(double *x_i, double **centroids, int K, int d) {

    int min_index, m;
    double curr_distance, min_distance;

    min_index = 0;
    min_distance = distance(x_i, centroids[0], d);

    for (m = 1 ; m < K ; m++) {

        curr_distance = distance(x_i, centroids[m], d);

        if (curr_distance < min_distance) {
            min_distance = curr_distance;
            min_index = m;
        }
        
    }
    return min_index;
}

static double** py_data_points_to_arr(PyObject *py_data_points, int N, int d) {

    double **data_points;
    int i,j, l;
    PyObject *row;

    l = PyList_Size(py_data_points);
    if(l != N){
        PyErr_SetString(PyExc_NameError,"size does not match");
        return NULL;
    }

    data_points = (double**) calloc(N, sizeof(double));
    assert(data_points != NULL && "Error: problem with memory\n");
    for (i=0 ; i< N ; i++) {
        data_points[i] = (double*) calloc(d, sizeof(double));
        assert(data_points[i] != NULL && "Error: problem with memory\n");
    }
    
    for (i=0 ; i< N ; i++) {
        row = PyList_GetItem(py_data_points, i);
        if (!PyList_Check(row)) {
            PyErr_SetString(PyExc_NameError,"not a list");
                  return NULL;
        }
        for (j=0 ; j < d ; j++) {
            data_points[i][j] = PyFloat_AsDouble(PyList_GetItem(row, j));
        }
    }

    return data_points;
}

static double** py_centroids_to_arr(PyObject *py_centroids, int K, int d) {

    double **centroids;
    int i,j, l;
    PyObject *row;

    l = PyList_Size(py_centroids);
    if(l != K){
        PyErr_SetString(PyExc_NameError,"size does not match");
        return NULL;
    }

    centroids = (double**) calloc(K, sizeof(double));
    assert(centroids != NULL && "Error: problem with memory\n");
    for (i=0 ; i< K ; i++) {
        centroids[i] = (double*) calloc(d, sizeof(double));
        assert(centroids[i] != NULL && "Error: problem with memory\n");
    }
    for (i=0 ; i< K ; i++) {
        row = PyList_GetItem(py_centroids, i);
        if (!PyList_Check(row)) {
            PyErr_SetString(PyExc_NameError,"not a list");
                  return NULL;
        }
        for (j=0 ; j < d ; j++) {
            centroids[i][j] = PyFloat_AsDouble(PyList_GetItem(row, j));
        }
    }

    return centroids;
}


static PyObject* convert_c_arr_to_py_object(double **arr, int K, int d) {

    int i,j;
    PyObject *py_arr, *row, *num;

    py_arr = PyList_New(K);
    for (i=0 ; i< K ; i++) {
        row = PyList_New(d);
        for (j=0 ; j<d ;j++) {
            num = Py_BuildValue("d", arr[i][j]);
            PyList_SetItem(row, j, num);
        }
        PyList_SetItem(py_arr, i, row);
    }

    return py_arr;
}

/*
 * The wrapping function needs a PyObject* self argument.
 * This is a requirement for all functions and methods in the C API.
 * It has input PyObject *args from Python.
 */
static PyObject* fit(PyObject *self, PyObject *args)
{
    int K;
    int N;
    int d;
    int max_iter;
    double **data_points, **init_centroids, **res_centroids;

    PyObject *py_init_centroids, *py_data_points, *py_res_centroids;

    /* This parses the Python arguments*/
    if(!PyArg_ParseTuple(args, "iiiiOO", &K,&N,&d,&max_iter, &py_data_points, &py_init_centroids)) {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

    data_points = py_data_points_to_arr(py_data_points, N, d);
    init_centroids = py_centroids_to_arr(py_init_centroids, K, d);

    /* This retrieves the final centroids after k means */
    res_centroids = k_means(K, N, d, max_iter, data_points, init_centroids);

    py_res_centroids = convert_c_arr_to_py_object(res_centroids, K, d);

    /* free memory */
    free_memory_array(data_points, N);
    free_memory_array(init_centroids, K);
    free_memory_array(res_centroids, K);

    return Py_BuildValue("O", py_res_centroids);
}   


/*
 * This array tells Python what methods this module has.
 * We will use it in the next structure
 */
static PyMethodDef capiMethods[] = {
    {"fit",                   /* the Python method name that will be used */
      (PyCFunction) fit, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("k means algorithm")}, /*  The docstring for the function */
    {NULL, NULL, 0, NULL}     /* The last entry must be all NULL as shown to act as a
                                 sentinel. Python looks for this entry to know that all
                                 of the functions for the module have been defined. */
};

/* This initiates the module using the above definitions. */

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    capiMethods /* the PyMethodDef array from before containing the methods of the extension */
};


/*
 * The PyModuleDef structure, in turn, must be passed to the interpreter in the module’s initialization function.
 * The initialization function must be named PyInit_name(), where name is the name of the module and should match
 * what we wrote in struct PyModuleDef.
 * This should be the only non-static item defined in the module file
 */

PyMODINIT_FUNC
PyInit_mykmeanssp(void) /* what should we write? mykeanssp? */
{
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}