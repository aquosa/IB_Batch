#ifdef _USE_FTP_API_
/////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 SISTEMAS S.A.
// Proyecto Sistema OnLine de Atencion de Tarjetas
// Libreria de Utilidades Generales
// Funcion de Transferencia de Archivo via FTP
//
// Plataforma MsWindows-Intel 32 bits, Lenguaje Visual C/C++ 5.0
//
// Fecha      Autor   Obs
// 2000.01.19 mdc     Inicial
// 2000.02.02 mdc     Win32 API "FormatMessage()" para descripcion de errores
// 
/////////////////////////////////////////////////////////////////////////////////////////

// Header MsWindows
#include <windows.h>
// Header ANSI String
#include <string.h>
// Header MsWindows Internet API (debe linkeditarse "wininet.lib")
#include <wininet.h>
// Header propio
#include <qusrinc/stdiol.h>

/////////////////////////////////////////////////////////////////////////////
// Prototipos de funciones locales a este modulo unicamente
int check_ftp_logger(char *szLoggerText, boolean_t bIsFilename,
					 char *szHost, char *szFileFrom, char *szFileTo);
/////////////////////////////////////////////////////////////////////////////


// Macro de copia de string terminada en NULL
#define StringCopy(des,src,len)		if(des && src && len) { strncpy(des,src,len); des[len-1] = 0x00; }



////////////////////////////////////////////////////////////////////////////////
// FTP_API_Send_File
////////////////////////////////////////////////////////////////////////////////
int ftp_api_send_file(char *pszHost,char *pszUser, char *pszPassword,
					  char *pszFileFrom, char *pszFileTo, 
					  char *pszMode, char *pszErrorTxt, unsigned short nMaxTxt )
{
	HINTERNET	hinetSession = NULL,
				hinetService = NULL;
	DWORD		dwError = 0;
	DWORD		dwBinaryAscii = FTP_TRANSFER_TYPE_BINARY ;
	CHAR		szBuffer[10240] = {0};
	DWORD		nBuffLen = sizeof szBuffer;


	// PRECONDITION
	if( !pszHost || !pszUser || !pszPassword || !pszFileFrom || !pszFileTo || !pszMode)
		return (-1);

	// MODE
	if(*pszMode == 'A' || *pszMode == 'a')
		dwBinaryAscii = FTP_TRANSFER_TYPE_ASCII;
	else if(*pszMode == 'B' || *pszMode == 'b')
		dwBinaryAscii = FTP_TRANSFER_TYPE_BINARY;	


	// OPEN INTERNET SESSION
	hinetSession = InternetOpen(		
		"File Transfering Utility" /* Agent Name */,
		INTERNET_OPEN_TYPE_DIRECT /* Access Type : Locally */,
		NULL /* ProxyName */,
		NULL /* ProxyBypass */,
		INTERNET_FLAG_ASYNC /* Connection Options */ );
	// CHECK SESSION
	if(hinetSession == NULL)
	{
		dwError = GetLastError();
		if(ERROR_INTERNET_EXTENDED_ERROR == dwError)
		{
			nBuffLen = sizeof szBuffer;
			InternetGetLastResponseInfo(&dwError,szBuffer,&nBuffLen);
			StringCopy(pszErrorTxt,szBuffer,min(nBuffLen,nMaxTxt));
			dwError = GetLastError();
		}
		// RETURN ERROR
		return(dwError);
	}

	// OPEN INTERNET SERVICE
	hinetService = InternetConnect(
		hinetSession /* Session handle */,
		pszHost /* IP Host-Address */,
		INTERNET_DEFAULT_FTP_PORT /* IP Port */,
		pszUser /* Username */,
		pszPassword /* Password */,
		INTERNET_SERVICE_FTP /* Service */,
		INTERNET_FLAG_PASSIVE /* Service options */,
		0 );
	// CHECK SERVICE
	if(hinetService == NULL)
	{
		dwError = GetLastError();
		if(ERROR_INTERNET_EXTENDED_ERROR == dwError)
		{
			nBuffLen = sizeof szBuffer;
			InternetGetLastResponseInfo(&dwError,szBuffer,&nBuffLen);
			StringCopy(pszErrorTxt,szBuffer,min(nBuffLen,nMaxTxt));
			dwError = GetLastError();
		} else
		{
			nBuffLen = sizeof szBuffer;
			// Win32 API Format Message
			if(FormatMessage(  
				FORMAT_MESSAGE_FROM_SYSTEM, // source and processing options 
				NULL, // pointer to message source 
				dwError, // GetLastError() returned code
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) pszErrorTxt, // pointer to message buffer 
				min(nBuffLen,nMaxTxt),  // buffer size
				NULL ) == 0) // address of array of message inserts 
				// Mensaje de Error por defecto
				StringCopy(pszErrorTxt,"INTERNET_SERVICE_FTP",min(20,nMaxTxt));
		}
		// CLOSE SESSION
		InternetCloseHandle ( hinetSession );
		// RETURN ERROR
		return(dwError);
	}

	// SET DIRECTORY
	if(!FtpSetCurrentDirectory(
		hinetService, /* FTP Session handle */
		"/" /* Root Directory */ ) )
	{
		// WARNING, NO ROOT DIRECTORY
		dwError = GetLastError();
		if(ERROR_INTERNET_EXTENDED_ERROR == dwError)
		{
			nBuffLen = sizeof szBuffer;
			InternetGetLastResponseInfo(&dwError,szBuffer,&nBuffLen);
			StringCopy(pszErrorTxt,szBuffer,min(nBuffLen,nMaxTxt));
			dwError = GetLastError();
		}
		// NO ERROR
		dwError = 0;
	};

	// SEND FILE
	if(!FtpPutFile(
		hinetService /* FTP Service Session handle */,
		pszFileFrom /* Local File for reading data */,
		pszFileTo /* Remote File for sending data */,
		dwBinaryAscii /* Options */,
		0 /* Context */ ))
	{
		dwError = GetLastError();
		if(ERROR_INTERNET_EXTENDED_ERROR == dwError)
		{
			nBuffLen = sizeof szBuffer;
			InternetGetLastResponseInfo(&dwError,szBuffer,&nBuffLen);
			StringCopy(pszErrorTxt,szBuffer,min(nBuffLen,nMaxTxt));
			dwError = GetLastError();
			/////////////////////////////////////////////////////////////////////
			// Verificacion de errores en el logger de FTP.			
			check_ftp_logger( szBuffer, is_false, pszHost, pszFileFrom, pszFileTo );
			/////////////////////////////////////////////////////////////////////
		}
	}

	// CLOSE SERVICE
	InternetCloseHandle ( hinetService );
	// CLOSE SESSION
	InternetCloseHandle ( hinetSession );
	
	// Return Ok-Error
	return (dwError);
};
/////////////////////////////////////////////////////////////////////////////////////////

#endif // _USE_FTP_API_