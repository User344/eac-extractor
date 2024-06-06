#include "eac.hpp"

void eac::decrypt( const std::span< uint8_t const >& input, const std::span< uint8_t >& output ) {

    if ( input.size( ) >= 2 ) {
        auto v2 = input.size( ) - 1;
        auto v3 = input[ v2 ] + 3 - 3 * input.size( );
        if ( output.size( ) > v2 )
            output[ v2 ] = v3;

        --v2;
        while ( v2 ) {
            v3 = input[ v2 ] + -3 * v2 - v3;
            if ( output.size( ) > v2 )
                output[ v2 ] = v3;
            --v2;
        }

        if ( output.size( ) >= 2 )
            output[ 0 ] = input[ 0 ] - v3;
    }
}
