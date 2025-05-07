// string_utils.c
#include <ctype.h>
#include "string_utils.h"

// 实现忽略大小写的字符串比较函数
int strcmp_ignore_case(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        // 将字符转换为小写后进行比较
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) {
            return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        }
        s1++;
        s2++;
    }
    // 处理字符串长度不同的情况
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}
