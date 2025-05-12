#include "api_handler.h"
#include "parser.h"
#include "engine.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

/// 查询命令处理函数：解析 body 并执行
static void handle_query(Storage* store, const char* body, char* response, int max_len) {
    KvCommand cmd;
    if (parse_input(body, &cmd) != 0) {
        snprintf(response, max_len, "{\"error\": \"Invalid query syntax\"}\n");
        return;
    }

    ExecutionResult result = engine_execute(store, &cmd);
    snprintf(response, max_len, "%s\n", result.message);
}

/// 健康检查处理函数
static void handle_health(Storage* store, const char* body, char* response, int max_len) {
    (void)store;
    (void)body;
    snprintf(response, max_len, "{\"status\": \"ok\"}\n");
}

/// 返回 index.html 主页内容
static void handle_index(Storage* store, const char* body, char* response, int max_len) {
    (void)store;
    (void)body;

    FILE* file = fopen("web/index.html", "r");
    if (!file) {
        snprintf(response, max_len, "{\"error\": \"index.html not found\"}\n");
        return;
    }

    char file_content[8192];
    size_t bytes_read = fread(file_content, 1, sizeof(file_content) - 1, file);
    file_content[bytes_read] = '\0';

    fclose(file);

    snprintf(response, max_len,
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s", bytes_read, file_content);
}

/// 静态资源通用处理器：支持 .css/.js/.html 等文件
static void handle_static(Storage* store, const char* body, char* response, int max_len, const char* path) {
    (void)store;
    (void)body;

    // 构造完整文件路径
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "web%s", path);

    FILE* file = fopen(filepath, "rb");
    if (!file) {
        snprintf(response, max_len, "HTTP/1.0 404 Not Found\r\nContent-Type: text/plain\r\n\r\nFile not found");
        return;
    }

    struct stat st;
    stat(filepath, &st);
    size_t file_size = st.st_size;

    // 判断 MIME 类型
    const char* content_type = "application/octet-stream";
    if (strstr(path, ".css")) {
        content_type = "text/css";
    } else if (strstr(path, ".js")) {
        content_type = "application/javascript";
    } else if (strstr(path, ".html")) {
        content_type = "text/html";
    } else if (strstr(path, ".png")) {
        content_type = "image/png";
    } else if (strstr(path, ".jpg") || strstr(path, ".jpeg")) {
        content_type = "image/jpeg";
    } else if (strstr(path, ".svg")) {
        content_type = "image/svg+xml";
    }

    // 读取内容
    char* file_content = malloc(file_size + 1);
    fread(file_content, 1, file_size, file);
    file_content[file_size] = '\0';
    fclose(file);

    snprintf(response, max_len,
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "\r\n", content_type, file_size);
    memcpy(response + strlen(response), file_content, file_size);
    response[strlen(response) + file_size] = '\0';

    free(file_content);
}

/// 路由定义
typedef struct {
    const char* path;
    ApiHandlerFn handler;
} ApiRoute;

static ApiRoute routes[] = {
    { "/api/query", handle_query },
    { "/api/health", handle_health },
    { "/", handle_index }
};

#define NUM_ROUTES (sizeof(routes) / sizeof(routes[0]))

/// 请求调度器，根据 path 调用对应处理函数
void handle_api_request(Storage* store, const char* path, const char* body, char* response, int max_len) {
    for (size_t i = 0; i < NUM_ROUTES; ++i) {
        if (strcmp(path, routes[i].path) == 0) {
            routes[i].handler(store, body, response, max_len);
            return;
        }
    }

    // fallback: 尝试作为静态文件处理
    handle_static(store, body, response, max_len, path);
}
