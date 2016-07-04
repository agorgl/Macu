/*********************************************************************************************************************/
/*                                                  /===-_---~~~~~~~~~------____                                     */
/*                                                 |===-~___                _,-'                                     */
/*                  -==\\                         `//~\\   ~~~~`---.___.-~~                                          */
/*              ______-==|                         | |  \\           _-~`                                            */
/*        __--~~~  ,-/-==\\                        | |   `\        ,'                                                */
/*     _-~       /'    |  \\                      / /      \      /                                                  */
/*   .'        /       |   \\                   /' /        \   /'                                                   */
/*  /  ____  /         |    \`\.__/-~~ ~ \ _ _/'  /          \/'                                                     */
/* /-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`                                                      */
/*                   \_|      /        _)   ;  ),   __--~~                                                           */
/*                     '~~--_/      _-~/-  / \   '-~ \                                                               */
/*                    {\__--_/}    / \\_>- )<__\      \                                                              */
/*                    /'   (_/  _-~  | |__>--<__|      |                                                             */
/*                   |0  0 _/) )-~     | |__>--<__|     |                                                            */
/*                   / /~ ,_/       / /__>---<__/      |                                                             */
/*                  o o _//        /-~_>---<__-~      /                                                              */
/*                  (^(~          /~_>---<__-      _-~                                                               */
/*                 ,/|           /__>--<__/     _-~                                                                  */
/*              ,//('(          |__>--<__|     /                  .----_                                             */
/*             ( ( '))          |__>--<__|    |                 /' _---_~\                                           */
/*          `-)) )) (           |__>--<__|    |               /'  /     ~\`\                                         */
/*         ,/,'//( (             \__>--<__\    \            /'  //        ||                                         */
/*       ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'                                          */
/*     `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/                                                     */
/*   ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~                                                       */
/*    ;'( ')/ ,)(                              ~~~~~~~~~~                                                            */
/*   ' ') '( (/                                                                                                      */
/*     '   '  `                                                                                                      */
/*********************************************************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdarg.h>
#include <sock.h>
#include <tinycthread.h>
#include <vector.h>
#include <http_parser.h>
#include <leak_detect.h>

#define BUFSIZE HTTP_MAX_HEADER_SIZE

typedef void(*webserv_route_fn)();
struct webserv {
    int running; /* Master switch */
    sock_t ls; /* Socket listening for connections */
    webserv_route_fn route_fn; /* Routing function */
};

/* Connected client state */
struct webserv_cli_data {
    /* Connection state */
    sock_t sock;
    char ipstr[INET6_ADDRSTRLEN];
    /* Request state */
    int request_complete;
    enum http_method method;
    char* url;
};

enum webserv_log_level {
    SL_ERROR,
    SL_WARN,
    SL_INFO,
    SL_DEBUG,
};

/*---------------------------------------------------------------
 * Webserv internals
 *---------------------------------------------------------------*/
static void slog(enum webserv_log_level ll, const char* fmt, ...)
{
    /* Construct log prefix */
    const char* prefix = "";
    switch(ll) {
        case SL_ERROR:
            prefix = "[ERROR]";
        case SL_WARN:
            prefix = "[WARN]";
        case SL_INFO:
            prefix = "[INFO]";
        case SL_DEBUG:
            prefix = "[DEBUG]";
    }

    /* Construct full log format to pass to printing functions */
    int nfmtlen = strlen(prefix) + 1 + strlen(fmt) + 1;
    char* nfmt = malloc(nfmtlen);
    strcpy(nfmt, prefix);
    strcat(nfmt, " ");
    strcat(nfmt, fmt);

    va_list vl;
    va_start(vl, fmt);

    switch(ll) {
        case SL_ERROR:
        case SL_WARN:
            vfprintf(stderr, nfmt, vl);
        case SL_INFO:
        case SL_DEBUG:
            vprintf(nfmt, vl);
    }
    va_end(vl);
    free(nfmt);
}

static int on_message_begin(struct http_parser* p)
{
    struct webserv_cli_data* client_data = (struct webserv_cli_data*)p->data;
    client_data->method = p->method;
    return 0;
}

static int on_url(struct http_parser* p, const char* at, size_t length)
{
    struct webserv_cli_data* client_data = (struct webserv_cli_data*)p->data;
    client_data->url = malloc(length + 1);
    strncpy(client_data->url, at, length);
    client_data->url[length] = '\0';
    return 0;
}

static int on_message_complete(struct http_parser* p)
{
    struct webserv_cli_data* client_data = (struct webserv_cli_data*)p->data;
    client_data->request_complete = 1;
    return 0;
}

static int webserv_parse_request(struct webserv_cli_data* client_data)
{
    struct http_parser parser;
    struct http_parser_settings settings;
    size_t nparsed;
    char reqbuf[BUFSIZE];

    /* Initialize parser */
    http_parser_settings_init(&settings);
    settings.on_message_begin = on_message_begin;
    settings.on_url = on_url;
    settings.on_message_complete = on_message_complete;
    http_parser_init(&parser, HTTP_REQUEST);
    parser.data = client_data;
    client_data->request_complete = 0;

    while(1) {
        /* Fetch next chunk of request data */
        memset(reqbuf, 0, sizeof(reqbuf));
        int len = recv(client_data->sock, reqbuf, BUFSIZE, 0);
        if (len == 0) {
            slog(SL_DEBUG, "Connection closed by client\n");
            return -1;
        } else if (len == -1) {
            slog(SL_DEBUG, "Client connection dropped\n");
            return -1;
        }
        slog(SL_DEBUG, "Received %d length packet\n", len);

        /* Parse the received chunk */
        nparsed = http_parser_execute(&parser, &settings, reqbuf, len);
        if (nparsed != (size_t)len) {
            enum http_errno herr = HTTP_PARSER_ERRNO(&parser);
            slog(SL_ERROR,
                    "Error while parsing request: %s (%s)\n",
                    http_errno_description(herr),
                    http_errno_name(herr));
            return -1;
        }

        /* Check if end of request reached */
        if(client_data->request_complete)
            break;
    }
    return 0;
}

/* Client thread parameters */
struct webserv_cli_thrd_params {
    struct webserv* ws;
    struct webserv_cli_data cli_data;
};

static int webserv_cli_thrd(void* arg)
{
    /* Extract thread params */
    struct webserv_cli_thrd_params* thrd_params = (struct webserv_cli_thrd_params*) arg;
    struct webserv_cli_data* client_data = &(thrd_params->cli_data);

    /* Show log entry */
    slog(SL_DEBUG, "Client connected: %s\n", client_data->ipstr);

    /* Parse and receive http request in chuncks */
    if (webserv_parse_request(client_data) == 0)
        thrd_params->ws->route_fn(thrd_params->ws, client_data);

    /* Disconnect and clean allocated thread params */
    if(client_data->url)
        free(client_data->url);
    sock_close(client_data->sock);
    slog(SL_DEBUG, "Client disconnected: %s\n", client_data->ipstr);
    free(thrd_params);
    return 0;
}

/*---------------------------------------------------------------
 * Webserv public
 *---------------------------------------------------------------*/
void webserv_init(struct webserv* ws, webserv_route_fn route_fn)
{
    memset(ws, 0, sizeof(struct webserv));
    ws->route_fn = route_fn;
}

void webserv_destroy(struct webserv* ws)
{
    memset(ws, 0, sizeof(struct webserv));
}

int webserv_run(struct webserv* ws)
{
    /* Fill in addrinfo struct */
    const char* host = "0.0.0.0";
    const char* port = "80";

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo* servinfo;
    int status = getaddrinfo(host, port, &hints, &servinfo);
    if (status) {
        slog(SL_ERROR, "Getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }

    /* Create listen socket */
    ws->ls = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (!sock_valid(ws->ls)) {
        slog(SL_ERROR, "Invalid socket error\n");
        return 1;
    }

    /* Bind */
    status = bind(ws->ls, servinfo->ai_addr, servinfo->ai_addrlen);
    if (status != 0) {
        slog(SL_ERROR, "Could not bind on port: %s\n", port);
        sock_close(ws->ls);
        return 1;
    }
    freeaddrinfo(servinfo);

    /* Listen */
    status = listen(ws->ls, 3);
    if (status != 0) {
        slog(SL_ERROR, "Could not listen on port: %s\n", port);
        sock_close(ws->ls);
        return 1;
    }

    /* Thread identifiers */
    struct vector thrd_ids;
    vector_init(&thrd_ids, sizeof(thrd_t));

    /* Accept loop */
    ws->running = 1;
    slog(SL_INFO, "Waiting for connections...\n");
    while(ws->running) {
        struct sockaddr_storage raddr;
        int sockaddrlen = sizeof(struct sockaddr_storage);
        sock_t cs = accept(ws->ls, (struct sockaddr*)&raddr, &sockaddrlen);
        if (!sock_valid(cs)) {
            slog(SL_ERROR, "Could not accept connection\n");
            continue;
        }

        /* Prepare client thread params */
        struct webserv_cli_thrd_params* cd = malloc(sizeof(struct webserv_cli_thrd_params));
        memset(cd, 0, sizeof(struct webserv_cli_thrd_params));
        cd->ws = ws;
        cd->cli_data.sock = cs;

        /* Get client ip address */
        void* addr;
        if (raddr.ss_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in*)&raddr;
            addr = &(ipv4->sin_addr);
        } else {
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)&raddr;
            addr = &(ipv6->sin6_addr);
        }
        inet_ntop(raddr.ss_family, addr, cd->cli_data.ipstr, sizeof(cd->cli_data.ipstr));

        /* On successful accept launch client thread */
        thrd_t thrd;
        status = thrd_create(&thrd, webserv_cli_thrd, cd);
        if (status == thrd_success) {
            vector_append(&thrd_ids, &thrd);
            /* thrd_detach(thrd); */
        } else {
            slog(SL_WARN, "Could not create client thread\n");
            sock_close(cs);
        }
    }

    /* Join all threads */
    for (size_t i = 0; i < thrd_ids.size; ++i) {
        thrd_t* tid = vector_at(&thrd_ids, i);
        thrd_join(*tid, 0);
    }
    vector_destroy(&thrd_ids);

    /* Close listen socket */
    sock_close(ws->ls);
    /* Normal termination */
    return 0;
}

void webserv_shutdown(struct webserv* ws)
{
    ws->running = 0;
    sock_close(ws->ls);
}

void webserv_route(struct webserv* ws, struct webserv_cli_data* client_data)
{
    /* Print request info */
    slog(SL_INFO, "Got %s request on %s\n", http_method_str(client_data->method), client_data->url);

    /* Check for die request */
    if (strncmp("/die", client_data->url, 4) == 0) {
        slog(SL_INFO, "Shutting down...\n");
        webserv_shutdown(ws);
    }

    /* Send response */
    char* rbuf = "HTTP/1.1 200 OK\r\n"
                 "Server: WebServ\r\n"
                 "Content-Type: text/html\r\n"
                 "\r\n"
                 "Hello laaadieees!\r\n";
    int slen = strlen(rbuf);
    sendall(client_data->sock, rbuf, &slen);
}

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;
    ld_init();
    sock_init();

    struct webserv ws;
    webserv_init(&ws, webserv_route);
    webserv_run(&ws);
    webserv_destroy(&ws);

    sock_destroy();
    ld_print_leaks();
    ld_shutdown();
    return 0;
}
