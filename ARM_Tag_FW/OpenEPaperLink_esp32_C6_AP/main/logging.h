#pragma once

#if CONFIG_OEPL_DEBUG_PRINT
   #define LOG(format, ... ) printf("%s: " format,__FUNCTION__,## __VA_ARGS__)
   #define LOG_RAW(format, ... ) printf(format,## __VA_ARGS__)
   void DumpHex(void *AdrIn,int Len);
   #define LOG_HEX(x,y) DumpHex(x,y)
#else
   #define LOG(format, ... )
   #define LOG_RAW(format, ... )
   #define LOG_HEX(x,y)
#endif

#if CONFIG_OEPL_VERBOSE_DEBUG
   #define LOGV(format, ... ) LOG(format,## __VA_ARGS__)
   #define LOGV_RAW(format, ... ) LOG_RAW(format,## __VA_ARGS__)
   #define LOGV_HEX(x,y) LOG_HEX(x,y)
#else
   #define LOGV(format, ... )
   #define LOGV_RAW(format, ... )
   #define LOGV_HEX(x,y)
#endif

