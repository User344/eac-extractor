#include "parser.hpp"
#include "eac.hpp"
#include "common/memory.hpp"

#include <assert.h>
#include <print>

template < typename Ty >
struct eac_vec_s {
    Ty* begin;
    Ty* end;
    Ty* end_of_storage;
};

c_parser::c_parser( void* pe ) : m_pe( pe ) {}

void c_parser::parse_driver( std::vector< parser_module_s >& modules ) const {
    // Full pattern: 8B 48 04 48 3B CE 48 0F 42 F1 E8
    // Can be shortened to 48 0F 42 F1 E8
    // Alternatively E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 0F 57 C0 48 8D 15

    // Alternatively: 65 48 8B 04 25 ? ? ? ? 83 78 68 7A 75 40 48 8B 55 ? 49 8B CF E8 ? ? ? ?
    // Context:
    // clang-format off
	// .text:00007FFAE551A3D4 41 FF D6                        call    r14                        ; call to Decompress
	// .text:00007FFAE551A3D7 85 C0                           test    eax, eax
	// .text:00007FFAE551A3D9 75 4F                           jnz     short loc_7FFAE551A42A
	// .text:00007FFAE551A3DB 65 48 8B 04 25 30 00 00 00      mov     rax, gs:30                 ; <- pattern starts here
	// .text:00007FFAE551A3E4 83 78 68 7A                     cmp     dword ptr [rax+68h], 7Ah   ; LastError == 7A
	// .text:00007FFAE551A3E8 75 40                           jnz     short loc_7FFAE551A42A
	// .text:00007FFAE551A3EA 48 8B 55 67                     mov     rdx, [rbp+57h+arg_0]
	// .text:00007FFAE551A3EE 49 8B CF                        mov     rcx, r15
	// .text:00007FFAE551A3F1 E8 22 02 FD FF                  call    sub_7FFAE54EA618           ; copy result
    // clang-format on

    using get_driver_t = bool( uint64_t, uint64_t, eac_vec_s< uint8_t >* );

    const auto get_driver_addr = memory::dref(
        m_pe.find_pattern_ida( "E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 0F 57 C0 48 8D 15" ), 1 );
    if ( get_driver_addr == 0 ) {
        std::println( "[!] Can't parse driver because couldn't find pattern!" );
        return;
    }

    const auto get_driver_fn = reinterpret_cast< get_driver_t* >( get_driver_addr );
    auto result = false;
    eac_vec_s< uint8_t > v { };

    // Needs to be in lambda or wont compile.
    const auto result2 = [ get_driver_fn, &result, &v ]( ) mutable {
        __try {
            result = get_driver_fn( 0, 0, &v );
            return true;
        } __except ( EXCEPTION_EXECUTE_HANDLER ) { return false; }
    }( );

    if ( result == false ) {
        std::println( "[!] Can't parse driver because get_driver {}!",
                      result2 ? "failed" : "crashed" );
        return;
    }

    auto& module = modules.emplace_back( );
    module.name = "driver";
    module.ext = "sys";
    module.buffer.assign( v.begin, v.end );

    std::println( "[!] Obtained driver" );
}

void c_parser::parse_usermode( std::vector< parser_module_s >& modules ) const {
    const auto a = m_pe.get_export_addr( "a" );
    if ( a == 0 ) {
        std::println( "[!] Can't parse usermode because couldn't find export a!" );
        return;
    }

    const auto a_size = m_pe.get_function_size( a );
    const auto main = memory::dref( memory::find_pattern_ida( a, a_size, "E8 ? ? ? ? B0 01" ), 1 );
    if ( main == 0 ) {
        std::println( "[!] Can't parse usermode because couldn't find main function!" );
        return;
    }

    const auto main_size = m_pe.get_function_size( main );
    const auto marker_addr =
        memory::find_pattern_ida( main, main_size, "8B 05 ? ? ? ? 48 8D BD ? ? ? ? 8B 15" );
    if ( marker_addr == 0 ) {
        std::println( "[!] Can't parse usermode because couldn't find marker address!" );
        return;
    }

    const auto size_addr = memory::dref( marker_addr, 15 );
    assert( size_addr != 0 );

    const auto buffer_size = memory::read< uint32_t >( size_addr );

    const auto first_lea = memory::find_pattern_ida( marker_addr, 0xFF, "48 8D 05" );
    if ( first_lea == 0 ) {
        std::println( "[!] Can't parse usermode because couldn't find first lea!" );
        return;
    }

    const auto buffer_addr =
        memory::dref( memory::find_pattern_ida( first_lea + 7, 32, "48 8D 05" ), 3 );
    if ( buffer_addr == 0 ) {
        std::println( "[!] Can't parse usermode because couldn't find buffer!" );
        return;
    }

    auto& module = modules.emplace_back( );
    module.name = "usermode";
    module.ext = "dll";
    module.buffer = eac::decrypt( { reinterpret_cast< uint8_t* >( buffer_addr ), buffer_size } );

    std::println( "[!] Obtained usermode" );
}

std::vector< parser_module_s > c_parser::parse( ) const {
    std::vector< parser_module_s > result;

    parse_driver( result );
    parse_usermode( result );

    return result;
}
