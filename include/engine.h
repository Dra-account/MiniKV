#ifndef ENGINE_H
#define ENGINE_H

#include "parser.h"
#include "storage.h"

typedef struct {
    int code;          // 返回的状态码，0为成功，非0为失败
    char message[256]; // 存储JSON响应
} ExecutionResult;

// 执行命令，返回执行结果
ExecutionResult engine_execute(Storage* storage, const KvCommand* cmd);

#endif
