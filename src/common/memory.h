#pragma once
#include <stdint.h>
#include <string>

namespace memory {

	template < typename _PR >
	uintptr_t find_pattern_ex( uintptr_t address, size_t size, _PR pred ) {
		for ( auto i = 0u; i < size; i++ )
			if ( pred( reinterpret_cast< const uint8_t* >( address + i ) ) )
				return address + i;

		return 0;
	}

	inline uintptr_t dref( uintptr_t address, uint32_t offset ) {
		if ( address == 0 )
			return 0;

		return address + static_cast< int32_t >(
							 ( *reinterpret_cast< int32_t* >( address + offset ) + offset ) +
							 sizeof( int32_t ) );
	}

	template < typename T >
	T read( uintptr_t address ) {
		return *reinterpret_cast< T* >( address );
	}

	uintptr_t find_pattern_code( uintptr_t address, size_t size, const std::string& pattern,
								 const std::string& mask );

	uintptr_t find_pattern_ida( uintptr_t address, size_t size, const std::string& pattern );


} // namespace memory
