#include "utils/utils.h"
#include "utils/rand.h"
#include "maps/maker.h"
#include "maps/paths.h"


extern inline void free_endpoints(struct Endpoints *pts);

/* constrain token locations to cell edges
 * ┌ ╳ ┬───┐
 * │ 2 │ 9 ◯
 * ├───┼ ◯ ┤
 * ╳ 3 │ 5 │
 * └───┴───┘
 * and ensure start and goal are in oposite quadrants */
struct Endpoints *define_endpoints(struct CostMap *map)
{
	struct Endpoints *pts;
	struct Coords *start;
	struct Coords *goal;
	struct Coords *horz;
	struct Coords *vert;

	HANDLE_MALLOC(pts,   sizeof(struct Endpoints));
	HANDLE_MALLOC(start, sizeof(struct Coords));
	HANDLE_MALLOC(goal,  sizeof(struct Coords));
	HANDLE_MALLOC(horz,  sizeof(struct Coords));
	HANDLE_MALLOC(vert,  sizeof(struct Coords));

	horz->x = map->res->x * 2lu;
	vert->x = horz->x - 1lu;

	vert->y = map->res->y;
	horz->y = vert->y - 1lu;

	const int32_t l_x = (int32_t) horz->x;
	const int32_t l_y = (int32_t) horz->y;
	const int32_t h_x = l_x / 2;
	const int32_t h_y = l_y / 2;

	int32_t s_x;
	int32_t s_y;
	int32_t g_x;
	int32_t g_y;

	if (coin_flip()) {
		s_x = rand_in_int_range(0,   h_x - 1);
		g_x = rand_in_int_range(h_x, l_x);

	} else {
		s_x = rand_in_int_range(h_x, l_x);
		g_x = rand_in_int_range(0,   h_x - 1);
	}

	if (coin_flip()) {
		s_y = rand_in_int_range(0,   h_y - 1);
		g_y = rand_in_int_range(h_y, l_y);

	} else {
		s_y = rand_in_int_range(h_y, l_y);
		g_y = rand_in_int_range(0,   h_y - 1);
	}

	start->x = (size_t) s_x;
	goal->x  = (size_t) g_x;

	/* avoid placing tokens on corner or join pieces */
	start->y = (size_t) (((s_x & 1) && (s_y > 0)) ? (s_y - 1) : s_y);
	goal->y  = (size_t) (((g_x & 1) && (g_y > 0)) ? (g_y - 1) : g_y);

	pts->start = start;
	pts->goal  = goal;
	pts->horz  = horz;
	pts->vert  = vert;

	return pts;
}
