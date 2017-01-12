#include "maps/printer/map_tokens.h"
#include <math.h>

/* 12-13 bytes */
/* const struct Token COST_TOKENS[] = { */
/* 	{.chars = FAINT  WHITE   "▁", .size = 12lu}, */
/* 	{.chars = FAINT  BLACK   "▁", .size = 12lu}, */
/* 	{.chars = FAINT  BLUE    "▂", .size = 12lu}, */
/* 	{.chars = NORMAL CYAN    "▃", .size = 13lu}, */
/* 	{.chars = NORMAL GREEN   "▄", .size = 13lu}, */
/* 	{.chars = NORMAL YELLOw  "▅", .size = 13lu}, */
/* 	{.chars = BRIGHT RED     "▆", .size = 12lu}, */
/* 	{.chars = BRIGHT MAGENTA "▇", .size = 12lu}, */
/* 	{.chars = BRIGHT BLACK   "█", .size = 12lu} */
/* }; */

#define BLOCKS

#define PUT_HORIZONTAL_LINES(ptr)	\
do {					\
	PUT_BOX_CHAR_LIGHT_H_LINE(ptr);	\
	PUT_BOX_CHAR_LIGHT_H_LINE(ptr);	\
	PUT_BOX_CHAR_LIGHT_H_LINE(ptr);	\
} while (0)

#define PUT_VERTICAL_LINE(ptr) PUT_BOX_CHAR_LIGHT_V_LINE(ptr)

#define PUT_DEFAULT_COLORS(ptr)		\
do {					\
	PUT_ANSI_WHITE_BG(ptr);		\
	PUT_ANSI_BLACK(ptr);		\
} while (0)

#define PUT_RESET_DEFAULT(ptr)		\
do {					\
	PUT_ANSI_RESET(ptr);		\
	PUT_DEFAULT_COLORS(ptr);	\
} while (0)

#define PUT_TERMINATE_LINE(ptr)		\
do {					\
	PUT_ANSI_RESET(ptr);		\
	PUT_CHAR(ptr, '\n');		\
} while (0)

void (*COST_TOKEN_SETTERS[])(char **) = {
	set_cost_token_0, set_cost_token_1, set_cost_token_2, set_cost_token_3,
	set_cost_token_4, set_cost_token_5, set_cost_token_6, set_cost_token_7,
	set_cost_token_8
};

struct JoinSetters TOP_LINE_JOIN_SETTERS = {
	.left   = set_top_left_join,
	.center = set_top_center_join,
	.right  = set_top_right_join
};

struct JoinSetters MID_LINE_JOIN_SETTERS = {
	.left   = set_mid_left_join,
	.center = set_mid_center_join,
	.right  = set_mid_right_join
};

struct JoinSetters BOT_LINE_JOIN_SETTERS = {
	.left   = set_bot_left_join,
	.center = set_bot_center_join,
	.right  = set_bot_right_join
};


extern inline void set_start_token(char **dbl_ptr);
extern inline void set_goal_token(char **dbl_ptr);

/* LINE JOIN SETTER FUNCTIONS
 ******************************************************************************/
/* top line
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐ */
void set_top_left_join(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;
	PUT_BOX_CHAR_LIGHT_NW_CORNER(ptr);
	*dbl_ptr = ptr;
}
void set_top_center_join(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;
	PUT_BOX_CHAR_LIGHT_N_JOIN(ptr);
	*dbl_ptr = ptr;
}
void set_top_right_join(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;
	PUT_BOX_CHAR_LIGHT_NE_CORNER(ptr);
	*dbl_ptr = ptr;
}

/* mid line
 * ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤ */
void set_mid_left_join(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;
	PUT_BOX_CHAR_LIGHT_W_JOIN(ptr);
	*dbl_ptr = ptr;
}
void set_mid_center_join(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;
	PUT_BOX_CHAR_LIGHT_CROSS(ptr);
	*dbl_ptr = ptr;
}
void set_mid_right_join(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;
	PUT_BOX_CHAR_LIGHT_E_JOIN(ptr);
	*dbl_ptr = ptr;
}

/* bot line
 * └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘ */
void set_bot_left_join(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;
	PUT_BOX_CHAR_LIGHT_SW_CORNER(ptr);
	*dbl_ptr = ptr;
}
void set_bot_center_join(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;
	PUT_BOX_CHAR_LIGHT_S_JOIN(ptr);
	*dbl_ptr = ptr;
}
void set_bot_right_join(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;
	PUT_BOX_CHAR_LIGHT_SE_CORNER(ptr);
	*dbl_ptr = ptr;
}



/* COMPLETE LINE SETTERS
 ******************************************************************************/
/* unbroken line (top, mid, or bot) */
void set_unbroken_line(char **dbl_ptr,
		       const size_t res_y,
		       struct JoinSetters *join_setters)
{
	char *ptr = *dbl_ptr;

	void (*set_left_join)(char **)	 = join_setters->left;
	void (*set_center_join)(char **) = join_setters->center;
	void (*set_right_join)(char **)  = join_setters->right;

	PUT_DEFAULT_COLORS(ptr);
	set_left_join(&ptr);
	PUT_HORIZONTAL_LINES(ptr);
	set_rem_unbroken_line(&ptr,
			      res_y - 1lu,
			      set_center_join,
			      set_right_join);
	*dbl_ptr = ptr;
}

/* unbroken line (top, mid, or bot) */
void set_line_with_token(char **dbl_ptr,
			 const size_t res_y,
			 const size_t token_y,
			 void (*token_setter)(char **),
			 struct JoinSetters *join_setters)
{
	char *ptr = *dbl_ptr;

	void (*set_left_join)(char **)	 = join_setters->left;
	void (*set_center_join)(char **) = join_setters->center;
	void (*set_right_join)(char **)  = join_setters->right;

	PUT_DEFAULT_COLORS(ptr);
	set_left_join(&ptr);

	for (size_t y = 0lu; y < token_y; ++y) {

		PUT_HORIZONTAL_LINES(ptr);
		set_center_join(&ptr);
	}

	/* set token */
	PUT_SPACE(ptr);
	token_setter(&ptr);
	PUT_RESET_DEFAULT(ptr);
	PUT_SPACE(ptr);

	/* set remainder of unbroken line */
	set_rem_unbroken_line(&ptr,
			      res_y - (token_y + 1lu),
			      set_center_join,
			      set_right_join);
	*dbl_ptr = ptr;
}

/* remaining unbroken line (top, mid, or bot) */
void set_rem_unbroken_line(char **dbl_ptr,
			   const size_t rem_y,
			   void (*set_center_join)(char **),
			   void (*set_right_join)(char **))
{
	char *ptr = *dbl_ptr;

	for (size_t y = 0lu; y < rem_y; ++y) {

		set_center_join(&ptr);
		PUT_HORIZONTAL_LINES(ptr);
	}

	set_right_join(&ptr);
	PUT_TERMINATE_LINE(ptr);

	*dbl_ptr = ptr;
}



/* COST TOKEN SETTER FUNCTIONS
 ******************************************************************************/
/* (lowest cost → highest cost) */
void set_cost_token_0(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;

	PUT_ANSI_FAINT(ptr);

#ifdef BLOCKS
	PUT_SPACE(ptr);
#else
	PUT_CHAR(ptr, '1');
#endif

	*dbl_ptr = ptr;
}
void set_cost_token_1(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;

	PUT_ANSI_FAINT(ptr);
	PUT_ANSI_BLACK(ptr);

#ifdef BLOCKS
	PUT_BLOCK_CHAR_BASE_FILL(ptr, 1);
#else
	PUT_CHAR(ptr, '2');
#endif

	*dbl_ptr = ptr;
}
void set_cost_token_2(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;

	PUT_ANSI_FAINT(ptr);
	PUT_ANSI_BLUE(ptr);

#ifdef BLOCKS
	PUT_BLOCK_CHAR_BASE_FILL(ptr, 2);
#else
	PUT_CHAR(ptr, '3');
#endif

	*dbl_ptr = ptr;
}
void set_cost_token_3(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;

	PUT_ANSI_CYAN(ptr);

#ifdef BLOCKS
	PUT_BLOCK_CHAR_BASE_FILL(ptr, 3);
#else
	PUT_CHAR(ptr, '4');
#endif

	*dbl_ptr = ptr;
}
void set_cost_token_4(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;

	PUT_ANSI_GREEN(ptr);

#ifdef BLOCKS
	PUT_BLOCK_CHAR_BASE_FILL(ptr, 4);
#else
	PUT_CHAR(ptr, '5');
#endif

	*dbl_ptr = ptr;
}
void set_cost_token_5(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;

	PUT_ANSI_YELLOW(ptr);

#ifdef BLOCKS
	PUT_BLOCK_CHAR_BASE_FILL(ptr, 5);
#else
	PUT_CHAR(ptr, '6');
#endif

	*dbl_ptr = ptr;
}
void set_cost_token_6(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;

	PUT_ANSI_BRIGHT(ptr);
	PUT_ANSI_RED(ptr);

#ifdef BLOCKS
	PUT_BLOCK_CHAR_BASE_FILL(ptr, 6);
#else
	PUT_CHAR(ptr, '7');
#endif

	*dbl_ptr = ptr;
}
void set_cost_token_7(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;

	PUT_ANSI_BRIGHT(ptr);
	PUT_ANSI_MAGENTA(ptr);

#ifdef BLOCKS
	PUT_BLOCK_CHAR_BASE_FILL(ptr, 7);
#else
	PUT_CHAR(ptr, '8');
#endif

	*dbl_ptr = ptr;
}
void set_cost_token_8(char **dbl_ptr)
{
	char *ptr = *dbl_ptr;

	PUT_ANSI_BRIGHT(ptr);
	PUT_ANSI_BLACK(ptr);

#ifdef BLOCKS
	PUT_BLOCK_CHAR_BASE_FILL(ptr, 8);
#else
	PUT_CHAR(ptr, '9');
#endif


	*dbl_ptr = ptr;
}

/* COMPLETE COST TOKEN ROW SETTERS
 ******************************************************************************/
/* set unbroken row of cost tokens */
void set_unbroken_cost_row(char **dbl_ptr,
			   const size_t res_y,
			   const int min_cost,
			   const double token_ratio,
			   int *cost_row)
{
	char *ptr = *dbl_ptr;

	PUT_DEFAULT_COLORS(ptr);
	PUT_VERTICAL_LINE(ptr);
	set_rem_unbroken_cost_row(&ptr,
				  0lu,
				  res_y,
				  min_cost,
				  token_ratio,
				  cost_row);
	*dbl_ptr = ptr;
}


/* set row of cost tokens containing either a 'start' or 'goal' token */
void set_cost_row_with_token(char **dbl_ptr,
			     const size_t res_y,
			     const size_t token_y,
			     const int min_cost,
			     const double token_ratio,
			     void (*token_setter)(char **),
			     int *cost_row)
{
	char *ptr = *dbl_ptr;

	PUT_DEFAULT_COLORS(ptr);

	for (size_t y = 0lu; y < token_y; ++y) {
		PUT_VERTICAL_LINE(ptr);
		PUT_SPACE(ptr);
		set_cost_token(&ptr,
			       cost_row[y],
			       min_cost,
			       token_ratio);
		PUT_RESET_DEFAULT(ptr);
		PUT_SPACE(ptr);
	}

	/* set token (no need to pad) */
	token_setter(&ptr);
	PUT_RESET_DEFAULT(ptr);

	/* set remainder of unbroken line */
	set_rem_unbroken_cost_row(&ptr,
				  token_y,
				  res_y,
				  min_cost,
				  token_ratio,
				  cost_row);
	*dbl_ptr = ptr;
}


/* set remainder of cost (not start/goal) tokens according to 'cost_row' */
void set_rem_unbroken_cost_row(char **dbl_ptr,
			       size_t y,
			       const size_t res_y,
			       const int min_cost,
			       const double token_ratio,
			       int *cost_row)
{
	char *ptr = *dbl_ptr;

	while (y < res_y) {

		PUT_SPACE(ptr);
		set_cost_token(&ptr,
			       cost_row[y],
			       min_cost,
			       token_ratio);
		PUT_RESET_DEFAULT(ptr);
		PUT_SPACE(ptr);
		PUT_VERTICAL_LINE(ptr);

		++y;
	}

	PUT_TERMINATE_LINE(ptr);

	*dbl_ptr = ptr;
}


/* interpolate cost to index for access to setter functions array,
 * 'COST_TOKEN_SETTERS' */
inline void set_cost_token(char **dbl_ptr,
			   const int cost,
			   const int min_cost,
			   const double token_ratio)
{

	const double interp_i = ((double) (cost - min_cost)) * token_ratio;

	void (*setter)(char **) = COST_TOKEN_SETTERS[(size_t) round(interp_i)];

	setter(dbl_ptr);
}
