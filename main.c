#include "../stash-http.h"

void method_get(HTTP* server, HTTPreq* req)
{
    // params
    /*
    
    route("/goods/:id" (req, res) => { 
        console.log(req.params.id)
    });

    if(params_route(req, "/goods/:id")){
        printf(get_params(req, "/goods/:id", "id"))
    }

    get_params(req, "/goods/:id", "id")

    */
    if(strcmp(req->path, "/") == 0)
        send_response(req, "text/html", "<h1>hello world!</h1>", 200);
    
    printf("/goods/:id\n%s\n", req->path);
}

int main()
{
    HTTP* server = malloc(sizeof(HTTP));
    server->port = 3000;
    init_server(server);

    server->get = method_get;
    
    server_listen(server);
    return 0;
}