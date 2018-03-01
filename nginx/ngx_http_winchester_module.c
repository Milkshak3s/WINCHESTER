//
// Created by Chris on 3/1/2018.
//

#include <nginx.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <stdio.h>

/* Module context */
static ngx_http_module_t ngx_http_winchester_module_ctx = {
        NULL, /* pre-config */
        NULL, /* post-config */
        NULL, /* create main config */
        NULL, /* init main config */
        NULL, /* create server config */
        NULL, /* merge server config */
        NULL, /* create location config */
        NULL /* merge location config */
};

/* Module definition */
ngx_module_t ngx_http_winchester_module = {
        NGX_MODULE_V1,
        &ngx_http_winchester_module_ctx, /* module context */
        NULL,                            /* module directives*/
        NGX_HTTP_MODULE,                 /* module type */
        NULL,                            /* init master */
        NULL,                            /* init module */
        NULL,                            /* init process */
        NULL,                            /* init thread */
        NULL,                            /* exit thread */
        NULL,                            /* exit process */
        NULL,                            /* exit master */
        NGX_MODULE_V1_PADDING
};