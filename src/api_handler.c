#include "api_handler.h"
#include "parser.h"
#include "engine.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 每个路径对应一个处理函数
static void handle_query(Storage* store, const char* body, char* response, int max_len) {
    KvCommand cmd;
    if (parse_input(body, &cmd) != 0) {
        snprintf(response, max_len, "{\"error\": \"Invalid query syntax\"}\n");
        return;
    }

    ExecutionResult result = engine_execute(store, &cmd);
    snprintf(response, max_len, "%s\n", result.message);
}

static void handle_health(Storage* store, const char* body, char* response, int max_len) {
    (void)store;  // unused
    (void)body;
    snprintf(response, max_len, "{\"status\": \"ok\"}\n");
}

// 处理 / 请求，返回 index.html 文件内容
static void handle_index(Storage* store, const char* body, char* response, int max_len) {
    (void)store;  // unused
    (void)body;

    // 读取 index.html 文件内容
    FILE* file = fopen("web/index.html", "r");
    if (!file) {
        snprintf(response, max_len, "{\"error\": \"index.html not found\"}\n");
        return;
    }

    // 读取文件内容并构建响应
    char file_content[1024];
    size_t bytes_read = fread(file_content, 1, sizeof(file_content) - 1, file);
    if (bytes_read < 0) {
        snprintf(response, max_len, "{\"error\": \"Failed to read index.html\"}\n");
        fclose(file);
        return;
    }

    file_content[bytes_read] = '\0';  // 确保字符串结束

    // 构造响应头
    snprintf(response, max_len,
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s", bytes_read, file_content);

    fclose(file);
}

// 定义路由表
typedef struct {
    const char* path;
    ApiHandlerFn handler;
} ApiRoute;

static ApiRoute routes[] = {
    { "/api/query", handle_query },
    { "/api/health", handle_health },
    { "/", handle_index },  // 新增：返回 index.html
};

#define NUM_ROUTES (sizeof(routes) / sizeof(routes[0]))

// 调度器
void handle_api_request(Storage* store, const char* path, const char* body, char* response, int max_len) {
    for (size_t i = 0; i < NUM_ROUTES; ++i) {
        if (strcmp(path, routes[i].path) == 0) {
            routes[i].handler(store, body, response, max_len);
            return;
        }
    }

    snprintf(response, max_len, "{\"error\": \"Unknown API endpoint\"}\n");
}
