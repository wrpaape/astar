#ifndef GRAPHS_INSPECT_H_
#define GRAPHS_INSPECT_H_
void tour_graph(struct Vertex *vert);
void print_edge_bheap(struct BHeap *heap);

void edge_to_string(char *buffer, const void *vedge);

extern const char *LABELS[];
#endif /* ifndef GRAPHS_INSPECT_H_ */
