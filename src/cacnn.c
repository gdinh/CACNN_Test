#include "cacnn.h"
#define min( a, b ) (a < b ? a : b)
#define CACHESIZE 8000
// Communication Avoiding Convolution
int convolve_cacnn
(
	float* in, float* out, float** filters, uint32_t C, uint32_t K, uint32_t W,
	uint32_t H, uint32_t R, uint32_t S, uint32_t sigmaW, uint32_t sigmaH,
	uint32_t C_block, uint32_t K_block, uint32_t W_block, uint32_t H_block,
	uint32_t RP_block, uint32_t RPP_block, uint32_t SP_block, uint32_t SPP_block
)
{
	//set up the list for cache tracking
	std::list<unsigned long long int> simulated_cache;
	unsigned int cache_misses = 0;
	unsigned long long int index_o, index_i, index_f;
	unsigned int current_cache_count, prior_cache_count;

	// Main Iterations
	uint32_t c,   k,   w,   h,   rp,   rpp,   sp,   spp;
	uint32_t c_b, k_b, w_b, h_b, rp_b, rpp_b, sp_b, spp_b;
	uint32_t c_p, k_p, w_p, h_p, rp_p, rpp_p, sp_p, spp_p;

	uint32_t r_bound = R/sigmaW;
	uint32_t s_bound = S/sigmaH;
	uint32_t in_H = sigmaH*(H - 1) + S;
	uint32_t in_W = sigmaW*(W - 1) + R;

	// Blocking
	for ( c_b = 0; c_b < C; c_b += C_block )
	{
		for ( k_b = 0; k_b < K; k_b += K_block )
		{
			for ( w_b = 0; w_b < W; w_b += W_block )
			{
				for ( h_b = 0; h_b < H; h_b += H_block )
				{
					for ( rp_b = 0; rp_b < r_bound; rp_b += RP_block )
					{
						for ( rpp_b = 0; rpp_b < sigmaW; rpp_b += RPP_block )
						{
							for ( sp_b = 0; sp_b < s_bound; sp_b += SP_block )
							{
								for ( spp_b = 0; spp_b < sigmaH; spp_b += SPP_block )
								{

	// Piecing
	for ( c_p = 0; c_p < min( C - c_b, C_block ); c_p += 1 )
	{
		for ( k_p = 0; k_p < min( K - k_b, K_block ); k_p += 1 )
		{
			for ( w_p = 0; w_p < min( W - w_b, W_block ); w_p += 1 )
			{
				for ( h_p = 0; h_p < min( H - h_b, H_block ); h_p += 1 )
				{
					for ( rp_p = 0; rp_p < min( r_bound - rp_b, RP_block ); rp_p += 1 )
					{
						for ( rpp_p = 0; rpp_p < min( sigmaW - rpp_b, RPP_block ); rpp_p += 1 )
						{
							for ( sp_p = 0; sp_p < min( s_bound - sp_b, SP_block ); sp_p += 1 )
							{
								for ( spp_p = 0; spp_p < min( sigmaH - spp_b, SPP_block ); spp_p += 1 )
								{
									c   = c_b   + c_p;
									k   = k_b   + k_p;
									w   = w_b   + w_p;
									h   = h_b   + h_p;
									rp  = rp_b  + rp_p;
									rpp = rpp_b + rpp_p;
									sp  = sp_b  + sp_p;
									spp = spp_b + spp_p;
									index_i = (c*in_H*in_W) + (spp + sigmaH*(sp + h))*(in_W) + (rpp + sigmaW*(rp + w));
									index_o = k*W*H + h*W + w;
									index_f = k + K * ((c*R*S) + (sigmaH*sp + spp)*(R) + (sigmaW*rp + rpp) );

									//offset everything by two bits and then edit the trailing bits so they're distinguishable
									index_i = index_i << 2;
									index_o = (index_o << 2) + 1;
									index_f = (index_f << 2) + 2;

									//now test if each of these are in the fake cache:

									prior_cache_count = simulated_cache.size();
 									simulated_cache.remove(index_i);
									current_cache_count = simulated_cache.size();
									if(prior_cache_count == current_cache_count){
										//CACHE MISS
										//NOTHING DELETED
										cache_misses++;
										//put the element in the cache
										simulated_cache.emplace_back(index_i);
										//remove LRU if cache too big
										current_cache_count++;
										if(current_cache_count > CACHESIZE){
											simulated_cache.pop_front();
											current_cache_count--;
										}
									}else{
										//CACHE HIT. TACK IT ON THE END TO AVOID LRU REMOVAL. NO MODS.
										simulated_cache.emplace_back(index_i);
									}

									prior_cache_count = current_cache_count;
 									simulated_cache.remove(index_o);
									current_cache_count = simulated_cache.size();
									if(prior_cache_count == current_cache_count){
										//CACHE MISS
										//NOTHING DELETED
										cache_misses++;
										//put the element in the cache
										simulated_cache.emplace_back(index_o);
										//remove LRU if cache too big
										current_cache_count++;
										if(current_cache_count > CACHESIZE){
											simulated_cache.pop_front();
											current_cache_count--;
										}
									}else{
										//CACHE HIT. TACK IT ON THE END TO AVOID LRU REMOVAL. NO MODS.
										simulated_cache.emplace_back(index_o);
									}

									prior_cache_count = current_cache_count;
 									simulated_cache.remove(index_f);
									current_cache_count = simulated_cache.size();
									if(prior_cache_count == current_cache_count){
										//CACHE MISS
										//NOTHING DELETED
										cache_misses++;
										//put the element in the cache
										simulated_cache.emplace_back(index_f);
										//remove LRU if cache too big
										current_cache_count++;
										if(current_cache_count > CACHESIZE){
											simulated_cache.pop_front();
											current_cache_count--;
										}
									}else{
										//CACHE HIT. TACK IT ON THE END TO AVOID LRU REMOVAL. NO MODS.
										simulated_cache.emplace_back(index_f);
									}

									// float u = filters[k][ (c*R*S) + (sigmaH*sp + spp)*(R) + (sigmaW*rp + rpp) ];
									// float v = in[ (c*in_H*in_W) + (spp + sigmaH*(sp + h))*(in_W) + (rpp + sigmaW*(rp + w)) ];
									// out[ k*W*H + h*W + w ] += u*v;
								}
							}
						}
					}
				}
			}
		}
	}
								}
							}
						}
					}
				}
			}
		}
	}

	printf("");
	return 0;
}