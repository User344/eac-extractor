#include "net.hpp"
#include <curl/easy.h>

bool net::init( ) { return curl_global_init( CURL_GLOBAL_ALL ) == CURLE_OK; }

void net::cleanup( ) { curl_global_cleanup( ); }

net::result_t net::request_sync( const std::string& url, const std::vector< std::string >& headers,
                                 write_callback_t write_callback, void* write_data,
                                 progress_callback_t progress_callback, void* progress_data ) {
    auto curl = curl_easy_init( );
    if ( !curl )
        return { CURLE_FAILED_INIT, 0 };

    curl_easy_setopt( curl, CURLOPT_URL, url.data( ) );
    curl_easy_setopt( curl, CURLOPT_DNS_SHUFFLE_ADDRESSES, 1 );
    curl_easy_setopt( curl, CURLOPT_HTTPGET, 1 );

    if ( !headers.empty( ) ) {
        curl_slist* curl_headers = NULL;

        for ( const auto& header : headers ) {
            curl_slist_append( curl_headers, header.data( ) );
        }

        curl_easy_setopt( curl, CURLOPT_HTTPHEADER, curl_headers );
    }

    if ( write_callback ) {
        curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_callback );
        curl_easy_setopt( curl, CURLOPT_WRITEDATA, write_data );
    }

    if ( progress_callback ) {
        curl_easy_setopt( curl, CURLOPT_NOPROGRESS, 0 );
        curl_easy_setopt( curl, CURLOPT_XFERINFOFUNCTION, progress_callback );
        curl_easy_setopt( curl, CURLOPT_XFERINFODATA, progress_data );
    }

    result_t result { };
    result.curl_code = curl_easy_perform( curl );

    if ( result.curl_code == CURLE_OK )
        curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &result.response_code );

    curl_easy_cleanup( curl );
    return result;
}

net::response_t net::download_data_sync( const std::string& url,
                                         const std::vector< std::string >& headers,
                                         progress_callback_t progress_callback,
                                         void* progress_data ) {
    const auto cb = []( char* data, size_t size, size_t nmemb, void* userdata ) -> size_t {
        size_t real_size = size * nmemb;
        auto response = ( response_t* )userdata;

        response->buffer.insert( response->buffer.end( ), data, data + real_size );
        return real_size;
    };

    response_t response { };
    response.result = request_sync( url, headers, cb, &response, progress_callback, progress_data );

    return response;
}
