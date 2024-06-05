#include "pe.h"
#include "memory.h"

c_pe::c_pe( void* pointer ) {
	m_base = reinterpret_cast< uintptr_t >( pointer );

	const auto dos = static_cast< IMAGE_DOS_HEADER* >( pointer );
	m_nt = reinterpret_cast< IMAGE_NT_HEADERS* >( m_base + dos->e_lfanew );
	m_sections = IMAGE_FIRST_SECTION( m_nt );
}

size_t c_pe::get_image_base( ) const { return m_base; }
size_t c_pe::get_image_size( ) const { return m_nt->OptionalHeader.SizeOfImage; }

IMAGE_SECTION_HEADER* c_pe::get_sections( size_t& out_count ) const {
	out_count = m_nt->FileHeader.NumberOfSections;
	return m_sections;
}

uintptr_t c_pe::get_export_addr( const std::string& name ) const {
	const auto module = reinterpret_cast< HMODULE >( m_base );
	return reinterpret_cast< uintptr_t >( ::GetProcAddress( module, name.c_str( ) ) );
}

uintptr_t c_pe::find_pattern_ida( const std::string& pattern ) const {
	return memory::find_pattern_ida( m_base, get_image_size( ), pattern );
}

uintptr_t c_pe::find_pattern_code( const std::string& pattern, const std::string& mask ) const {
	return memory::find_pattern_code( m_base, get_image_size( ), pattern, mask );
}

size_t c_pe::get_function_size( uintptr_t address ) const {
	// I could cache function sizes to a unordered_map but there is no point since
	// we only call this functions a few times.

	const auto rva = address - m_base;

	const auto exception_dir =
		m_nt->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXCEPTION ];
	const auto count = exception_dir.Size / sizeof( IMAGE_RUNTIME_FUNCTION_ENTRY );

	for ( auto i = 0; i < count; ++i ) {
		const auto entry = reinterpret_cast< IMAGE_RUNTIME_FUNCTION_ENTRY* >(
			m_base + exception_dir.VirtualAddress + i * sizeof( IMAGE_RUNTIME_FUNCTION_ENTRY ) );

		if ( entry->BeginAddress == rva ) {
			return entry->EndAddress - entry->BeginAddress;
		}
	}

	return 0;
}
