
#ifndef LIBLOG_H
#define LIBLOG_H

#ifdef __cplusplus
extern "C" {
#endif

#define	LOG_EMERG   0  /* system is unusable */
#define	LOG_ALERT   1  /* action must be taken immediately */
#define	LOG_CRIT    2  /* critical conditions */
#define	LOG_ERR     3  /* error conditions */
#define	LOG_WARNING 4  /* warning conditions */
#define	LOG_NOTICE  5  /* normal but significant condition */
#define	LOG_INFO    6  /* informational */
#define	LOG_DEBUG   7  /* debug-level messages */
#define	LOG_VERB    8  /* verbose messages */


typedef enum {
    LOG_STDIN   = 0, /*stdin*/
    LOG_STDOUT  = 1, /*stdout*/
    LOG_STDERR  = 2, /*stderr*/
    LOG_FILE    = 3,
    LOG_RSYSLOG = 4,

    LOG_MAX_OUTPUT = 255
} log_type_t;

int log_init(int type, const char *ident);
void log_deinit();

//设置日志等级 默认LOG_INFO
void log_set_level(int level);
void log_set_split_size(int size);
void log_set_rotate(int enable);
int log_set_path(const char *path);
int log_print(int lvl, const char *tag, const char *file, int line,
        const char *func, const char *fmt, ...);

#define LOG_LEVEL_ENV     "LIBLOG_LEVEL"
#define LOG_OUTPUT_ENV    "LIBLOG_OUTPUT"

#define LOG_TAG "tag"

#define log_err(...) log_print(LOG_ERR, LOG_TAG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_warn(...) log_print(LOG_WARNING, LOG_TAG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_info(...) log_print(LOG_INFO, LOG_TAG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_debug(...) log_print(LOG_DEBUG, LOG_TAG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_verb(...) log_print(LOG_VERB, LOG_TAG, __FILE__, __LINE__, __func__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif
