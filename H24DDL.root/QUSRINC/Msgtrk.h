//
// IT24 Sistemas S.A.
// Message Tracker Library (MTL)
//
// Libreria de Registracion de Mensajes (genericos y/o ISO-8583)
// en formato ASCII-texto, una linea por registro, como para poder
// ser importada o vista por otras aplicaciones .
// Opcional dia a dia, controlando el cambio de tracking externamente,
// e internamente tambien.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1997.10.01      mdc     Base
// (Inicial)    1997.10.03      mdc     Tracking Dia a dia
// (Inicial)    1997.10.21      mdc     Nombres constantes
// (Inicial)    1997.11.11      mdc     Autochequeo de tracking diario/StartTime
// (Inicial)    1997.11.14      mdc     Chequeo de tracking diario externamente
// (Alfa)       1998.09.03      mdc     Buffer en 4 kbytes
// (Alfa)       1998.10.27      mdc     Buffer en 8 kbytes
// (Alfa)       1999.04.23      mdc     SZTRKFNAMELEN
// (Beta)       2002.01.31      mdc     Open() y Close() en cada Write()
// (Beta)       2003.07.04      mdc     Cambio de libreria estandar IO.H por STDIO.H
//

#ifndef _MSGTRK_H_
#define _MSGTRK_H_

// Header ANSI de fecha+hora
#include <time.h>
// Header ANSI de manejo de archivos
#include <stdlib.h>
#include <stdio.h>

#include <string>

// Headers propios
#include "alias.h"
#include "typedefs.h"

// Constante de Tamano del Mensaje Interno a Registrar
#define BTRKMESSAGESIZE		8192

// Constante de Longitud de Nombre y Ruta
#define SZTRKFNAMELEN		256
#define SZTRKPNAMELEN		256
#ifndef SZFNAMELEN
#define SZFNAMELEN			SZTRKFNAMELEN
#endif // SZFNAMELEN
#define MSGTRKFILENAME		"MTLOGGER.TRK"		// Message tracker
#define ERRTRKFILENAME		"ERLOGGER.TRK"		// Error tracker
#define ECHOTRKFILENAME		"CHLOGGER.TRK"		// Echo  tracker
#define AUDTRKFILENAME		"AULOGGER.TRK"		// Audit tracker
#define DEBTRKFILENAME		"DELOGGER.TRK"		// Debug tracker

// Pointer to FILE structure
typedef FILE *PFILE;

// Clase MessageTracker (MsgTrk)
class MessageTracker
    {
    public:
        
        // Metodos publicos de la Clase        
        EXPORT MessageTracker(LPCSTR=NULL,BOOL=FALSE); // Constructor
        EXPORT ~MessageTracker(); // Destructor
        
		BOOL   EXPORT CheckTrackingDay(void);       // Solicitud de Chequeo diario
        BOOL   EXPORT Close(void);                  // Cierre
		BOOL   EXPORT IsByDay(void);                // Por dia?
        BOOL   EXPORT Open(LPCSTR,BOOL=TRUE);       // Apertura
        BOOL   EXPORT Read(LPBYTE,SHORT,PSHORT);    // Lectura de STORED
		BOOL   EXPORT ReOpen(BOOL=TRUE);            // Reapertura
		time_t EXPORT StartTime(void);			    // Hora de inicio		
        BOOL   EXPORT Write(LPBYTE,SHORT,BOOL=TRUE);// Escritura de STORED				
		PFILE  EXPORT GetFileHandle();				// Recuperar el HFILE

		BOOL   EXPORT OpenWithFullPath(std::string&, BOOL = TRUE);       // Apertura
        
    protected:                           
    
        // Metodos protegidos de la Clase
        BOOL Create(LPCSTR);                 // Creacion de Archivo
        BOOL ReCreate(void);                 // Recrear
		BOOL SetTodayFileName(LPCSTR,LPSTR); // Nombre de Archivo al Dia

        // Variables protegidas de la Instancia
        CHAR   szFileName[SZTRKFNAMELEN];// Nombre adoptado		
        FILE   *hfFile;					// Handler de Archivo                
		WORD   cbReadWrite;				// Contador de Bytes I/O
		BOOL   bByDay;					// Tracker dia a dia?
		time_t tTrackStartTime;			// Hora de inicio
		BOOL   bOpenCloseInWrite;		// Open y Close en cada Write? 

    };                   
    
// Tipo Puntero a Clase MsgTrk
typedef MessageTracker* pMessageTracker ;

#endif // _MSGTRK_H_

