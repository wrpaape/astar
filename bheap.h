struct BHeap {
	size_t count;
	size_t alloc;
	void **nodes;
	int (*compare)(const void *,
		       const void *);
};

/* initialize, destroy, resize
 ******************************************************************************/
inline struct BHeap *init_bheap(int (*compare)(const void *,
					       const void *))
{
	struct BHeap *heap;

	HANDLE_MALLOC(heap, sizeof(struct BHeap));
	HANDLE_MALLOC(heap->nodes, sizeof(void *) * 2lu);

	/* include sentinel node at index 0 in 'count', i.e. count >= 1 */
	heap->count   = 1lu;
	heap->alloc   = 2lu;
	heap->compare = compare;

	return heap;
}

inline struct BHeap *init_bheap_with_size(int (*compare)(const void *,
							 const void *),
					  const size_t size)
{
	struct BHeap *heap;

	HANDLE_MALLOC(heap, sizeof(struct BHeap));
	HANDLE_MALLOC(heap->nodes, sizeof(void *) * size);

	/* include sentinel node at index 0 in 'count', i.e. count >= 1 */
	heap->count   = 1lu;
	heap->alloc   = size;
	heap->compare = compare;

	return heap;
}

inline void clear_bheap(struct BHeap *heap)
{
	heap->count = 1lu;
}

inline void free_bheap(struct BHeap *heap)
{
	free(heap->nodes);
	free(heap);
}

inline void resize_bheap(struct BHeap *heap,
			 const size_t size)
{
	void **nodes = realloc(heap->nodes, sizeof(void *) * size);

	if (nodes == NULL) {
		EXIT_ON_FAILURE("failed to reallocate number of nodes"
				"from %lu to %lu",
				heap->alloc, size);
	}

	heap->nodes = nodes;
	heap->alloc = size;
}



/* insertion
 ******************************************************************************/
void do_insert(void **nodes,
	       void *next,
	       const size_t next_i,
	       int (*compare)(const void *,
			      const void *));

void bheap_insert_array(struct BHeap *heap,
			const size_t length,
			void **array);

inline void bheap_insert(struct BHeap *heap,
			 void *next)
{
	do_insert(heap->nodes, next, heap->count, heap->compare);

	++(heap->count);

	if (heap->count == heap->alloc)
		resize_bheap(heap, heap->alloc * 2lu);
}




/* extraction
 ******************************************************************************/
void *bheap_extract(struct BHeap *heap);

void do_shift(void **nodes,
	      void *next,
	      const size_t next_i,
	      const size_t base_i,
	      int (*compare)(const void *,
			     const void *));



/* display
 ******************************************************************************/
void print_bheap(struct BHeap *heap,
		 void (*node_to_string)(char *, const void *));




/* convenience, misc
 ******************************************************************************/
struct BHeap *array_into_bheap(const size_t length,
			       void **array,
			       int (*compare)(const void *,
					      const void *));

static inline size_t next_pow_two(size_t num);
