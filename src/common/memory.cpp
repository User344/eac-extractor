#include "memory.h"
#include <sstream>

#define in_range( x, a, b ) ( x >= a && x <= b )
#define get_bits( x )                                                                              \
    ( in_range( ( x & ( ~0x20 ) ), 'A', 'F' ) ? ( ( x & ( ~0x20 ) ) - 'A' + 0xA )                  \
                                              : ( in_range( x, '0', '9' ) ? x - '0' : 0 ) )
#define get_byte( x ) ( get_bits( x[ 0 ] ) << 4 | get_bits( x[ 1 ] ) )

uintptr_t memory::find_pattern_code( uintptr_t addr, size_t size, const std::string& pattern,
                                     const std::string& mask ) {
    auto data_compare = [ pattern, mask ]( const uint8_t* data ) -> bool {
        auto p = reinterpret_cast< const uint8_t* >( pattern.data( ) );
        auto m = mask.data( );
        for ( ; *m; ++m, ++data, ++p )
            if ( *m == 'x' && *data != *p )
                return false;
        return *m == 0;
    };

    auto mask_len = mask.length( );
    if ( mask_len > size )
        return 0ull;

    return find_pattern_ex( addr, size - mask_len, data_compare );
}

uintptr_t memory::find_pattern_ida( uintptr_t address, size_t size, const std::string& pattern ) {
    std::stringstream ps;
    std::stringstream ms;

    for ( auto current = pattern.begin( ); current != pattern.end( ); ) {
        const auto c = *current;

        if ( c == '\?' ) {
            ps.put( '\0' );
            ms.put( '?' );
            current += *( current + 1 ) == '\?' ? 2 : 1;
        } else if ( c != ' ' ) {
            ps.put( get_byte( current ) );
            ms.put( 'x' );
            current += 2;
        } else {
            current += 1;
        }
    }

    return find_pattern_code( address, size, ps.str( ), ms.str( ) );
}
