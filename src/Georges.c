#include "move.h"
#include "graph.h"
#include "player.h"
#include "dir.h"
#include "game.h"
#include <math.h>
#include <limits.h>

struct player_data_t * player_data;

// Returns a random queen to move
unsigned int select_random_queen(){
  unsigned int queen_src = player_data->queens_idx[player_data->player_id][rand() % player_data->num_queens];
  while (is_blocked(queen_src, player_data->graph, player_data->queens)){
    queen_src = player_data->queens_idx[player_data->player_id][rand() % player_data->num_queens];
  }
  return queen_src;
}

// Returns the euclidian distance between the idx and the target
float heuristic(unsigned int idx, float target_col, float target_row){
  unsigned int width = (unsigned int) player_data->width;
  float col = (float) (idx % width);
  float row = (float) (idx / width);
  col = col - target_col;
  row = row - target_row;
  return sqrt(col * col + row * row);
}

// Returns the idx where the queen will shoot
unsigned int shoot_arrow_corner(struct set_t * list){
  unsigned int width = (unsigned int) player_data->width;
  unsigned int idx = list->idx[0];
  float max = heuristic(idx, (float) (width - 1) / 2, (float) (width - 1) / 2);
  for (unsigned int i = 1; i < list->len; i++){
    unsigned int new_idx = list->idx[i];
    float new_heuristic = heuristic(new_idx, (float) (width - 1) / 2, (float) (width - 1) / 2);
    if (new_heuristic > max){
      max = new_heuristic;
      idx = new_idx;
    }
  }
return idx;
}

// Returns a position close to the middle of the board
unsigned int mid_move(struct set_t * set){
  unsigned int width = (unsigned int) player_data->width;
  unsigned int idx = set->idx[0];
  float min = heuristic(idx, (float) (width - 1) / 2, (float) (width - 1) / 2);
  for (unsigned int i = 1; i < set->len; i++){
    unsigned int new_idx = set->idx[i];
    float new_heuristic = heuristic(new_idx, (float) (width - 1) / 2, (float) (width - 1) / 2);
    if (new_heuristic < min){
      min = new_heuristic;
      idx = new_idx;
    }
  }
return idx;
}

char const* get_player_name(){
  return "Georges";
}

void initialize(unsigned int player_id, struct graph_t* graph,unsigned int num_queens, unsigned int* queens_idx[NUM_PLAYERS]){
  player_data = malloc(sizeof(struct player_data_t));
  player_data->graph = graph;
  player_data->player_id = player_id;
  player_data->num_queens = num_queens;
  player_data->graph->num_vertices = graph->num_vertices;
  player_data->graph->t = graph->t;
  player_data->width = (int) sqrt(graph->num_vertices);
  for (int i = 0; i < NUM_PLAYERS; i++) {
    player_data->queens_idx[i] = queens_idx[i];
  }
  // Creation of the long queen array: 0 everywhere except for the queens for each player
  unsigned int* queens[NUM_PLAYERS];
  for (int i = 0; i < NUM_PLAYERS; i++){
    queens[i] = malloc(sizeof(unsigned int) * graph->num_vertices);
    for (unsigned int j = 0; j < graph->num_vertices; j++){
      queens[i][j] = 0;
    }
    for (unsigned int j = 0; j < num_queens; j++){
      if (queens_idx[i][j] != 0) queens[i][queens_idx[i][j]] = 1;
    }
    player_data->queens[i] = queens[i];
  }
}

struct move_t play(struct move_t previous_move) {
  // Update graph and queens of the player with the opponent's move
  update_graph_and_queens(next_player(player_data->player_id), player_data->graph, player_data->queens, player_data->queens_idx, player_data->num_queens, previous_move);

  // Strategy: choose a random queen and move it to 1 in one of the 8 directions. Then shoot an arrow from the initial position of the queen.
  struct move_t move = {};
  // Chose move.queen_src
  move.queen_src = select_random_queen();
  // Chose move.queen_dst
 struct set_t* list_move;
  list_move = malloc(sizeof(struct set_t));
  list_move->idx = malloc(sizeof(unsigned int) * player_data->graph->num_vertices);
  list_move->len = 0;
  list_accessible(move.queen_src, list_move, player_data->graph, player_data->queens);
  // Handle the case src queen is blocked
  while (list_move->len == 0){
    move.queen_src = select_random_queen();
    list_accessible(move.queen_src, list_move, player_data->graph, player_data->queens);
  }
  move.queen_dst = mid_move(list_move);
  // Chose move.arrow_dst
  list_move->len = 0;
  player_data->queens[player_data->player_id][move.queen_src] = 0;
  player_data->queens[player_data->player_id][move.queen_dst] = 1;
  list_accessible(move.queen_dst, list_move, player_data->graph, player_data->queens);
  player_data->queens[player_data->player_id][move.queen_src] = 1;
  player_data->queens[player_data->player_id][move.queen_dst] = 0;
  move.arrow_dst = shoot_arrow_corner(list_move);
  // Free memory
  free(list_move->idx);
  free(list_move);

  // Update graph and queens of the player with the chosen move
  update_graph_and_queens(player_data->player_id, player_data->graph, player_data->queens, player_data->queens_idx, player_data->num_queens, move);
  // Send the chosen move to the server
  return move;
}

void finalize(){
  gsl_spmatrix_uint_free(player_data->graph->t);
  free(player_data->graph);
  for (int i = 0; i < NUM_PLAYERS; i++) {
    free(player_data->queens_idx[i]);
    free(player_data->queens[i]);
  }
  free(player_data);
}