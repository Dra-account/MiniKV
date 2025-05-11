#ifndef API_HANDLER_H
#define API_HANDLER_H

#include "storage.h"


// 处理 API 请求
// 传入：
//     path、body
// 传出：
//     response
void handle_api_request(Storage* store, const char* path, const char* body, char* response, int max_len);


#endif
