#include "test_David.h"



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


void test_index_accessible_david(void) {
    printf("%s", __func__);
    struct player_data_t * player;
    player = malloc(sizeof(struct player_data_t));
    int m = 3;
    player->graph = square(m);
    struct stock_position * stock_player = malloc(1*sizeof(struct stock_position));
    stock_player->index_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->dir_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->len = 0;
    for(int i = 0 ; i < NUM_PLAYERS ; i ++) {
        player->queens[i] = (unsigned int *) malloc((m*m)*sizeof(unsigned int));
        player->queens_idx[i] = calloc(m*m,sizeof(unsigned int));
    }
    player->player_id = 0;
    player->num_queens = 4;
    for(int i = 0 ; i < m*m ; i ++) {
        player->queens[player->player_id][i] = 0;
        player->queens[next_player(player->player_id)][i] = 0;
    }
    //test  index = 8
    index_accessible(8,stock_player,player);
    int list_index_test_8[6] = {5,2,7,6,4,0};
    int list_dir_test_8[6] = {1,1,7,7,8,8};
    assert(stock_player->len == 6);
    for(int i = 0 ; i < stock_player->len ; i ++ ) {
        assert( (int)stock_player->index_tab[i] == list_index_test_8[i]);
        assert( (int) stock_player->dir_tab[i] == list_dir_test_8[i]);
    }
    //test index 0
    index_accessible(0,stock_player,player);
    int list_index_test_0[6] = {1,2,4,8,3,6};
    int list_dir_test_0[6] = {3,3,4,4,5,5};
    assert(stock_player->len == 6);
    for(int i = 0 ; i < stock_player->len ; i ++ ) {
        assert( (int) stock_player->index_tab[i] == list_index_test_0[i]);
        assert( (int) stock_player->dir_tab[i] == list_dir_test_0[i]);
    }
    for (int i = 0; i < NUM_PLAYERS; i++) {
        free(player->queens_idx[i]);
        free(player->queens[i]);
    }
    gsl_spmatrix_uint_free(player->graph->t);
    free(player->graph);
    free(player);
    free(stock_player->index_tab);
    free(stock_player->dir_tab);
    free(stock_player);
    printf("\tOK\n");
}

void test_possible_direction_queen_david(void) {
    printf("%s", __func__);
    struct player_data_t * player;
    player = malloc(sizeof(struct player_data_t));
    int m = 3;
    player->graph = square(m);
    struct stock_position * stock_player = malloc(1*sizeof(struct stock_position));
    stock_player->index_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->dir_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->len = 0;
    for(int i = 0 ; i < NUM_PLAYERS ; i ++) {
        player->queens[i] = (unsigned int *) malloc((m*m)*sizeof(unsigned int));
        player->queens_idx[i] = calloc(m*m,sizeof(unsigned int));
    }
    player->player_id = 0;
    player->num_queens = 4;
    for(int i = 0 ; i < m*m ; i ++) {
        player->queens[player->player_id][i] = 0;
        player->queens[next_player(player->player_id)][i] = 0;
    }
    //test  index = 8
    assert(possible_direction_queen(player,8) == 3 );
    //test index = 0
    assert(possible_direction_queen(player,0) == 3 );
    //test index = 1
    assert(possible_direction_queen(player,1) == 5 );
    //test index = 2
    assert(possible_direction_queen(player,2) == 3 );
    for (int i = 0; i < NUM_PLAYERS; i++) {
        free(player->queens_idx[i]);
        free(player->queens[i]);
    }
    gsl_spmatrix_uint_free(player->graph->t);
    free(player->graph);
    free(player);
    free(stock_player->index_tab);
    free(stock_player->dir_tab);
    free(stock_player);
    printf("\tOK\n");
}

void test_total_simple_mouv_possible_david(void) {
    printf("%s", __func__);
    struct player_data_t * player;
    player = malloc(sizeof(struct player_data_t));
    int m = 3;
    player->graph = square(m);
    struct stock_position * stock_player = malloc(1*sizeof(struct stock_position));
    stock_player->index_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->dir_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->len = 0;
    for(int i = 0 ; i < NUM_PLAYERS ; i ++) {
        player->queens[i] = (unsigned int *) malloc((m*m)*sizeof(unsigned int));
        player->queens_idx[i] = calloc(m*m,sizeof(unsigned int));
    }
    player->player_id = 0;
    player->num_queens = 4;
    for(int i = 0 ; i < m*m ; i ++) {
        player->queens[player->player_id][i] = 0;
        player->queens[next_player(player->player_id)][i] = 0;
    }
    //test  index = 8
    assert(total_simple_mouv_possible(player,8) == 6 );
    //test index = 0
    assert(total_simple_mouv_possible(player,0) == 6 );
    //test index = 1
    assert(total_simple_mouv_possible(player,1) == 6 );
    //test index = 2
    assert(total_simple_mouv_possible(player,2) == 6 );
    for (int i = 0; i < NUM_PLAYERS; i++) {
        free(player->queens_idx[i]);
        free(player->queens[i]);
    }
    gsl_spmatrix_uint_free(player->graph->t);
    free(player->graph);
    free(player);
    free(stock_player->index_tab);
    free(stock_player->dir_tab);
    free(stock_player);
    printf("\tOK\n");
}

void test_negative_min_distance_queen_arrow_david(void) {

    printf("%s", __func__);
    //player init
    struct player_data_t * player;
    player = malloc(sizeof(struct player_data_t));
    int m = 10;
    player->graph = square(m);
    //stock init
    struct stock_position * stock_player = malloc(1*sizeof(struct stock_position));
    stock_player->index_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->dir_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->len = 0;
    //queens init
    for(int i = 0 ; i < NUM_PLAYERS ; i ++) {
        player->queens[i] = (unsigned int *) malloc((m*m)*sizeof(unsigned int));
        player->queens_idx[i] = calloc(m*m,sizeof(unsigned int));
    }
    player->player_id = 0;
    player->num_queens = 4;
    for(int i = 0 ; i < m*m ; i ++) {
        player->queens[player->player_id][i] = 0;
        player->queens[next_player(player->player_id)][i] = 0;
    }
    player->queens[next_player(player->player_id)][0] = 1;
    player->queens[next_player(player->player_id)][20] = 1;
    player->queens[next_player(player->player_id)][55] = 1;
    player->queens[next_player(player->player_id)][90] = 1;
    //test point euclidian distance
    int dist  = point_euclidian_distance( player->graph, 0, 0);
    for(int i = 1 ; i < (int) player->graph->num_vertices ; i ++){
        assert( (point_euclidian_distance( player->graph, 0, i) > 0) && ((int) point_euclidian_distance( player->graph, 0, i) >= dist));
        dist = (int) (point_euclidian_distance( player->graph, 0, i));
        //printf("la distance entre 0 et %d est de %d \n",i,dist);
  
    }
    //printf("\n\t total point euclidian distance OK\n\n");
    //test negative min distance queen arrow
    for(int i = 0 ; i < 5 ; i++ ){
    //because the arrows are normally closer to the queen in 0
          int distt = negative_min_distance_queen_arrow(player,i);
          assert(distt == -(int)point_euclidian_distance(player->graph,0,i));
          assert(!(distt == -(int)point_euclidian_distance(player->graph,20,i)));
          assert(!(distt == -(int)point_euclidian_distance(player->graph,55,i)));
          assert(!(distt == -(int)point_euclidian_distance(player->graph,90,i)));

          //printf("l'opposé la distance de la flèche en %d à la reine adverse en 0 est de %d \n",i,distt);
    }
    for(int i = 80 ; i < 90 ; i++ ){
    //because the arrows are normally closer to the queen in 90
          int distt = negative_min_distance_queen_arrow(player,i);
          assert(distt == -(int)point_euclidian_distance(player->graph,90,i));
          assert(!(distt == -(int)point_euclidian_distance(player->graph,0,i)));
          assert(!(distt == -(int)point_euclidian_distance(player->graph,55,i)));
          assert(!(distt == -(int)point_euclidian_distance(player->graph,20,i)));
          //printf("l'opposé la distance de la flèche en %d à la reine adverse en 0 est de %d \n",i,distt);
    }
    //printf("\n\t test negative min distance queen arrow OK\n\n");
    for (int i = 0; i < NUM_PLAYERS; i++) {
        free(player->queens_idx[i]);
        free(player->queens[i]);
    }
    gsl_spmatrix_uint_free(player->graph->t);
    free(player->graph);
    free(player);
    free(stock_player->index_tab);
    free(stock_player->dir_tab);
    free(stock_player);
    printf("\tOK\n");
}



void test_heuristic_score_queen_david(void) {
    printf("%s", __func__);
    struct player_data_t * player;
    player = malloc(sizeof(struct player_data_t));
    int m = 3;
    player->graph = square(m);
    struct stock_position * stock_player = malloc(1*sizeof(struct stock_position));
    stock_player->index_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->dir_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->len = 0;
    for(int i = 0 ; i < NUM_PLAYERS ; i ++) {
        player->queens[i] = (unsigned int *) malloc((m*m)*sizeof(unsigned int));
        player->queens_idx[i] = calloc(m*m,sizeof(unsigned int));
    }
    player->player_id = 0;
    player->num_queens = 4;
    for(int i = 0 ; i < m*m ; i ++) {
        player->queens[player->player_id][i] = 0;
        player->queens[next_player(player->player_id)][i] = 0;
    }
    //test heuristic score for queen
    assert( (heuristic_score(player,0,0,0) == 9));
    assert( (heuristic_score(player,8,0,0) == 9));
    assert( (heuristic_score(player,2,0,0) == 9));
    assert( (heuristic_score(player,1,0,0) == 11));
    for (int i = 0; i < NUM_PLAYERS; i++) {
        free(player->queens_idx[i]);
        free(player->queens[i]);
    }
    gsl_spmatrix_uint_free(player->graph->t);
    free(player->graph);
    free(player);
    free(stock_player->index_tab);
    free(stock_player->dir_tab);
    free(stock_player);
    printf("\tOK\n");
}


void test_heuristic_score_arrow_david(void) {
    printf("%s", __func__);
    //player init
    struct player_data_t * player;
    player = malloc(sizeof(struct player_data_t));
    int m = 10;
    player->graph = square(m);
    //stock init
    struct stock_position * stock_player = malloc(1*sizeof(struct stock_position));
    stock_player->index_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->dir_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->len = 0;
    //queens init
    for(int i = 0 ; i < NUM_PLAYERS ; i ++) {
        player->queens[i] = (unsigned int *) malloc((m*m)*sizeof(unsigned int));
        player->queens_idx[i] = calloc(m*m,sizeof(unsigned int));
    }
    player->player_id = 0;
    player->num_queens = 4;
    for(int i = 0 ; i < m*m ; i ++) {
        player->queens[player->player_id][i] = 0;
        player->queens[next_player(player->player_id)][i] = 0;
    }
    player->queens[next_player(player->player_id)][0] = 1;
    player->queens[next_player(player->player_id)][20] = 1;
    player->queens[next_player(player->player_id)][55] = 1;
    player->queens[next_player(player->player_id)][90] = 1;
    //test heuristic score for arrow

    //we are going to test two very similar points and we are going to vary some parameters
    //here the distance of the arrow to an opponent
    assert( ( heuristic_score(player, 11, 88, 1) > heuristic_score(player,11,30,1) ) ) ;//because the arrow 88 is very close is closer to a queen, the queen 90 ;
    assert( heuristic_score(player, 11, 19 , 1) > heuristic_score(player,11,10,1) ); //because the arrow 19 is very close is closer to a queen, the queen 20 ;
    assert( !(heuristic_score(player, 11, 54, 1) < heuristic_score(player,11, 80, 1))); //because the arrow 54 is very close is closer to a queen, the queen 55 ;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        free(player->queens_idx[i]);
        free(player->queens[i]);
    }
    gsl_spmatrix_uint_free(player->graph->t);
    free(player->graph);
    free(player);
    free(stock_player->index_tab);
    free(stock_player->dir_tab);
    free(stock_player);
    printf("\tOK\n");
}



void test_queen_dst_heuristic_max_selection_david(void) {
    printf("%s", __func__);
    //player init
    struct player_data_t * player;
    player = malloc(sizeof(struct player_data_t));
    int m = 3;
    player->graph = square(m);
    //stock init
    struct stock_position * stock_player = malloc(1*sizeof(struct stock_position));
    stock_player->index_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->dir_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->len = 0;
    //queens init
    for(int i = 0 ; i < NUM_PLAYERS ; i ++) {
        player->queens[i] = calloc((m*m),sizeof(unsigned int));
        player->queens_idx[i] = calloc(m*m,sizeof(unsigned int));
    }
    player->player_id = 0;
    player->num_queens = 4;
    //for index_src = 0
    index_accessible(0,stock_player,player);
    assert(queen_dst_heuristic_max_selection(0, stock_player ,player) == 4);
    //for index_src = 8
    index_accessible(8,stock_player,player);
    for (int i = 0; i < NUM_PLAYERS; i++) {
        free(player->queens_idx[i]);
        free(player->queens[i]);
    }
    gsl_spmatrix_uint_free(player->graph->t);
    free(player->graph);
    free(player);
    free(stock_player->index_tab);
    free(stock_player->dir_tab);
    free(stock_player);
    printf("\tOK\n");
}



void test_arrow_dst_heuristic_max_selection_david(void) {
    printf("%s", __func__);
    //player init
    struct player_data_t * player;
    player = malloc(sizeof(struct player_data_t));
    int m = 10;
    player->graph = square(m);
    //stock init
    struct stock_position * stock_player = malloc(1*sizeof(struct stock_position));
    stock_player->index_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->dir_tab = calloc(player->graph->num_vertices,sizeof(unsigned int));
    stock_player->len = 0;
    //queens init
    for(int i = 0 ; i < NUM_PLAYERS ; i ++) {
        player->queens[i] = (unsigned int *) malloc((m*m)*sizeof(unsigned int));
        player->queens_idx[i] = calloc(m*m,sizeof(unsigned int));
    }
    player->player_id = 0;
    player->num_queens = 4;
    for(int i = 0 ; i < m*m ; i ++) {
        player->queens[player->player_id][i] = 0;
        player->queens[next_player(player->player_id)][i] = 0;
    }
    player->queens[next_player(player->player_id)][0] = 1;
    player->queens[next_player(player->player_id)][20] = 1;
    player->queens[next_player(player->player_id)][55] = 1;
    player->queens[next_player(player->player_id)][90] = 1;
    //for index_src = 0
    index_accessible(0,stock_player,player);
    assert(arrow_dst_heuristic_max_selection(0, stock_player ,player) == 1);
    index_accessible(8,stock_player,player);
    assert(arrow_dst_heuristic_max_selection(8, stock_player ,player) == 1);
    index_accessible(80,stock_player,player);
    assert(arrow_dst_heuristic_max_selection(80, stock_player ,player) == 91);
    index_accessible(75,stock_player,player);
    assert(arrow_dst_heuristic_max_selection(75, stock_player ,player) == 53);
    index_accessible(45,stock_player,player);
    assert(arrow_dst_heuristic_max_selection(45, stock_player ,player) == 1);
    for (int i = 0; i < NUM_PLAYERS; i++) {
        free(player->queens_idx[i]);
        free(player->queens[i]);
    }
    gsl_spmatrix_uint_free(player->graph->t);
    free(player->graph);
    free(player);
    free(stock_player->index_tab);
    free(stock_player->dir_tab);
    free(stock_player);
    printf("\tOK\n");
}
