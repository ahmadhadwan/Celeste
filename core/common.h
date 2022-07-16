#ifndef __CELESTE_COMMON_H__
#define __CELESTE_COMMON_H__

#define CELESTE_OK                         0

#ifndef NDEBUG
    #define CELESTE_LOG(msg, ...) printf(msg, __VA_ARGS__);
#else
    #define CELESTE_LOG(msg, ...)
#endif

#define CELESTE_LOG_ERROR(msg)            fprintf(stderr, "%s:%s: %s\n", __FILE__, __FUNCTION__, msg)
#define CELESTE_LOG_ERROR_PATH(msg, path) fprintf(stderr, "%s:%s: %s `%s`\n", __FILE__, __FUNCTION__, msg, path)

#endif /* __CELESTE_COMMON_H__ */
