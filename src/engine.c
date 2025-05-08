#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "storage.h"

ExecutionResult engine_execute(Storage* storage, const KvCommand* cmd) {
    ExecutionResult result = {0};  // 默认返回成功

    switch (cmd->type) {
        case CMD_SET:
            // 执行 SET 命令
            if (storage_set(storage, cmd->key, cmd->value) == 0) {
                snprintf(result.message, sizeof(result.message), "{\"status\": \"ok\"}");
                result.code = 0;  // 成功
            } else {
                snprintf(result.message, sizeof(result.message), "{\"error\": \"set failed\"}");
                result.code = -1; // 失败
            }
            break;
        
        case CMD_GET: {
            // 执行 GET 命令
            const char* val = storage_get(storage, cmd->key);
            if (val) {
                snprintf(result.message, sizeof(result.message), "{\"value\": \"%s\"}", val);
                result.code = 0;  // 成功
            } else {
                snprintf(result.message, sizeof(result.message), "{\"error\": \"not found\"}");
                result.code = -1; // 失败
            }
            break;
        }
        default:
            snprintf(result.message, sizeof(result.message), "{\"error\": \"unknown command\"}");
            result.code = -1; // 失败
            break;
    }

    return result;
}
