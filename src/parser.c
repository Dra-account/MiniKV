#include <string.h>
#include <stdio.h>
#include "../include/parser.h"

int parser_input(const char* input, KvCommand* cmd) {
    char op[16] = {0};
    int matched = sscanf(input, "%15s %127s %255[^\n]", op, cmd->key, cmd->value);
    if (matched == 2 && strcmp(op, "GET") == 0 ) {
        cmd->type = CMD_GET;
	return 0;
    } else if (matched == 3 && strcmp(op, "SET") == 0) {
        cmd->type = CMD_SET;
	return 0;
    }
    
    cmd->type = CMD_UNKNOWN;
    return -1;
}
