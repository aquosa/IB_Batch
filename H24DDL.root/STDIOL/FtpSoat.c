#ifdef _USE_FTP_API_
//                                                              
// IT24 Sistemas S.A.
// Standard I/O Library Extension (STDIOL)
//
// Tarea		Fecha			Autor	Observaciones
// (Inicial)	1999.07.13		mdc		DoFileTransfering() acepta hosts opcionales, y
//										chequear si es Guardian-Tandem el FileSystem de destino.
// (Alfa)   	1999.09.23		mdc		DoFileTransfering() acepta booleano de reemplazo de "."
// (Alfa)   	1999.09.29		mdc		Agregada move_file();
// (Alfa)   	1999.10.06		mdc		Excepciones x FIID en file-tranfer-protocol.
// (Alfa)   	1999.11.09		mdc		DoFileTransfering agrega notificacion a usuario ante fallos
// 				1999.11.15		GDG		open_file con append, solo p/iAccessMode 1(write)
// 				1999.11.15		MDC		Ajuste en condicion de terminacion de lectura por CRLF
// (Alfa)       1999.11.16      mdc     get_file_size();
// (Alfa)       2000.01.06      mdc     Asumir FIID no formateable en path si no hay mascara, en UNIX-Style.
// (Alfa)   	2000.01.19		mdc		ftp_api_send_file() como alternativa a ftp_send_file().
// (Alfa)   	2000.02.03		mdc		DoFileTransfering_Ext()
// (Alfa)   	2000.02.09		mdc		DoFileTransfering() mapea a DoFileTransfering_Ext().
// (Alfa)   	2000.02.21		mdc		hexcharbinary() tambien acepta minusculas en hexadecimal
// (Alfa)   	2000.03.06      mdc     Agregado host "MLINKRPT".

// Ms-Windows Header
#include <windows.h> 

// ANSI C/C++ Standard Library functions
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h> 
#include <errno.h>
#include <math.h>
// Header propio
#include <qusrinc/stdiol.h>

///////////////////////////////////////////////////////////////////////////////
// Macro de Longitud de buffer de mensajes predefinida
#define MSG_DEFAULT_LEN   3072
// Longitud default de FIID como string
#define FIID_DEFAULT_LEN  4
///////////////////////////////////////////////////////////////////////////////

/* Host IDs predefinidos para hacer envios via FTP */
/* Utilizados en "DoFileTransfering()" */
/* 1999.09.23, mdc. En base a IDs hardcodeados en otros modulos */
/* 2000.03.06, mdc. Agregado _FTP_MLINKRPT_HOST_ID_             */
#define _BASE24_HOST_ID_		(0)
#define _MLINK_HOST_ID_			(1)
#define _MLINKRPT_HOST_ID_		(2)
#define _FTP_BASE24_HOST_ID_	_BASE24_HOST_ID_
#define _FTP_MLINK_HOST_ID_		_MLINK_HOST_ID_
#define _FTP_MLINKRPT_HOST_ID_	_MLINKRPT_HOST_ID_


///////////////////////////////////////////////////////////////////// 
// Funcion de transferencia de archivo mediante servicio de FTP del
// Sistema Operativo 
///////////////////////////////////////////////////////////////////// 
int ftp_send_file(char *szHost,char *szUser, char *szPassword, char *szMode,
				  char *szFileFrom, char *szFileTo, char *szLogger)
{
	// Formato de script de File-Transfer-Protocol para Tandem-Guardian 
	char   szScriptFrmt[] = "open %s\n"
							"user %s %s\n"
							"%s\n"
							"send %s %s\n"
							"close\n" ;
	// Buffer de script de FTP
	char   szScript[MSG_DEFAULT_LEN] = {0};
	// Buffer de comando de FTP o NET (128 caracteres mas que el buffer de lectura, para el comando)
	char   szCommand[MSG_DEFAULT_LEN+128] = {0};
	// Logger
	char	szLogFile[256] = {0};
	// Handler para archivo temporal de scripting de FTP
	FILE	*hTmpFile     = NULL;	
	char    *pchExtension = NULL,
			szAuxTmpFile[] = {".\\transfer.ftp"},
			*pszTmpFile   = szAuxTmpFile;
	int     iRetValSys    = 0,
			iRetValRemove = 0;
	// Auxiliares para verificacion del logger de FTP
	pvoid_t hLogFile = NULL;
	long	lSize = 0L;

	//////////////////////////////////////////////////////
	// Ejemplo de script de FTP:
	// " open 200.200.4.4 
	//   user test.super 12345678 
	//   binary 
	//   send c:\soat\0268\ex990101 $db24.soat0268.ex990101
	//   close "
	//////////////////////////////////////////////////////
	 	
	//////////////////////////////////////////////////////
	// Ejemplo de comando de FTP:
	// " ftp.exe -n -s:transfer.ftp > transfer.log "
	//////////////////////////////////////////////////////
	
	// Precondiciones : todos punteros no nulos
	if(!(szHost && szUser && szPassword && szMode && szFileFrom && szFileTo))
		return (-1);

	// Formateo de script de File-Transfer-Protocol para Tandem-Guardian 
	sprintf(szScript, szScriptFrmt,
			szHost,
			szUser, szPassword,
			szMode,
			szFileFrom, szFileTo);
#if defined( _USE_TMP_FILENAME_ )
	// Nombre para archivo temporal
	pszTmpFile = tmpnam(NULL);
#endif // _USE_TMP_FILENAME_
	if(pszTmpFile == NULL)
		return (LRESULT)FALSE;
	// Apertura de archivo temporal
	hTmpFile = fopen(pszTmpFile, "wt");
	if(hTmpFile == NULL)
	{
		// Reintento de apertura de archivo temporal, ya que el nombre
		// generado puede ser sobre un directorio sin permisos...
		// Se intenta directorio actual
		pszTmpFile = szAuxTmpFile;
		hTmpFile = fopen(pszTmpFile, "wt");
		if(hTmpFile == NULL)
			return (-2);
	};
	// Escritura al archivo temporal
	fprintf(hTmpFile, szScript);
	// Flush al archivo temporal
	fflush(hTmpFile);
	
	// Se parametriza el Logger?
	if(szLogger != NULL)
	{
		// De entrada o de salida es este parametro? Verificar 1er. caracter
		if(szLogger[0] != 0x00 && szLogger[0] != ' ')
			strcpy(szLogFile, szLogger);
		else
		{
			// Asumir que es de salida y copiar el nombre adoptado para el logger
			strcpy(szLogFile, pszTmpFile);
			strcat(szLogFile, ".log");
			strcpy(szLogger, szLogFile);
		};
	}
	else
	{
		strcpy(szLogFile, pszTmpFile);
		strcat(szLogFile, ".log");
	};
	// Formateo de Comando FTP al Sistema Operativo	
	sprintf(szCommand, "ftp -n -s:%s > %s", pszTmpFile, szLogger);
	// Comando al Sistema Operativo
	iRetValSys = system(szCommand);
	
	// Flush al archivo temporal
	fflush(hTmpFile);
	// Cerrar archivo temporal, y borrarlo por ende
	fclose(hTmpFile);

	// While file remains... try to delete it
	do	
		iRetValRemove = remove(pszTmpFile);
	while( errno != ENOENT && errno != EACCES && iRetValRemove != 0 );

	/////////////////////////////////////////////////////////////////////
	// Verificacion de errores en el logger de FTP.
	// Adaptado de codigo anterior en una funcion. 2000.01.19,mdc.
	check_ftp_logger( szLogger, is_true, szHost, szFileFrom, szFileTo );
	/////////////////////////////////////////////////////////////////////

	// Retorno dependiente de "system()"
	return (iRetValSys);
};

#if( _LINK_FTP_FUNCTION_ == true )
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Funcion de transferencia de archivo mediante servicio de FTP del
// Sistema Operativo , usando parametros desde un Profile 
// 1999.07.07 mdc Base
// 1999.09.23 mdc Agregado bool bExtAllowed
// 1999.11.09 mdc Agregada la notificacion a usuarios via intranet
// 2000.01.19 mdc Usar FTP mediante APIs
// 2000.02.09 mdc Hacerla obsoleta
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int DoFileTransfering_Obsolete(char *szFIID, char *szXferFile, char *szMode,
					  char *szLogger, char *szINIFileName, int iHost, 
					  boolean_t bExtDisable )
{
	// Local Xfer variables
	char	szXFerHost[128] = {0},
			szXFerUser[32]  = {0},
			szXFerPassw[32] = {0},
			szXFerMode[16]  = {0},
			szXFerRemotePath[128] = {0},
			szXFerFileFrom[256]   = {0},
			szXFerFileTo[256]     = {0},
			szFileName[256]       = {0},
			szErrorTxt[256]       = {"(SIN DESCRIPCION)"}; /* Texto de error de FTP x APIs. 2000.01.19,mdc */
	char	*pchExtension = NULL;
	// Seccion de configuracion del host elegido
	char	szSectionFrmt[]       = {"FTPHost%i"};
	char	szProfileSection[256] = {"FTPHost0"};
	// Seccion de configuracion del host con excepciones x/FIID, 1999.10.06, mdc.
	char	szSectionFrmtFIID[]   = {"FTPHost%i-%s"};
	// Booleano que indica si se ha reemplazo el FIID en el path
	boolean_t bFiidReplaced = is_false;
	// Valor de retorno de ftp_send_file()
	int		iRetVal = 0;

	// Precondicion : Host parametrizado valido {0=Principal,1..N=Opcionales}
	if(iHost < 0 || iHost > 99 || szFIID == NULL)
		return (-1);

// Para MsWin16/32/2000 utilizar APIs de lecturas de .INIs
#if defined(_WIN32) || defined(_WINDOWS)
	//////////////////////////////////////////////////////////////////////////////
	// Hosts parametrizado {0=Principal,1..N=Opcionales} con excepciones x fiid?
	// 1999.10.06, mdc.
	sprintf(szProfileSection, szSectionFrmtFIID, iHost, szFIID);
	//////////////////////////////////////////////////////////////////////////////
	// Verificar si existe, y leer variables de Xfer para FTP con excepcion x FIID
	//////////////////////////////////////////////////////////////////////////////
	GetPrivateProfileString(szProfileSection,// Seccion 
		"XFerHost",               // Item Host Address
		"(NULL)",                 // Valor NULL 
		szXFerHost,               // Buffer de Copia
		sizeof szXFerHost,        // Longitud de Buffer
		szINIFileName);           // Archivo Inicializacion
	// No hay excepciones para este Host? Tomarlo de seccion general entonces...
	if(strcmp(szXFerHost,"(NULL)") == 0)
		// Hosts parametrizado {0=Principal,1..N=Opcionales}
		sprintf(szProfileSection,szSectionFrmt, iHost);
	//////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	// Verificar si existe, y leer variables de Xfer para FTP
	//////////////////////////////////////////////////////////////////////////////
	GetPrivateProfileString(szProfileSection,// Seccion 
		"XFerHost",               // Item Host Address
		"127.0.0.1",              // Loopback por default IP 
		szXFerHost,               // Buffer de Copia
		sizeof szXFerHost,        // Longitud de Buffer
		szINIFileName);           // Archivo Inicializacion
	GetPrivateProfileString(szProfileSection,// Seccion 
		"XFerUser",				  // Item Usuario
		"test.super",             // Usuario por default en sistema remoto
		szXFerUser,               // Buffer de Copia
		sizeof szXFerUser,        // Longitud de Buffer
		szINIFileName);           // Archivo Inicializacion
	GetPrivateProfileString(szProfileSection,// Seccion 
		"XFerPassw",			  // Item password
		"12345678",               // Password de usuario
		szXFerPassw,              // Buffer de Copia
		sizeof szXFerPassw,       // Longitud de Buffer
		szINIFileName);           // Archivo Inicializacion
	GetPrivateProfileString(szProfileSection,// Seccion 
		"XFerMode",				  // Item Modo de Transferencia
		"binary",                 // Binario x default
		szXFerMode,               // Buffer de Copia
		sizeof szXFerMode,        // Longitud de Buffer
		szINIFileName);           // Archivo Inicializacion
	GetPrivateProfileString(szProfileSection,// Seccion 
		"XFerRemotePath",		  // Item Path remoto
		"\\system.$vol",          // Generico x default
		szXFerRemotePath,         // Buffer de Copia
		sizeof szXFerRemotePath,  // Longitud de Buffer
		szINIFileName);           // Archivo Inicializacion
	//////////////////////////////////////////////////////////////////////////////
#endif // WINDOWS

	// Formateo de nombres de archivos desde-hasta
	strcpy(szXFerFileFrom, szXferFile);
	strcpy(szFileName    , szXferFile);
	
	// ...Y asegurar que no posea la extension MS-DOS/WINDOWS correspondiente 
	// Solo si asi es requerida. 
	// 1999.09.23, mdc.
	if(bExtDisable == is_true)
	{
		pchExtension = strrchr(szFileName, '.');
		if(pchExtension != NULL)
			(*pchExtension) = 0x00;
	};
	// ...Ni los divisores de directorios-subdirectorios
	pchExtension = strrchr(szFileName, '\\');
	if(pchExtension != NULL)
		strcpy(szFileName, pchExtension+1);

// Segun el sistema definido, pueden formatearse de diversa forma los "paths"
#if defined( _SYSTEM_SOAT_ )
	// Bajo SOAT se agrega el chequeo de FIID parametrizable en Path de destino...
	// Ahora bien, ese FIID puede representarse con los 4 digitos de longitud, o menos.
	// 1999.09.23, mdc.
	if(pchExtension = strstr(szXFerRemotePath, "@@@@"))
		{
		strncpy(pchExtension, szFIID, FIID_DEFAULT_LEN);
		bFiidReplaced = is_true;
		}
	else if (pchExtension = strstr(szXFerRemotePath, "@@@"))
		{
		strncpy(pchExtension, szFIID + 1, FIID_DEFAULT_LEN - 1);
		bFiidReplaced = is_true;
		}
	else if (pchExtension = strstr(szXFerRemotePath, "@@"))
		{
		strncpy(pchExtension, szFIID + 2, FIID_DEFAULT_LEN - 2);
		bFiidReplaced = is_true;
		};
	// fin-1999.09.23, mdc.

	// Chequear si es Guardian-Tandem el FileSystem de destino...
	if(strchr(szXFerRemotePath, '$') != NULL)
	{
		// Formatear para "\System.$Volume.Subvolume.Filename" en Guardian-Tandem
		if(bFiidReplaced == is_false)
			sprintf(szXFerFileTo  , "%s.SOAT%s.%s", szXFerRemotePath, szFIID, szFileName);
		else
			sprintf(szXFerFileTo  , "%s.%s", szXFerRemotePath, szFileName);
	}
	// Chequear si es FAT-16/32 el FileSystem de destino...
	else if(strchr(szXFerRemotePath, ':') != NULL || strchr(szXFerRemotePath, '\\') != NULL)
	{
		// Formatear para "Drive:\Directory\Filename" en MS-DOS/WINDOWS 	
		if(bFiidReplaced == is_false)
			sprintf(szXFerFileTo  , "%s\\%s\\%s", szXFerRemotePath, szFIID, szFileName);
		else
			sprintf(szXFerFileTo  , "%s\\%s", szXFerRemotePath, szFileName);
	}
	else
	{
		// Asumir formateando para "/Root/Directory/Filename" en UNIX
		if(bFiidReplaced == is_false)
			////////////////////////////////////////////////////////////////////////////////
			// 2000.01.06 mdc
			// Si no hubo una mascara de formateo con FIID, simplemente asumir que 
			// no debe haber un FIID en el path, si es que es de estilo Unix.
			// Si debiera haber un FIID, agregarlo como mascara "@@@@" en el .INI
			// Esta linea es la que debiera ir si hubiera de ponerse un FIID:
			// "sprintf(szXFerFileTo  , "%s/%s/%s", szXFerRemotePath, szFIID, szFileName);"
			////////////////////////////////////////////////////////////////////////////////
			sprintf(szXFerFileTo  , "%s/%s", szXFerRemotePath, szFileName);
		else
			sprintf(szXFerFileTo  , "%s/%s", szXFerRemotePath, szFileName);
	};
#else
	// Chequear si es Guardian-Tandem el FileSystem de destino...
	if(strchr(szXFerRemotePath, '$') != NULL)
		// Formatear para "\System.$Volume.Subvolume.Filename" en Guardian-Tandem
		sprintf(szXFerFileTo  , "%s.%s.%s", szXFerRemotePath, szFIID, szFileName);	
	// Chequear si es FAT-16/32 el FileSystem de destino...
	else if(strchr(szXFerRemotePath, ':') != NULL || strchr(szXFerRemotePath, '\\') != NULL)
		// Formatear para "Drive:\Directory\Filename" en MS-DOS/WINDOWS 		
		sprintf(szXFerFileTo  , "%s\\%s\\%s", szXFerRemotePath, szFIID, szFileName);
	else
		// Asumir formateando para "/Root/Directory/Filename" en UNIX
		sprintf(szXFerFileTo  , "%s/%s/%s", szXFerRemotePath, szFIID, szFileName);
#endif // _SYSTEM_SOAT_ 


//////////////////////////////////////////////////////////////////////////////////////
// Segun la definicion de uso de API de FTP o COMANDO de FTP
//////////////////////////////////////////////////////////////////////////////////////
	// Enviar via API de FTP... default usuario "anonymous"
	iRetVal = ftp_api_send_file(szXFerHost,szXFerUser,szXFerPassw,
								szXFerFileFrom,szXFerFileTo,szMode,
								szErrorTxt,sizeof(szErrorTxt));
	// Error?
	if(iRetVal != 0 && szLogger != NULL)
	{
		// Dirigir al logger el texto de error de FTP 
		FILE *hfFTPMsg = fopen(szLogger,"w");
		if(hfFTPMsg)
		{
			fprintf(hfFTPMsg,"Error API FTP [%i]=%s\r\n",iRetVal,szErrorTxt);
			fclose(hfFTPMsg);
			hfFTPMsg = NULL;
		};
	};
//////////////////////////////////////////////////////////////////////////////////////
	

	// Retornar valor de ftp_send_file()
	return (iRetVal);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // _LINK_FTP_FUNCTION_

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Funcion de transferencia de archivo mediante comando del S.O., usando parametros desde un Profile 
// 1999.11.26 mdc Base
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int DoFileCopy(char *szFIID, char *szXferFile, char *szINIFileName )
{
	// Local Copy variables
	char	szXFerRemotePath[128] = {0},
			szXFerFileFrom[256]   = {0},
			szXFerFileTo[256]     = {0},
			szFileName[256]       = {0},
			szCommand[512]        = {0};
	char	*pchExtension = NULL;
	char	szProfileSection[256] = {"StartUp"};
	// Booleano que indica si se ha reemplazo el FIID en el path
	boolean_t bFiidReplaced = is_false;
	// Valor de retorno de ftp_send_file()
	int		iRetVal = 0;

// Para MsWin16/32/2000 utilizar APIs de lecturas de .INIs
#if defined(_WIN32) || defined(_WINDOWS)
	// Path de copia
	GetPrivateProfileString(szProfileSection,// Seccion 
		"ReportOutputPath",		// Item Path remoto
		"(none)",				// Generico x default
		szXFerRemotePath,       // Buffer de Copia
		sizeof szXFerRemotePath,// Longitud de Buffer
		szINIFileName);         // Archivo Inicializacion
	//////////////////////////////////////////////////////////////////////////////
#endif // WINDOWS

	// Formateo de nombres de archivos desde-hasta
	strcpy(szXFerFileFrom, szXferFile);
	strcpy(szFileName    , szXferFile);
	
	// Sin los divisores de directorios-subdirectorios
	pchExtension = strrchr(szFileName, '\\');
	if(pchExtension != NULL)
		strcpy(szFileName, pchExtension+1);

// Segun el sistema definido, pueden formatearse de diversa forma los "paths"
#if defined( _SYSTEM_SOAT_ )
	// Bajo SOAT se agrega el chequeo de FIID parametrizable en Path de destino...
	// Ahora bien, ese FIID puede representarse con los 4 digitos de longitud, o menos.	
	if(szFIID)
	{
		if(pchExtension = strstr(szXFerRemotePath, "@@@@"))
			{
			strncpy(pchExtension, szFIID, FIID_DEFAULT_LEN);
			bFiidReplaced = is_true;
			}
		else if (pchExtension = strstr(szXFerRemotePath, "@@@"))
			{
			strncpy(pchExtension, szFIID + 1, FIID_DEFAULT_LEN - 1);
			bFiidReplaced = is_true;
			}
		else if (pchExtension = strstr(szXFerRemotePath, "@@"))
			{
			strncpy(pchExtension, szFIID + 2, FIID_DEFAULT_LEN - 2);
			bFiidReplaced = is_true;
			};
	};
#endif // _SYSTEM_SOAT_

	// Chequear si es FAT-16/32 el FileSystem de destino...
	if(strchr(szXFerRemotePath, ':') != NULL || strchr(szXFerRemotePath, '\\') != NULL)
	{
		// Formatear para "Drive:\Directory\Filename" en MS-DOS/WINDOWS 	
		if(bFiidReplaced == is_false)
			sprintf(szXFerFileTo  , "%s\\%s\\%s", szXFerRemotePath, szFIID, szFileName);
		else
			sprintf(szXFerFileTo  , "%s\\%s", szXFerRemotePath, szFileName);
	}
	else
	{
		// Asumir formateando para "/Root/Directory/Filename" en UNIX
		if(bFiidReplaced == is_false)
			sprintf(szXFerFileTo  , "%s/%s/%s", szXFerRemotePath, szFIID, szFileName);
		else
			sprintf(szXFerFileTo  , "%s/%s", szXFerRemotePath, szFileName);
	};

	// Copiar mediante API : segun retorno booleano, indicar en iRetVal : 0=Ok, -1=Error
	iRetVal = CopyFile(szXFerFileFrom, szXFerFileTo, FALSE ) 
				? 0 
				: (-1);	

	// Retornar valor de CopyFile() : 0=Ok, N=Error
	return (iRetVal);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// Verificar errores de transferencia en el logger
// 2000.01.19, mdc, en base a codigo de funcion ftp_send_file();
//
// szLogger=string del nombre del archivo
// bMustExist=booleano que indica si debe existir o no el archivo cuando hay errores
//////////////////////////////////////////////////////////////////////////////////////
int check_ftp_logger(char *szLoggerText, boolean_t bIsFilename,
					 char *szHost, char *szFileFrom, char *szFileTo)
{
	// Buffer de comando de FTP o NET (128 caracteres mas que el buffer de lectura, para el comando)
	char   szCommand[MSG_DEFAULT_LEN+128] = {0};
	int    iRetValSys = 0;

	// Es nombre de archivo o directamente texto de error?
	if(bIsFilename)
	{
		// Auxiliares para verificacion del logger de FTP
		pvoid_t hLogFile = NULL;
		long	lSize = 0L;
		// Buffer de lectura del logger de FTP
		char   szReadBuffer[MSG_DEFAULT_LEN] = {0};

		// Apertura de archivo de logger en forma de solo lectura
		hLogFile = open_file(szLoggerText, 2, 2);
		if(hLogFile == NULL)
			return (-1);
		// Lectura y verificacion de palabras criticas ("denied","found","can't copy")
		if(read_file(hLogFile, szReadBuffer, sizeof szReadBuffer, &lSize) > 0)
		{
			// Cerrar el logger
			close_file(hLogFile);
			hLogFile = NULL;
			// Verificar transferencia satisfactoria
			if(	strstr(strlwr(szReadBuffer), "transfer complete") == NULL )
			{
				// Por error, informar en forma general a los usuarios conectados
				// Codigo de retorno local de ejecucion de comando 
				int iRetValCmd = 0;
				// Formateo de Comando NET SEND al Sistema Operativo	
				sprintf(szCommand, "net send /domain:computos FTP: "
					"FALLO EN TRANSFERENCIA DESDE '%s' HACIA '%s' EN '%s'", 
					szFileFrom, szFileTo, szHost);
				
				// Comando al Sistema Operativo
				iRetValCmd = system(szCommand);

				// Asegurar error de retorno distinto de cero
				if(0 == iRetValSys)
					iRetValSys = 1;
			};//end-if
		};//end-if
		// Cerrar el logger
		if(hLogFile)
			close_file(hLogFile);
	}
	else
	{
		// Por error, informar en forma general a los usuarios conectados
		// Codigo de retorno local de ejecucion de comando 
		int iRetValCmd = 0;
		// Formateo de Comando NET SEND al Sistema Operativo	
		sprintf(szCommand, "net send /domain:computos FTP: "			
			"FALLO EN TRANSFERENCIA EN '%s' DESDE '%s' HACIA '%s' = '%s'", 
			szHost, szFileFrom, szFileTo, 
			szLoggerText);				
		// Comando al Sistema Operativo
		iRetValCmd = system(szCommand);	
		// Con error de retorno
		iRetValSys = 1;
	};
	// OK?
	return (iRetValSys);
};
//////////////////////////////////////////////////////////////////////////////////////

#if( _LINK_FTP_FUNCTION_ == true )
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Funcion de transferencia de archivo mediante servicio de FTP del
// Sistema Operativo , usando parametros desde base de datos via odbc
// 2000.02.03 mdc Inicial en base a "DoFileTransfering".
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int DoFileTransfering_Ext(char *szFIID, char *szXferFile, char *szLoggerFile,
						  char *szHostType, boolean_t bExtDisable, 
						  char *szExtErrorMsg, size_t nErrorTxt )
{
	// Local Xfer variables
	char	szXFerFIID[32+1]        = {0},
			szXFerHostType[32+1]    = {0},
			szXFerHost[128+1]       = {0},
			szXFerUser[32+1]        = {0},
			szXFerPassw[32+1]       = {0},
			szXFerMode[16+1]        = {0},
			szXFerRemotePath[128+1] = {0},
			szXFerFileFrom[256+1]   = {0},
			szXFerFileTo[256+1]     = {0},
			szFileName[256+1]       = {0},
			szErrorTxt[256+1]       = {"(SIN DESCRIPCION)"}; /* Texto de error de FTP x APIs. 2000.01.19,mdc */
	char	*pchExtension = NULL;
	// Booleano que indica si se ha reemplazo el FIID en el path
	boolean_t bFiidReplaced = is_false;
	// Valor de retorno de ftp_send_file()
	int		iRetVal = 0;
	// Ptr a Archivo de logger
	FILE    *hfFTPMsg = NULL;

	// Precondicion : Host parametrizado valido {0=Principal,1..N=Opcionales}
	if(szHostType == NULL || szFIID == NULL)
		return (-1);

	///////////////////////////////////////////////////////////////////////////
	// Leer parametros desde base de datos via odbc
	strcpy(szXFerFIID,szFIID);         // El FIID es de input, no bindearle una columna
	strcpy(szXFerHostType,szHostType); // El HOST-Type es de input, no bindearle una columna
	ftp_read_db_params(
		szXFerHostType, sizeof(szXFerHostType)-1,
		szXFerFIID, sizeof(szXFerFIID)-1,
		szXFerHost, sizeof(szXFerHost)-1,
		szXFerUser, sizeof(szXFerUser)-1,
		szXFerPassw, sizeof(szXFerPassw)-1,
		szXFerMode, sizeof(szXFerMode)-1,
		szXFerRemotePath, sizeof(szXFerRemotePath)-1 
		);
	///////////////////////////////////////////////////////////////////////////

	// Formateo de nombres de archivos desde-hasta
	strcpy(szXFerFileFrom, szXferFile);
	strcpy(szFileName    , szXferFile);
	
	// ...Y asegurar que no posea la extension MS-DOS/WINDOWS correspondiente 
	// Solo si asi es requerida. 
	// 1999.09.23, mdc.
	if(bExtDisable == is_true)
	{
		pchExtension = strrchr(szFileName, '.');
		if(pchExtension != NULL)
			(*pchExtension) = 0x00;
	};
	// ...Ni los divisores de directorios-subdirectorios
	pchExtension = strrchr(szFileName, '\\');
	if(pchExtension != NULL)
		strcpy(szFileName, pchExtension+1);

// Segun el sistema definido, pueden formatearse de diversa forma los "paths"
#if defined( _SYSTEM_SOAT_ )
	// Bajo SOAT se agrega el chequeo de FIID parametrizable en Path de destino...
	// Ahora bien, ese FIID puede representarse con los 4 digitos de longitud, o menos.
	// 1999.09.23, mdc.
	if(pchExtension = strstr(szXFerRemotePath, "@@@@")) // FIID_DEFAULT_LEN = STRLEN("@@@@") = 4
		{
		strncpy(pchExtension, szFIID, FIID_DEFAULT_LEN);
		bFiidReplaced = is_true;
		}
	else if (pchExtension = strstr(szXFerRemotePath, "@@@"))
		{
		strncpy(pchExtension, szFIID + 1, FIID_DEFAULT_LEN - 1);
		bFiidReplaced = is_true;
		}
	else if (pchExtension = strstr(szXFerRemotePath, "@@"))
		{
		strncpy(pchExtension, szFIID + 2, FIID_DEFAULT_LEN - 2);
		bFiidReplaced = is_true;
		};
	// fin-1999.09.23, mdc.

	// Chequear si es Guardian-Tandem el FileSystem de destino...
	if(strchr(szXFerRemotePath, '$') != NULL)
	{
		/////////////////////////////////////////////////////////////////////////
		// Si es hacia Tandem y no se requeria el reemplazo de la extension,
		// dara un error de transferencia, asi que, se asegura que no
		// haya extensiones. 2000.02.03,mdc		
		if(bExtDisable == is_false && (pchExtension = strrchr(szFileName, '.')) )		
			(*pchExtension) = 0x00;		
		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////
		// Se asume que el XFerRemotePath contiene un "." final?
		if(szXFerRemotePath[strlen(szXFerRemotePath)-1] == '.')
			szXFerRemotePath[strlen(szXFerRemotePath)-1] = 0x00;
		/////////////////////////////////////////////////////////
		// Formatear para "\System.$Volume.Subvolume.Filename" en Guardian-Tandem
		if(bFiidReplaced == is_false)
			sprintf(szXFerFileTo  , "%s.SOAT%s.%s", szXFerRemotePath, szFIID, szFileName);
		else
			sprintf(szXFerFileTo  , "%s.%s", szXFerRemotePath, szFileName);
	}
	// Chequear si es FAT-16/32 el FileSystem de destino...
	else if(strchr(szXFerRemotePath, ':') != NULL || strchr(szXFerRemotePath, '\\') != NULL)
	{
		/////////////////////////////////////////////////////////
		// Se asume que el XFerRemotePath contiene un "\" final?
		if(szXFerRemotePath[strlen(szXFerRemotePath)-1] == '\\')
			szXFerRemotePath[strlen(szXFerRemotePath)-1] = 0x00;
		/////////////////////////////////////////////////////////
		// Formatear para "Drive:\Directory\Filename" en MS-DOS/WINDOWS 	
		if(bFiidReplaced == is_false)
			sprintf(szXFerFileTo  , "%s\\%s\\%s", szXFerRemotePath, szFIID, szFileName);
		else
			sprintf(szXFerFileTo  , "%s\\%s", szXFerRemotePath, szFileName);
	}
	else
	{
		/////////////////////////////////////////////////////////
		// Se asume que el XFerRemotePath contiene un "/" final?
		if(szXFerRemotePath[strlen(szXFerRemotePath)-1] == '/')
			szXFerRemotePath[strlen(szXFerRemotePath)-1] = 0x00;
		/////////////////////////////////////////////////////////
		// Asumir formateando para "/Root/Directory/Filename" en UNIX-WinFTP
		if(bFiidReplaced == is_false)
			////////////////////////////////////////////////////////////////////////////////
			// 2000.01.06 mdc
			// Si no hubo una mascara de formateo con FIID, simplemente asumir que 
			// no debe haber un FIID en el path, si es que es de estilo Unix.
			// Si debiera haber un FIID, agregarlo como mascara "@@@@" en el .INI
			// Esta linea es la que debiera ir si hubiera de ponerse un FIID:
			// "sprintf(szXFerFileTo  , "%s/%s/%s", szXFerRemotePath, szFIID, szFileName);"
			////////////////////////////////////////////////////////////////////////////////
			sprintf(szXFerFileTo  , "%s/%s", szXFerRemotePath, szFileName);
		else
			sprintf(szXFerFileTo  , "%s/%s", szXFerRemotePath, szFileName);
	};
#else
	// Chequear si es Guardian-Tandem el FileSystem de destino...
	if(strchr(szXFerRemotePath, '$') != NULL)
		// Formatear para "\System.$Volume.Subvolume.Filename" en Guardian-Tandem
		sprintf(szXFerFileTo  , "%s.%s.%s", szXFerRemotePath, szFIID, szFileName);	
	// Chequear si es FAT-16/32 el FileSystem de destino...
	else if(strchr(szXFerRemotePath, ':') != NULL || strchr(szXFerRemotePath, '\\') != NULL)
		// Formatear para "Drive:\Directory\Filename" en MS-DOS/WINDOWS 		
		sprintf(szXFerFileTo  , "%s\\%s\\%s", szXFerRemotePath, szFIID, szFileName);
	else
		// Asumir formateando para "/Root/Directory/Filename" en UNIX
		sprintf(szXFerFileTo  , "%s/%s/%s", szXFerRemotePath, szFIID, szFileName);
#endif // _SYSTEM_SOAT_ 


	// Enviar via API de FTP... default usuario "anonymous"
	iRetVal = ftp_api_send_file(szXFerHost,szXFerUser,szXFerPassw,
								szXFerFileFrom,szXFerFileTo,szXFerMode,
								szErrorTxt,sizeof(szErrorTxt));
	// Error? Y hay un logger de envio?
	if(iRetVal != 0 && szLoggerFile != NULL)
	{
		// Por error, dirigir al logger el texto de FTP 
		hfFTPMsg = fopen(szLoggerFile,"w");
		if(hfFTPMsg)
		{
			fprintf(hfFTPMsg,
				"Libreria STDIOL, funcion DoFileTransfering()\r\n"
				"Error en envio de FTP via WinAPI, [%i]=%s,\r\n"
				"hacia %s como %s\r\n",
				iRetVal,szErrorTxt,szXFerHost,szXFerFileTo);
			fclose(hfFTPMsg);
			hfFTPMsg = NULL;
		};
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////////
		// 2000.03.06,mdc. Agrega un logger aunque haya dado ok.
		// Por OK, dirigir al logger el texto de FTP 
		hfFTPMsg = fopen(szLoggerFile,"w");
		if(hfFTPMsg)
		{
			fprintf(hfFTPMsg,
				"Libreria STDIOL, funcion DoFileTransfering()\r\n"
				"Ok en envio de FTP via WinAPI,\r\n hacia %s como %s\r\n",
				szXFerHost,szXFerFileTo);
			fclose(hfFTPMsg);
			hfFTPMsg = NULL;
		};
		//////////////////////////////////////////////////////////////////////////////
	};
	// Error? Y hay un buffer de copia del mensaje?
	if(iRetVal != 0	&& szExtErrorMsg != NULL)
	{
		///////////////////////////////////////////////////////////////////////////
		// Si hay buffer de mensaje de error, copiarlo...
		if(szExtErrorMsg && nErrorTxt > 0)
			{
			strncpy(szExtErrorMsg, szErrorTxt, min(nErrorTxt,sizeof(szErrorTxt)-1));
			szExtErrorMsg[min(nErrorTxt,sizeof(szErrorTxt)-1)] = 0x00;
			}
		///////////////////////////////////////////////////////////////////////////
	};
	

	// Retornar valor de ftp_send_file()
	return (iRetVal);
};
#endif //_LINK_FTP_FUNCTION_

// Macro de control del uso de nombre temporal
#define _USE_TMP_FILENAME_ 


// Macro que define algunas porciones del sig. codigo para ser especificas al SOAT
#ifndef _SYSTEM_SOAT_
#define  _SYSTEM_SOAT_ 
#endif // _SYSTEM_SOAT_


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Funcion de transferencia de archivo mediante APIs de FTP del Operativo
// 2000.02.09 mdc Remapea a DoFileTransfering_Ext()
// 2000.03.06 mdc "MLINKRPT"
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int DoFileTransfering(char *szFIID, char *szXferFile, char *szMode,
					  char *szLoggerFile, char *szINIFileName, int iHost, 
					  boolean_t bExtDisable )
{
	// Texto interno de error
	char szErrorMsg[256] = {"(SIN DESCRIPCION)"}; 
	size_t nErrorMsg = sizeof(szErrorMsg);
	// Host ID x default
	char *pszHostType =  "TANDEM";
	// Segun el ID binario del host, traducir a ID alfanumerico 
	if(_MLINK_HOST_ID_ == iHost)
		pszHostType = "MLINK";
	else if(_MLINKRPT_HOST_ID_ == iHost)
		pszHostType = "MLINKRPT"; // 2000.03.06,mdc.
	else
		pszHostType = "TANDEM";
	// Remapeo a DoFileTransfering_Ext()
	return DoFileTransfering_Ext( szFIID, szXferFile, szLoggerFile,
						  pszHostType, bExtDisable, 
						  szErrorMsg, nErrorMsg );
};

#endif // _USE_FTP_API_