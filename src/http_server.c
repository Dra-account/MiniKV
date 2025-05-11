#include "api_handler.h"
#include "http_server.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 2048


void send_response(int client_sock, const char *msg, size_t msg_len, int status_code) {
    
    // 是否为完整响应（含 HTTP 头），用于区分 HTML vs JSON
    int is_full_http_response = 0;

    // 如果返回内容以 "HTTP/" 开头，视为完整响应（如 handle_index 返回）
    if (strncmp(msg, "HTTP/", 5) == 0) {
        is_full_http_response = 1;
    }

    if (is_full_http_response) {
        // 已包含完整 HTTP 头
        write(client_sock, msg, strlen(msg));
    } else {
        char *response = (char *)malloc(BUFFER_SIZE * 2);
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
        snprintf(response, BUFFER_SIZE * 2,
            "%s\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %zu\r\n"
            "\r\n"
            "%s", status_line, msg_len, msg);
        write(client_sock, response, strlen(response));
        free(response);
    }
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
    char method[8], path[128];
    if (sscanf(buffer, "%7s %127s", method, path) != 2) {
         send_response(client_sock, "{\"error\": \"Malformed request line\"}", strlen("{\"error\": \"Malformed request line\"}"), 400);
        close(client_sock);
        return;
    }

    // 找到请求体（跳过请求头）
    char* body = strstr(buffer, "\r\n\r\n");
    if (body) {
        body += 4;
    } else {
        body = "";
    }
    
    // 使用 API 处理模块生成响应体
    char msg[BUFFER_SIZE * 2];
    memset(msg, 0, sizeof(msg));

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
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) < 0) {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
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

