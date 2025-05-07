#include <stdio.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/engine.h"
#include "../include/storage.h"

int main() {
    Storage* store = storage_create();
    char input[512];
    KvCommand cmd;

    printf("MiniKV > ");
    while (fgets(input, sizeof(input), stdin)) {
        if (parser_input(input, &cmd) ==0) {
	    engine_execute(store, &cmd);
	} else {
	    printf("Invalid command.\n");
	}
	printf("MiniKV > ");
    }

    storage_free(store);
    return 0;
}
