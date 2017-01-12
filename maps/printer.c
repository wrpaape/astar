#include "utils/utils.h"
#include "maps/maker.h"
#include "maps/paths.h"
#include "maps/printer.h"
#include "maps/printer/map_tokens.h"

#define PAD_SIZE 2lu
#define HANDLE_FOPEN(FILE_PTR, FILENAME, MODE)				\
do {									\
	FILE_PTR = fopen(FILENAME, MODE);				\
	if (FILE_PTR == NULL)						\
		EXIT_ON_FAILURE("failed to open file '%s' (mode '%s')",	\
				FILENAME, MODE);			\
} while (0)



void pretty_print_cost_map(char *buffer,
			   struct CostMap *map,
			   struct Endpoints *pts)
{
	/* unpack info */

	const size_t start_x = pts->start->x;
	const size_t start_y = pts->start->y;

	const size_t goal_x = pts->goal->x;
	const size_t goal_y = pts->goal->y;

	const size_t last_row = pts->horz->x;
	const size_t res_y    = map->res->y;

	const int min_cost = map->est->min;
	const int max_cost = map->est->max;

	int **costs = map->costs;

	const double token_ratio = ((double) TOKEN_SPAN)
				 / ((double) (max_cost - min_cost));

	/* clears stdout */
	PUT_ANSI_CLEAR(buffer);

	/* set top line */
	if (start_x == 0lu) {
		set_line_with_token(&buffer,
				    res_y,
				    goal_y,
				    set_start_token,
				    &TOP_LINE_JOIN_SETTERS);
	} else if (goal_x == 0lu) {
		set_line_with_token(&buffer,
				    res_y,
				    goal_y,
				    set_goal_token,
				    &TOP_LINE_JOIN_SETTERS);
	} else {
		set_unbroken_line(&buffer,
				  res_y,
				  &TOP_LINE_JOIN_SETTERS);
	}

	size_t char_row = 1lu;

	/* fill map */
	while (1) {
		/* set row of cost tokens */
		if (start_x == char_row) {
			set_cost_row_with_token(&buffer,
						res_y,
						start_y,
						min_cost,
						token_ratio,
						set_start_token,
						costs[char_row / 2lu]);
		} else if (goal_x == char_row) {
			set_cost_row_with_token(&buffer,
						res_y,
						goal_y,
						min_cost,
						token_ratio,
						set_goal_token,
						costs[char_row / 2lu]);
		} else {
			set_unbroken_cost_row(&buffer,
					      res_y,
					      min_cost,
					      token_ratio,
					      costs[char_row / 2lu]);
		}

		++char_row;

		/* all cost cells have been printed to 'buffer', break */
		if (char_row == last_row)
			break;


		/* set mid line */
		if (start_x == char_row) {
			set_line_with_token(&buffer,
					    res_y,
					    start_y,
					    set_start_token,
					    &MID_LINE_JOIN_SETTERS);
		} else if (goal_x == char_row) {
			set_line_with_token(&buffer,
					    res_y,
					    goal_y,
					    set_goal_token,
					    &MID_LINE_JOIN_SETTERS);
		} else {
			set_unbroken_line(&buffer,
					  res_y,
					  &MID_LINE_JOIN_SETTERS);
		}

		++char_row;
	}



	/* set bot line */
	if (start_x == last_row) {
		set_line_with_token(&buffer,
				    res_y,
				    start_y,
				    set_start_token,
				    &BOT_LINE_JOIN_SETTERS);

	} else if (goal_x == last_row) {
		set_line_with_token(&buffer,
				    res_y,
				    goal_y,
				    set_goal_token,
				    &BOT_LINE_JOIN_SETTERS);

	} else {
		set_unbroken_line(&buffer,
				  res_y,
				  &BOT_LINE_JOIN_SETTERS);
	}


	*buffer = '\0';
}

void cost_map_to_csv(char *filename,
		     struct CostMap *map)
{
	const size_t res_x = map->res->x;
	const size_t res_y = map->res->y;


	FILE *map_file;
	int **costs;
	int *cost_row;
	size_t x, y;

	HANDLE_FOPEN(map_file, filename, "w");

	costs = map->costs;

	x = 0lu;

	while (1) {
		cost_row = costs[x];

		y = 0lu;

		while (1) {
			fprintf(map_file, "%d", cost_row[y]);

			++y;

			if (y == res_y)
				break;

			fputc(',', map_file);
		}

		++x;

		if (x == res_x)
			break;

		fputc('\n', map_file);
	}


	fclose(map_file);
}
