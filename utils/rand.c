#include "utils/rand.h"

extern inline void init_rng(void);
extern inline bool coin_flip(void);
extern inline int32_t rand_in_int_range(const int32_t lbound,
					const int32_t rbound);
extern inline double rand_in_dub_range(const double lbound,
				       const double rbound);
