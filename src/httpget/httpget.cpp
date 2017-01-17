// http://curl.haxx.se/libcurl/c/getinmemory.html

#include <stdio.h>
#include <string>

#include <curl/curl.h>

void downloadSimpleGet(const std::string& url)
{
	CURL *curlHandle;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);

	curlHandle = curl_easy_init();	
	curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, 2L);

	res = curl_easy_perform(curlHandle);
	if (res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}

	curl_easy_cleanup(curlHandle);
	curl_global_cleanup();
}

void usage()
{
	printf("httpget.exe <url>\n");
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		usage();
		return 1;
	}

	std::string url = argv[1];
	auto pos = url.rfind("/");
	if (pos == std::string::npos)
	{
		usage();
		return 2;
	}

	downloadSimpleGet(url);
	return 0;
}
