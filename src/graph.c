#include "graph.h"

enum dir_t dir_mat(unsigned int i, unsigned int j, int width){
    int row_i = i / width;
    int col_i = i % width;
    int row_j = j / width;
    int col_j = j % width;

    if ((row_i - 1 == row_j) && (col_i == col_j)) return DIR_NORTH;
    if ((row_i - 1 == row_j) && (col_i + 1 == col_j)) return DIR_NE;
    if ((row_i == row_j) && (col_i + 1 == col_j)) return DIR_EAST;
    if ((row_i + 1 == row_j) && (col_i + 1 == col_j)) return DIR_SE;
    if ((row_i + 1 == row_j) && (col_i == col_j)) return DIR_SOUTH;
    if ((row_i + 1 == row_j) && (col_i - 1 == col_j)) return DIR_SW;
    if ((row_i == row_j) && (col_i - 1 == col_j)) return DIR_WEST;
    if ((row_i - 1 == row_j) && (col_i - 1 == col_j)) return DIR_NW;
    return NO_DIR;
}

enum dir_t dir_mat_grid(unsigned int i, unsigned int j, int width){
    int row_i = i / width;
    int col_i = i % width;
    int row_j = j / width;
    int col_j = j % width;

    if ((row_i - 1 == row_j) && (col_i == col_j)) return DIR_NORTH;
    if ((row_i + 1 == row_j) && (col_i == col_j)) return DIR_SOUTH;
    if ((row_i == row_j) && (col_i + 1 == col_j)) return DIR_EAST;
    if ((row_i == row_j) && (col_i - 1 == col_j)) return DIR_WEST;
    return NO_DIR;
}

struct graph_t * grid(unsigned int m){
    struct graph_t * plt = (struct graph_t *)malloc(sizeof(struct graph_t));
    plt->num_vertices = m * m;
    plt->t = gsl_spmatrix_uint_alloc(plt->num_vertices, plt->num_vertices);
    if (plt->t != NULL){
        for (unsigned int i =0; i < plt->num_vertices; i++){
            for (unsigned int j =0; j < plt->num_vertices; j++){
                if (dir_mat_grid(i, j, m) != 0){
                    gsl_spmatrix_uint_set(plt->t, i, j, dir_mat_grid(i, j, m));
                }
            }
        }
        gsl_spmatrix_uint *compressed_t = gsl_spmatrix_uint_compress(plt->t, GSL_SPMATRIX_CSR);
        gsl_spmatrix_uint_free(plt->t);
        plt->t = compressed_t;
    }
    return plt;
}

struct graph_t * square(unsigned int m){
    struct graph_t * plt = (struct graph_t *)malloc(sizeof(struct graph_t));
    plt->num_vertices = m * m;
    plt->t = gsl_spmatrix_uint_alloc(plt->num_vertices, plt->num_vertices);
    if (plt->t != NULL){
        for (unsigned int i =0; i < plt->num_vertices; i++){
            for (unsigned int j =0; j < plt->num_vertices; j++){
                if (dir_mat(i, j, m) != 0){
                    gsl_spmatrix_uint_set(plt->t, i, j, dir_mat(i, j, m));
                }
            }
        }
        gsl_spmatrix_uint *compressed_t = gsl_spmatrix_uint_compress(plt->t, GSL_SPMATRIX_CSR);
        gsl_spmatrix_uint_free(plt->t);
        plt->t = compressed_t;
    }
    return plt;
}

int is_empty_line(struct graph_t * world, unsigned int i){
    for (unsigned int j = 0; j < world->num_vertices; j++){
        if( gsl_spmatrix_uint_get(world->t, i, j)  >= FIRST_DIR) return 1;
    }
    return 0;  
}

int is_in_array(unsigned int j, unsigned int * array, unsigned int len){
    for (unsigned int i = 0; i < len; i++){
        if (array[i] == j) return 1;
    }
    return 0;
}

struct graph_t * donut(unsigned int m){
    struct graph_t * square = (struct graph_t *)malloc(sizeof(struct graph_t));
    square->num_vertices = m*m;
    square->t = gsl_spmatrix_uint_alloc(square->num_vertices, square->num_vertices);
    struct graph_t * plt = (struct graph_t *)malloc(sizeof(struct graph_t));
    plt->num_vertices = ( 8 * m * m ) / 9;
    plt->t = gsl_spmatrix_uint_alloc(plt->num_vertices, plt->num_vertices); 
    if(square->t != NULL){
        for (unsigned int i =0; i < square->num_vertices; i++){
            for (unsigned int j =0; j < square->num_vertices; j++){
                if (dir_mat(i, j, m) != 0){
                    gsl_spmatrix_uint_set(square->t, i, j, dir_mat(i, j, m));
                }
            }
        }
        unsigned int width = sqrt(square->num_vertices);
        unsigned int *disable_vertice = malloc((square->num_vertices - plt->num_vertices) * sizeof(unsigned int));
        int disable_idx = 0;
        for (unsigned int i =0; i < square->num_vertices; i++){
            unsigned int row = i / width;
            unsigned int col = i % width;
            if (( (width/3) <= col) && (col < (2*width/3)) && ((width/3) <= row) && (row < (2*width/3))){
                for(unsigned int j = 0; j < square->num_vertices; j++){
                    gsl_spmatrix_uint_set(square->t, i, j, 0);
                    gsl_spmatrix_uint_set(square->t, j, i, 0);
                }
                disable_vertice[disable_idx] = i;
                disable_idx++;
            }
        }
        unsigned int idx = 0;
        for(unsigned int i = 0; i < square->num_vertices; i++){
            unsigned int col_idx = 0;
            if ( is_empty_line(square, i)){
                for (unsigned int j =0; j < square->num_vertices; j++){
                    if (!is_in_array(j, disable_vertice, disable_idx)){
                        int coeff = gsl_spmatrix_uint_get(square->t, i, j);
                        if ( coeff >= FIRST_DIR){
                            gsl_spmatrix_uint_set(plt->t, idx, col_idx, coeff);
                        }
                        col_idx++;
                    }
                }
                idx++;
            }  
        }
        gsl_spmatrix_uint *compressed_t = gsl_spmatrix_uint_compress(plt->t, GSL_SPMATRIX_CSR);
        gsl_spmatrix_uint_free(plt->t);
        plt->t = compressed_t;
        free_graph(square);
        free(disable_vertice);
        return plt;
    }
    return NULL;
}   

void free_graph(struct graph_t *plt) {
    gsl_spmatrix_uint_free(plt->t);
    free(plt);
}

struct graph_t * world_init(char board_shape, int m)
{
    if (m < 5){
        printf("Error: board must be at least 5x5\n");
        return NULL;
    }
    switch (board_shape)
    {
    case 'g':
        return grid(m);
        break;
    case 'c':
        return square(m);
        break;        
    case 'd':
        if (m % 3 != 0){
            printf("Error, m needs to be equal to 0 modulo 3\n");
            return NULL;
        } else return donut(m);
        break;
    default:
        printf("Error: board must be 'g' 'c', 'd', 't' or '8'\n");
        return NULL;
        break;
    }
}