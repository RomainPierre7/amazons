#ifndef __VIEW_H__
#define __VIEW_H__

#include "move.h"

#define COL_NORMAL "\033[00m"
#define COL_RED "\033[31;01m"
#define COL_GREEN "\033[32;01m"
#define COL_YELLOW "\033[33;01m"
#define COL_WHITE "\033[37;01m"
#define COL_BLACK "\033[30;01m"

struct set_t{
  unsigned int * idx;
  unsigned int len;
};

struct move_t;

// Choose the first player
int choose_random_player();

// Select the next player
int next_player(int current_player);

// Play the move on the world
void update_graph_and_queens(int player_id, struct graph_t * world, unsigned int * queens[NUM_PLAYERS], unsigned int * queens_idx[NUM_PLAYERS], int num_queens, struct move_t move);


// break the lines between cells 
void break_link(struct graph_t * world, unsigned int arrow_dst);

//Returns if there is a queen at the position i
int is_queen(unsigned int i, unsigned int * queens[NUM_PLAYERS]);

//Returns if there is a queen of the player at the position i
int is_queen_of(unsigned int i, unsigned int queens[]);

//Fill the list
void list_accessible(unsigned int idx, struct set_t * list, struct graph_t * world, unsigned int * queens[NUM_PLAYERS]);

// Return the neighbor of idx in the direction dir, if there is no neighbor, return -1
unsigned int neighbor_dir(unsigned int idx, enum dir_t dir, gsl_spmatrix_uint* graph);

// Return if a queen is blocked or not
int is_blocked(unsigned int idx,struct graph_t* graph, unsigned int * queens[NUM_PLAYERS]);

// Returns the dir from the source to the destination
enum dir_t get_CSR(struct graph_t * world, unsigned int src, unsigned int dst);

#endif // __VIEW_H__
