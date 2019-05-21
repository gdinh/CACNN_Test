#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include "constats.h"
#include "convolve.h"
#include "cacnn.h"
#include "carma.h"

uint32_t __C;
uint32_t __K;
uint32_t __W;
uint32_t __H;
uint32_t __R;
uint32_t __S;
uint32_t __SIGMAW;
uint32_t __SIGMAH;

uint32_t __C_B;
uint32_t __K_B;
uint32_t __W_B;
uint32_t __H_B;
uint32_t __RP_B;
uint32_t __RPP_B;
uint32_t __SP_B;
uint32_t __SPP_B;

uint32_t __TRIALS;
uint32_t __L1_SIZE;

int compare (const void * a, const void * b)
{
	if ( *(double*)a > *(double*)b )
		return 1;

	else if ( *(double*)a < *(double*)b )
		return -1;

	else
		return 0;
}

double median ( double* data, uint32_t size )
{
	qsort( data, size, sizeof( double ), compare );

	if ( size % 2 == 0 )
		return data[ size / 2 ];

	else
		return ( data[ size / 2 ] + data[ size / 2 + 1 ] ) / 2;
}

double mean ( double* data, uint32_t size )
{
	 double sum = 0;
	 uint32_t i;

	 for ( i = 0; i < size; ++i )
	 	sum += data[ i ];

	 return sum / (double) size;
}

double read_timer ( void )
{
	static int initialized = 0;
	static struct timeval start;
	struct timeval end;
	if( initialized == 0 )
	{
		gettimeofday( &start, NULL );
		initialized = 1;
	}
	gettimeofday( &end, NULL );
	return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

// Creates filters for testing
int create_filters ( float*** out, uint32_t filter_width, uint32_t filter_height, uint32_t filter_count, uint32_t filter_channels )
{
	int ret = 0;
	uint32_t f, i, j, z;
	uint32_t filters_allocated = 0;

	(*out) = (float**) malloc( sizeof(float*) * filter_count );

	if ( (*out) == NULL )
	{
		ret = -ENOMEM;
		goto fail;
	}

	for ( f = 0; f < filter_count; f++ )
	{
		float* filter = (float*) malloc( sizeof(float) * filter_width * filter_height * filter_channels );

		if ( filter == NULL )
		{
			ret = -ENOMEM;
			goto fail;
		}

		filters_allocated++;

		for ( z = 0; z < filter_channels; ++z )
		{
			for ( i = 0; i < filter_height; ++i )
			{
				for ( j = 0; j < filter_width; ++j )
				{
					filter[ (z * filter_height * filter_width) + (i * filter_width) + j ] = 1.0f;//1.0f/(float)(filter_width*filter_height);//((float) rand()) / ((float) RAND_MAX );
				}
			}
		}

		(*out)[ filters_allocated - 1 ] = filter;
	}

	exit:
		return ret;
	fail:
		for ( i = 0; i < filters_allocated; ++i )
			free( (*out)[i] );
		free( (*out) );
		return ret;
}

// Creates input images for testing
int create_image ( float** out, uint32_t out_width, uint32_t out_height, uint32_t out_channels )
{
	int ret = 0;
	uint32_t i, j, z;

	(*out) = (float*) malloc( sizeof(float) * out_width * out_height * out_channels );

	if ( (*out) == NULL )
	{
		ret = -ENOMEM;
		goto fail;
	}

	for ( z = 0; z < out_channels; ++z )
	{
		for ( i = 0; i < out_height; ++i )
		{
			for ( j = 0; j < out_width; ++j )
			{
				(*out)[ (z * out_height * out_width) + (i * out_width) + j ] = 1.0f;//((float) rand()) / ((float) RAND_MAX );
			}
		}
	}

	exit:
		return ret;
	fail:
		free( (*out) );
		return ret;
}

// Image to Column Matrix format
int im2col
(
	float* in, uint32_t in_width, uint32_t in_height, uint32_t in_channels,
	uint32_t filter_width, uint32_t filter_height, uint32_t filter_count, uint32_t sigmaw, uint32_t sigmah,
	float** out, uint32_t* out_rows, uint32_t* out_cols
)
{
	int ret = 0;

	// Calculate out dimensions
	uint32_t out_width = ( ( ( in_width - filter_width ) / sigmaw ) + 1 );
	uint32_t out_height = ( ( ( in_height - filter_height ) / sigmah ) + 1 );
	*out_cols = in_channels * filter_width * filter_height;
	*out_rows = out_height * out_width;

	// Allocate out memory
	(*out) = (float*) malloc( sizeof(float) * (*out_rows) * (*out_cols) );

	if ( (*out) == NULL )
	{
		ret = -ENOMEM;
		goto exit;
	}
	// printf("hi\n");
	uint32_t z, x, y, i, j;

	for ( x = 0; x < *out_rows; ++x )
	{
		for ( y = 0; y < *out_cols; ++y )
		{
			(*out)[ x * (*out_cols) + y ] = 1.0f;
		}
	}

	// for ( z = 0; z < in_channels; ++z )
	// {
	// 	// printf("z: %d\n", z);
	// 	for ( y = 0; y < in_height - filter_height + 1; y += sigmah )
	// 	{
	// 		// printf("y: %d\n", y);
	// 		for ( x = 0; x < in_width - filter_width + 1; x += sigmaw )
	// 		{
	// 			// printf("x: %d\n", x);
	// 			for ( j = 0; j < filter_height; ++j )
	// 			{
	// 				for ( i = 0; i < filter_width; ++i )
	// 				{
	// 					// Looks like skipping rows with stride, prob shouldnt be doing that
	// 					// printf( "row: %d, col: %d\n", ( y*out_width + x ), ( (z*filter_width*filter_height) + (j*filter_width) + (i) ) );
	// 					(*out)[ ( ( (y/sigmah)*out_width + x ) * (*out_cols) )
	// 							+ ( (z*filter_width*filter_height) + (j*filter_width) + (i) ) ]
	// 							= in[ (z*in_width*in_height) + ((y+j)*in_width) + ((x+i)) ];
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	exit:
		return ret;
}

// Kernels to Column Matrix format (?format name?)
int ker2col
(
	float** filters, uint32_t filter_width, uint32_t filter_height, uint32_t filter_count, uint32_t filter_channels,
	float** out, uint32_t* out_rows, uint32_t* out_cols
)
{
	int ret = 0;

	// Calculate out dimensions
	*out_rows = filter_channels * filter_width * filter_height;
	*out_cols = filter_count;

	// Allocate out memory
	(*out) = (float*) malloc( sizeof(float) * (*out_rows) * (*out_cols) );

	if ( (*out) == NULL )
	{
		ret = -ENOMEM;
		goto exit;
	}

	uint32_t f, i, j, z, x, y;

	for ( x = 0; x < *out_rows; ++x )
	{
		for ( y = 0; y < *out_cols; ++y )
		{
			(*out)[ x * (*out_cols) + y ] = 1.0f;
		}
	}

	// for ( z = 0; z < filter_channels; ++ z )
	// {
	// 	for ( i = 0; i < filter_height; ++i )
	// 	{
	// 		for ( j = 0; j < filter_width; ++j )
	// 		{
	// 			for ( f = 0; f < filter_count; ++f )
	// 			{
	// 				(*out)[ ( (z*filter_height*filter_width + i*filter_width + j) * (*out_cols) ) + f ]
	// 					= filters[f][ z*filter_height*filter_width + i*filter_width + j ];
	// 			}
	// 		}
	// 	}
	// }

	exit:
		return ret;
}

int time_test ( void )
{

}

int main ( int argc, const char* argv[] )
{
	if ( argc < 18 )
	{
		fprintf( stderr, "Need to specifiy parameters.\n" );
		return -1;
	}

	 __C       = atoi(argv[1]);
	 __K       = atoi(argv[2]);
	 __W       = atoi(argv[3]);
	 __H       = atoi(argv[4]);
	 __R       = atoi(argv[5]);
	 __S       = atoi(argv[6]);
	 __SIGMAW  = atoi(argv[7]);
	 __SIGMAH  = atoi(argv[8]);

	 __C_B     = atoi(argv[9]);
	 __K_B     = atoi(argv[10]);
	 __W_B     = atoi(argv[11]);
	 __H_B     = atoi(argv[12]);
	 __RP_B    = atoi(argv[13]);
	 __RPP_B   = atoi(argv[14]);
	 __SP_B    = atoi(argv[15]);
	 __SPP_B   = atoi(argv[16]);

	 __TRIALS  = atoi(argv[17]);
	 __L1_SIZE = atoi(argv[18]);

	 // Test Prep
	 	// Create Input
	 	// Create Filters
	 	// Allocate Out
	 	// Create Input_Matrix
	 	// Create Ker_Matrix
	 	// Allocate Out_Matrix
	 	// Create Junk Array
	 	// Create Data Space
	 // For each algorithm in {im2col, convolve_std, convolve_cacnn}
	 	// Repeat TRIALS times
	 		// Load All Data (touch input, zero output)
	 		// Load Junk Data (clear L1)
	 		// Start time
	 		// Run Algorithm
	 		// Stop time
	 // Print results

	 int ret;
	 uint32_t i = 0;
	 volatile float checksum = 0.0f;

	 // Test Prep
	 // Create Input
	 uint32_t in_width     = __SIGMAW * ( __W - 1) + __R;
	 uint32_t in_height    = __SIGMAH * ( __H - 1) + __S;
	 uint32_t in_channels  = __C;
	 float*   in = NULL;

	 if ( create_image ( &in, in_width, in_height, in_channels ) )
	 {
	 	fprintf( stderr, "Failed to create input image.\n" );
	 	ret = -1;
	 	goto fail0;
	 }

	 // Create Filters
	 uint32_t filter_width    = __R;
	 uint32_t filter_height   = __S;
	 uint32_t filter_channels = in_channels;
	 uint32_t filter_count    = __K;
	 float**  filters         = NULL;

	 if ( create_filters( &filters, filter_width, filter_height, filter_count, filter_channels ) )
	 {
	 	fprintf( stderr, "Failed to create filters.\n" );
	 	ret = -1;
	 	goto fail1;
	 }

	 // Allocate Out
	 uint32_t out_width    = __W;
	 uint32_t out_height   = __H;
	 uint32_t out_channels = __K;
	 float*   out          = (float*) malloc( sizeof(float) * out_channels * out_height * out_width );

	 if ( out == NULL )
	 {
	 	fprintf( stderr, "Failed to allocate out matrix.\n" );
	 	ret = -ENOMEM;
	 	goto fail2;
	 }

	 // Create Input_Matrix
	 uint32_t in_rows;
	 uint32_t in_cols;
	 float*   in_matrix = NULL;

	 if ( im2col( in, in_width, in_height, in_channels, filter_width, filter_height, filter_count, __SIGMAW, __SIGMAH, &in_matrix, &in_rows, &in_cols ) )
	 {
	 	fprintf( stderr, "Failed to convert image to matrix.\n" );
	 	ret = -1;
	 	goto fail3;
	 }

	 // Create Ker_Matrix
	 uint32_t ker_rows;
	 uint32_t ker_cols;
	 float*   ker_matrix = NULL;

	 if ( ker2col( filters, filter_width, filter_height, filter_count, in_channels, &ker_matrix, &ker_rows, &ker_cols ) )
	 {
	 	fprintf( stderr, "Failed to convert kernels to matrix.\n" );
	 	ret = -1;
	 	goto fail4;
	 }

	 // Allocate Out_Matrix
	 uint32_t out_rows = in_rows;
	 uint32_t out_cols = ker_cols;
	 float*   out_matrix = NULL;

	 out_matrix = (float*) malloc( sizeof(float) * out_rows * out_cols );

	 if ( out_matrix == NULL )
	 {
	 	ret = -ENOMEM;
	 	goto fail5;
	 }

	 // Create Junk Array
	 float* junk = (float*) malloc( __L1_SIZE );

	 if ( junk == NULL )
	 {
	 	fprintf( stderr, "Failed to create junk array.\n" );
	 	ret = -ENOMEM;
	 	goto fail6;
	 }

	 // Create Data Space
	 double* data_std    = (double*) malloc( sizeof(double) * __TRIALS );
	 double* data_cacnn  = (double*) malloc( sizeof(double) * __TRIALS );
	 double* data_im2col = (double*) malloc( sizeof(double) * __TRIALS );


	 // For each algorithm in {im2col, convolve_std, convolve_cacnn}: convolve_std
	 // Repeat TRIALS times
	 uint64_t a, b, c, d;
	 for ( i = 0; i < __TRIALS; ++i )
	 {
	 	// Load All Data (touch input, zero output)
	 	for ( c = 0; c < in_channels; ++c )
	 		for ( b = 0; b < in_height; ++b )
	 			for ( a = 0; a < in_width; ++a )
	 				checksum += in[ c*(in_width*in_height) + b*(in_width) + a ];

	 	for ( d = 0; d < filter_count; ++d )
	 		for ( c = 0; c < filter_channels; ++c )
	 			for ( b = 0; b < filter_height; ++b )
	 				for ( a = 0; a < filter_width; ++a )
	 					checksum += filters[d][ c*(filter_width*filter_height) + a*filter_width + b ];

	 	memset( out, 0, out_channels * out_width * out_height );

	 	// Load Junk Data (clear L2)
	 	for ( a = 0; a < __L1_SIZE / sizeof(float); ++a )
	 		checksum += junk[a];

	 	// Start time
	 	volatile double start = read_timer();

	 	// Run Algorithm
	 	convolve_std( in, out, filters, in_channels, out_channels, out_width, out_height, filter_width, filter_height, __SIGMAW, __SIGMAH );

	 	// Stop time
	 	volatile double end = read_timer();
	 	data_std[i] = end - start;
	 }

	 // For each algorithm in {im2col, convolve_std, convolve_cacnn}: convolve_cacnn
	 for ( i = 0; i < __TRIALS; ++i )
	 {
	 	// Load All Data (touch input, zero output)
	 	for ( c = 0; c < in_channels; ++c )
	 		for ( b = 0; b < in_height; ++b )
	 			for ( a = 0; a < in_width; ++a )
	 				checksum += in[ c*(in_width*in_height) + b*(in_width) + a ];

	 	for ( d = 0; d < filter_count; ++d )
	 		for ( c = 0; c < filter_channels; ++c )
	 			for ( b = 0; b < filter_height; ++b )
	 				for ( a = 0; a < filter_width; ++a )
	 					checksum += filters[d][ c*(filter_width*filter_height) + a*filter_width + b ];

	 	memset( out, 0, out_channels * out_width * out_height );

	 	// Load Junk Data (clear L2)
	 	for ( a = 0; a < __L1_SIZE / sizeof(float); ++a )
	 		checksum += junk[a];

	 	// Start time
	 	volatile double start = read_timer();

	 	// Run Algorithm
	 	convolve_cacnn( in, out, filters, in_channels, out_channels, out_width,
	     	            out_height, filter_width, filter_height, __SIGMAW, __SIGMAH,
	         	        __C_B, __K_B, __W_B, __H_B, __RP_B, __RPP_B, __SP_B, __SPP_B );

	 	// Stop time
	 	volatile double end = read_timer();
	 	data_cacnn[i] = end - start;
	 }

	 // // For each algorithm in {im2col, convolve_std, convolve_cacnn}: im2col
	 for ( i = 0; i < __TRIALS; ++i )
	 {
	 	// Load All Data (touch input, zero output)
	 	for ( c = 0; c < in_rows; ++c )
	 		for ( b = 0; b < in_cols; ++b )
	 				checksum += in_matrix[ c*(in_cols) + b ];

	 	for ( c = 0; c < ker_rows; ++c )
	 		for ( b = 0; b < ker_cols; ++b )
	 				checksum += ker_matrix[ c*(ker_cols) + b ];

	 	memset( out_matrix, 0, out_rows * out_cols );

	 	// Load Junk Data (clear L2)
	 	for ( a = 0; a < __L1_SIZE / sizeof(float); ++a )
	 		checksum += junk[a];

	 	// Start time
	 	volatile double start = read_timer();

	 	// Run Algorithm

	 	multiply( in_rows, ker_rows, ker_cols, in_matrix, ker_matrix, out_matrix, 10 );

	 	// Stop time
	 	volatile double end = read_timer();
	 	data_im2col[i] = end - start;
	 }

	 // Print results
	 // printf( "Standard Convolution Timing (Seconds)\n" );
	 // printf( "mean = %lf, median = %lf\n", mean( data_std, __TRIALS ), median( data_std, __TRIALS ) );

	 // printf( "Communication Avoiding Convolution Timing (Seconds)\n" );
	 // printf( "mean = %lf, median = %lf\n", mean( data_cacnn, __TRIALS ), median( data_cacnn, __TRIALS ) );

	 // printf( "Im2Col Matrix Multiplication Timing (Seconds)\n" );
	 // printf( "mean = %lf, median = %lf\n", mean( data_im2col, __TRIALS ), median( data_im2col, __TRIALS ) );

	 fail7:
	 	free( data_std );
	 	free( data_cacnn );
	 	free( data_im2col );
	 fail6:
	 	free( out_matrix );
	 fail5:
	 	free( ker_matrix );
	 fail4:
	 	free( in_matrix );
	 fail3:
	 	free( out );
	 fail2:
	 	for ( i = 0; i < filter_count; ++i )
	 		free( filters[i] );
	 	free( filters );
	 fail1:
	 	free( in );
	 fail0:
	 	return ret;
}
