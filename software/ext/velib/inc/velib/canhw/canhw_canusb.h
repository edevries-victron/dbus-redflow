// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CANHW_LAWICEL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CANHW_LAWICEL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CANHW_LAWICEL_EXPORTS
#define CANHW_LAWICEL_API __declspec(dllexport)
#else
#define CANHW_LAWICEL_API __declspec(dllimport)
#endif

// This class is exported from the canhw_lawicel.dll
class CANHW_LAWICEL_API Ccanhw_lawicel {
public:
	Ccanhw_lawicel(void);
	// TODO: add your methods here.
};

extern CANHW_LAWICEL_API int ncanhw_lawicel;

CANHW_LAWICEL_API int fncanhw_lawicel(void);
