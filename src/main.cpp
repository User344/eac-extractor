#include <format>
#include <print>
#include <filesystem>

#include "dumper.hpp"
#include "eac.hpp"
#include "parser.hpp"

#include "common/net.hpp"
#include "common/io.hpp"

#include <Windows.h>

static net::response_t download_loader( const std::string& productID,
                                        const std::string& deploymentID,
                                        const std::string& system ) {
    if ( !net::init( ) ) {
        std::println( "[!] Error! Failed to initialize curl!" );
        return { { CURLE_FAILED_INIT, 0 } };
    }

    std::print( "[-] Downloading loader..." );
    const auto url = std::format( "http://modules-cdn.eac-prod.on.epicgames.com/modules/"
                                  "{}/{}/{}",
                                  productID, deploymentID, system );

    const auto progress_callback = []( void*, curl_off_t dltotal, curl_off_t dlnow, curl_off_t,
                                       curl_off_t ) -> CURLcode {
        if ( dltotal <= 0 )
            return CURLE_OK;

        auto progress = ( ( dlnow * 100 ) / dltotal );
        std::print( "\r[-] Downloading loader: {}%", progress );

        return CURLE_OK;
    };
    const std::vector< std::string > headers = { "Accept: application/octet-stream",
                                                 "Accept-Encoding: identity",
                                                 "Cache-Control: max-age=0",
                                                 "User-Agent: EasyAntiCheat-Client/1.0" };
    const auto response = net::download_data_sync( url, headers, progress_callback );
    std::println( "" ); // new line

    net::cleanup( );
    return response;
}

int main( int argc, char* argv[] ) {
    if ( argc < 3 ) {
        std::println( "Usage: {} <product id> <deployment id> [system]", argv[ 0 ] );
        return 1;
    }

    const std::string productID = argv[ 1 ];
    const std::string deploymentID = argv[ 2 ];
    const std::string system = argc > 3 ? argv[ 3 ] : "win64";

    const auto response = download_loader( productID, deploymentID, system );

    if ( response.result.curl_code != CURLE_OK ) {
        std::println( "[!] Error! Failed to download loader! Curl error: {}",
                      static_cast< int >( response.result.curl_code ) );
        return 2;
    }

    if ( response.result.response_code != 200 ) {
        std::println( "[!] Error! Failed to download loader! Response code: {}",
                      static_cast< int >( response.result.response_code ) );
        return 3;
    }

    const auto hash = std::hash< std::string > { }(
        std::string( response.buffer.begin( ), response.buffer.end( ) ) );

    std::println( "[-] Decrypting loader..." );
    const auto loader_buffer = eac::decrypt( response.buffer );

    std::println( "[-] Creating output directory..." );
    const auto output_dir = std::filesystem::current_path( ) / "output" / productID / deploymentID /
                            std::format( "{:x}", hash );
    std::filesystem::create_directories( output_dir );

    const auto loader_path = ( output_dir / "loader.dll" ).string( );

    std::println( "[-] Saving loader..." );
    if ( !io::write( loader_path, loader_buffer ) ) {
        std::println( "[!] Error! Failed to save loader!" );
        return 4;
    }

    std::println( "[-] Loading loader binary..." );
    const auto loader = ::LoadLibraryA( loader_path.data( ) );
    if ( loader == INVALID_HANDLE_VALUE ) {
        std::println( "[!] Error! Failed to load loader! Error code: {}", ::GetLastError( ) );
        return 5;
    }

    std::println( "[-] Dumping loader..." );
    if ( !dumper::dump( ( output_dir / "loader.dmp" ).string( ), loader ) ) {
        ::FreeLibrary( loader );
        return 6;
    }

    std::println( "[-] Parsing modules..." );
    const auto parser = c_parser( loader );
    const auto modules = parser.parse( );

    std::println( "[-] Saving files..." );
    for ( const auto& module : modules ) {
        auto path = output_dir / module.name;
        path.replace_extension( module.ext );

        if ( !io::write( path.string( ), module.buffer ) ) {
            std::println( "Error! Failed to write module {} to disk!", module.name );
            break;
        }
    }

    std::println( "Cleaning up and exiting..." );
    ::FreeLibrary( loader );
    return 0;
}
