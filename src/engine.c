#include <stdio.h>
#include "../include/engine.h"

int engine_execute(Storage* storage, const KvCommand* cmd) {
    switch (cmd->type) {
        case CMD_SET:
            storage_set(storage, cmd->key, cmd->value);
            printf("OK\n");
            return 0;
	case CMD_GET: {
            const char* val = storage_get(storage, cmd->key);
	    if (val) printf("%s\n", val);
	    else printf("NULL\n");
	    return 0;
        }
	default:
            printf("Error: unknown command.\n");
	    return 1;
    }
}
