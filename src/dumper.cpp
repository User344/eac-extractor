#include "dumper.h"
#include "common/io.h"
#include "common/pe.h"

#include <vector>
#include <print>

bool dumper::dump( const std::string& path, void* image ) {
	const auto image_pe = c_pe( image );
	const auto size = image_pe.get_image_size( );

	std::vector< uint8_t > buffer( size );
	memcpy( buffer.data( ), image, size );

	const auto dump_pe = c_pe( buffer.data( ) );
	size_t count = 0;
	auto section = dump_pe.get_sections( count );
	for ( auto i = 0; i < count; ++i, ++section ) {
		section->PointerToRawData = section->VirtualAddress;
		section->SizeOfRawData = section->Misc.VirtualSize;
	}

	return io::write( path, buffer );
}
