#include <curl/curl.h>
#include <stdio.h>

#define BUFFERSIZE 4000
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    FILE* file = (FILE*)userp; // Treat userp as a FILE* pointer
    size_t total_size = size * nmemb;

    // Write the data directly to the file
    size_t written = fwrite(contents, 1, total_size, file);
    if (written < total_size) {
        fprintf(stderr, "Error writing to file\n");
        return 0; // Stop further writes on failure
    }

    return total_size;
}

int main(int argc, char* argv[]) {

    CURLcode global_init = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (global_init != CURLE_OK) {
        fprintf(stderr, "CURL global initialization failed: %s\n", curl_easy_strerror(global_init));
        return 1;
    }

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        return 1;
    }

    CURL* handle = curl_easy_init();
    if (!handle) {
        fprintf(stderr, "Failed to initialize CURL\n");
        curl_global_cleanup();
        return 1;
    }

    FILE* file = NULL;
    errno_t err = fopen_s(&file, "chessinfo.txt", "a");
    if (err != 0 || file == NULL) {
        fprintf(stderr, "Error opening file for writing\n");
        curl_easy_cleanup(handle);
        return 1;
    }
    char url[100];
    sprintf_s(url, sizeof(url), "https://api.chess.com/pub/player/%s", argv[1]);
    printf("%s", url);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, file); 

    CURLcode res = curl_easy_perform(handle);
    if (res != CURLE_OK) {
        fprintf(stderr, "CURL error: %s\n", curl_easy_strerror(res));
    }

    fclose(file); // Close the file after downloading
    curl_easy_cleanup(handle);

    curl_global_cleanup();
    return 0;
}
