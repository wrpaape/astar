#ifndef GRAPHS_GRAPHS_H_
#define GRAPHS_GRAPHS_H_
enum VertexID {
	START, _GOAL,
	VERT1, VERT2, VERT3, VERT4, VERT5, VERT6, VERT7,
	TEL1A, TEL2A, TEL3A, TEL4A, TEL5A, TEL6A,
	TEL1B, TEL2B, TEL3B, TEL4B, TEL5B, TEL6B,
	DEAD1, DEAD2, DEAD3, DEAD4, DEAD5, DEAD6, DEAD7, DEAD8, DEAD9
};

struct Vertex {
	enum VertexID id;
	int edge_count;
	struct Edge **edges;
};

struct Edge {
	int cost;
	struct Vertex *next;
};
#endif /* ifndef GRAPHS_GRAPHS_H_ */
