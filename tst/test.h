#ifndef __TEST_H__
#define __TEST_H__

#include "../src/game.h"
#include "../src/graph.h"
#include "../src/move.h"
#include "../src/player.h"
#include "../src/dir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include "../src/server_aux.h"

void print_adj_graph1(struct graph_t * graph);

void free_queens(unsigned int *queens[NUM_PLAYERS], unsigned int *queens_idx[NUM_PLAYERS]);

void init_queens_test(unsigned int *queens[NUM_PLAYERS], struct graph_t * world);

void init_queen_idx_test(unsigned int *queens_idx[NUM_PLAYERS], unsigned int num_queens);

void free_set(struct set_t * a_free);

void test__dir_mat(void);

void test__dir_mat_grid(void);

void test__init_queens (void);

void test__is_invalid(void);

void test__neigh_dir(void);

void test__list_accessible(void);

void test__get_CSR(void);

void test__break_link(void);

void test__update_graph_and_queens(void);

void test__neighbor_dir(void);

void test__is_blocked(void);

void test__donut_grid(void);

#endif