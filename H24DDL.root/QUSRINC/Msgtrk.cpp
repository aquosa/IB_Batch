//
// IT24 Sistemas S.A.
// Message Tracker Library 
//
//     Libreria de Registracion de Mensajes (genericos y/o ISO-8583), en
// formato ASCII-Text, y una linea por registro, como para poder
// ser importada o vista por otras aplicaciones posteriormente.
//     Opcionalmente, el tracking por dia es en un archivo distinto, bajo el
// nombre formateado con fecha ISO-YYMMDD.
//   Las funciones de I/O son estandard de bajo nivel ANSI C y ANSI UNIX.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1997.10.01      mdc     Base
// (Inicial)    1997.10.03      mdc     Tracking por dia
// (Inicial)    1997.10.21      mdc     Precondiciones extendidas
// (Inicial)    1997.11.11      mdc     Chequeo de Tracking diario internamente
// (Inicial)    1997.11.14      mdc     Chequeo de Tracking diario externamente
// (Inicial)    1997.11.26      mdc     Variables locales no estaticas
// (Beta)       1998.07.22      mdc     strcpy x lstrcpy
// (Beta)       1998.11.01      mdc     hFile = NULL en constructor
// (Beta)       2002.01.31      mdc     Open() y Close() en cada Write()
// (Beta)       2002.05.09      mdc     Estadisticas, incluye #header con TIMESTAMP por linea.
// (Beta)       2003.07.04      mdc     Cambio de libreria estandar IO.H por STDIO.H
//

// Header propio/local
#include "msgtrk.h"
#include "typedefs.h"

// Headers globales
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <windows.h>
#endif // WIN32

///////////////////////////////////////////////////////////
// Constructor
EXPORT MessageTracker::MessageTracker(LPCSTR pszFName, BOOL bTrackByDay) 
    { 
    // Asegurar handle de flujo de I/O en NULL    
    hfFile = NULL;
    // Bytes de I/O
    cbReadWrite = 0;
    // Hora de Inicio de Traking a cero
    tTrackStartTime = NULL;
    // Open y close en cada Write?
    bOpenCloseInWrite = TRUE;
    // Apertura opcional
    if(pszFName != NULL) 
        Open(pszFName,bTrackByDay);        
    }

///////////////////////////////////////////////////////////
// Destructor
EXPORT MessageTracker::~MessageTracker(void) 
    { 
    // Cierre
    Close();
    // Bytes de I/O
    cbReadWrite = 0;
    // Hora de Inicio de Traking a cero
    tTrackStartTime = NULL;
    }

///////////////////////////////////////////////////////////
// Apertura 
BOOL EXPORT MessageTracker::Open(LPCSTR pszFName, BOOL bTrackByDay)
    {    
    // Precondicion: Cerrado
    Close();          
    // Precondicion: Nombre parametrizado 
    if(pszFName == NULL) 
        return FALSE;

    // Hora de Inicio/ReInicio actual tomada del Sistema (ANSI Unix)
    tTrackStartTime = time(NULL);

    // Tracking dia a dia?
    if((bByDay = bTrackByDay) == TRUE)
        {                
        // Formatear fecha de hoy al nombre del archivo parametrizado,
        // en formato ISO-YYMMDD
        if(!SetTodayFileName(pszFName, (LPSTR)szFileName))
            return FALSE;            
        }
    else
        {
        // Copia de Nombre parametrizado
        strcpy(szFileName, pszFName);    
        }

    // Error al abrir en modo ASCII-Text compartido?   - FIX VISUAL .2005 , "atr+"  
    if((hfFile = fopen((LPSTR)szFileName, "a+" )) == NULL)
        {
        // Intentar crearlo para saber si no existe ya
        if (!Create((LPSTR)szFileName)) return FALSE;
        // Una vez creado, cerrarlo y reabrirlo compartido
        Close();
        // Abrirlo - FIX VISUAL .2005 , "atr+"
        return ((hfFile = fopen((LPSTR)szFileName, "a+" )) == NULL) 
                 ? FALSE 
                 : TRUE ;
        }
    else 
        {
        // Ok
        return TRUE;
        }
    }


// AFP: Apertura con Path completo 
BOOL EXPORT MessageTracker::OpenWithFullPath(std::string &file, BOOL bTrackByDay)
{    
	//std::string file(szFile);

    // Precondicion: Cerrado
    Close();          
    // Precondicion: Nombre parametrizado 
    if(file == "") 
        return FALSE;
	
	if(bTrackByDay)
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time (&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer,80,"%y%m%d",timeinfo);
		std::string str(buffer);

		std::size_t i = file.find("YYMMDD");

		if(std::string::npos == i)
		{
			i = file.find("yymmdd");
		}

		if(std::string::npos != i)
		{
			file.replace(i, 6, str);
		}
	}

	strcpy(szFileName, file.c_str());
	
	if((hfFile = fopen((LPSTR)szFileName, "a+" )) == NULL)
	{
		// Intentar crearlo para saber si no existe ya
		if (!Create((LPSTR)szFileName)) return FALSE;
		// Una vez creado, cerrarlo y reabrirlo compartido
		Close();
		// Abrirlo - FIX VISUAL .2005 , "atr+"
		return ((hfFile = fopen((LPSTR)szFileName, "a+" )) == NULL) 
				 ? FALSE 
				 : TRUE ;
	}
	else 
	{
		// Ok
		return TRUE;
	}
}


///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
// Lectura de siguiente registro 
BOOL EXPORT MessageTracker::Read(LPBYTE lpbBuffer, SHORT cbLen,PSHORT pcbCopy)
    {
    // Precondicion: handle valido
    if(NULL == hfFile) 
        return FALSE;    
    // Precondicion: puntero a buffer y longitudes validos
    if(lpbBuffer == NULL || pcbCopy == NULL || cbLen <= 0) 
        return FALSE;

    // Lectura en si misma de 1 registro hasta CarriageReturn+LineFeed,
    // que es el modo ASCII-Text:
    cbReadWrite = fread((LPBYTE)lpbBuffer, cbLen, 1, hfFile);
    // Copia externa de los bytes leidos
    (*pcbCopy) = cbReadWrite ;
    // Retorno incorrecto?
    return ((cbReadWrite != 0) && (cbReadWrite != NULL)) 
            ? TRUE 
            : FALSE;
    }       

///////////////////////////////////////////////////////////     
// Escritura de registro
BOOL EXPORT MessageTracker::Write(LPBYTE lpbBuffer, SHORT cbLen, BOOL bLineEnd)
    {    
    // Buffer local de formateo ASCII-Text (y no estatico)
    char szTrkBuffer[BTRKMESSAGESIZE * 10];
    short cbCommit = 0;
    short cbHdrLen = 0;
    // Agregada en cada linea, el TIMESTAMP para estadisticas posteriores
    // En plataforma WINDOWS, resolucion al milisegundo....
    stSYSTEMTIME stSystemTime;
    // En otras plataformas, resolucion al segundo a falta de mejor opcion
    time_t tTimestamp = time( NULL );
    tm *ptmLocal      = localtime( &tTimestamp );

    
    // Precondicion: handle valido y abierto
    if(NULL == hfFile) 
        return FALSE;
    // Precondicion: puntero a buffer y longitudes validos
    if(lpbBuffer == NULL || cbLen <= 0) 
        return FALSE;
    // Precondicion: si es a diario, autochequear fecha de tracking    
    if(bByDay)
        CheckTrackingDay();

    // Verificar longitud y forzarla si se excede el maximo
    // (menos 2 caracteres=LineFeed+NULL)
    if((cbLen+2) > sizeof szTrkBuffer) 
        cbLen = sizeof(szTrkBuffer)-2; // (menos 2 caracteres=LineFeed+NULL)

    //////////////////////////////////////////////////////////////////////
    // Fecha y Hora de Transmision, formato YY/MM/DD HH:MM:SS 
    // Fecha de Transmision, formato MM/DD/YYYY (XMIT-DATE), ISO-FIELD-07    
    tTimestamp = time( NULL );
    ptmLocal   = localtime( &tTimestamp );
    stSystemTime.wYear    = ptmLocal->tm_year + 1900; /* years since 1900 */
    stSystemTime.wMonth   = ptmLocal->tm_mon + 1;     /* months since January - [0;11] */
    stSystemTime.wDay     = ptmLocal->tm_mday;        /* day of the month - [1;31] */
    stSystemTime.wHour    = ptmLocal->tm_hour;        /* hours since midnight - [0;23] */        
    stSystemTime.wMinute  = ptmLocal->tm_min;         /* minutes after the hour - [0;59] */
    stSystemTime.wSecond  = ptmLocal->tm_sec;         /* seconds after the minute - [0;59] */
    stSystemTime.wMilliseconds = 0;                   /* default is zero milliseconds */
#ifdef _WIN32
    GetLocalTime( (SYSTEMTIME *)&stSystemTime );
#endif // _WIN32
    sprintf(szTrkBuffer,"%04i/%02i/%02i %02i:%02i:%02i:%03i |", 
            stSystemTime.wYear,           
            stSystemTime.wMonth,
            stSystemTime.wDay,
            stSystemTime.wHour,
            stSystemTime.wMinute,
            stSystemTime.wSecond,
            stSystemTime.wMilliseconds );
    // Header opcional de estadisticas medira N caracteres.
    cbHdrLen = strlen(szTrkBuffer);
    //////////////////////////////////////////////////////////////////////

    // Debe agregarse un LineFeed al final del buffer, para
    // formatear el Tracker linea a linea, 1 para cada registro.
    // Entonces se copia localmente y se agrega el LineFeed+NULL.    
    // Considerar que puede haber un header opcional de N bytes....
    memcpy((LPBYTE)szTrkBuffer + cbHdrLen, lpbBuffer, cbLen);    
    // ...si corresponde, ya que es opcional, y por omision es TRUE:
    if(bLineEnd)
    {
        szTrkBuffer[cbLen+cbHdrLen]   = '\n';    // Line Feed
        szTrkBuffer[cbLen+cbHdrLen+1] =   0 ;    // Null Terminated String
        cbLen++;
    }

    // Posicionarse al final del archivo (aunque ya fue abierto _O_APPEND)
    fseek(hfFile, 0L, SEEK_END);

    // Escritura en si misma, mas el opcional del header
    // Opcionalmente se agrega un header de estadisticas.
    cbReadWrite = (SHORT)fwrite((LPBYTE)szTrkBuffer, cbLen + cbHdrLen, 1, hfFile);
    
    // Asegurar que se baje el buffer del Sistema Operativo a disco
    cbCommit = fflush(hfFile);

    // Postcondicion: cerrar y flushear el archivo a disco, y reabrirlo
    // para evitar perder el contenido ante muerte subita del proceso (Win95,Win98)
    if(bOpenCloseInWrite)
        ReOpen();
    
    // Retorno incorrecto?
    return( (cbReadWrite == NULL) || 
            (cbReadWrite == 0)         || 
            (cbReadWrite < (cbLen + cbHdrLen) ) ) 
        ? FALSE 
        : TRUE;
    }       

///////////////////////////////////////////////////////////        
// Cierre
BOOL EXPORT MessageTracker::Close()
    {        
    // Precondicion: Handle valido
    if (NULL == hfFile) 
        return FALSE;
    // Cierre
    if (fclose(hfFile) == NULL) 
        return FALSE;
    // Asegurar handle en NULL
    hfFile = NULL;
    // Retorno Ok
    return TRUE;
    }

///////////////////////////////////////////////////////////    
// Creacion
BOOL MessageTracker::Create(LPCSTR pszFName)
    {    
    // Precondicion : NO DEBE haber handle : Debe estar cerrado
    if(NULL != hfFile)
        return FALSE;
    // Creacion    - FIX VISUAL .2005 "tw" 
    hfFile = fopen(pszFName, "w") ;
    return (NULL == hfFile) ? FALSE 
                            : TRUE;
    }    

///////////////////////////////////////////////////////////        
// Re-Apertura
BOOL EXPORT MessageTracker::ReOpen(BOOL bTrackByDay)
    {   
    // Nombre temporal de archivo actual
    char szTempName[SZFNAMELEN];
    
    // Copiar nombre actual como temporal
    strcpy(szTempName, szFileName);
    // Abrirlo, chequeando tracking diario : No usar "freopen".
    return( Open((LPCSTR)szTempName, bTrackByDay) );
    }    

///////////////////////////////////////////////////////////
// Re-Creacion
BOOL MessageTracker::ReCreate(void)
    {    
    // Precondicion: Archivo cerrado
    Close();
    // Creacion de archivo
    return Create(szFileName);
    }

///////////////////////////////////////////////////////////
// Por dia?
BOOL EXPORT MessageTracker::IsByDay(void)
    {
    // Booleano de Tracking dia por dia
    return (bByDay);
    }

///////////////////////////////////////////////////////////
// Nombre de archivo formateado al dia de la fecha
// Esta es la unica funcion no estandard segun Sistema Operativo en que 
// se implemente, debido al formato de los nombres en el File System
BOOL MessageTracker::SetTodayFileName(LPCSTR pszInFName, LPSTR pszOutFName)
    {
    char    szTempFName[SZFNAMELEN];  // Nombre de archivo temporal
    char    szTodayDate[SZFNAMELEN];  // Fecha ASCII de hoy
    char   *pszExtension = NULL;      // Ptr a CharId de Extension "."
    char   *pszFile      = NULL;      // Ptr a CharId de File "\"

    // Precondicion: Nombre base
    if(pszInFName == NULL) 
        return FALSE;
    // Precondicion: Extension "." presente
    if((pszExtension = (PSTR) strrchr(pszInFName,'.')) == NULL) 
        pszExtension = ".trk"; // default
        
    
    // Inicio del Nombre de Archivo, denotada por el ultimo "\"
#ifdef _WIN32
    pszFile = (PSTR) strrchr(pszInFName,'\\');    
#else
    pszFile = strrchr(pszInFName,'/');    
#endif // _WIN32,_AS400,_RS6000

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
        return FALSE;
    // Copia del Path hasta el Folder/Directorio
    strncpy(szTempFName, pszInFName, (size_t)(pszFile-pszInFName));
    szTempFName[(size_t)(pszFile-pszInFName)] = 0x00;

    // Si existe un Path previo, concatenar el separador de Paths:
    if((size_t)(pszFile-pszInFName) != 0)        
#ifdef _WIN32
        strcat(szTempFName, "\\");
#else
        strcat(szTempFName, "/");
#endif // _WIN32
        
    // Concatenar iniciales, de MessageTracker por default
    // si no hay caracteres iniciales alfabeticos en el nombre del archivo:
    if( isalpha((int)(*pszFile)) && isalpha((int)(*(pszFile+1))) )
        strncat(szTempFName, pszFile, 2);
    else
        // Concatenar prefijo por default "MT"
        strcat(szTempFName, "MT");    

    // Concatenar formato fecha
    strcat(szTempFName, szTodayDate);
    // Concatenar extension
    strcat(szTempFName, pszExtension);

    // Copia externa?
    if(pszOutFName != NULL) 
        strcpy(pszOutFName, szTempFName);    
    // Retorno Ok
    return TRUE;
    }

///////////////////////////////////////////////////////////
// Hora de Inicio del Tracking
time_t EXPORT MessageTracker::StartTime(void)
    {
    // Retorno hora de inicio del tracking
    return (tTrackStartTime);
    }

///////////////////////////////////////////////////////////
// Chequeo de distinto dia de Tracking con respecto al Sistema
BOOL EXPORT MessageTracker::CheckTrackingDay(void)
    {    

    // Precondicion: Tracking a diario establecido
    if(!bByDay) 
        return FALSE;

    // Variables y estructuras auxiliares de Time y Date del Sistema
    time_t tSystemTime        = time(NULL);                 // System Time
    tm     *ptmSystemTime     = localtime(&tSystemTime); // Local System Time
    int    iSystemDay         = ptmSystemTime->tm_yday;  // Local System Day of Year
    // Variables y estructuras auxiliares de Time y Date del Tracker
    tm     *ptmTrackStartTime = localtime(&tTrackStartTime); // Local Trk StartTime
    int    iTrackStartDay     = ptmTrackStartTime->tm_yday;  // Local Trk Day of Year    

    // Chequeo de distinto dia de Tracking con respecto al Sistema
    return (iSystemDay != iTrackStartDay) 
        ? ReOpen(bByDay)
        : FALSE ;

    }


///////////////////////////////////////////////////////////
// Recuperar el HFILE interno
///////////////////////////////////////////////////////////
PFILE EXPORT MessageTracker::GetFileHandle()                
{
    return hfFile;
}

///////////////////////////////////////////////////////////