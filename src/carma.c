// Source: https://github.com/dose78/CARMA/blob/master/carma_single.c

#include "carma.h"
#define SPLIT_M 1
#define SPLIT_K 2
#define SPLIT_N 3

// Split largest dimension
int dim_to_split ( int m, int k, int n )
{
	if ( n >= k && n >= m ) return SPLIT_N;
	if ( m >= k && m >= n ) return SPLIT_M;
	return SPLIT_K;
}

static linked_hash_set<unsigned long long int> simulated_cache;
static unsigned int cache_misses = 0;
static unsigned long long int num_iter = 0;
static unsigned int current_cache_count, prior_cache_count;

static void access ( uint64_t address )
{
	num_iter++;
	prior_cache_count = simulated_cache.size();
	simulated_cache.erase(address);
	current_cache_count = simulated_cache.size();
	if(prior_cache_count == current_cache_count){
		//CACHE MISS
		//NOTHING DELETED
		cache_misses++;
		//put the element in the cache
		simulated_cache.insert(address);
		//remove LRU if cache too big
		if(current_cache_count > CACHESIZE){
			simulated_cache.erase(std::prev(simulated_cache.end()));
		}
	}else{
		//CACHE HIT. TACK IT ON THE END TO AVOID LRU REMOVAL. NO MODS.
		simulated_cache.insert(address);
	}
}

void inner_multiply ( int m, int k, int n, float *A, int LDA, float *B, int LDB, float *C, int LDC, int depth, int max_depth )
{
	//begin benchmarking shibboleth
	//set up the list for cache tracking


	if ( depth >= max_depth || m <= 4 || n <= 4 || k <= 4 )
	{
		for ( int i = 0; i < m; ++i )
			for ( int j = 0; j < n; ++j )
				for ( int l = 0; l < k; ++l )
				{
					access( reinterpret_cast<std::uintptr_t>( &A[ i*LDA + l ] ) );
					access( reinterpret_cast<std::uintptr_t>( &B[ l*LDB + j ] ) );
					access( reinterpret_cast<std::uintptr_t>( &C[ i*LDC + j ] ) );

					//C[ i*LDC + j ] += A[ i*LDA + l ] * B[ l*LDB + j ];
				}
		return;
	}

	int next_depth = depth + 1;
	int dim        = dim_to_split( m, k, n );

	if ( dim == SPLIT_N )
	{
		inner_multiply(m, k, n/2, A, LDA, B, LDB, C, LDC, next_depth, max_depth);
		inner_multiply(m, k, n/2, A, LDA, B + n/2, LDB, C + n/2, LDC, next_depth, max_depth);
	}
	else if ( dim == SPLIT_M )
	{
		inner_multiply(m/2, k, n, A, LDA, B, LDB, C, LDC, next_depth, max_depth);
		inner_multiply(m/2, k, n, A + m/2*LDA, LDA, B, LDB, C + m/2*LDC, LDC, next_depth, max_depth);
	}
	else // SPLIT_K
	{
		inner_multiply(m, k/2, n, A, LDA, B, LDB, C, LDC, next_depth, max_depth);
		inner_multiply(m, k/2, n, A + k/2, LDA, B + k/2*LDB, LDB, C, LDC, next_depth, max_depth);
	}

	return;
}

void multiply ( int m, int k, int n, float *A, float *B, float *C, int max_depth )
{
	inner_multiply( m, k, n, A, k, B, n, C, n, 0, max_depth );
	// printf("Cache size: %u\n", CACHESIZE);
	// printf("# of occupied spots after finishing (should be CACHESIZE): %u\n", current_cache_count);
	printf("# of cache misses: %u\n", cache_misses);
	// printf("# of iterations: %u\n", num_iter);
}