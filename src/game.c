#include <stdlib.h>
#include <limits.h>
#include "game.h"

int choose_random_player()
{
    return rand() % NUM_PLAYERS;
}

int next_player(int current_player)
{
    return (current_player + 1) % NUM_PLAYERS;
}

void break_link(struct graph_t *world, unsigned int arrow_dst)
{
    if (arrow_dst != world->num_vertices - 1)
    {
        for (size_t k = world->t->p[arrow_dst]; (int)k < world->t->p[arrow_dst + 1]; k++)
        {
            world->t->data[k] = 0;
        }
        for (size_t k = 0; k < world->t->nz; k++)
            if ((unsigned int)world->t->i[k] == arrow_dst)
            {
                world->t->data[k] = 0;
            }
    }
    else
    {
        for (size_t k = world->t->p[arrow_dst]; k < world->t->nz; k++)
        {
            world->t->data[k] = 0;
        }
        for (size_t k = 0; k < world->t->nz; k++)
            if ((unsigned int)world->t->i[k] == arrow_dst)
            {
                world->t->data[k] = 0;
            }
    }
}

void update_graph_and_queens(int player_id, struct graph_t *world, unsigned int *queens[NUM_PLAYERS], unsigned int * queens_idx[NUM_PLAYERS], int num_queens, struct move_t move)
{
    if (move.queen_src == UINT_MAX || move.queen_dst == UINT_MAX || move.arrow_dst == UINT_MAX) return; //initial move or invalid move
    queens[player_id][move.queen_src] = 0;
    queens[player_id][move.queen_dst] = 1;
    for (int i = 0; i < num_queens; i++)
    {
        if (queens_idx[player_id][i] == move.queen_src)
        {
            queens_idx[player_id][i] = move.queen_dst;
            break;
        }
    }
    break_link(world, move.arrow_dst);
}

int is_queen(unsigned int i, unsigned int *queens[NUM_PLAYERS])
{
    for (int j = 0; j < NUM_PLAYERS; j++)
    {
        if (queens[j][i] == 1)
            return 1;
    }
    return 0;
}

int is_queen_of(unsigned int i, unsigned int queens[])
{
    return queens[i];
}

void list_accessible(unsigned int idx, struct set_t * list, struct graph_t * world, unsigned int * queens[NUM_PLAYERS]){
  list->len = 0;
  for (enum dir_t i = FIRST_DIR; i <= LAST_DIR; i++){
    unsigned int new_idx = neighbor_dir(idx, i, world->t);
    while ((new_idx != UINT_MAX) && (!is_queen(new_idx, queens))){
      list->idx[list->len] = new_idx;
      list->len++;
      new_idx = neighbor_dir(new_idx, i, world->t);
    }
  }
}

unsigned int neighbor_dir(unsigned int idx, enum dir_t dir, gsl_spmatrix_uint* graph) {
    unsigned int col = 0;
    for (int j = graph->p[idx]; j < graph->p[idx + 1]; j++) {
        if (graph->data[j] == dir){
            col = graph->i[j];
            return col;
        } 
    }
    return UINT_MAX;
}

int is_blocked(unsigned int idx,struct graph_t* graph, unsigned int * queens[NUM_PLAYERS]){
struct set_t* list_move;
  list_move = malloc(sizeof(struct set_t));
  list_move->idx = malloc(sizeof(unsigned int) * graph->num_vertices);
  list_move->len = 0;
  list_accessible(idx, list_move, graph, queens);
  if (list_move->len == 0){
    free(list_move->idx);
    free(list_move);
    return 1;
  }
  free(list_move->idx);
  free(list_move);
  return 0;
}

enum dir_t get_CSR(struct graph_t * world, unsigned int src, unsigned int dst){
    for(int j = world->t->p[src]; j < world->t->p[src+1]; j++){
        if ((unsigned int)world->t->i[j] == dst) return world->t->data[j];
    }
    return NO_DIR;
}