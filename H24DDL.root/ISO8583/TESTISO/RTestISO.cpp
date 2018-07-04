//
// IT24 Sistemas S.A.
// ISO-8583-Message Propietary Library Tester
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1997.09.26      mdc     Base
// (Inicial)    1997.10.06      mdc     Base
// (Inicial)    1997.10.28      mdc     Base
// (Inicial)    1997.12.02      mdc     Orientado a PAS
// (Beta)       1998.01.21      mdc     Extensión de campos y mensajes evaluados
// (Beta)       1998.02.10      mdc     Extensión de campos y mensajes evaluados
// (Beta)       1998.06.22      mdc     typedefs.h
// (Beta)       1998.07.29      mdc     SOAT message
// (Beta)       1999.11.02      sbb     Parseo campo 126 - SOAT
// (Beta)       2002.02.01      mdc     ATM Transaction
// (Beta)       2005.05.08      mdc     Packed Decimal
//
/* Headers MS-Windows */
#if ((defined(_WIN32) || defined(_WINDOWS))&& defined(_NOTIFY_OPERATOR_))
#include <windows.h>
#endif /* _NOTIFY_OPERATOR_ */


// ANSI headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Propietary headers
#include <qusrinc/typedefs.h>
#include <qusrinc/isoredef.h>
#include <qusrinc/iso8583.h>
// Header EXTRACT BASE24 RED LINK
#include <qusrinc/extrctst.h>
#include <qusrinc/stdiol.h>
#include <qusrinc/databatc.h>

/**************************************************/
/* Mensajes */
/**************************************************/

char szIO_Error[]   = "Error al intentar abrir el archivo <%s>:<%i>\r\n";
char szARGC_Error[] = "Argumentos invalidos,especifique archivo\r\n";

// TIPIFICACION DE FUNCION 
typedef char * (WINAPI *AUTORIZADOR_FINANSUR) (char Buffer[]);


// Main function
int main(int argc, char *argv[], char *envp[])
	{
	char    szTrxFilename[256] = {0x00},
            szOutputFile[256]  = {0x00};	
	int		iRetval = 0;
	FILE	*hFile = NULL;
	char    szISOReqTrx[1024*100] = {0x00};
	char	*pszSeparator  = NULL;
	short	nLen = 0;	
	HINSTANCE   hModule    = LoadLibrary( "LKPONLIN.DLL" );
	AUTORIZADOR_FINANSUR   ptrFunc = (AUTORIZADOR_FINANSUR)GetProcAddress( hModule, "LKPONLIN");
	msgEXTRACT        extractBuff;
	char              *Buffer        =(char *)&extractBuff;
    char              szTextToPacked[]                          = "+1234567";
    unsigned char     uchPackedDecimal[sizeof(szTextToPacked)]  = {0,0,0,0,0,0,0,0};
    SALDOS  enSALDOS;
    MOVCUE  enMOVCUE;
    size_t  nRet = 0;


    nRet = TextToPacked(uchPackedDecimal, szTextToPacked, sizeof(uchPackedDecimal));    
    nRet = sizeof(enSALDOS);
    nRet = sizeof(enMOVCUE);


    /*------------------------------*\
    *  Invocar programa autorizador  *
    \*------------------------------*/
	if(hModule && ptrFunc)
       (*ptrFunc)( Buffer );

	if(2 >= argc)
        strcpy(szTrxFilename, argv[1]);
    else
    {
        fprintf(stdout,szARGC_Error);
        return -1;
    }

	hFile = fopen(szTrxFilename, "rt");
	if(!hFile)
    {
        fprintf(stdout,szIO_Error, szTrxFilename, errno);
		return -2;
    }
    else
    {
        strcpy(szOutputFile, szTrxFilename);
        strcat(szOutputFile, ".OUT");
    }

    //////////////////////////////////////////////////////////////	
	for(short nCounter = 1;
		hFile && 
		fgets( szISOReqTrx, sizeof szISOReqTrx, hFile ) && 
		nCounter < 999;
		nCounter++ )
	{	
		pszSeparator = strstr(szISOReqTrx, "ISO");
		if(!pszSeparator)
			break;
		nLen = strlen(pszSeparator);
		iRetval = ISO_Test( (PBYTE)pszSeparator, nLen, true, 
                            szOutputFile, 1==nCounter );		
	}
	//////////////////////////////////////////////////////////////
	
    fclose( hFile );
	hFile = NULL;

    // OK!
    return 0;
}    


