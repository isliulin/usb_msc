#ifndef   __LOG_H__
#define   __LOG_H__

#include "stdint.h"
#include "SEGGER_RTT.h"

#ifdef __cplusplus
extern "C" {
#endif	
  
#define LOG_LEVEL_OFF          0U
#define LOG_LEVEL_ERROR        1U
#define LOG_LEVEL_WARNING      2U
#define LOG_LEVEL_INFO         3U
#define LOG_LEVEL_DEBUG        4U

/*******配置开始******************************/

#define LOG_GLOBAL_LEVEL       LOG_LEVEL_DEBUG              
#define LOG_USE_COLORS         1
#define LOG_USE_TIMESTAMP      1   

/*******配置结束******************************/
void log_init(void);
uint32_t log_time(void);

#if     LOG_USE_COLORS  >  0
#define LOG_ERROR_COLOR        RTT_CTRL_TEXT_RED
#define LOG_WARNING_COLOR      RTT_CTRL_TEXT_MAGENTA
#define LOG_INFO_COLOR         RTT_CTRL_TEXT_GREEN
#define LOG_DEBUG_COLOR        RTT_CTRL_TEXT_BLUE
#else 
#define LOG_USE_COLORS
#define LOG_WARNING_COLOR
#define LOG_INFO_COLOR
#define LOG_DEBUG_COLOR 
#endif 


#if     LOG_USE_TIMESTAMP  >   0
#define LOG_TIME_VALUE        log_time()
#define LOG_TIME_STRING       "[%8d]"

#else
#define LOG_TIME_VALUE         0
#define LOG_TIME_STRING       "[%1d]"
#endif

#define LOG_BREAK              ""

#define LOG_ERROR_PREFIX   LOG_ERROR_COLOR   LOG_TIME_STRING LOG_MODULE_NAME LOG_BREAK "[ERROR]"
#define LOG_WARNING_PREFIX LOG_WARNING_COLOR LOG_TIME_STRING LOG_MODULE_NAME LOG_BREAK "[WARNING]"
#define LOG_INFO_PREFIX    LOG_INFO_COLOR    LOG_TIME_STRING LOG_MODULE_NAME LOG_BREAK "[INFO]"
#define LOG_DEBUG_PREFIX   LOG_DEBUG_COLOR   LOG_TIME_STRING LOG_MODULE_NAME LOG_BREAK "[DEBUG]"


#define  LOG_OUT(format,arg...)     SEGGER_RTT_printf(0,format,##arg)
#define  LOG_INIT()                 SEGGER_RTT_Init() 


#define log_error(format,arg...)                                                          \
    if ((LOG_MODULE_LEVEL >= LOG_LEVEL_ERROR) &&                                          \
        (LOG_LEVEL_ERROR <= LOG_GLOBAL_LEVEL))                                            \
    {                                                                                     \
      LOG_OUT(LOG_ERROR_PREFIX"line:%d\r\n"format,LOG_TIME_VALUE,__LINE__,##arg);                            \
    }
#define log_warning(format,arg...)                                                        \
    if ((LOG_MODULE_LEVEL >= LOG_LEVEL_WARNING) &&                                        \
        (LOG_LEVEL_WARNING <= LOG_GLOBAL_LEVEL))                                          \
    {                                                                                     \
      LOG_OUT(LOG_WARNING_PREFIX"line:%d\r\n"format,LOG_TIME_VALUE,__LINE__,##arg);                          \
    }
  

#define log_info(format,arg...)                                                           \
    if ((LOG_MODULE_LEVEL >= LOG_LEVEL_INFO) &&                                           \
        (LOG_LEVEL_INFO <= LOG_GLOBAL_LEVEL))                                             \
    {                                                                                     \
      LOG_OUT(LOG_INFO_PREFIX"line:%d\r\n"format,LOG_TIME_VALUE,__LINE__,##arg);                             \
    }


#define log_debug(format,arg...)                                                          \
    if ((LOG_MODULE_LEVEL >= LOG_LEVEL_DEBUG) &&                                          \
        (LOG_LEVEL_DEBUG <= LOG_GLOBAL_LEVEL))                                            \
    {                                                                                     \
      LOG_OUT(LOG_DEBUG_PREFIX"line:%d\r\n"format,LOG_TIME_VALUE,__LINE__,##arg);                            \
    }
__weak void log_assert_handler(int line,char *file_name);

#define log_assert(expr)                                                  \
{                                                                         \
if((void *)(expr) == 0){                                                  \
log_assert_handler(__LINE__,__FILE__);	                                  \
}                                                                         \
}                                                                           

#ifdef __cplusplus
}
#endif		

#endif