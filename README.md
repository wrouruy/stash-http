## How to import the lib?
1. clone the lib: ``` git clone https://github.com/wrouruy/stash-http.git ```
2. import to your ***.c** file:
```
#include "stash-http/stash-http.h"
```

## Library in using:
```
#include "stash-http/stash-http.h"

void method_get(HTTP* server, HTTPreq* req)
{
    /* send_response(http-req, content-type, content, status-code) */

    // Home endpoint: "/"
    if(strcmp(req->path, "/") == 0)
        send_response(req, "text/html", "<h1>hello world!</h1>", 200);

    // About us endpoint: "/about"
    else if(strcmp(req->path, "/about") == 0)
        send_response(req, "text/html", "<h1>about us</h1>", 200);

    // Error 404 endpoint
    else
        send_response(req, "text/html", "<h1>Error 404<br>Page Not Found</h1>", 404);
}

int main()
{
    HTTP* server = malloc(sizeof(HTTP)); // create server
    server->port = 3000;   // set port for server
    init_server(server);   // initilization server

    server->get = method_get; // set method get
    
    server_listen(server); // listen
    return 0;
}
```
