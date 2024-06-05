#pragma once
#include "common/pe.h"
#include <vector>

struct parser_module_s {
	std::string name;
	std::string ext;
	std::vector< uint8_t > buffer;
};

class c_parser {
public:
	c_parser( void* pe );

	[[nodiscard]] std::vector< parser_module_s > parse( ) const;

private:
	void parse_driver( std::vector< parser_module_s >& modules ) const;
	void parse_usermode( std::vector< parser_module_s >& modules ) const;

	c_pe m_pe;
};
