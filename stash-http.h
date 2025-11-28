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

typedef struct
{
    char* path;
    char* conn;
    char* queries;
    int   client_fd;
    char* client_ip;
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

void send_response(HTTPreq* req, char* type, char* content, short status)
{
    signal(SIGPIPE, SIG_IGN);
    size_t clen = strlen(content);
    size_t hlen = 256 + clen + strlen(req->conn);
    char* header = malloc(hlen);
    snprintf(header, hlen,
             "HTTP/1.1 %d OK\r\n"
             "Content-type: %s; charset=utf-8\r\n"
             "Content-Length: %d\r\n"
             "Connection: %s\r\n"
             "\r\n", status, type, clen, req->conn);

    write(req->client_fd, header, strlen(header));
    write(req->client_fd, content, clen);

    free(header);
}

void server_listen(HTTP* server)
{
    struct sockaddr_in client_addr;
    int client_fd;
    socklen_t client_len = sizeof(client_addr);
    while (true) {
        client_fd = accept(server->fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        handle_client(server, client_fd, client_addr);
        close(client_fd);
    }
    close(server->fd);
}