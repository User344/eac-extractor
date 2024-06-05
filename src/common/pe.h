#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <Windows.h>

class c_pe final {
public:
	c_pe( void* pointer );

	[[nodiscard]] uintptr_t get_image_base( ) const;
	[[nodiscard]] size_t get_image_size( ) const;
	[[nodiscard]] IMAGE_SECTION_HEADER* get_sections( size_t& out_count ) const;

	[[nodiscard]] uintptr_t get_export_addr( const std::string& name ) const;

	[[nodiscard]] uintptr_t find_pattern_ida( const std::string& pattern ) const;
	[[nodiscard]] uintptr_t find_pattern_code( const std::string& pattern,
											   const std::string& mask ) const;

	[[nodiscard]] size_t get_function_size( uintptr_t address ) const;

private:
	uintptr_t m_base;
	IMAGE_NT_HEADERS* m_nt;
	IMAGE_SECTION_HEADER* m_sections;
};
