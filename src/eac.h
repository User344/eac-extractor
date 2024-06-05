#pragma once
#include <stdint.h>
#include <vector>

namespace eac {

	// Decrypt buffer using EAC specific encryption.
	void decrypt( const void* input, size_t input_size, void* output, size_t output_size );

	inline std::vector< uint8_t > decrypt( const std::vector< uint8_t >& input ) {
		std::vector< uint8_t > output( input.size( ) );
		decrypt( input.data( ), input.size( ), output.data( ), output.size( ) );
		return output;
	}

} // namespace eac
