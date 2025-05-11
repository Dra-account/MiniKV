#include "api_handler.h"
#include "http_server.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024


void send_response(int client_sock, const char *msg, size_t msg_len, int status_code) {
    char *response = (char *)malloc(BUFFER_SIZE);
    if (!response) {
        perror("malloc");
        return;
    }
    const char *status_line;
    if (status_code == 200) {
        status_line = "HTTP/1.0 200 OK";
    } else {
        status_line = "HTTP/1.0 400 Bad Request";
    }
    snprintf(response, BUFFER_SIZE,
        "%s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s", status_line, msg_len, msg);
    write(client_sock, response, strlen(response));
    free(response);
}

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

    // 提取 HTTP 方法和路径
    char method[8], path[64];
    if (sscanf(buffer, "%7s %63ss", method, path) != 2) {
         send_response(client_sock, "{\"error\": \"Malformed request line\"}", strlen("{\"error\": \"Malformed request line\"}"), 400);
        close(client_sock);
        return;
    }

    // 找到请求体（跳过请求头）
    char* body = strstr(buffer, "\r\n\r\n");
    if (!body || strlen(body) < 4) {
        send_response(client_sock, "{\"error\": \"Malformed request body\"}", strlen("{\"error\": \"Malformed request body\"}"), 400);
        close(client_sock);
        return;
    }
    body += 4;  // 跳过 "\r\n\r\n"，body 现在指向请求体部分

    // 使用 API 处理模块生成响应体
    char msg[BUFFER_SIZE];
    handle_api_request(store, path, body, msg, sizeof(msg));
    
    send_response(client_sock, msg, strlen(msg), 200);

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

