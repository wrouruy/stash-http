#pragma once

typedef struct {
    HTTP* server;
    int client_fd;
    struct sockaddr_in client_addr;
} ClientArgs;

ssize_t read_headers(int fd, char *buf, size_t max) {
    size_t total = 0;
    while (total < max - 1) {
        ssize_t n = read(fd, buf + total, 1);
        if (n <= 0) break;
        total += n;

        if (total >= 4 &&
            strstr(buf, "\r\n\r\n")) // end HTTP header
            break;
    }
    buf[total] = '\0';
    return total;
}

void handle_client(HTTP* server, int client_fd, struct sockaddr_in client_addr) {
    // timeout for read() not sleep
    struct timeval timeout = {1, 0};
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    char buf[8192];
    int keep_alive = 0;

    do {
        memset(buf, 0, sizeof(buf));
        ssize_t n = read_headers(client_fd, buf, sizeof(buf));
        if (n <= 0) break;

        char method[16] = {0};
        char path_raw[2048] = {0};
        char version[16] = {0};

        if (sscanf(buf, "%15s %2047s %15s", method, path_raw, version) != 3)
            break;

        // split path and query
        char *q = strchr(path_raw, '?');
        char path[2048];
        char *queries = NULL;

        if (q) {
            size_t plen = q - path_raw;
            memcpy(path, path_raw, plen);
            path[plen] = '\0';
            queries = strdup(q + 1);
        } else
            strcpy(path, path_raw);

        // URL decode
        char decoded_path[2048];
        urldecode(decoded_path, path);

        // comparisons keep-alive
        if (strstr(buf, "Connection: keep-alive") ||
            strstr(buf, "Connection: Keep-Alive"))
            keep_alive = 1;
        else
            keep_alive = 0;

        printf("Method: \033[1m%s\033[0m, Path: \033[1m%s\033[0m, Version: \033[1m%s\033[0m\n", method, decoded_path, version);

        // forming req
        HTTPreq* req = malloc(sizeof(HTTPreq));
        req->client_fd = client_fd;
        req->conn = keep_alive ? "keep-alive" : "close";
        req->path = strdup(decoded_path);
        req->queries = queries;
        req->client_ip = inet_ntoa(client_addr.sin_addr);
        req->body = buf;

        // call func depent methods
        if (strcmp(method, "GET") == 0)
            server->get(server, req);

        if (strcmp(method, "POST") == 0)
            server->post(server, req);

        // cleanup
        free(req->path);
        if (req->queries) free(req->queries);
        free(req);
    } while (keep_alive);

    close(client_fd);
}

void* thread_func(void* arg) {
    ClientArgs* args = (ClientArgs*)arg;

    handle_client(args->server, args->client_fd, args->client_addr);

    free(args);
    return NULL;
}