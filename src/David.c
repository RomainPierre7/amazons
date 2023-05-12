#include "move.h"
#include "graph.h"
#include "player.h"
#include "dir.h"
#include "game.h"
#include <math.h>
#include <limits.h>

struct player_data_t * player_data;

char const* get_player_name(){
  return "David";
}

struct stock_position {
  unsigned int * index_tab;
  unsigned int * dir_tab;
  int len;
};

struct stock_position * data_queen_src;
struct stock_position * data_queen_dst;


void print_adj_graph(struct player_data_t * player){
    //print adj graph
    int m = sqrt(player->graph->num_vertices);
    for(int i = 0  ; i < m*m; i ++){
        for(int j = 0 ; j < m*m ; j++){
            int a = gsl_spmatrix_uint_get(player->graph->t,i,j);
            printf("%d ",a);
        }
        printf("\n");
    }
}


unsigned int select_random_queen() {
  unsigned int queen_src = player_data->queens_idx[player_data->player_id][rand() % player_data->num_queens];
  while (is_blocked(queen_src, player_data->graph, player_data->queens)) {
    queen_src = player_data->queens_idx[player_data->player_id][rand() % player_data->num_queens];
  }
  return queen_src;
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

void index_accessible(unsigned int queen_index, struct stock_position * stock, struct player_data_t * player) {
  stock->len = 0;
  for(int dir = FIRST_DIR ; dir <= LAST_DIR ; dir ++) {
    unsigned int queen_index_cp = neighbor_dir(queen_index, dir, player->graph->t);
    while( queen_index_cp != UINT_MAX && (!is_queen(queen_index_cp,player->queens))) {
      stock->index_tab[stock->len] = queen_index_cp;
      stock->dir_tab[stock->len] = dir;
      stock->len+= 1;
      queen_index_cp = neighbor_dir(queen_index_cp, dir, player->graph->t);
    }
  }
}

//heuristique function 1
//gives the number of directions the player can go in
int possible_direction_queen(struct player_data_t * player, unsigned int queen_index) {
  struct stock_position * stock = (struct stock_position *) malloc(sizeof(struct stock_position));
  stock->index_tab = malloc(sizeof(unsigned int) * player->graph->num_vertices);
  stock->dir_tab = malloc(sizeof(unsigned int) * player->graph->num_vertices);
  stock->len = 0;
  index_accessible(queen_index, stock, player); 
  int tab[9]={0,0,0,0,0,0,0,0,0};
  int count = 0;
  if ( stock->len == 0){
    free(stock->index_tab);
    free(stock->dir_tab);
    free(stock);
    return 0;
  }
  for(int i = 0 ; i < stock->len ; i ++){
    tab[stock->dir_tab[i]]+=1;
  }
  tab[0]=0;
  for(int i = 0 ; i < 9; i ++){
    if(tab[i]!=0){
        count+=1;
    }
  }
  free(stock->index_tab);
  free(stock->dir_tab);
  free(stock);
  return count;
}


//heuristic 2 
int total_simple_mouv_possible( struct player_data_t * player, unsigned int queen_index ) {
  //heuristic to know how many simple moves a player is able to make 
  //The more moves a player is able to make, the safer he is
  struct stock_position * stock = (struct stock_position *) malloc(sizeof(struct stock_position));
  stock->index_tab = malloc(sizeof(unsigned int) * player->graph->num_vertices);
  stock->dir_tab = malloc(sizeof(unsigned int) * player->graph->num_vertices);
  stock->len = 0;
  index_accessible(queen_index, stock, player); 
  int len = stock->len;
  free(stock->index_tab);
  free(stock->dir_tab);
  free(stock);
  return len; 
}


unsigned int point_euclidian_distance( struct graph_t * graph, unsigned int index1, unsigned int index2) {
  unsigned int width = sqrt(graph->num_vertices);
  unsigned int x1 = index1/width;
  unsigned int y1 = index1 - x1;
  unsigned int x2 = index2/width;
  unsigned int y2 = index2 - x2;
  unsigned int distance_euclidienne = sqrt(((x2 - x1)*(x2 - x1)) + ((y2 - y1)*(y2 - y1)));
  return distance_euclidienne;
}

//heuristic 3
//calculates the distance between the drawn arrow and all the opponent queens
//returns the opposite of the minimum distance
int negative_min_distance_queen_arrow(struct player_data_t * player, unsigned int arrow_index) {
  unsigned int num_vertices = player->graph->num_vertices;
  unsigned int j = 0;
  int opposing_queen[player->num_queens];
  for(int i = 0 ; i < (int) player->num_queens ; i++){
    opposing_queen[i] = 0;
  }
  for(int i = 0 ; i < (int) num_vertices ; i ++ ) {
    if(player->queens[next_player(player->player_id)][i] == 1) {
      opposing_queen[j] = point_euclidian_distance(player->graph,arrow_index,i);
      j += 1;
    }
  }
  unsigned int min = opposing_queen[0];
  for(int k = 0 ; k < (int) player->num_queens ; k++){
    if(opposing_queen[k] < (int)min){
      min = opposing_queen[k];
    }
  }
  return -min;
}

int heuristic_score( struct player_data_t * player, unsigned int queen_index, unsigned int arrow_index, int is_arrow_now ) {
  if( is_arrow_now == 0 ) {
    return possible_direction_queen(player, queen_index) + total_simple_mouv_possible(player, queen_index);
  }
  return negative_min_distance_queen_arrow(player, arrow_index);
}

//selects the queen with the highest score 
int queen_dst_heuristic_max_selection(unsigned int queen_index, struct stock_position * stock, struct player_data_t * player) {
  int max = -10;
  unsigned int len = stock->len;
  for(int i = 0 ; i < (int) len ; i ++) {
    if( heuristic_score(player,stock->index_tab[i],0,0) >= max) {
      max = heuristic_score(player,stock->index_tab[i],0,0);
      queen_index = stock->index_tab[i];
    }
  } 
  return queen_index;
}

//selects the arrow with the highest score 
int arrow_dst_heuristic_max_selection(unsigned int queen_index, struct stock_position * stock, struct player_data_t * player) {
  int max = negative_min_distance_queen_arrow(player,stock->index_tab[0]);
  unsigned int len = stock->len;
  for(int i = 0 ; i < (int) len ; i ++) {
    if(negative_min_distance_queen_arrow(player,stock->index_tab[i]) >= max) {
      max = negative_min_distance_queen_arrow(player,stock->index_tab[i]);
      queen_index = stock->index_tab[i];
    }
  } 
  return queen_index;
}

struct move_t play(struct move_t previous_move) {
  // Update graph and queens of the player with the opponent's move
  update_graph_and_queens(next_player(player_data->player_id), player_data->graph, player_data->queens, player_data->queens_idx, player_data->num_queens, previous_move);
  // Strategy: choose a random queen and move it to 1 in one of the 8 directions. Then shoot an arrow from the initial position of the queen.
  struct move_t move = {};
  // Chose move.queen_src
  move.queen_src = select_random_queen();
  // Chose move.queen_dst
  data_queen_src = malloc(sizeof(struct stock_position));
  data_queen_src->index_tab = malloc(sizeof(unsigned int) * player_data->graph->num_vertices);
  data_queen_src->dir_tab = malloc(sizeof(unsigned int) * player_data->graph->num_vertices);
  data_queen_src->len = 0;
  data_queen_dst = malloc(sizeof(struct stock_position));
  data_queen_dst->index_tab = calloc(player_data->graph->num_vertices,sizeof(unsigned int));
  data_queen_dst->dir_tab = calloc(player_data->graph->num_vertices,sizeof(unsigned int));
  data_queen_dst->len = 0;
  index_accessible(move.queen_src, data_queen_src, player_data);
  // Handle the case src queen is blocked
  while (data_queen_src->len == 0){
    move.queen_src = select_random_queen();
    index_accessible(move.queen_src, data_queen_src, player_data);
  }
  //Chose move.queen_dst
  move.queen_dst = queen_dst_heuristic_max_selection(move.queen_src, data_queen_src,player_data);
  //Chose move.arrow.dst
  player_data->queens[player_data->player_id][move.queen_src] = 0;
  player_data->queens[player_data->player_id][move.queen_dst] = 1;
  index_accessible(move.queen_dst, data_queen_dst, player_data);
  player_data->queens[player_data->player_id][move.queen_src] = 1;
  player_data->queens[player_data->player_id][move.queen_dst] = 0;
  move.arrow_dst = (unsigned int ) arrow_dst_heuristic_max_selection(move.queen_dst,data_queen_dst,player_data); 
  // Free memory
  free(data_queen_src->index_tab);
  free(data_queen_src->dir_tab);
  free(data_queen_src);
  free(data_queen_dst->index_tab);
  free(data_queen_dst->dir_tab);
  free(data_queen_dst);
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