#ifndef __TEST_DAVID_H__
#define __TEST_DAVID_H__
#include "test.h"

struct stock_position {
  unsigned int * index_tab;
  unsigned int * dir_tab;
  int len;
};

void print_adj_graph(struct player_data_t * player);

void index_accessible(unsigned int queen_index, struct stock_position * stock, struct player_data_t * player);

int possible_direction_queen(struct player_data_t * player, unsigned int queen_index);

int total_simple_mouv_possible( struct player_data_t * player, unsigned int queen_index );

unsigned int point_euclidian_distance( struct graph_t * graph, unsigned int index1, unsigned int index2);

int negative_min_distance_queen_arrow(struct player_data_t * player, unsigned int arrow_index);

int heuristic_score( struct player_data_t * player, unsigned int queen_index, unsigned int arrow_index, int is_arrow_now );

int queen_dst_heuristic_max_selection(unsigned int queen_index, struct stock_position * stock, struct player_data_t * player);

int arrow_dst_heuristic_max_selection(unsigned int queen_index, struct stock_position * stock, struct player_data_t * player);

void test_index_accessible_david(void);

void test_possible_direction_queen_david(void);

void test_total_simple_mouv_possible_david(void);

void test_negative_min_distance_queen_arrow_david(void);

void test_heuristic_score_queen_david(void);

void test_heuristic_score_arrow_david(void);

void test_queen_dst_heuristic_max_selection_david(void);

void test_arrow_dst_heuristic_max_selection_david(void);





#endif