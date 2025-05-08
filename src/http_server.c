#include "http_server.h"
#include "engine.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// 处理客户端请求
void handle_client(int client_sock, Storage* store) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // 读取客户端请求
    bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        perror("read");
        close(client_sock);
        return;
    }
    buffer[bytes_read] = '\0'; // 确保字符串结尾

    // 找到请求体（跳过请求头）
    char* body = strstr(buffer, "\r\n\r\n");
    if (!body || strlen(body) < 4) {
        const char* msg = "{\"error\": \"Malformed request\"}";
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response),
            "HTTP/1.0 400 Bad Request\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %zu\r\n"
            "\r\n"
            "%s", strlen(msg), msg);
        write(client_sock, response, strlen(response));
        close(client_sock);
        return;
    }

    body += 4;  // 跳过 "\r\n\r\n"，body 现在指向请求体部分

    // 解析 SQL 命令
    KvCommand cmd;
    if (parse_input(body, &cmd) != 0) {
        const char* msg = "{\"error\": \"Invalid command\"}";
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response),
            "HTTP/1.0 400 Bad Request\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %zu\r\n"
            "\r\n"
            "%s", strlen(msg), msg);
        write(client_sock, response, strlen(response));
        close(client_sock);
        return;
    }

    // 执行命令
    ExecutionResult result = engine_execute(store, &cmd);

    // 构建 JSON 响应
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s", strlen(result.message), result.message);
    write(client_sock, response, strlen(response));

    // 关闭连接
    close(client_sock);
}

// 启动 HTTP 服务
void http_server_start(Storage* store, int port) {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        exit(1);
    }

    if (listen(server_sock, 5) < 0) {
        perror("listen");
        close(server_sock);
        exit(1);
    }

    printf("HTTP server started on port %d...\n", port);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        handle_client(client_sock, store);
    }

    close(server_sock);
}
