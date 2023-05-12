#ifndef _AMAZON_GRAPH_H_
#define _AMAZON_GRAPH_H_

#include <stddef.h>
#include "dir.h"

#include <gsl/gsl_spmatrix.h>
#include <gsl/gsl_spmatrix_uint.h>
#include <gsl/gsl_spblas.h>

struct graph_t * world_init(char board_shape, int m);

struct graph_t {
  unsigned int num_vertices; // Number of vertices in the graph
  gsl_spmatrix_uint* t;      // Sparse matrix of size n*n,
                      // t[i][j] > 0 means there is an edge from i to j
                      // t[i][j] == DIR_NORTH means that j is NORTH of i
                      // t[i][j] == DIR_SOUTH means that j is SOUTH of i
                      // and so on
};

// Returns the directions between two points
enum dir_t dir_mat(unsigned int i, unsigned int j, int width);

// Returns the directions between two points (only 4 directions)
enum dir_t dir_mat_grid(unsigned int i, unsigned int j, int width);

// Initialize a square grid
struct graph_t * square(unsigned int m);

// Returns if the row i has all of its values equal to 0
int is_empty_line(struct graph_t * world, unsigned int i);

// Returns if j is in the array 
int is_in_array(unsigned int j, unsigned int * array, unsigned int len);

// Initialize a donut shaped grid
struct graph_t * donut(unsigned int m);

// Free the graph which represent the world
void free_graph( struct graph_t * plt);

// Initialize the world with the shape asked by the user
struct graph_t * world_init(char board_shape, int m);

#endif // _AMAZON_GRAPH_H_
