
#include "liblog.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(__linux__) || defined(__CYGWIN__)
#include <pthread.h>
#endif



static void test_no_init(void)
{
    int i = 1;
    char tmp[32] = "abcd";
    log_err("test rsyslog\n");
    log_warn("debug msg %d, %s\n", i, tmp);
    log_debug("debug msg %d, %s\n", i, tmp);
    log_info("debug msg %d, %s\n", i, tmp);
    log_verb("debug msg %d, %s\n", i, tmp);
    log_deinit();
}

static void test_rsyslog(void)
{
    int i = 1;
    char tmp[32] = "abcd";
    log_deinit();
    log_init(LOG_RSYSLOG, NULL);
    log_err("err msg  rsyslog\n");
    log_warn("warn msg %d, %s\n", i, tmp);
    log_debug("debug msg %d, %s\n", i, tmp);
    log_info("info msg %d, %s\n", i, tmp);
    log_verb("verb msg %d, %s\n", i, tmp);
    log_deinit();
}

static void test_file_name(void)
{
    int i = 1;



    log_init(LOG_FILE, "foo.log");

    log_set_split_size(10);
    log_err("debug msg i=%d\n", i);
    log_warn("debug msg i=%d\n", i);
    log_info("debug msg i=%d\n", i);

    log_deinit();
}

static void test_file_noname(void)
{
    int i = 1;
    
    log_init(LOG_FILE, NULL);
    log_set_split_size(1 * 1024 * 1024);
    log_err("debug msg i = %d\n", i);
    log_warn("debug msg\n");
    log_info("debug msg\n");

    log_deinit();
}



int main(int argc, char **argv)
{

    // test_no_init();
    // test_file_name();
    test_rsyslog();
    

    // test_file_noname();
    return 0;
}
