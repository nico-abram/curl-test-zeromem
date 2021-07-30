#define CURL_STATICLIB

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

char *memory;
char *memory_ptr;

// 250 MiB
#define MEM_SIZE (1024 * 1024 * 250)

void *malloc_callback(size_t size) {
  *((size_t *)memory_ptr) = size;
  void *ptr = memory_ptr + sizeof(size_t);
  memory_ptr += size + sizeof(size_t);
  return ptr;
}

void free_callback(void *ptr) {}

void *realloc_callback(char *ptr, size_t size) {
  void *new_ptr = malloc_callback(size);
  if (!ptr)
    return new_ptr;

  size_t old_size = *((size_t *)(ptr - sizeof(size_t)));
  memcpy(new_ptr, ptr, old_size);
  return new_ptr;
}

char *strdup_callback(const char *str) {
  size_t len = strlen(str);
  char *ret = malloc_callback(len + 1);
  strcpy(ret, str);
  return ret;
}

void *calloc_callback(size_t nmemb, size_t size) {
  void *ret = malloc_callback(nmemb * size);
  memset(ret, 0, nmemb * size);
  return ret;
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, void *s) {
  char buf[1024 * 4];
  memcpy(buf, ptr, nmemb * size);
  buf[nmemb * size] = '\0';
  printf("%s", buf);

  return size * nmemb;
}

void main() {
  memory = malloc(MEM_SIZE);
  memory_ptr = memory;

  curl_global_init_mem(CURL_GLOBAL_ALL, malloc_callback, free_callback,
                       realloc_callback, strdup_callback, calloc_callback);

#define __PASSWORD "Passw0rd"
  char pass[] = __PASSWORD;
  char body[] = "{\"pw\": \""__PASSWORD"\"}";

  CURL *curl = curl_easy_init();

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://postman-echo.com/post");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(body));

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

    struct curl_slist *headers = NULL;
    curl_slist_append(headers, "Accept: application/json");
    curl_slist_append(headers, "Content-Type: application/json");
    curl_slist_append(headers, "charset: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
      printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

    curl_easy_cleanup(curl);

    int found = 0;
    for (int i = 0; i < MEM_SIZE - strlen(pass); i++) {
      if (memcmp(memory + i, pass, strlen(pass)) == 0) {
        printf("\nFound pass at %d", i);
        found = 1;
      }
    }

    if (!found) printf("\nPassword not found in dynamic allocations!");

  } else
    printf("curl_easy_init() failed");

  curl_global_cleanup();
}