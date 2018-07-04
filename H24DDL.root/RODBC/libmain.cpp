//////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A.
// Proyecto Pago Automatico de Servicios
// R-OpenDataBaseConnectivity Class's Library Entry Point
//
// Tarea        Fecha           Autor   Observaciones
// (Alfa)       1998.08.03      mdc     Base 
//
//
//////////////////////////////////////////////////////////////////////

#define DLLMAIN
// Header WIN32
#include <windows.h>
// Header RODBC
#include <qusrinc/rodbc.h>

// Segun la inicializacion para MS-Windows (16 o 32Bits)
#if defined( _WIN32 )

// MS-WindowsNT 32Bits Library Entry Point
BOOL EXPORT WINAPI DllMain( HINSTANCE hinstDLL,
					        DWORD fdwReason,
						    LPVOID lpvReserved )
	{
	// Segun razon de invocacion
	switch (fdwReason)
		{		
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:     	    
			break;
	    case DLL_THREAD_DETACH:			
			break;
	    case DLL_PROCESS_DETACH:
			break;
		}
	// Ok
	return TRUE;
	}
#else 
// MS-Windows 16Bits Library Entry Point
int EXPORT CALLBACK LibMain(HINSTANCE hInst, WORD wDataSeg, 
                            WORD cbHeap    , LPSTR lpszCmdLine )
	{
    return (1);
    }

// MS-Windows 16Bits Library Exit Point
int EXPORT CALLBACK WEP(int nExit)
	{
	return (nExit);
	}

#endif // _WINDOWS