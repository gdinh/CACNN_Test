#ifndef CACNN_CARMA_TEST_LIB_
#define CACNN_CARMA_TEST_LIB_

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <cblas.h>
#include <linked_hash.hpp>

extern uint32_t CACHESIZE;

// CARMA
void multiply ( int m, int k, int n, float *A, float *B, float *C, int max_depth );

#endif
