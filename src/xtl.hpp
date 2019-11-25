#pragma once

namespace ext {

// 安全删除对象
#define SAFE_DELETE(p) \
    do { \
        if(p) { \
            delete p; \
            p = NULL; \
        } \
    } while(false)

// 安全删除对象数组
#define SAFE_DELETE_VEC(p) \
    do { \
        if(p) { \
            delete p[]; \
            p = NULL; \
        } \
    } while(false)

// 安全删除内存
#define SAFE_FREE(p) \
    do { \
        if(p) { \
            free(p); \
            p = NULL; \
        } \
    } while(false)

}
