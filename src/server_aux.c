#include <dlfcn.h>
#include "server_aux.h"

void *dl_client(const char *lib)
{
    void *client = dlopen(lib, RTLD_LAZY);
    if (!client)
    {
        fputs(dlerror(), stderr);
        exit(1);
    }
    return client;
}

void *dl_symbole(const void *lib, char *sym)
{
    void *symbole = dlsym((void *)lib, sym);
    char *error;
    if ((error = dlerror()) != NULL)
    {
        fputs(error, stderr);
        exit(1);
    }
    return symbole;
}

struct player_t dl_player(char *lib)
{
    struct player_t player = {};
    player.lib = dl_client(lib);
    player.get_player_name = (char const *(*)(void))dl_symbole(player.lib, "get_player_name");
    player.initialize = (void (*)(unsigned int, struct graph_t *, unsigned int, unsigned int **))dl_symbole(player.lib, "initialize");
    player.play = (struct move_t(*)(struct move_t))dl_symbole(player.lib, "play");
    player.finalize = (void (*)(void))dl_symbole(player.lib, "finalize");
    return player;
}

void init_queens(int width, unsigned int *queens[NUM_PLAYERS], unsigned int *queens_idx[NUM_PLAYERS], int num_queens)
{
    for (int i = 0; i < num_queens; i++)
    {
        queens_idx[0][i] = 0;
        queens_idx[1][i] = 0;
    }
    for (int i = 0; i < width * width; i++)
    {
        queens[0][i] = 0;
        queens[1][i] = 0;
    }
    int idx;
    for (idx = 0; idx < (width - 1) / 3; idx++)
    {
        queens_idx[0][idx] = 3 * idx + 2;
        queens_idx[1][idx] = (width - 1) * width + 3 * idx + 2;

        queens[0][3 * idx + 2] = 1;
        queens[1][(width - 1) * width + 3 * idx + 2] = 1;
    }
    for (int i = 0; idx < num_queens - 1; i++)
    {
        queens_idx[0][idx] = width * 3 * i + width * 2;
        queens_idx[0][idx + 1] = width * 3 * i + width * 2 + width - 1;
        queens_idx[1][idx] = width * (width - 1) - width * 3 * i - width * 2;
        queens_idx[1][idx + 1] = width * (width - 1) - width * 3 * i - width * 2 + width - 1;

        queens[0][width * 3 * i + width * 2] = 1;
        queens[0][width * 3 * i + width * 2 + width - 1] = 1;
        queens[1][width * (width - 1) - width * 3 * i - width * 2] = 1;
        queens[1][width * (width - 1) - width * 3 * i - width * 2 + width - 1] = 1;

        idx += 2;
    }
}

void send_client_copy(struct graph_t *world, unsigned int *queens_idx[NUM_PLAYERS], int num_queens, struct players_t players)
{
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        struct graph_t *graph_copy = malloc(sizeof(struct graph_t));
        graph_copy->num_vertices = world->num_vertices;

        gsl_spmatrix_uint *copie = gsl_spmatrix_uint_alloc(graph_copy->num_vertices, graph_copy->num_vertices);
        gsl_spmatrix_uint *compressed_copie = gsl_spmatrix_uint_compress(copie, GSL_SPMATRIX_CSR);
        gsl_spmatrix_uint_free(copie);
        gsl_spmatrix_uint_memcpy(compressed_copie, world->t);
        graph_copy->t = compressed_copie;

        unsigned int *queens_copy[NUM_PLAYERS];
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            queens_copy[i] = (unsigned int *)malloc(num_queens * sizeof(unsigned int));
            for (int j = 0; j < num_queens; j++)
            {
                queens_copy[i][j] = queens_idx[i][j];
            }
        }

        players.player[i].initialize(i, graph_copy, num_queens, queens_copy);
    }
}

int is_queen_invalid(int current_player, struct move_t move, struct graph_t *world, unsigned int *queens[NUM_PLAYERS])
{
    if (is_queen_of(move.queen_src, queens[current_player]) == 0)
    {
        return 1;
    }
    struct set_t * list = (struct set_t *)malloc(sizeof(struct set_t));
    list->idx = (unsigned int *)malloc(sizeof(unsigned int) * world->num_vertices);
    list->len = 0;
    list_accessible(move.queen_src, list, world, queens);
    for (unsigned int i = 0; i < list->len; i++){
        if (list->idx[i] == move.queen_dst) {
            free(list->idx);
            free(list);
            return 0;
        }
    }
    free(list->idx);
    free(list);
    return 1;
}

int is_arrow_invalid(int current_player, struct move_t move, struct graph_t *world, unsigned int *queens[NUM_PLAYERS]){
    struct set_t * list = (struct set_t *)malloc(sizeof(struct set_t));
    list->idx = (unsigned int *)malloc(sizeof(unsigned int) * world->num_vertices);
    list->len = 0;
    queens[current_player][move.queen_src] = 0;
    queens[current_player][move.queen_dst] = 1;
    list_accessible(move.queen_dst, list, world, queens);
    queens[current_player][move.queen_src] = 1;
    queens[current_player][move.queen_dst] = 0;
    for (unsigned int i = 0; i < list->len; i++){
            if (list->idx[i] == move.arrow_dst) {
                free(list->idx);
                free(list);
                return 0;
        }
    }
    free(list->idx);
    free(list);
    return 1;
}

int is_invalid(int current_player, struct move_t move, struct graph_t *world, unsigned int *queens[NUM_PLAYERS])
{
    if (move.queen_src == UINT_MAX || move.queen_dst == UINT_MAX || move.arrow_dst == UINT_MAX)
    {
        printf("Invalid move\n");
        return 1;
    }
    if (is_queen_invalid(current_player, move, world, queens))
    {
        printf("Invalid queen move\n");
        return 1;
    }
    if (is_arrow_invalid(current_player, move, world, queens))
    {
        printf("Invalid arrow move\n");
        return 1;
    }
    return 0;
}

// return : Boolean if a position is winning.
int is_winning(struct graph_t *world, int current_player, unsigned int *queens[NUM_PLAYERS], unsigned int *queens_idx[NUM_PLAYERS], int num_queen)
{
    int other_player = next_player(current_player);
    for (int i = 0; i < num_queen; i++)
    {
        if (!is_blocked(queens_idx[other_player][i], world, queens))
        {
            return 0;
        }
    }
    return 1;
}

// Do not print diag
void print_board(int width, struct graph_t *world, unsigned int *queens[NUM_PLAYERS])
{
    int *south;
    south = (int *)malloc(width * sizeof(int));
    for (int i = 0; i < width * width; i++)
    {
        if (i % width == 0)
        {
            printf("\n");
            for (int j = 0; j < width; j++)
            {
                if (south[j] == 0)
                    printf("    ");
                else if (south[j] == 1)
                    printf(COL_GREEN "↕   " COL_NORMAL);
            }
            for (int j = 0; j < width; j++)
            {
                south[j] = 0;
            }
            printf("\n");
        }
        if (is_queen_of(i, queens[0]))
            printf(COL_YELLOW "♛ " COL_NORMAL);
        else if (is_queen_of(i, queens[1]))
            printf(COL_RED "♛ " COL_NORMAL);
        else if (is_queen(i, queens))
            printf("♛ ");
        else
            printf("⧈ ");
        if ((i % width < width - 1) && (get_CSR(world, i, i + 1) == DIR_EAST))
            printf(COL_GREEN "↔ " COL_NORMAL);
        else
            printf("  ");
        if ((i / width < width - 1) && (get_CSR(world, i, i + width) == DIR_SOUTH))
            south[i % width] = 1;
        else
        {
            south[i % width] = 0;
        }
    }
    printf("\n\n");
    free(south);
}

void print_adj(int width, struct graph_t *world)
{
    for (int i = 0; i < width * width; i++)
    {
        for (int j = 0; j < width * width; j++)
        {
            printf("%u ", get_CSR(world, i, j));
        }
        printf("\n");
    }
}