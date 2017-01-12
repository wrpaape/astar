#include "utils/utils.h"
#include "graphs.h"
#include "bheap.h"
#include "inspect.h"

const char *LABELS[] = {
	[START] = "start",    [_GOAL] = "goal!",    [VERT1] = "vertex 1",
	[VERT2] = "vertex 2", [VERT3] = "vertex 3", [VERT4] = "vertex 4",
	[VERT5] = "vertex 5", [VERT6] = "vertex 6", [VERT7] = "vertex 7",
	[TEL1A] = "teleporter entrance 1", [TEL2A] = "teleporter entrance 2",
	[TEL3A] = "teleporter entrance 3", [TEL4A] = "teleporter entrance 4",
	[TEL5A] = "teleporter entrance 5", [TEL6A] = "teleporter entrance 6",
	[TEL1B] = "teleporter exit 1",	   [TEL2B] = "teleporter exit 2",
	[TEL3B] = "teleporter exit 3",	   [TEL4B] = "teleporter exit 4",
	[TEL5B] = "teleporter exit 5",	   [TEL6B] = "teleporter exit 6",
	[DEAD1] = "dead end 1", [DEAD2] = "dead end 2", [DEAD3] = "dead end 3",
	[DEAD4] = "dead end 4", [DEAD5] = "dead end 5", [DEAD6] = "dead end 6",
	[DEAD7] = "dead end 7", [DEAD8] = "dead end 8", [DEAD9] = "dead end 9"
};

void tour_graph(struct Vertex *vert)
{
	struct Edge **edges;

	int edge_count, i, choice, total;
	char buffer[32];

	edge_count = vert->edge_count;
	edges      = vert->edges;
	total = 0;

	while (1) {

		printf("current vertex:\n   %s\noptions (cost):\n",
		       LABELS[vert->id]);

		for (i = 0; i < edge_count; ++i) {
			printf("    %d. %-25s (%d)\n",
			       i, LABELS[edges[i]->next->id], edges[i]->cost);
		}

		fgets(buffer, sizeof(char) * 4llu, stdin);

		if (buffer[0] == 'q')
			exit(0);

		choice = (int) strtol(buffer, NULL, 10);

		if ((choice < 0) ||
		    (choice >= edge_count)) {
			puts("invalid selection");
			continue;
		}

		total	  += edges[choice]->cost;
		vert       = edges[choice]->next;
		edge_count = vert->edge_count;

		if (edge_count == 0)
			break;

		edges = vert->edges;
	}

	printf("YOU %s!\n\ntotal cost: %d\n",
	       (vert->id == _GOAL) ? "WIN" : "LOSE", total);
}

void print_edge_bheap(struct BHeap *heap)
{
	void **nodes = heap->nodes;
	const size_t count = heap->count;

	for (size_t i = 1; i < count; ++i) {
		printf("nodes[%zu]: %d\n", i,
		       ((struct Edge *) nodes[i])->cost);
		fflush(stdout);
	}
}

void edge_to_string(char *buffer, const void *vedge)
{
	struct Edge *edge = (struct Edge *) vedge;

	if (edge->next != NULL)
		sprintf(buffer, "  next vertex: %s\n", LABELS[edge->next->id]);

	sprintf(buffer, "  cost:        %d\n", edge->cost);
}

