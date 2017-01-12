#include "utils/utils.h"   /* HANDLE_MALLOC */
#include "graphs.h"  /* typedefs */
#include "builder.h" /* protoypes */

#define INIT(VERT, ID, EDGE_COUNT)					\
do {									\
	HANDLE_MALLOC(VERT, sizeof(struct Vertex));			\
	VERT->id	  = ID;						\
	VERT->edge_count  = EDGE_COUNT;					\
	if (EDGE_COUNT > 0) {						\
		HANDLE_MALLOC(VERT->edges,				\
			      sizeof(struct Edge*) * EDGE_COUNT);	\
	} else {							\
		VERT->edges = NULL;					\
	}								\
} while (0)


#define SET_EDGES(VERT, COSTS, NEXTS)					\
do {									\
	int costs[] = COSTS;						\
	struct Vertex *nexts[] = NEXTS;					\
	set_edges(VERT, costs, nexts);					\
} while (0)

#define WRAP(...) __VA_ARGS__

#define TELEPORT_COST 5

/*				graph1
 *
 * currently 1 way from start to goal at cost of 42
 * ╔═══════╦═══════╦═══════╦═══════╦═══════╦═══════╦═══════╦═══════╦═══════╗
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ║               ║ TEL4B ║ DEAD6 ║ DEAD2           TEL5B ║         TEL2B ║
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ╠══   ══╬══   ══╬══   ══╬══   ══╬═══════╬═══════╬═══════╬══   ══╬═══════╣
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ║       ║               ║ TEL3B ║         VERT1         ║         DEAD9 ║
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ╠══   ══╬═══════╬═══════╬═══════╬══   ══╬══   ══╬══   ══╬═══════╬═══════╣
 * ║       ║       ║*     *║       ║       ║       ║       ║       ║       ║
 * ║               ║ GOAL! ║               ║       ║ DEAD8 ║ TEL6B         ║
 * ║       ║       ║*     *║       ║       ║       ║       ║       ║       ║
 * ╠═══════╬══   ══╬══   ══╬══   ══╬═══════╬══   ══╬═══════╬═══════╬══   ══╣
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ║ TEL5A   VERT4         ║       ║ TEL3A ║       ║ DEAD7         ║ TEL4A ║
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ╠═══════╬═══════╬═══════╬══   ══╬══   ══╬══   ══╬═══════╬══   ══╬═══════╣
 * ║       ║       ║       ║       ║       ║*     *║       ║       ║       ║
 * ║ TEL1B ║         TEL6A ║               ║ START           VERT2   DEAD1 ║
 * ║       ║       ║       ║       ║       ║*     *║       ║       ║       ║
 * ╠══   ══╬══   ══╬═══════╬═══════╬═══════╬══   ══╬═══════╬═══════╬═══════╣
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ║       ║       ║ TEL2A   VERT6         ║       ║         VERT5         ║
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ╠══   ══╬══   ══╬═══════╬══   ══╬══   ══╬══   ══╬══   ══╬══   ══╬══   ══╣
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ║         VERT7 ║ DEAD4 ║       ║       ║       ║ DEAD3 ║       ║       ║
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ╠═══════╬══   ══╬══   ══╬══   ══╬══   ══╬══   ══╬═══════╬══   ══╬══   ══╣
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ║ DEAD5         ║               ║         VERT3                 ║ TEL1A ║
 * ║       ║       ║       ║       ║       ║       ║       ║       ║       ║
 * ╚═══════╩═══════╩═══════╩═══════╩═══════╩═══════╩═══════╩═══════╩═══════╝
 */

struct Vertex *build_graph1(void)
{
	struct Vertex *start, *goal,
		    *vert1, *vert2, *vert3, *vert4, *vert5, *vert6, *vert7,
		    *tel1a, *tel2a, *tel3a, *tel4a, *tel5a, *tel6a,
		    *tel1b, *tel2b, *tel3b, *tel4b, *tel5b, *tel6b,
		    *dead1, *dead2, *dead3, *dead4, *dead5, *dead6, *dead7,
		    *dead8, *dead9;

	INIT(start, START, 3); INIT(goal,  _GOAL,  0); INIT(vert1, VERT1, 2);
	INIT(vert2, VERT2, 2); INIT(vert3, VERT3, 2); INIT(vert4, VERT4, 2);
	INIT(vert5, VERT5, 2); INIT(vert6, VERT6, 2); INIT(vert7, VERT7, 2);
	INIT(tel1a, TEL1A, 1); INIT(tel2a, TEL2A, 1); INIT(tel3a, TEL3A, 1);
	INIT(tel4a, TEL4A, 1); INIT(tel5a, TEL5A, 1); INIT(tel6a, TEL6A, 1);
	INIT(tel1b, TEL1B, 1); INIT(tel2b, TEL2B, 1); INIT(tel3b, TEL3B, 1);
	INIT(tel4b, TEL4B, 1); INIT(tel5b, TEL5B, 1); INIT(tel6b, TEL6B, 1);
	INIT(dead1, DEAD1, 0); INIT(dead2, DEAD2, 0); INIT(dead3, DEAD3, 0);
	INIT(dead4, DEAD4, 0); INIT(dead5, DEAD5, 0); INIT(dead6, DEAD6, 0);
	INIT(dead7, DEAD7, 0); INIT(dead8, DEAD8, 0); INIT(dead9, DEAD9, 0);

	SET_EDGES(start, WRAP({2, 3, 3}),	WRAP({vert2, vert1, vert3}));
	SET_EDGES(vert1, WRAP({2, 7}),		WRAP({dead8, tel3a}));
	SET_EDGES(vert2, WRAP({1, 2}),		WRAP({dead1, dead7}));
	SET_EDGES(vert3, WRAP({4, 4}),		WRAP({vert5, vert6}));
	SET_EDGES(vert4, WRAP({1, 2}),		WRAP({tel5a, goal }));
	SET_EDGES(vert5, WRAP({2, 3}),		WRAP({dead3, tel1a}));
	SET_EDGES(vert6, WRAP({1, 4}),		WRAP({tel2a, dead4}));
	SET_EDGES(vert7, WRAP({2, 3}),		WRAP({dead5, tel6a}));
	SET_EDGES(tel1a, WRAP({TELEPORT_COST}), WRAP({tel1b}));
	SET_EDGES(tel2a, WRAP({TELEPORT_COST}), WRAP({tel2b}));
	SET_EDGES(tel3a, WRAP({TELEPORT_COST}), WRAP({tel3b}));
	SET_EDGES(tel4a, WRAP({TELEPORT_COST}), WRAP({tel4b}));
	SET_EDGES(tel5a, WRAP({TELEPORT_COST}), WRAP({tel5b}));
	SET_EDGES(tel6a, WRAP({TELEPORT_COST}), WRAP({tel6b}));
	SET_EDGES(tel1b, WRAP({3}),		WRAP({vert7}));
	SET_EDGES(tel2b, WRAP({3}),		WRAP({dead9}));
	SET_EDGES(tel3b, WRAP({1}),		WRAP({dead6}));
	SET_EDGES(tel4b, WRAP({7}),		WRAP({vert4}));
	SET_EDGES(tel5b, WRAP({2}),		WRAP({dead2}));
	SET_EDGES(tel6b, WRAP({2}),		WRAP({tel4a}));

	return start;
}

void set_edges(struct Vertex *vert, int *costs, struct Vertex **nexts)
{
	const int edge_count = vert->edge_count;
	struct Edge *edge;

	for (int i = 0; i < edge_count; ++i) {

		HANDLE_MALLOC(edge, sizeof(struct Edge));

		edge->cost = costs[i];
		edge->next = nexts[i];

		vert->edges[i] = edge;
	}
}
