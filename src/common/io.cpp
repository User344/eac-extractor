#include "io.h"

#include <fstream>

bool io::write( const std::string& path, const void* buffer, size_t size ) {
    std::ofstream ostream( path, std::ios::out | std::ios::binary );
    if ( ostream.bad( ) )
        return false;

    ostream.write( static_cast< const char* >( buffer ), size );
    ostream.close( );

    return true;
}
