#include "log.h"

#define  LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#define  LOG_MODULE_NAME     "log"


void log_init(void)
{
 LOG_INIT();
 log_debug("log init done.\r\n");
}


__weak uint32_t log_time(void)
{
return 0;
}

__weak void log_assert_handler(int line,char *file_name)
{
   log_error("#############系统断言错误! ##############\r\n");
   log_error("断言文件：%s.\r\n",file_name);
   log_error("断言行号：%u.\r\n",line);
   while(1);
}





