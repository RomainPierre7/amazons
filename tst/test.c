#include "test.h"

void print_adj_graph1(struct graph_t * graph){
    printf("<GE taille lig %zu col %zu nb non zero %zu (sur %zu)>\n",
       graph->t->size1, graph->t->size2, graph->t->nz,
       graph->t->size1 * graph->t->size2);

    for(unsigned int i = 0  ; i < graph->num_vertices; i ++){
        printf("\nrow : %d|\t", i);
        for(unsigned int j = 0 ; j < graph->num_vertices ; j++){
            int a = gsl_spmatrix_uint_get(graph->t,i,j);
            printf("%d ",a);
        }
        printf("\n");
    }
}

void free_queens(unsigned int *queens[NUM_PLAYERS], unsigned int *queens_idx[NUM_PLAYERS])
{
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        free(queens_idx[i]);
    }
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        free(queens[i]);
    }
}

void init_queens_test(unsigned int *queens[NUM_PLAYERS], struct graph_t * world){
        for (int i = 0; i < NUM_PLAYERS; i++)
    {
        queens[i] = (unsigned int *)malloc(world->num_vertices * sizeof(unsigned int));
    }
}

void init_queen_idx_test(unsigned int *queens_idx[NUM_PLAYERS], unsigned int num_queens){
        for (int i = 0; i < NUM_PLAYERS; i++)
    {
        queens_idx[i] = (unsigned int *)malloc(num_queens * sizeof(unsigned int));
    }
}

void free_set(struct set_t * a_free){
    free(a_free->idx);
    free(a_free);
}

void test__dir_mat(void){
    printf("%s", __func__);
    
    unsigned int width = 8;
    unsigned int src[] = {0, 8, 14, 22, 44, 33, 20, 18, 11};
    unsigned int dst[] = {0, 0, 7, 23, 53, 41, 27, 17, 2};
    for(enum dir_t i = FIRST_DIR; i < LAST_DIR+1; i++){
        assert(dir_mat(src[i], dst[i], width) == i);
    }

    printf("\tOK\n");
}

void test__dir_mat_grid(void){
    printf("%s", __func__);

    unsigned int width = 8;
    unsigned int src[] = {8, 10, 15, 55};
    unsigned int dst[] = {0, 11, 23, 54};
    enum dir_t res[] = {DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST};
    for(int i = 0; i < 4; i++){
        assert(dir_mat_grid(src[i], dst[i], width) == res[i]);
    }

    printf("\tOK\n");
}



void test__init_queens (void){
    printf("%s", __func__);
    // m = number of columns
    unsigned int m = 8;
    struct graph_t * world = square(m);
    int num_queens = 4 * (m / 10 + 1);
    unsigned int *queens[NUM_PLAYERS];
    init_queens_test(queens, world);
    unsigned int *queens_idx[NUM_PLAYERS];
    init_queen_idx_test(queens_idx, num_queens);

    init_queens(m, queens, queens_idx, num_queens);

    unsigned int tab[] = {2, 5, 16, 23};
    for(int i = 0; i < num_queens; i++){
        assert(queens[0][tab[i]] == 1);
        assert(queens_idx[0][i] == tab[i]);
    }
    assert(queens[0][0] == 0);

    free_graph(world);
    free_queens(queens, queens_idx);

    printf("\tOK\n");
}

void test__is_invalid(void){
    printf("%s", __func__);

    unsigned int m = 8;
    struct graph_t * world = square(m);
    int num_queens = 4 * (m / 10 + 1);
    unsigned int *queens[NUM_PLAYERS];
    init_queens_test(queens, world);
    unsigned int *queens_idx[NUM_PLAYERS];
    init_queen_idx_test(queens_idx, num_queens);
    init_queens(m, queens, queens_idx, num_queens);
    struct move_t t[3];
    unsigned int dst[] = {10, 17, 34};
    unsigned int dst_arr[] = {3, 3, 17};
    int res_is_invalid[] = {0, 1, 1};
        for(int i =0; i < 3; i++){
        t[i].queen_src = 2;
        t[i].queen_dst = dst[i];
        t[i].arrow_dst = dst_arr[i];
        assert(is_invalid(0,t[i], world, queens) == res_is_invalid[i]);
    }

    free_graph(world);
    free_queens(queens, queens_idx);

    printf("\tOK\n");
}

void test__neigh_dir(void)
{
    printf("%s", __func__);

    unsigned int m = 7;
    struct graph_t *world = square(m);
    int num_queens = 4 * (m / 10 + 1);
    unsigned int *queens[NUM_PLAYERS];
    init_queens_test(queens, world);
    unsigned int *queens_idx[NUM_PLAYERS];
    init_queen_idx_test(queens_idx, num_queens);
    unsigned int idx[NUM_DIRS] = {0, 2, 5, 16, 19, 23, 35, 41};
    unsigned int res[NUM_DIRS] = {0, UINT_MAX, UINT_MAX, 17, 27, 30, UINT_MAX, 40};
    for (enum dir_t i = FIRST_DIR; i < NUM_DIRS; i++)
    {
        assert(neighbor_dir(idx[i], i, world->t) == res[i]);
    }
    free_queens(queens, queens_idx);
    free_graph(world);
    printf("\tOK\n");
}

void test__list_accessible(void)
{
    printf("%s", __func__);

    unsigned int m = 7;
    struct graph_t *world = square(m);
    int num_queens = 4 * (m / 10 + 1);
    unsigned int *queens[NUM_PLAYERS];
    init_queens_test(queens, world);
    unsigned int *queens_idx[NUM_PLAYERS];
    init_queen_idx_test(queens_idx, num_queens);
    struct set_t *test = (struct set_t *)malloc(sizeof(struct set_t));
    test->idx = (unsigned int *)malloc(sizeof(unsigned int) * world->num_vertices);

    for (unsigned int i = 0; i < NUM_PLAYERS; i++)
    {
        for (unsigned int j = 0; j < world->num_vertices; j++)
        {
            queens[i][j] = 0;
        }
    }

    queens[0][3] = 1;
    queens[0][1] = 1;
    list_accessible(3, test, world, queens);
    unsigned int res_test[] = {4, 5, 6, 11, 19, 27, 10, 17, 24, 31, 38, 45, 9, 15, 21, 2};
    unsigned int len_res = 16;

    for (unsigned int i = 0; i < len_res; i++)
    {
        assert(test->idx[i] == res_test[i]);
    }
    assert(test->len == len_res);
    free_set(test);
    free_graph(world);
    free_queens(queens, queens_idx);
    printf("\tOK\n");
}


void test__get_CSR(void){
    printf("%s", __func__);

    unsigned int m = 7;
    struct graph_t *world = square(m);

    unsigned int src[5] = {0, 15, 47, 35, 32};
    unsigned int dst[5] = {7, 14, 48, 43, 25};
    enum dir_t res[5] = {DIR_SOUTH, DIR_WEST,DIR_EAST ,DIR_SE, DIR_NORTH};
    

    for (int j = 0; j < 5; j++){
        assert(get_CSR(world, src[j], dst[j]) == res[j]);
    }
    free_graph(world);
    printf("\tOK\n");
}

void test__break_link(void){
    printf("%s", __func__);

    unsigned int m = 8;
    struct graph_t *world = square(m);
    unsigned int arrow1 = 28;
    unsigned int arrow2 = 63;
    break_link(world, arrow1);
    for(int i = world->t->p[28]; i < world->t->p[29]; i++){
        assert(world->t->data[i] == 0);
    }
    for(int i = 0; (long unsigned int)i < world->t->nz; i++){
        if (world->t->i[i] == (int)arrow1) assert(world->t->data[i] == 0);
    }
    break_link(world, arrow2);
    for(int i = world->t->p[63]; (long unsigned int)i < world->t->nz; i++){
        
        assert(world->t->data[i] == 0);
    }
    for(int i = 0; (long unsigned int)i < world->t->nz; i++){
        if (world->t->i[i] == (int)arrow2) assert(world->t->data[i] == 0);
    }

    free_graph(world);
    printf("\tOK\n");
}

void test__update_graph_and_queens(void){
    printf("%s", __func__);

    unsigned int m = 8;
    struct graph_t * world = square(m);
    int num_queens = 4 * (m / 10 + 1);        
    unsigned int * queens[NUM_PLAYERS];
    unsigned int * queens_idx[NUM_PLAYERS];
    init_queens_test(queens, world);
    init_queen_idx_test(queens_idx, num_queens);
    init_queens(m, queens, queens_idx, num_queens);
    int player_id = 0;
    struct move_t move1 = {2, 0, 8};
    update_graph_and_queens(player_id, world, queens, queens_idx, num_queens, move1);
    struct move_t move2 = {16, 20, 28};
    update_graph_and_queens(player_id, world, queens, queens_idx, num_queens, move2);
    assert(queens[player_id][move1.queen_src] == 0);
    assert(queens[player_id][move1.queen_dst] == 1);
    assert(queens_idx[player_id][0] == 0);
    assert(queens[player_id][move2.queen_src] == 0);
    assert(queens[player_id][move2.queen_dst] == 1);
    assert(queens_idx[player_id][2] == 20);

    free_graph(world);
    free_queens(queens, queens_idx);

    printf("\tOK\n");
}

void test__neighbor_dir(void){
    printf("%s", __func__);

    unsigned int m = 8;
    struct graph_t * world = square(m);
    unsigned int idx1 = 18;
    enum dir_t test[] = {DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST};
    unsigned int res1[] = {10, 19, 26, 17};
    unsigned int idx2 = 63;
    unsigned int res2[] = {55, UINT_MAX, UINT_MAX, 62};

    for(int i = 0; i < 4; i++){
        assert(neighbor_dir(idx1, test[i], world->t) == res1[i]);
        assert(neighbor_dir(idx2, test[i], world->t) == res2[i]);
    }

    free_graph(world);
    printf("\tOK\n");
}

void test__is_blocked(void){
    printf("%s", __func__);
    
    unsigned int m = 8;
    struct graph_t * world = square(m);
    unsigned int *queens[NUM_PLAYERS];
    unsigned int * queens_idx[NUM_PLAYERS];
    int num_queens = 4 * (m / 10 + 1);        
    init_queen_idx_test(queens_idx, num_queens);
    init_queens_test(queens, world);
    init_queens(m, queens, queens_idx, num_queens);
    unsigned int idx = 2;
    assert(is_blocked(idx, world, queens) == 0);
    int tab[] = {1, 3, 9, 10, 11};
    for (int i = 0; i < 5; i++){
        queens[0][tab[i]] = 1;
    }
    assert(is_blocked(idx, world, queens) == 1);

    free_queens(queens, queens_idx);
    free_graph(world);
    printf("\tOK\n");
}

void test__donut_grid(){
    printf("%s", __func__);

    unsigned int m = 3;
    struct graph_t * world = donut(m);
    free_graph(world);
    printf("\tOK\n");
}