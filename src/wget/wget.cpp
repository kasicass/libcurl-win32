// http://curl.haxx.se/libcurl/c/getinmemory.html

#include <stdio.h>
#include <string>

#include <curl/curl.h>

struct MemoryChunk
{
	char *memory;
	size_t size;
};

static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	MemoryChunk *chunk = (MemoryChunk*)userp;

	chunk->memory = (char*)realloc(chunk->memory, chunk->size + realsize);
	if (chunk->memory == NULL)
	{
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(chunk->memory + chunk->size, contents, realsize);
	chunk->size += realsize;
	printf("downloading, %lu bytes\n", (long)chunk->size);
	return realsize;
}

void downloadSimple(const std::string& url, const std::string& filename)
{
	FILE *fp = fopen(filename.c_str(), "wb");
	if (fp == NULL)
	{
		fprintf(stderr, "fopen(\"%s\") failed\n", filename.c_str());
		return;
	}

	CURL *curlHandle;
	CURLcode res;
	MemoryChunk chunk = {0};

	curl_global_init(CURL_GLOBAL_ALL);

	curlHandle = curl_easy_init();	
	curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
	curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void*)&chunk);
	curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	res = curl_easy_perform(curlHandle);
	if (res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}
	else
	{
		printf("Done, downloaded %lu bytes.", (long)chunk.size);

		fwrite(chunk.memory, chunk.size, 1, fp);
		fclose(fp);

		free(chunk.memory);
	}

	curl_easy_cleanup(curlHandle);
	curl_global_cleanup();
}

void usage()
{
	printf("wget.exe [-v|-verbose] <url>\n");
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		usage();
		return 1;
	}

	std::string url = argv[argc-1];
	auto pos = url.rfind("/");
	if (pos == std::string::npos)
	{
		usage();
		return 2;
	}

	std::string filename = url.substr(pos+1);
	downloadSimple(url, filename);
	return 0;
}
