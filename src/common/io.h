#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace io {

	bool write( const std::string& path, const void* buffer, size_t size );

	inline bool write( const std::string& path, const std::vector< uint8_t > buffer ) {
		return write( path, buffer.data( ), buffer.size( ) );
	}

} // namespace io
