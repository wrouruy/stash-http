#pragma once

void handle_client(HTTP* server, int client_fd, struct sockaddr_in client_addr) {
    char buf[4096];
    ssize_t n;
    int keep_alive = 0;

    do {
        // clear buffer
        memset(buf, 0, sizeof(buf));
        n = read(client_fd, buf, sizeof(buf) - 1);
        if (n <= 0) break;
        buf[n] = '\0';

        char method[16], path_with_q[1024], version[16];
        if (sscanf(buf, "%15s %1023s %15s", method, path_with_q, version) != 3)
            break; // bad req - close

        char path[1024];
        char decoded_path[1024];
        char *q = strchr(path_with_q, '?');
        if (q) {
            size_t plen = q - path_with_q;
            if (plen >= sizeof(path)) plen = sizeof(path)-1;
            memcpy(path, path_with_q, plen);
            path[plen] = '\0';
        } else {
            strncpy(path, path_with_q, sizeof(path)-1);
            path[sizeof(path)-1] = '\0';
        }

        urldecode(decoded_path, path);
        printf("Method: %s, Path: %s, Version: %s\n", method, decoded_path, version);

        HTTPreq* req = malloc(sizeof(HTTPreq));
        
        if (strstr(buf, "Connection: keep-alive") || strstr(buf, "Connection: Keep-Alive")) {
            keep_alive = 1;
            req->conn = "keep-alive";
        } else {
            keep_alive = 0;
            req->conn = "close";
        }

        req->path = decoded_path;
        req->client_fd = client_fd;
        req->queries = q;
        req->client_ip = inet_ntoa(client_addr.sin_addr);

        // printf("%s\n", client_addr.sin_port);

        if(strcmp(method, "GET") == 0)
            server->get(server, req);

        if(strcmp(method, "POST") == 0)
            server->post(server, req);

    } while (keep_alive);
}