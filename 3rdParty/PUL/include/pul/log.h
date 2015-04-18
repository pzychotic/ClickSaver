/* Poly
   Useful
   Lib */

#ifndef __PUL_LOG_H__
#define __PUL_LOG_H__

void puInitLog();
void puCloseLog();
void puWriteLogEntry( PUU8* _pCategory, PUU8* _pMessage, PUU32 _Param1, PUU32 _Param2, PUU32 _Param3, PUU32 _Param4 );

//#ifdef _DEBUG
//#define PULOG( Category, Message, Param1, Param2, Param3, Param4 ) puWriteLogEntry( Category, Message, Param1, Param2, Param3, Param4 );
//#else
#define PULOG( Category, Message, Param1, Param2, Param3, Param4 )
//#endif

#endif