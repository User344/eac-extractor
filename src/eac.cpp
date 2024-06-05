#include "eac.h"

void eac::decrypt( const void* input_ptr, size_t input_size, void* output_ptr,
				   size_t output_size ) {
	const auto input = static_cast< const uint8_t* >( input_ptr );
	const auto output = static_cast< uint8_t* >( output_ptr );

	if ( input_size >= 2 ) {
		auto v2 = input_size - 1;
		auto v3 = input[ v2 ] + 3 - 3 * input_size;
		if ( output_size > v2 )
			output[ v2 ] = v3;

		--v2;
		while ( v2 ) {
			v3 = input[ v2 ] + -3 * v2 - v3;
			if ( output_size > v2 )
				output[ v2 ] = v3;
			--v2;
		}

		if ( output_size >= 2 )
			output[ 0 ] = input[ 0 ] - v3;
	}
}
