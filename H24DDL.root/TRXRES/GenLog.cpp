//
// IT24 Sistemas S.A.
//
// Clase de logeo generico de mensajes estructurados
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2002.07.17      gsi     Inicial
// (Alfa)       2003.07.03      mdc     Portabilidad de plataforma UNIX/AS400/RS6000/WINDOWS
// (Beta)       2006.05.15      mdc     IS_VALID_POINTER(x)	(NULL != x && ((void *)0xdddddddd != x) && ((void *)0xcdcdcdcd != x))
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <qusrinc/GenLog.h>
#include <qusrinc/typedefs.h>
#ifdef _WIN32
#include <windows.h>
#endif // WIN32

// Constante de Longitud de Nombre y Ruta
#ifndef SZTRKFNAMELEN
#define SZTRKFNAMELEN		256
#define SZTRKPNAMELEN		256
#endif // SZTRKFNAMELEN
#ifndef SZFNAMELEN
#define SZFNAMELEN			SZTRKFNAMELEN
#endif // SZNAMELEN

#define IS_VALID_POINTER(x)	(NULL != x && \
							((void *)0xdddddddd != x) && \
							((void *)0xcdcdcdcd != x) && \
							((void *)0xffffffff != x))

/////////////////////////////////////////////////////////
GenericLog::GenericLog()
{
	fLog_fd = NULL;
};

/////////////////////////////////////////////////////////
GenericLog::~GenericLog()
{
	if( NULL != fLog_fd )
		Close();
};

/////////////////////////////////////////////////////////
int GenericLog::Open(const char *logname)
{
	// Nombre de archivo diario , local al procedimiento
	char	szFilename[SZTRKFNAMELEN]={0x00};
	// Precondicion
	if ( NULL == fLog_fd )			
	{
		// Nombre de archivo diario , local al procedimiento
		SetTodayFileName(logname, szFilename);
		// Abrir archivo diario de logg SIAF
		fLog_fd=fopen(szFilename, "a");
		// OK
		return 0;
	}
	else
		// Error
		return -1;
};

/////////////////////////////////////////////////////////
int GenericLog::Put( char *fmt, ...)
{
	va_list args;
	stSYSTEMTIME si = {0,0,0,0,0,0,0};
	time_t tSysTime  = time(NULL);            // System Time
	tm     *ptmLTime = localtime(&tSysTime);  // Local System Time
	BOOL   bSTDOUT   = FALSE;

	// Precondicion
	if ( NULL == fLog_fd || NULL == fmt )
		// Error
		return -1;

	// Timestamp actual y local
	tSysTime  = time(NULL);            // System Time
	ptmLTime  = localtime(&tSysTime);  // Local System Time
    si.wYear         = ptmLTime->tm_year + 1900;
    si.wMonth        = ptmLTime->tm_mon + 1;
    si.wDayOfWeek    = ptmLTime->tm_wday;
    si.wDay          = ptmLTime->tm_mday;
    si.wHour         = ptmLTime->tm_hour;
    si.wMinute       = ptmLTime->tm_min;
    si.wSecond       = ptmLTime->tm_sec;
    si.wMilliseconds = 0;
#ifdef _WIN32
	GetLocalTime( (SYSTEMTIME *)&si ); // Definicion hasta el milisegundo
#endif // WIN32

	va_start(args, fmt);
	// Loggear mensaje con fecha y hora
	fprintf(fLog_fd,"%02d/%02d/%04d|%02d:%02d:%02d.%03d->", 
		si.wDay, si.wMonth, si.wYear, si.wHour, si.wMinute,si.wSecond, si.wMilliseconds);
	// No emitir al STD OUT si no explicitamente
	if( bSTDOUT )
		vprintf(fmt, args);
	if (fLog_fd!=NULL)  {
		vfprintf(fLog_fd, fmt, args);
		fflush(fLog_fd);
	}
	va_end(args);
	// ok
	return 0;
};

int GenericLog::DraftPut( char *fmt, ...)
{
	va_list args;
	BOOL   bSTDOUT = FALSE;

	// Precondicion
	if ( NULL == fLog_fd || NULL == fmt )
		// Error
		return -1;


	va_start(args, fmt);
	
	// No emitir al STD OUT si no explicitamente
	if( bSTDOUT )
		vprintf(fmt, args);	
	// Loggear mensaje con fecha y hora
	if (fLog_fd!=NULL)  {
		vfprintf(fLog_fd, fmt, args);
		fflush(fLog_fd);
	}
	va_end(args);
	// ok
	return 0;
};

/////////////////////////////////////////////////////////
void GenericLog::Close()
{
    if (fLog_fd != NULL && IS_VALID_POINTER(fLog_fd) )
	{
		fclose(fLog_fd);
		fLog_fd = NULL;
	};
};

/////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// Nombre de archivo formateado al dia de la fecha
// Esta es la unica funcion no estandard segun Sistema Operativo en que 
// se implemente, debido al formato de los nombres en el File System
boolean_t GenericLog::SetTodayFileName(LPCSTR pszInFName, LPSTR pszOutFName)
	{
	char    szTempFName[SZFNAMELEN];  // Nombre de archivo temporal
	char    szTodayDate[SZFNAMELEN];  // Fecha ASCII de hoy
	char   *pszExtension = NULL;      // Ptr a CharId de Extension "."
	char   *pszFile      = NULL;      // Ptr a CharId de File "\"

	// Precondicion: Nombre base
	if(pszInFName == NULL) 
		return is_false;
	// Precondicion: Extension "." presente
	// Fix .Net 2005 
	if((pszExtension = strrchr( (char*)pszInFName,'.')) == NULL) 
		pszExtension = ".txt"; // default
		
	
	// Inicio del Nombre de Archivo, denotada por el ultimo "\"
#ifdef _WIN32
	// Fix .Net 2005 
	pszFile = strrchr( (char *)pszInFName,'\\');	
#elif _RS6000
    pszFile = strrchr(pszInFName,'/');	
#elif _AS400
    pszFile = NULL;
#endif // _WIN32,_RS6000,_AS400
	// Si no existe... 
	if (pszFile == NULL) pszFile = (LPSTR)pszInFName; // Inicio en 1er caracter alfabetico
	else                 pszFile++;                   // sino en siguiente despues de '\'

	// Fecha y Hora actuales y locales del Sistema
	time_t tSystemTime   = time(NULL);               // System Time
	tm     *ptmLocalTime = localtime(&tSystemTime);  // Local System Time
	// Formateo ISO-YYMMDD para fecha
	strftime(szTodayDate, sizeof szTodayDate, "%y%m%d", ptmLocalTime);

	// Copia del Path hasta el Folder/Directorio
	// Verifica longitud de nombre, asegurando "xxYYMMDD.ext"
	if((pszFile-pszInFName) > (SZFNAMELEN-sizeof("xxYYMMDD.ext"))) 
		return is_false;
	// Copia del Path hasta el Folder/Directorio
	strncpy(szTempFName, pszInFName, (size_t)(pszFile-pszInFName));
	szTempFName[(size_t)(pszFile-pszInFName)] = 0x00;

	// Si existe un Path previo, concatenar el separador de Paths:
	if((size_t)(pszFile-pszInFName) != 0)		
#ifdef _WIN32
		strcat(szTempFName, "\\");
#elif _RS6000
        strcat(szTempFName, "/");
#endif // _WIN32,_RS6000,_AS400
		
	// Concatenar iniciales, de MessageTracker por default
	// si no hay caracteres iniciales alfabeticos en el nombre del archivo:
	if( isalpha((int)(*pszFile)) && isalpha((int)(*(pszFile+1))) )
		strncat(szTempFName, pszFile, 2);
	else
		// Concatenar prefijo por default "LG"
		strcat(szTempFName, "LG");	

	// Concatenar formato fecha
	strcat(szTempFName, szTodayDate);
	// Concatenar extension
	strcat(szTempFName, pszExtension);

	// Copia externa?
	if(pszOutFName != NULL) 
		strcpy(pszOutFName, szTempFName);	
	// Retorno Ok
	return is_true;
	}
/////////////////////////////////////////////////////////
