#ifndef __TEST_GEORGES_H__
#define __TEST_GEORGES_H__
#include "test.h"

float heuristic(unsigned int idx, float target_col, float target_row, unsigned int width);

unsigned int shoot_arrow_corner(struct set_t * list, unsigned int width);

unsigned int mid_move(struct set_t * set, unsigned int width);

void test__shoot_arrow_corner(void);

void test__mid_move(void);

#endif