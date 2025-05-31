#if !defined TCATIOAPI
#define TCATIOAPI

extern "C"
_declspec(dllexport) 
long _stdcall TCatIoOpen();

extern "C"
_declspec(dllexport) 
long _stdcall TCatIoClose();

extern "C"
_declspec(dllexport) 
long _stdcall TCatIoInputUpdate( unsigned short nPort );

extern "C"
_declspec(dllexport) 
long _stdcall TCatIoOutputUpdate( unsigned short nPort );

extern "C"
_declspec(dllexport) 
long _stdcall TCatIoGetInputPtr( unsigned short nPort, void** ppInput, int nSize );

extern "C"
_declspec(dllexport) 
long _stdcall TCatIoGetOutputPtr( unsigned short nPort, void** ppOutput, int nSize ); 

extern "C"
_declspec(dllexport) 
long _stdcall TCatIoGetCpuTime( __int64* pCpuTime );

extern "C"
_declspec(dllexport) 
long _stdcall TCatIoGetCpuCounter( __int64* pCpuCount );

extern "C"
_declspec(dllexport) 
long _stdcall TCatIoReset();

#endif