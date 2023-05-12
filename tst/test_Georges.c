#include "test_Georges.h"




float heuristic(unsigned int idx, float target_col, float target_row, unsigned int width){
  float col = (float) (idx % width);
  float row = (float) (idx / width);
  col = col - target_col;
  row = row - target_row;
  return sqrt(col * col + row * row);
}

unsigned int mid_move(struct set_t * set, unsigned int width){
  unsigned int idx = set->idx[0];
  float min = heuristic(idx, (float) (width - 1) / 2, (float) (width - 1) / 2, width);
  for (unsigned int i = 1; i < set->len; i++){
    unsigned int new_idx = set->idx[i];
    float new_heuristic = heuristic(new_idx, (float) (width - 1) / 2, (float) (width - 1) / 2, width);
    if (new_heuristic < min){
      min = new_heuristic;
      idx = new_idx;
    }
  }
return idx;
}

unsigned int shoot_arrow_corner(struct set_t * list, unsigned int width){
  unsigned int idx = list->idx[0];
  float max = heuristic(idx, (float) (width - 1) / 2, (float) (width - 1) / 2, width);
  for (unsigned int i = 1; i < list->len; i++){
    unsigned int new_idx = list->idx[i];
    float new_heuristic = heuristic(new_idx, (float) (width - 1) / 2, (float) (width - 1) / 2, width);
    if (new_heuristic > max){
      max = new_heuristic;
      idx = new_idx;
    }
  }
return idx;
}



void init_set(struct set_t * list, struct graph_t * world){
    list->len = 0;
    list->idx = (unsigned int *)malloc(sizeof(unsigned int) * world->num_vertices);
}


void test__shoot_arrow_corner(void){
    printf("%s", __func__);

    unsigned int m = 8;
    struct graph_t * world = square(m);
    unsigned int * queens[NUM_PLAYERS];
    init_queens_test(queens, world);
    int num_queens = 4 * (m / 10 + 1);      
    unsigned int * queens_idx[NUM_PLAYERS];
    init_queen_idx_test(queens_idx, num_queens);
    struct set_t * list = (struct set_t *)malloc(sizeof(struct set_t));
    init_set(list, world);
    init_queens(m, queens, queens_idx, num_queens);
    unsigned int player_id = 0;
    struct move_t move = {2, 36,0};
    list_accessible(move.queen_dst, list, world, queens);
    move.arrow_dst = shoot_arrow_corner(list, m);
    assert(move.arrow_dst == 63);
    update_graph_and_queens(player_id, world, queens, queens_idx, num_queens, move);
    struct move_t move2 = {36, 62, 0};
    struct set_t * list2 = (struct set_t *)malloc(sizeof(struct set_t));
    init_set(list2, world);
    list_accessible(move2.queen_dst, list2, world, queens);
    move2.arrow_dst = shoot_arrow_corner(list2, m);
    assert(move2.arrow_dst == 6);
    free_queens(queens, queens_idx);
    free_set(list);
    free_set(list2);
    free_graph(world);
    printf("\tOK\n");
}

void test__mid_move(void){
  printf("%s", __func__);
  unsigned int m = 8;
  struct graph_t * world = square(m);
  unsigned int * queens[NUM_PLAYERS];
  init_queens_test(queens, world);
  int num_queens = 4 * (m / 10 + 1);      
  unsigned int * queens_idx[NUM_PLAYERS];
  init_queen_idx_test(queens_idx, num_queens);
  struct set_t * list = (struct set_t *)malloc(sizeof(struct set_t));
  init_set(list, world);
  init_queens(m, queens, queens_idx, num_queens);
  unsigned int player_id = 0;
  struct move_t move = {2, -1,0};
  list_accessible(move.queen_src, list, world, queens);
  move.queen_dst = mid_move(list, m);
  assert(move.queen_dst == 20);
  update_graph_and_queens(player_id, world, queens, queens_idx, num_queens, move);
  struct move_t move2 = {36, -1, 0};
  struct set_t * list2 = (struct set_t *)malloc(sizeof(struct set_t));
  init_set(list2, world);
  list_accessible(move2.queen_src, list2, world, queens);
  move2.queen_dst = mid_move(list2, m);
  assert(move2.queen_dst == 28);
  free_queens(queens, queens_idx);
  free_set(list);
  free_set(list2);
  free_graph(world);
  
  printf("\tOK\n");
}
