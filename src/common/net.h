#pragma once
#include <string>
#include <vector>
#include <curl/curl.h>

namespace net {

	struct result_t {
		CURLcode curl_code;
		long response_code;
	};

	struct response_t {
		result_t result;
		std::vector< uint8_t > buffer;
	};

	using write_callback_t = size_t ( * )( char* ptr, size_t size, size_t nmemb, void* userdata );
	using progress_callback_t = CURLcode ( * )( void* userdata, curl_off_t dltotal,
												curl_off_t dlnow, curl_off_t ultotal,
												curl_off_t ulnow );

	[[nodiscard]] bool init( );

	void cleanup( );

	result_t request_sync( const std::string& url, const std::vector< std::string >& headers,
						   write_callback_t write_callback = nullptr, void* write_data = nullptr,
						   progress_callback_t progress_callback = nullptr,
						   void* progress_data = nullptr );

	response_t download_data_sync( const std::string& url,
								   const std::vector< std::string >& headers,
								   progress_callback_t progress_callback = nullptr,
								   void* progress_data = nullptr );

} // namespace net
