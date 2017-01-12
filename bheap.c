#include "utils/utils.h"
#include "bheap.h"


/* initialize, destroy, resize
 ******************************************************************************/
extern inline struct BHeap *init_bheap(int (*compare)(const void *,
						      const void *));

extern inline void free_bheap(struct BHeap *heap);


extern inline void resize_bheap(struct BHeap *heap,
				const size_t size);




/* insertion
 ******************************************************************************/
extern inline void bheap_insert(struct BHeap *heap,
				void *next);

void bheap_insert_array(struct BHeap *heap,
			const size_t length,
			void **array)
{
	const size_t count = heap->count;
	const size_t next_count = count + length;

	if (heap->alloc < next_count)
		resize_bheap(heap, next_pow_two(next_count));

	void **nodes = heap->nodes;
	int (*compare)(const void *,
		       const void *) = heap->compare;


	for (size_t i = 0lu; i < length; ++i)
		do_insert(nodes, array[i], count + i, compare);

	heap->count = next_count;
}



void do_insert(void **nodes,
	       void *next,
	       const size_t next_i,
	       int (*compare)(const void *,
			      const void *))
{
	/* sentinel node has been reached, 'next' is new root node */
	if (next_i == 1lu) {
		nodes[1lu] = next;
		return;
	}


	const size_t parent_i = next_i / 2lu;
	void *parent	      = nodes[parent_i];

	if (compare(parent, next)) {
		nodes[next_i] = next;
		return;
	}

	nodes[next_i] = parent;
	do_insert(nodes, next, parent_i, compare);
}




/* extraction
 ******************************************************************************/
void *bheap_extract(struct BHeap *heap)
{

	if (heap->count == 1lu)
		return NULL;

	--(heap->count);

	const size_t base_i = heap->count;


	void **nodes = heap->nodes;
	void *root   = nodes[1lu];
	void *base   = nodes[base_i];

	do_shift(nodes, base, 1lu, base_i - 1lu, heap->compare);

	return root;
}

void do_shift(void **nodes,
	      void *next,
	      const size_t next_i,
	      const size_t base_i,
	      int (*compare)(const void *,
			     const void *))
{

	const size_t lchild_i = next_i * 2lu;

	/* if base level of heap has been reached (no more children), replace
	 **********************************************************************/
	if (lchild_i > base_i) {
		nodes[next_i] = next;
		return;
	}

	const size_t rchild_i = lchild_i + 1lu;

	void *lchild = nodes[lchild_i];

	/* compare left child with 'next':
	 *
	 * if 'lchild' belongs above 'next'...
	 **********************************************************************/
	if (compare(lchild, next)) {

		/* if base level of heap has been reached (no more children),
		 * place 'next' below 'lchild' and return
		 **************************************************************/
		if (rchild_i > base_i) {
			nodes[lchild_i] = next;
			nodes[next_i]	= lchild;
			return;
		}

		void *rchild = nodes[rchild_i];

		/* compare left child with right child:
		 *
		 * if 'lchild' belongs above 'rchild'...
		 **************************************************************/
		if (compare(lchild, rchild)) {
			/* place 'lchild' at 'next_i' and continue recursion
			 * down left branch
			 ******************************************************/
			nodes[next_i] = lchild;
			do_shift(nodes, next, lchild_i, base_i, compare);

		} else {
			/* place 'rchild' at 'next_i' and continue recursion
			 * down right branch
			 ******************************************************/
			nodes[next_i] = rchild;
			do_shift(nodes, next, rchild_i, base_i, compare);
		}
		return;
	}

	/* if base level of heap has been reached (no more children), place
	 * 'next' above 'lchild' (new base/last element) and return
	 **********************************************************************/
	if (rchild_i > base_i) {
		nodes[next_i] = next;
		return;
	}

	void *rchild = nodes[rchild_i];

	/* compare 'next' with right child:
	 *
	 * if 'rchild' belongs above 'next'...
	 **********************************************************************/
	if (compare(rchild, next)) {
		nodes[next_i] = rchild;
		do_shift(nodes, next, rchild_i, base_i, compare);
		return;
	}

	/* otherwise, 'next' belongs above lchild and rchild: place at 'next_i'
	 * and return
	 **********************************************************************/
	nodes[next_i] = next;
}




/* display
 ******************************************************************************/
void print_bheap(struct BHeap *heap,
		 void (*node_to_string)(char *, const void *))
{
	const size_t count = heap->count;

	if (count == 1lu) {
		puts("EMPTY");
		return;
	}

	void **nodes = heap->nodes;
	char buffer[256];

	for (size_t i = 1lu; i < count; ++i) {
		node_to_string(buffer, nodes[i]);
		printf("nodes[%zu]:\n%s\n", i, buffer);
	}
}




/* convenience, misc
 ******************************************************************************/
struct BHeap *array_into_bheap(const size_t length,
			       void **array,
			       int (*compare)(const void *,
					      const void *))
{
	struct BHeap *heap = init_bheap(compare);

	bheap_insert_array(heap, length, array);

	return heap;
}

inline size_t next_pow_two(size_t num)
{
	return 1lu << ((sizeof(size_t) * CHAR_BIT) - __builtin_clzl(num - 1lu));
}
