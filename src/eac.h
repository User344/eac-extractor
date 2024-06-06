#pragma once
#include <cstdint>
#include <vector>
#include <span>

namespace eac {

    // Decrypt buffer using EAC specific encryption.
    void decrypt( const std::span< uint8_t const >& input, const std::span< uint8_t >& output );

    inline std::vector< uint8_t > decrypt( const std::span< uint8_t const >& input ) {
        std::vector< uint8_t > output( input.size( ) );
        decrypt( input, { output.data( ), output.size( ) } );
        return output;
    }

} // namespace eac
