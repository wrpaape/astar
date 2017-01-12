#ifndef GRAPHS_UTILS_RAND_H_
#define GRAPHS_UTILS_RAND_H_
#include <pcg_basic.h> /* psuedorandom number generator */
#include <time.h>      /* unique seed */
#include <stdbool.h>

#define RNG_MAX UINT32_MAX

pcg32_random_t _RNG;

inline void init_rng(void)
{
	pcg32_srandom_r(&_RNG, time(NULL), (intptr_t)&_RNG);
}

inline bool coin_flip(void)
{
	return (bool) (pcg32_random_r(&_RNG) & 1u);
}

inline int32_t rand_in_int_range(const int32_t lbound,
				 const int32_t rbound)
{

	const uint32_t diff_plus_one = rbound - lbound + 1u;
	const uint32_t valid_limit   = RNG_MAX - (RNG_MAX % diff_plus_one);

	uint32_t rand;

	do {
		rand = pcg32_random_r(&_RNG);

	} while (rand > valid_limit);

	return ((int32_t) (rand % diff_plus_one)) + lbound;
}

inline double rand_in_dub_range(const double lbound,
				const double rbound)
{
	return (((double) pcg32_random_r(&_RNG)) / ((double) RNG_MAX))
	       * (rbound - lbound)
	       + lbound;

}
#endif /* ifndef GRAPHS_UTILS_RAND_H_ */
