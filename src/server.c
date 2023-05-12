#include <dlfcn.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "graph.h"
#include "move.h"
#include "server_aux.h"

int main(int argc, char* argv[])
{
    // Get execution options
    int opt;
    int width = 8;
    char board_shape = 'c';
    int print_on = 0;
    int random_seed = 1;
    while ((opt = getopt(argc, argv, "m:t:s:p")) != -1)
    {
        switch (opt)
        {
            case 'm':
                width = atoi(optarg);
                break;
            case 't':
                board_shape = optarg[0];
                break;
            case 's':
                random_seed = 0;
                srand(atoi(optarg));
                break;
            case 'p':
                print_on = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (random_seed)
    {
        srand(time(NULL));
    }

    // Loading dynamic libraries (clients)
    struct players_t players = {};
    int player_number = 0;
    for (int i = optind; i < argc; i++)
    {
        if (player_number >= NUM_PLAYERS)
        {
            fprintf(stderr, "Error: too many players\n");
            exit(EXIT_FAILURE);
        }
        players.player[player_number] = dl_player(argv[i]);
        player_number++;
    }

    // Game set up
    int current_player = choose_random_player();
    struct move_t previous_move = {-1, -1, -1};
    struct graph_t* world = world_init(board_shape, width);
    int turn = 1;
    int invalid = 0; // 1 if a player played an invalid move

    int num_queens = 4 * (width / 10 + 1);
    unsigned int* queens[NUM_PLAYERS];
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        queens[i] =
            (unsigned int*)malloc(world->num_vertices * sizeof(unsigned int));
    }
    unsigned int* queens_idx[NUM_PLAYERS];
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        queens_idx[i] =
            (unsigned int*)malloc(num_queens * sizeof(unsigned int));
    }
    init_queens(sqrt(world->num_vertices), queens, queens_idx, num_queens);
    send_client_copy(world, queens_idx, num_queens, players);

    if (print_on && board_shape != 'c')
    {
        printf("---------- Initial board ----------\n");
        print_board(width, world, queens);
    }

    // Game loop
    while (1)
    {
        struct player_t player = players.player[current_player];

        printf("---------- %s's turn : ---------- (turn n°%d)\n",
               player.get_player_name(), turn);

        struct move_t move = player.play(previous_move);

        if (is_invalid(current_player, move, world, queens)){
            invalid = 1;
        } 

        update_graph_and_queens(current_player, world, queens, queens_idx, num_queens, move);

        if (print_on)
        {
            if (board_shape == 'c' || board_shape == 'g') print_board(width, world, queens);
        }
        printf("\nQueen src: %d, Queen dst: %d, Arrow dst: %d\n\n",
               move.queen_src, move.queen_dst, move.arrow_dst);

        if (invalid){
            printf("Invalid move, %s loses the game !\n", player.get_player_name());
            break;
        }

        if (is_winning(world, current_player, queens, queens_idx, num_queens)){
            printf("\n%s won the game !\n", player.get_player_name());
            break;
        }

        previous_move = move;
        current_player = next_player(current_player);
        turn++;
    }

    // Free memory
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        players.player[i].finalize();
    }
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        dlclose(players.player[i].lib);
    }
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        free(queens[i]);
        free(queens_idx[i]);
    }
    free_graph(world);

    return EXIT_SUCCESS;
}
