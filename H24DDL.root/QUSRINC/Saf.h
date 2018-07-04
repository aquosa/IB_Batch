/////////////////////////////////////////////////////////////////////////////////////////////////
// IT24 Sistemas S.A.
// SAF Library Manager
//
// Libreria de Manejo de Transacciones Off-Line guardadas en un archivo para
// su despacho posterior (Store-And-Forward).
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1997.09.10      mdc     Base
// (Inicial)    1997.09.11      mdc     WORD x UINT en metodos (_WIN32)
// (Alfa)       1997.10.17      mdc     Primer Registro STORED
// (Beta)       1998.09.03      mdc     Buffer en 4kb
// (Alfa)       1999.04.23      mdc     Buffer en 8 kbytes
// (Alfa)       2002.07.25      mdc     Adaptacion a mensajes ISO8583 por LNET+ATM
//										time_t tSAFInit;      -- TimeStamp del inicio de SAF
//										time_t tLastSAFCheck; -- TimeStamp del ultimo chequeo SAF
// (Beta)       2002.08.16      mdc     Numero pseudo-random prefijado por instancia
// (Beta)       2002.09.04      mdc     #define SAF_RECORDS_ACTION_REQUIRED	(10)
// (Release-1)  2003.06.11      mdc     ::ReadStored(LPBYTE, size_t, size_t *,PDWORD=NULL,PDWORD=NULL);
// (Beta)       2003.07.04      mdc     Cambio de libreria estandar IO.H por STDIO.H
// (Delta)      2006.07.19      mdc     ::Open(LPSTR, BOOL bUseTodayMask = TRUE ); // Apertura con nombre YYMMDD.saf
// (Delta)      2006.07.26      mdc     SAFHDR y hfFileSav (para WRITE/READ simultaneos). lcCurrPos y lcPrevPos.
// (Delta)      2006.07.27      mdc     ReadStored( SAFREC *psafRec );// Lectura de STORED como registro SAF
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _STORE_AND_FORWARD_H_
#define _STORE_AND_FORWARD_H_

// Header Time ANSI Unix
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// Headers propios
#include "alias.h"
#include "typedefs.h"

// Constantes de Estados del Registro
#define SAF_RECSTATE_NULL         (0x0000) 
#define SAF_RECSTATE_STORED       (0xEEEE)
#define SAF_RECSTATE_FORWARDED    (0xFFFF)
#define SAF_RECSTATE_CANCELLED    (0xCCCC)

////////////////////////////////////////////////////////////////
// Constante de Limite de registros permitidos en SAF por default
// Calcular SAF_RECORDS_LIMIT*(BMESSAGESIZE+HEADERSIZE)=FILE_SIZE(bytes)
#define SAF_RECORDS_LIMIT			(1000)
// Para verificar si hay mas N registros presentes 
#define SAF_RECORDS_ACTION_REQUIRED	(10)
// Para verificar si hay mas de N minutos de inactividad
#define SAF_INACTIVITY_MINUTES		(1)
////////////////////////////////////////////////////////////////

// Constante de Longitud de Nombre
#define SZSAFFNAMELEN       (256)
// Nombre prefijado, a menos que sea DIA a DIA con RANDOM
#define SAFFILENAME			"SAFQUEUE.saf"

      
// Registro StoreAndForward (SAF) 
// Contiene Hora de Stored y Forwarded, identificando el (Topico+Producto) 
// que lo grabo, el Ordinal de Registro Autonumerado, el estado, la 
// cantidad de bytes, y el buffer de datos generico en si mismo.
typedef struct 
    {
	    char            chEyeCatcher;  // Senalador visual (referencial)
        long            dwRecord;      // Registro Ordinal Autonumerado
        time_t          tStdTime;      // Hora de STORED (Standard ANSI Unix)
        time_t          tFwdTime;      // Hora de FORWARDED (Standard ANSI Unix)    
        unsigned short  wState;        // Estado del Registro (stored,forwarded,cancelled)
	    long            dwTraceNum;    // Numero de seguimiento, TRACE-NUM
	    long            dwRefNum;      // Numero de referencia, REF-NUM
        unsigned short  cbBufferLen;   // Longitud del Buffer de datos generico
    } SAFHDR;
typedef struct
    {                                                  
	    char            chEyeCatcher;  // Senalador visual (referencial)
        long            dwRecord;      // Registro Ordinal Autonumerado
        time_t          tStdTime;      // Hora de STORED (Standard ANSI Unix)
        time_t          tFwdTime;      // Hora de FORWARDED (Standard ANSI Unix)    
        unsigned short  wState;        // Estado del Registro (stored,forwarded,cancelled)
	    long            dwTraceNum;    // Numero de seguimiento, TRACE-NUM
	    long            dwRefNum;      // Numero de referencia, REF-NUM
        unsigned short  cbBufferLen;   // Longitud del Buffer de datos generico
        char   bBuffer[8192-sizeof(SAFHDR)];// Buffer de datos generico
    } SAFREC;

// Constante de Tamano Maximo y Unico del Mensaje Interno a SAFear
#define SAFMESSAGESIZE    (8192-sizeof(SAFHDR))


// Clase StoreAndForward (SAF)
class StoreAndForward
    {
    public:
        
        // Metodos publicos de la Clase                
        EXPORT StoreAndForward(LPSTR=NULL); // Constructor
		EXPORT StoreAndForward(LPSTR,BOOL bUseRandom); // Constructor opcional
        EXPORT ~StoreAndForward(); // Destructor

        BOOL  EXPORT Close(void);                             // Cierre
		DWORD EXPORT ForwardedRecords(void);                  // Q Registros FORWARDED
		BOOL  EXPORT FreeStored();                            // Liberacion STORED
		BOOL  EXPORT Open(LPSTR, BOOL bUseTodayMask = TRUE ); // Apertura con nombre YYMMDD.saf
		BOOL  EXPORT ReadStored(PVOID pvBuf, size_t nLen, size_t *pnLen,PDWORD=NULL,PDWORD=NULL);// Lectura de STORED
        BOOL  EXPORT ReadStored( SAFREC *psafRec );// Lectura de STORED como registro SAF
		DWORD EXPORT RecordsLimit(DWORD=SAF_RECORDS_LIMIT);   // Limite de Registros
        BOOL  EXPORT ReOpen(void);                            // Reapertura
		BOOL  EXPORT SetCancelled(void);                      // Marca de CANCELLED
		BOOL  EXPORT SetFirstStored(void);                    // Ir al Primer STORED
        BOOL  EXPORT SetForwarded(void);                      // Marca de FORWARDED
        DWORD EXPORT StoredRecords(void);                     // Q Registros STORED                        
        BOOL  EXPORT WriteStored(LPBYTE,SHORT,DWORD=0L,DWORD=0L);// Escritura de STORED        		
		BOOL  EXPORT InitStatistics(LPSTR pszInFName = NULL, 
                                    BOOL bUseTodayMask = TRUE);// Inicializar Estadisticas
		BOOL  EXPORT IsActionRequired();					   // Se requiere procesar SAF ?
        BOOL  EXPORT UseRandomName(BOOL = TRUE);               // Usar nombre random ?
		BOOL  EXPORT Remove(void);                             // Borra el archivo
        
    protected:                           
    
        // Metodos protegidos de la Clase        
        BOOL EXPORT Create(LPSTR szFilename, BOOL bUseTodayMask = TRUE ); // Creacion de Archivo
        BOOL EXPORT ReCreate( BOOL bUseTodayMask = TRUE );           // Recrear
		BOOL EXPORT SetTodayFileName(LPCSTR pszInFName, LPSTR pszOutFName);

	private:
        // Variables protegidas de la Instancia
        char   szFileName[SZSAFFNAMELEN];// Nombre adoptado
        FILE  *hfFile   ,    			// Handler de Archivo
              *hfFileSav;				// Handler de Archivo para WRITE/READ simultaneo
        WORD   cbReadWrite;				// Contador de Bytes I/O
        DWORD  dwStdRecords;			// Registros STORED en SAF
        DWORD  dwFwdRecords;			// Registros FORWARDED del SAF
		DWORD  dwFirstStdRecord;		// Primer registro STORED
        DWORD  dwLastStdRecord;			// Ultimo registro STORED
        DWORD  dwLimit;					// Limite de Registros permitidos
        SAFREC safRecord;				// Registro Store-And-Forward
		time_t tSAFInit;				// TimeStamp del inicio de SAF
		time_t tLastSAFCheck;		    // TimeStamp del ultimo chequeo SAF
		long   lFileSize;				// Tamano de archivo
		int	   iRandomNum;				// Numero pseudo-random prefijado por instancia
		BOOL   bUseRandom;              // Usar nombre random ?
        /////////////////////////////////////////////////////////////////
        long    lcCurrPos;              // Posicion actual en el archivo
        long    lcPrevPos;              // Posicion previa en el archivo
        long    lcForwardPos;           // Posicion FWD/CANCEL en el archivo
        /////////////////////////////////////////////////////////////////
        
    };                   
    
// Tipo Puntero a Clase SAF
typedef StoreAndForward* pStoreAndForward ;

#endif // _STORE_AND_FORWARD_H_

