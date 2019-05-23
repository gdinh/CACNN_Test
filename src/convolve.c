#include "convolve.h"

// Standard Convolution
int convolve_std
(
	float* in, float* out, float** filters, uint32_t C, uint32_t K, uint32_t W,
	uint32_t H, uint32_t R, uint32_t S, uint32_t sigmaW, uint32_t sigmaH
)
{

	//begin benchmarking shibboleth
	//set up the list for cache tracking
	linked_hash_set<unsigned long long int> simulated_cache;
	unsigned int cache_misses = 0;
	unsigned long long int index_o, index_i, index_f, num_iter = 0;
	unsigned int current_cache_count, prior_cache_count;
	//end benchmarking shibboleth

	// Main Iterations
	uint32_t c, k, w, h, r, s;
	uint32_t in_H = sigmaH*(H - 1) + S;
	uint32_t in_W = sigmaW*(W - 1) + R;

	for ( c = 0; c < C; c++ )
	{
		for ( k = 0; k < K; k++ )
		{
			for ( w = 0; w < W; w++ )
			{
				for ( h = 0; h < H; h++ )
				{
					for ( r = 0; r < R; r++ )
					{
						for ( s = 0; s < S; s++ )
						{
							index_i = (c*in_H*in_W) + ((s + sigmaH*h)*in_W) + (r + sigmaW*w);
							index_o = k*W*H + h*W + w;
							index_f = k + K * (c*R*S + s*R + r  );

							//offset everything by two bits and then edit the trailing bits so they're distinguishable
							index_i = index_i << 2;
							index_o = (index_o << 2) + 1;
							index_f = (index_f << 2) + 2;

							//now test if each of these are in the fake cache:
							num_iter++;
							prior_cache_count = simulated_cache.size();
							simulated_cache.erase(index_i);
							current_cache_count = simulated_cache.size();
							if(prior_cache_count == current_cache_count){
								//CACHE MISS
								//NOTHING DELETED
								cache_misses++;
								//put the element in the cache
								simulated_cache.insert(index_i);
								//remove LRU if cache too big
								if(current_cache_count > CACHESIZE){
									simulated_cache.erase(std::prev(simulated_cache.end()));
								}
							}else{
								//CACHE HIT. TACK IT ON THE END TO AVOID LRU REMOVAL. NO MODS.
								simulated_cache.insert(index_i);
							}

							prior_cache_count = simulated_cache.size();
							simulated_cache.erase(index_o);
							current_cache_count = simulated_cache.size();
							if(prior_cache_count == current_cache_count){
								//CACHE MISS
								//NOTHING DELETED
								cache_misses++;
								//put the element in the cache
								simulated_cache.insert(index_o);
								//remove LRU if cache too big
								if(current_cache_count > CACHESIZE){
									simulated_cache.erase(std::prev(simulated_cache.end()));
								}
							}else{
								//CACHE HIT. TACK IT ON THE END TO AVOID LRU REMOVAL. NO MODS.
								simulated_cache.insert(index_o);
							}

							prior_cache_count = simulated_cache.size();
							simulated_cache.erase(index_f);
							current_cache_count = simulated_cache.size();
							if(prior_cache_count == current_cache_count){
								//CACHE MISS
								//NOTHING DELETED
								cache_misses++;
								//put the element in the cache
								simulated_cache.insert(index_f);
								//remove LRU if cache too big
								if(current_cache_count > CACHESIZE){
									simulated_cache.erase(std::prev(simulated_cache.end()));
								}
							}else{
								//CACHE HIT. TACK IT ON THE END TO AVOID LRU REMOVAL. NO MODS.
								simulated_cache.insert(index_f);
							}
							//float u = filters[k][ c*R*S + s*R + r ];
							//float v = in[ (c*in_H*in_W) + ((s + sigmaH*h)*in_W) + (r + sigmaW*w) ];
							//out[ k*W*H + h*W + w ] += u*v;
						}
					}
				}
			}
		}
	}
	// printf("Cache size: %u\n", CACHESIZE);
	// printf("# of occupied spots after finishing (should be CACHESIZE): %u\n", current_cache_count);
	printf("# of cache misses: %u\n", cache_misses);
	// printf("# of iterations: %u\n", num_iter);
	return 0;
}