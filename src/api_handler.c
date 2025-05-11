#include "api_handler.h"
#include "parser.h"
#include "engine.h"

#include <stdio.h>
#include <string.h>


void handle_api_request(Storage* store, const char* path, const char* body, char* response, int max_len) {
    if (strcmp(path, "/api/query") != 0) {
        snprintf(response, max_len, "{\"error\": \"Unkonwn API endpoint\"}");
	return;
    }

    KvCommand cmd;
    if (parse_input(body, &cmd) !=0) {
        snprintf(response, max_len, "{\"error\":\"Invalid query syntax\"}");
	return;
    }

    ExecutionResult result = engine_execute(store, &cmd);

    snprintf(response, max_len, "%s\n", result.message);
}
