#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "storage.h"


// 所有 API 处理函数的签名
typedef void (*ApiHandlerFn)(Storage* store, const char* body, char* response, int max_len);


// 路由器主入口
void handle_api_request(Storage* store, const char* path, const char* body, char* response, int max_len);


#endif
