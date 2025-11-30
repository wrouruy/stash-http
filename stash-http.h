#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct
{
    int   client_fd, times;
    char *path, *conn, *queries, *client_ip, *body;

    char headers[2048];
    size_t headers_len;
} HTTPreq;

typedef struct HTTP HTTP; // forward declaration

typedef struct HTTP
{
    int fd, opt;
    struct sockaddr_in addr; 

    unsigned short port;
    void (*get) (HTTP* server, HTTPreq* req);
    void (*post)(HTTP* server, HTTPreq* req);
} HTTP;

#include "utils/strlib.h"
#include "utils/utils.h"
#include "utils/file.h"
#include "query.h"
#include "params.h"
#include "dotenv.h"

void init_server(HTTP* server)
{
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->fd < 0) {
        perror("socket");
        exit(1);
    }
    server->opt = 1;

    setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &server->opt, sizeof(server->opt));
    memset(&server->addr, 0, sizeof(server->addr));
    server->addr.sin_family = AF_INET;
    server->addr.sin_addr.s_addr = INADDR_ANY;
    server->addr.sin_port = htons(server->port);

    if (bind(server->fd, (struct sockaddr*)&server->addr, sizeof(server->addr)) < 0) {
        perror("bind");
        close(server->fd);
        exit(1);
    }

    if (listen(server->fd, SOMAXCONN) < 0) {
        perror("listen");
        exit(1);
    }
}

void send_header(HTTPreq* req, const char* key, const char* value)
{
    if (req->headers_len >= sizeof(req->headers) - 4)
        return;

    int written = snprintf(
        req->headers + req->headers_len,
        sizeof(req->headers) - req->headers_len,
        "%s: %s\r\n",
        key, value
    );

    if (written > 0)
        req->headers_len += written;
}

void send_response(HTTPreq* req, const char* body, size_t len, int status)
{
    // send status
    dprintf(req->client_fd, "HTTP/1.1 %d OK\r\n", status);

    // send headers
    write(req->client_fd, req->headers, req->headers_len);

    // write to req->client_fd empty line
    write(req->client_fd, "\r\n", 2);

    // Send body
    write(req->client_fd, body, len);

    // for agains using
    req->headers_len = 0;
}


void printf_response(HTTPreq* req, short status, char* content, ...)
{
    va_list args;
    int written_chars;
    char* buffer = malloc(2048);
    size_t buffer_size = 2048;

    va_start(args, content);
    written_chars = vsnprintf(buffer, buffer_size, content, args);
    va_end(args);

    send_response(req, buffer, 2048, status);

    free(buffer);
}

void server_listen(HTTP* server)
{
    struct sockaddr_in client_addr;
    int client_fd;
    socklen_t client_len = sizeof(client_addr);
    while (true) {
        client_fd = accept(server->fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_fd < 0)
            continue;

        ClientArgs* args = malloc(sizeof(ClientArgs));
        args->server = server;
        args->client_fd = client_fd;
        args->client_addr = client_addr;

        pthread_t tid;
        pthread_create(&tid, NULL, thread_func, args);
        pthread_detach(tid);
    }
    close(server->fd);
}