#ifndef __DYNAMIC_LOAD_H__
#define __DYNAMIC_LOAD_H__

#include "player.h"

// Struct to contains all the function pointers for one player
struct player_t{
    void * lib;
    char const* (*get_player_name)(void);
    void (*initialize)(unsigned int player_id, struct graph_t* graph, unsigned int num_queens, unsigned int** queens);
    struct move_t (*play)(struct move_t);
    void (*finalize)(void);
};

// Struct to contains all the player_t struct
struct players_t{
    struct player_t player[NUM_PLAYERS];
};

// DL library
void * dl_client(const char * lib);

// DL symbole
void * dl_symbole(const void * lib, char * sym);

// Load into the struct all the function pointers
struct player_t dl_player(char * lib);

// Initialize the queens array
void init_queens(int width, unsigned int * queens[NUM_PLAYERS], unsigned int * queens_idx[NUM_PLAYERS], int num_queens);

// Send a copy of the board  and the queens array to the client
void send_client_copy(struct graph_t* world, unsigned int * queens_idx[NUM_PLAYERS], int num_queens, struct players_t players);

// Check if the move played is legit
int is_arrow_invalid(int current_player, struct move_t move, struct graph_t *world, unsigned int *queens[NUM_PLAYERS]);
int is_queen_invalid(int current_player, struct move_t move, struct graph_t *world, unsigned int *queens[NUM_PLAYERS]);
int is_invalid(int current_player, struct move_t move, struct graph_t *world, unsigned int *queens[NUM_PLAYERS]);

// Check if there is a winner (1 if the current player is blocked, 0 otherwise)
int is_winning(struct graph_t * world, int current_player, unsigned int * queens[NUM_PLAYERS], unsigned int * queens_idx[NUM_PLAYERS], int num_queen);

// Print the board
void print_board(int width, struct graph_t * world, unsigned int * queen[NUM_PLAYERS]);

// Print adjacence matrix
void print_adj(int width, struct graph_t * world);

#endif