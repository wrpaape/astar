#include "utils/utils.h"
#include "bheap.h"
#include "maps/maker.h"
#include "maps/paths.h"
#include "traversal/a_star.h"

#define GEN_BIAS 1.0
#define COST_BIAS 1.0
#define PROX_BIAS 1.0

extern inline void free_a_star_results(struct AStarResults *results);

int best_successor(const void *vnode1, const void *vnode2)
{
	struct AStarNode *node1 = (struct AStarNode *) vnode1;
	struct AStarNode *node2 = (struct AStarNode *) vnode2;


	return node1->score < node2->score;
}

void a_star_node_to_string(char *buffer, const void *vnode)
{
	struct AStarNode *node = (struct AStarNode *) vnode;

	sprintf(buffer, "  {\n"
			"    x:     %zu,\n"
			"    y:     %zu,\n"
			"    cost:  %d,\n"
			"    prox:  %zu,\n"
			"    score: %f\n"
			"  }\n",
		node->x, node->y, node->cost, node->prox, node->score);
}


struct AStarResults *a_star_least_cost_path(struct CostMap *map,
					    struct Endpoints *pts)
{
	/* unpack cost map and enpoints */
	const size_t min_cost = map->act->min;
	const size_t max_cost = map->act->max;

	const size_t x_start = pts->start->x;
	const size_t y_start = pts->start->y;

	const size_t x_goal = pts->goal->x;
	const size_t y_goal = pts->goal->y;

	const size_t x_max_horz = pts->horz->x;
	const size_t y_max_horz = pts->horz->y;

	const size_t x_max_vert = pts->vert->x;
	const size_t y_max_vert = pts->vert->y;


	/* initialize weights for determining heuristic, other constants */
	const size_t g_to_x_max = x_max_horz - x_goal;
	const size_t g_to_y_max = y_max_vert - y_goal;


	/* calculate furthest distance from goal within map bounds */
	const size_t max_prox = (g_to_x_max > x_goal ? g_to_x_max : x_goal)
			      + (g_to_y_max > y_goal ? g_to_y_max : y_goal);

	struct AStarConst CONST = {
		.costs	    = map->costs,
		.min_cost   = min_cost,
		.w_cost	    = COST_BIAS / ((double) (max_cost - min_cost)),
		.w_prox	    = PROX_BIAS / ((double) max_prox),
		.x_max_cost = map->res->x - 1lu,
		.y_max_cost = y_max_horz,
		.x_goal	    = x_goal,
		.y_goal	    = y_goal,
		.x_max_horz = x_max_horz,
		.y_max_horz = y_max_horz,
		.x_max_vert = x_max_vert,
		.y_max_vert = y_max_vert
	};

	/* initialize lookup table of successor expansion functions, 'exp_map' */
	const size_t count_rows	  = x_max_horz + 1lu;
	const size_t count_horz_y = y_max_vert;
	const size_t count_vert_y = y_max_vert + 1lu;

	const size_t horz_row_bytes = sizeof(ExpansionFun) * count_horz_y;
	const size_t vert_row_bytes = sizeof(ExpansionFun) * count_vert_y;

	ExpansionFun **exp_map;
	ExpansionFun *exp_row;

	size_t x, y;

	HANDLE_MALLOC(exp_map, sizeof(ExpansionFun *) * count_rows);

	/* set first row of horizontal nodes to min bounds */
	HANDLE_MALLOC(exp_row, horz_row_bytes);
	exp_map[0lu] = exp_row;

	for (y = 0lu; y < count_horz_y; ++y)
		exp_row[y] = &insert_children_MIN_BOUND_HORZ;


	/* fill inner nodes */
	x = 1lu;

	while (1) {
		/* set first and last cells of vert row to bounds */
		HANDLE_MALLOC(exp_row, vert_row_bytes);
		exp_map[x] = exp_row;

		exp_row[0lu] = &insert_children_MIN_BOUND_VERT;

		for (y = 1lu; y < y_max_vert; ++y)
			exp_row[y] = &insert_children_INNER_VERT;

		exp_row[y_max_vert] = &insert_children_MAX_BOUND_VERT;

		if (x == x_max_vert)
			break;

		/* set inner row of horz cells */
		++x;
		HANDLE_MALLOC(exp_row, horz_row_bytes);
		exp_map[x] = exp_row;

		for (y = 0lu; y < count_horz_y; ++y)
			exp_row[y] = &insert_children_INNER_HORZ;

		++x;
	}

	/* set last row of horizontal nodes to max bounds */
	HANDLE_MALLOC(exp_row, horz_row_bytes);
	exp_map[x_max_horz] = exp_row;

	for (y = 0lu; y < count_horz_y; ++y)
		exp_row[y] = insert_children_MAX_BOUND_HORZ;


	/* initialize root node */
	struct AStarNode *root = init_a_star_node(&CONST,
						  NULL,
						  0lu,
						  0,
						  x_start,
						  y_start);

	/* initialize priority list of successor nodes */
	struct BHeap *successors = init_bheap(&best_successor);

	/* insert root node */
	bheap_insert(successors, root);

	/* initialize path state */
	struct AStarNode *path;
	size_t branch_count = 0lu;

	/* begin pathfinding */
	printf("x_start: %zu\ny_start: %zu\n", x_start, y_start);
	printf("x_goal:  %zu\ny_goal:  %zu\n", x_goal,  y_goal);
	fflush(stdout);

	clock_t time_start = clock();

	a_star_find_path(&path,
			 &branch_count,
			 successors,
			 &CONST,
			 exp_map);

	clock_t time_finish = clock();

	/* build and return results accumulator */
	return a_star_build_results(&CONST,
				    path,
				    branch_count,
				    time_start,
				    time_finish);
}

void a_star_find_path(struct AStarNode **path,
		      size_t *branch_count,
		      struct BHeap *successors,
		      struct AStarConst *CONST,
		      ExpansionFun **exp_map)
{
	struct AStarNode *node;
	ExpansionFun exp_fun;

	while (1) {
		/* pop next successor, 'node' from 'successors' */
		node = bheap_extract(successors);

		/* if proximity to goal is zero... */
		if (node->prox == 0lu) {
			node->next = NULL; /* terminate 'path' */
			*path = node;	   /* set path to goal node */
			return;
		}

		/* otherwise look up node's expansion function */
		 exp_fun = exp_map[node->x][node->y];

		/* if node is closed, continue to the next successor */
		if (exp_fun == NULL)
			continue;

		/* otherwise close node */
		exp_map[node->x][node->y] = NULL;

		/* expand 'successors' to adjacent nodes */
		exp_fun(successors, CONST, node, branch_count);
	}

}

struct AStarResults *a_star_build_results(struct AStarConst *CONST,
					  struct AStarNode *reverse_path,
					  size_t branch_count,
					  clock_t time_start,
					  clock_t time_finish)
{
	struct AStarResults *results;
	struct AStarNode *node;
	struct AStarNode *prev_node;
	size_t best_step_count = 0lu;
	int total_cost = 0lu;

	for (node = reverse_path, prev_node = node->prev;
	     prev_node != NULL;
	     node = prev_node,	  prev_node = node->prev) {

		prev_node->next = node;
		++best_step_count;
		total_cost += node->cost;
	}

	HANDLE_MALLOC(results, sizeof(struct AStarResults));


	results->min_step_count  = node->prox;
	results->best_step_count = best_step_count;
	results->branch_count	 = branch_count;
	results->total_cost	 = total_cost;
	results->time_elapsed	 = time_finish - time_start;
	results->path		 = node;

	return results;
}

struct AStarNode *init_a_star_node(struct AStarConst *CONST,
				   struct AStarNode *parent,
				   const size_t gen,
				   const int cost,
				   const size_t x,
				   const size_t y)
{
	const size_t prox = calc_prox(x, y, CONST->x_goal, CONST->y_goal);

	const double score = ((cost * CONST->w_cost) + CONST->min_cost)
			   +  (prox * CONST->w_prox)
			   +  (gen  * GEN_BIAS);


	struct AStarNode *node;

	HANDLE_MALLOC(node, sizeof(struct AStarNode));

	node->gen   = gen;
	node->x	    = x;
	node->y	    = y;
	node->prox  = prox;
	node->cost  = cost;
	node->score = score;
	node->prev  = parent;

	return node;
}

inline size_t calc_prox(const size_t x0, const size_t y0,
			const size_t x1, const size_t y1)
{
	return (x1 > x0 ? (x1 - x0) : (x0 - x1))
	     + (y1 > y0 ? (y1 - y0) : (y0 - y1));
}

void report_a_star_results(struct AStarResults *results)
{
	struct AStarNode *node = results->path;

	size_t x_prev = node->x;
	size_t y_prev = node->y;

	size_t x_next, y_next;
	char *dir;

	printf("starting from: (%3zu, %3zu)\n\n", x_prev, y_prev);

	while (1) {
		node = node->next;

		if (node == NULL)
			break;

		x_next = node->x;
		y_next = node->y;

		if (x_next > x_prev)
			dir = "DOWN  ";

		else if (x_next < x_prev)
			dir = "UP    ";

		else if (y_next > y_prev)
			dir = "RIGHT ";

		else
			dir = "LEFT  ";

		printf("(%3zu, %3zu) %s to (%3zu, %3zu) at a cost of %d\n",
		       x_prev, y_prev, dir, x_next, y_next, node->cost);

		x_prev = x_next;
		y_prev = y_next;
	}

	printf("\nfinished at: (%3zu, %3zu)\n", x_prev, y_prev);


	printf("\n\n"
	       "min steps to goal: %zu\n"
	       "steps taken:       %zu\n"
	       "total cost:        %d\n"
	       "branches explored: %zu\n"
	       "time elapsed:      %f s\n",
	       results->min_step_count,
	       results->best_step_count,
	       results->total_cost,
	       results->branch_count,
	       ((double) results->time_elapsed) / (double) (CLOCKS_PER_SEC));
}


/* successor generators
 * ========================================================================== */
void insert_children_MIN_BOUND_HORZ(struct BHeap *successors,
				    struct AStarConst *CONST,
				    struct AStarNode *parent,
				    size_t *branch_count)
{
	const size_t gen      = parent->gen + 1lu;
	const size_t y_parent = parent->y;
	const size_t x_above  = 0lu;
	const size_t x_upper  = 1lu;
	const size_t x_ceil   = 2lu;
	const size_t y_upper  = y_parent + 1lu;
	const int cost_above  = CONST->costs[x_above][y_parent];

	/* insert upper sucessors */
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_above,
				      x_ceil,
				      y_parent));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_above,
				      x_upper,
				      y_parent));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_above,
				      x_upper,
				      y_upper));

	/* increment 'branch_count' */
	(*branch_count) += 3lu;
}

void insert_children_MAX_BOUND_HORZ(struct BHeap *successors,
				    struct AStarConst *CONST,
				    struct AStarNode *parent,
				    size_t *branch_count)
{
	const size_t gen      = parent->gen + 1lu;
	const size_t x_parent = parent->x;
	const size_t y_parent = parent->y;
	const size_t x_below  = CONST->x_max_cost;
	const size_t x_floor  = x_parent - 2lu;
	const size_t x_lower  = x_parent - 1lu;
	const size_t y_upper  = y_parent + 1lu;
	const int cost_below  = CONST->costs[x_below][y_parent];

	/* insert lower sucessors */
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_below,
				      x_floor,
				      y_parent));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_below,
				      x_lower,
				      y_parent));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_below,
				      x_lower,
				      y_upper));

	/* increment 'branch_count' */
	(*branch_count) += 3lu;
}

void insert_children_INNER_HORZ(struct BHeap *successors,
				struct AStarConst *CONST,
				struct AStarNode *parent,
				size_t *branch_count)
{
	const size_t gen      = parent->gen + 1lu;
	const size_t x_parent = parent->x;
	const size_t y_parent = parent->y;
	const size_t x_above  = x_parent / 2lu;
	const size_t x_below  = x_above  - 1lu;
	const size_t x_ceil   = x_parent + 2lu;
	const size_t x_upper  = x_parent + 1lu;
	const size_t x_lower  = x_parent - 1lu;
	const size_t x_floor  = x_parent - 2lu;
	const size_t y_upper  = y_parent + 1lu;
	const int cost_above  = CONST->costs[x_above][y_parent];
	const int cost_below  = CONST->costs[x_below][y_parent];

	/* insert upper sucessors */
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_above,
				      x_ceil,
				      y_parent));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_above,
				      x_upper,
				      y_parent));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_above,
				      x_upper,
				      y_upper));

	/* insert lower sucessors */
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_below,
				      x_floor,
				      y_parent));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_below,
				      x_lower,
				      y_parent));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_below,
				      x_lower,
				      y_upper));

	/* increment 'branch_count' */
	(*branch_count) += 6lu;
}

void insert_children_MIN_BOUND_VERT(struct BHeap *successors,
				    struct AStarConst *CONST,
				    struct AStarNode *parent,
				    size_t *branch_count)
{
	const size_t gen      = parent->gen + 1lu;
	const size_t x_parent = parent->x;
	const size_t y_parent  = 0lu;
	const size_t x_level  = x_parent / 2lu;
	const size_t x_upper  = x_parent + 1lu;
	const size_t x_lower  = x_parent - 1lu;
	const size_t y_right  = 1lu;
	const int cost_right  = CONST->costs[x_level][y_parent];

	/* insert right-side sucessors */
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_right,
				      x_parent,
				      y_right));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_right,
				      x_upper,
				      y_parent));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_right,
				      x_lower,
				      y_parent));

	/* increment 'branch_count' */
	(*branch_count) += 3lu;
}

void insert_children_MAX_BOUND_VERT(struct BHeap *successors,
				    struct AStarConst *CONST,
				    struct AStarNode *parent,
				    size_t *branch_count)
{
	const size_t gen      = parent->gen + 1lu;
	const size_t x_parent = parent->x;
	const size_t x_level = x_parent / 2lu;
	const size_t x_upper = x_parent + 1lu;
	const size_t x_lower = x_parent - 1lu;
	const size_t y_left  = CONST->y_max_cost;
	const int cost_left  = CONST->costs[x_level][y_left];

	/* insert left-side sucessors */
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_left,
				      x_parent,
				      y_left));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_left,
				      x_upper,
				      y_left));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_left,
				      x_lower,
				      y_left));

	/* increment 'branch_count' */
	(*branch_count) += 3lu;
}

void insert_children_INNER_VERT(struct BHeap *successors,
				struct AStarConst *CONST,
				struct AStarNode *parent,
				size_t *branch_count)
{
	const size_t gen      = parent->gen + 1lu;
	const size_t x_parent = parent->x;
	const size_t y_parent = parent->y;
	const size_t x_level = x_parent / 2lu;
	const size_t x_upper = x_parent + 1lu;
	const size_t x_lower = x_parent - 1lu;
	const size_t y_right = y_parent + 1lu;
	const size_t y_left  = y_parent - 1lu;
	const int cost_left  = CONST->costs[x_level][y_left];
	const int cost_right = CONST->costs[x_level][y_right];

	/* insert right-side sucessors */
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_right,
				      x_parent,
				      y_right));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_right,
				      x_upper,
				      y_parent));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_right,
				      x_lower,
				      y_parent));

	/* insert left-side sucessors */
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_left,
				      x_parent,
				      y_left));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_left,
				      x_upper,
				      y_left));
	bheap_insert(successors,
		     init_a_star_node(CONST,
				      parent,
				      gen,
				      cost_left,
				      x_lower,
				      y_left));

	/* increment 'branch_count' */
	(*branch_count) += 6lu;
}
