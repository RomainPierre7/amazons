#include "test.h"
#include "test_David.h"
#include "test_Georges.h"
int main()
{
    test__init_queens();
    test__list_accessible();
    test__neigh_dir();
    test__get_CSR();
    test__is_invalid();
    test__dir_mat();
    test__dir_mat_grid();
    test__break_link();
    test__update_graph_and_queens();
    test__neighbor_dir();
    test__is_blocked();
    test_index_accessible_david();
    test_possible_direction_queen_david();
    test_total_simple_mouv_possible_david();
    test_negative_min_distance_queen_arrow_david();
    test_heuristic_score_queen_david();
    test_heuristic_score_arrow_david();
    test_queen_dst_heuristic_max_selection_david();
    test_arrow_dst_heuristic_max_selection_david();
    test__shoot_arrow_corner();
    test__mid_move();
    return 0;
}