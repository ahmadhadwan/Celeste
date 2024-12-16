#ifndef __CELESTE_COMMON_H__
#define __CELESTE_COMMON_H__

#define CELESTE_OK 0

#ifndef CELESTE_RELEASE
    #define CELESTE_LOG(msg, ...) printf(msg, __VA_ARGS__);
#else
    #define CELESTE_LOG(msg, ...)
#endif

#define CELESTE_LOG_ERROR(msg)            fprintf(stderr, "%s:%s(%d) %s\n", __FILE__, __FUNCTION__, __LINE__, msg)
#define CELESTE_LOG_ERROR_PATH(msg, path) fprintf(stderr, "%s:%s(%d) %s `%s`\n", __FILE__, __FUNCTION__, __LINE__, msg, path)

#ifndef CELESTE_RELEASE
    #define CELESTE_ASSERT(x) (x ? x : exit(CELESTE_LOG_ERROR(#x) || 1))
#else
    #define CELESTE_ASSERT(x)
#endif

typedef int CLSTresult;

#endif /* __CELESTE_COMMON_H__ */
