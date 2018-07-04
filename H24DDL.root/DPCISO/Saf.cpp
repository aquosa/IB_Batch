/////////////////////////////////////////////////////////////////////////////////////////////////////////
// IT24 Sistemas S.A.
// SAF Library Manager
//
//   Libreria de Manejo de SAF (StoreAndForward), que es una cola en disco de
// acumulacion de transacciones entrantes que no pueden rutearse momentaneamente,
// y son grabadas para su posterior reintento de envio.
//   Posee por seguridad un limite logico de cantidad de registros maximos.
//   Las funciones de I/O son estandard de bajo nivel ANSI C y ANSI UNIX.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1997.09.10      mdc     Base
// (Inicial)    1997.09.11      mdc     _WIN32: WORD cambiados a UINT
// (Inicial)    1997.09.23      mdc     lcCurrPos (Cursor de I/O)
// (Inicial)    1997.10.17      mdc     Primer Registro STORED
// (Alfa)       1997.10.21      mdc     Precondiciones extendidas
// (Alfa)       1998.12.02      mdc     iMode para _sopen         
// (Alfa)       2002.07.25      mdc     Adaptacion a mensajes ISO8583 por LNET+ATM
// (Alfa)       2002.08.20      mdc     Remove() agregado. UseRandomNumber() tambien.
// (Beta)       2003.07.04      mdc     Cambio de libreria estandar IO.H por STDIO.H
// (Delta)      2006.07.19      mdc     ::Open(LPSTR, BOOL bUseTodayMask = TRUE ); // Apertura con nombre YYMMDD.saf
// (Delta)      2006.07.19      mdc     Precondicion de :Open() : que no este ya abierto.
// (Delta)      2006.07.26      mdc     Consideracion para FILE SYSTEM en modo bloques de 1024 bytes
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Header propio/local
#include <qusrinc/saf.h>
#include <qusrinc/typedefs.h>

// Headers globales
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

#ifdef _WIN32
#include <io.h>
#endif // _WIN32


#define RET_INVAL           (-1) 
#define EYE_CATCHER_SYMBOL  '<'

#ifndef _POSIX_        
#define file_open(name,mode) fopen(name,mode)
#else
#define file_open(name,mode) fopen(name,mode)
#endif /* POSIX */     

// Constructor
EXPORT StoreAndForward::StoreAndForward(LPSTR szXFileName, BOOL bUseRandomNumber) 
    { 
	/////////////////////////////////////////////////////////////////
	tSAFInit      = time(NULL); // TimeStamp del inicio de SAF
	tLastSAFCheck = 0L;         // TimeStamp del ultimo chequeo SAF    
	/////////////////////////////////////////////////////////////////
	// Nro. pseudo-aleatorio por instancia de clase.
	// El nro 971 es primo de forma de obtener una distribucion buena,
	// en un rango desde 0 a 999.
	// Iniciar criba de numeros pseudo-aleatorios
	srand( time( NULL ) );
	iRandomNum  = rand() % 971 ;
	bUseRandom  = bUseRandomNumber; // Usar nombre random ?
	/////////////////////////////////////////////////////////////////
	// Asegurar handle de flujo de I/O en NULL    
    hfFile = hfFileSav = NULL;
    // Bytes de I/O
    cbReadWrite = 0;
    // Limite logico de registros por default
    dwLimit = SAF_RECORDS_LIMIT;
    // Cuenta de Registros
    dwStdRecords = dwFwdRecords = 0L;
	// Cursores de Primero y Ultimo STORED
	dwFirstStdRecord = dwLastStdRecord = 0L;
	// Tamano de archivo inicialmente
	lFileSize = 0L;
    lcCurrPos = lcPrevPos = lcForwardPos = 0L;  // Posicion actual en el archivo
    // Apertura con nombre fijo , y Estadisticas  
	InitStatistics( szXFileName );
	}

// Constructor
EXPORT StoreAndForward::StoreAndForward(LPSTR szXFileName) 
    { 
	/////////////////////////////////////////////////////////////////
	tSAFInit      = time(NULL); // TimeStamp del inicio de SAF
	tLastSAFCheck = 0L;         // TimeStamp del ultimo chequeo SAF    
	/////////////////////////////////////////////////////////////////
	// Nro. pseudo-aleatorio por instancia de clase.
	// El nro 971 es primo de forma de obtener una distribucion buena,
	// en un rango desde 0 a 999.
	// Iniciar criba de numeros pseudo-aleatorios
	srand( time( NULL ) );
	iRandomNum  = rand() % 971 ;
	bUseRandom  = TRUE; // Usar nombre random ?
	/////////////////////////////////////////////////////////////////
	// Asegurar handle de flujo de I/O en NULL    
    hfFile = hfFileSav = NULL;
    // Bytes de I/O
    cbReadWrite = 0;
    // Limite logico de registros por default
    dwLimit = SAF_RECORDS_LIMIT;
    // Cuenta de Registros
    dwStdRecords = dwFwdRecords = 0L;
	// Cursores de Primero y Ultimo STORED
	dwFirstStdRecord = dwLastStdRecord = 0L;
	// Tamano de archivo inicialmente
	lFileSize = 0L;
    lcCurrPos = lcPrevPos = lcForwardPos = 0L;  // Posicion actual en el archivo
    // Apertura y Estadisticas
    if(szXFileName != NULL) 
		Open(szXFileName);
    }
// Destructor
EXPORT StoreAndForward::~StoreAndForward(void) 
    { 	
	int  iRetval = 0;
	// Precondicion : recuperar tamano de archivo
	if(NULL != hfFile)
    {
		fseek( hfFile, 0L, SEEK_END );
        lFileSize = ftell( hfFile );
        fseek( hfFile, 0L, SEEK_SET );
    }
    lcCurrPos = lcPrevPos = 0;
    // Cierre de archivo
    Close();
    // Estadisticas a cero
    dwLimit = dwStdRecords = dwFwdRecords = 0L;
	dwFirstStdRecord = dwLastStdRecord = 0L;
	iRandomNum  = 0 ;
	bUseRandom  = FALSE; // Usar nombre random ?
    }

// Apertura y Estadisticas
BOOL EXPORT StoreAndForward::Open(LPSTR szXFileName, BOOL bUseTodayMask )
    {    
	BOOL bRetVal = FALSE;
    
    // Precondicion: NO ABIERTO YA ! Pero no cerrarlo.
    if(NULL != hfFile)
        return TRUE;   
    
    /////////////////////////////////////////////////////
	// Cambiar nombre segun fecha de hoy
    if( bUseTodayMask )
	    SetTodayFileName ( szXFileName, szFileName );
    else
        strcpy( szFileName , szXFileName );
    /////////////////////////////////////////////////////

    // Error al abrir en modo BINARIO compartido?    
    if((hfFile = file_open((LPSTR)szFileName, "r+" )) == NULL)
        {
		// Intentar crearlo para saber si no existe ya
        if (!Create(szFileName, bUseTodayMask)) 
			return FALSE;
		// Una vez creado, cerrarlo y reabrirlo
        Close();
        bRetVal = ((hfFile = file_open((LPSTR)szFileName, "r+" )) == NULL) 
                 ? FALSE 
				 : InitStatistics(szFileName) ;
		// Postcondicion : recuperar tamano de archivo
		if(NULL != hfFile)
        {
		    fseek( hfFile, 0L, SEEK_END );
            lFileSize = ftell( hfFile );
            fseek( hfFile, 0L, SEEK_SET );
        }

        // Postcondicion : abrirlo tambien para WRITE/READ simultaneo        
        hfFileSav = file_open((LPSTR)szFileName, "r+" ) ;

		// Postcondicion : TRUE/FALSE segun se haya podido abrir
		return bRetVal;
        }
    else 
        {
		// Postcondicion : recuperar tamano de archivo
		if(NULL != hfFile)
        {
		    fseek( hfFile, 0L, SEEK_END );
            lFileSize = ftell( hfFile );
            fseek( hfFile, 0L, SEEK_SET );
        }
        
        // Postcondicion : abrirlo tambien para WRITE/READ simultaneo        
        hfFileSav = file_open((LPSTR)szFileName, "r+" ) ;
		
        // Postcondicion :Iniciar estadisticas
        return InitStatistics(szFileName);
        }
    }

// Establecer FORWARDED ultimo registro STORED leido
BOOL EXPORT StoreAndForward::SetForwarded(void)
    {
    // Precondicion: handle valido
    if(NULL == hfFile) 
        return FALSE;
    // Precondicion: handle valido
    if(NULL == hfFileSav) 
        return FALSE;
    
	/////////////////////////////////////////////
	// Grabar fecha-hora de este chequeo
	tLastSAFCheck = time( NULL );
	/////////////////////////////////////////////

	// Cursor큦 Current Position
	lcCurrPos = ftell(hfFile);

	// Es STORED?
    if( SAF_RECSTATE_STORED == safRecord.wState 
        &&
        EYE_CATCHER_SYMBOL  == safRecord.chEyeCatcher 
       )
        {    	
		// Retroceder 1 registro (hasta el inicio del actual)
		// antes de regrabar		
		if( fseek(hfFileSav, lcPrevPos, SEEK_SET) == RET_INVAL ) 	
            // Error
			return FALSE;			

        // Forwarded
        safRecord.wState = SAF_RECSTATE_FORWARDED;
        // Hora (TIME_T Standard ANSI Unix)
        safRecord.tFwdTime = time(NULL);
        
		// Escritura en si misma, en la posicion del registro actual
        cbReadWrite = (SHORT)fwrite( (LPBYTE)&safRecord, 1, sizeof(safRecord), hfFileSav);
	    // Cursor큦 Current Position
	    lcForwardPos = ftell(hfFileSav);	    
		
		// Error?
        if((cbReadWrite == RET_INVAL) || (cbReadWrite == 0))		
            return FALSE;			

		// Un Forwarded mas...
        if(dwFwdRecords < ULONG_MAX) 
            dwFwdRecords++;
        else 
            return FALSE;
        
		// Un Stored menos...
        if(dwStdRecords > 0L) dwStdRecords--;            

	    // Era primer registro STORED? Avanzar a siguiente registro...
		// ...que aunque no sea STORED, ya reasigna el cursor inicial a los mismos.
		if(dwFirstStdRecord == (DWORD)safRecord.dwRecord)
			dwFirstStdRecord++;

        // OK
		return TRUE;

        }
    else	
        // Error
        return FALSE;		
    };
           
// Establecer CANCELLED ultimo registro STORED leido
BOOL EXPORT StoreAndForward::SetCancelled(void)
    {
    // Precondicion: handle valido
    if(NULL == hfFile) 
		return FALSE;
    // Precondicion: handle valido para escritura
    if(NULL == hfFileSav) 
		return FALSE;

	// Posicion del Cursor de I/O sobre el archivo SAF:
	// Se asume que el ultimo registro leido es el que se
	// quiere marcar como CANCELLED.
	// Cursor큦 Current Position
	lcCurrPos = ftell(hfFile);

    // Es STORED?
    if( SAF_RECSTATE_STORED == safRecord.wState 
        &&
        EYE_CATCHER_SYMBOL  ==  safRecord.chEyeCatcher 
        )
        {    
		// Retroceder 1 registro (hasta el inicio del actual)
		// antes de regrabar		
		if( fseek(hfFileSav, lcPrevPos, SEEK_SET) == RET_INVAL ) 
            // Error
			return FALSE;			

        // Cancelled
        safRecord.wState = SAF_RECSTATE_CANCELLED; // Estado CANCELLED    

        // Escritura en si misma, en la posicion actual
        cbReadWrite = (SHORT)fwrite((LPBYTE)&safRecord, 1,  sizeof(safRecord), hfFileSav );
	    // Cursor큦 Current Position
	    lcForwardPos = ftell(hfFileSav);
		
		// Error?
        if((cbReadWrite == RET_INVAL) || (cbReadWrite == 0))
            return FALSE;
        
		// Un Stored menos...
        if(dwStdRecords > 0L) 
            dwStdRecords--;        
		
        // OK
		return TRUE;

        }   
    else 
        // Error
        return FALSE;
    }    

// Lectura de siguiente registro STORED
BOOL EXPORT StoreAndForward::ReadStored(PVOID  lpbBuffer, 
                                        size_t cbLen, 
										size_t *pcbCopy,
										PDWORD pdwTraceNum,
										PDWORD pdwRefNum)
    {
    // Precondicion
    if(NULL == hfFile)
		return FALSE;    

	// Cursor큦 Current Position
	lcPrevPos = lcCurrPos = ftell(hfFile);    
    // Check Positions as Multiple of Record Size
    if( lcCurrPos % sizeof(safRecord) != 0)
    {
        // Re-position in file 
        fseek(hfFile, lcCurrPos - (lcCurrPos % sizeof(safRecord)), SEEK_SET);
    	// Cursor큦 Current Position
	    lcPrevPos = lcCurrPos = ftell(hfFile);
    };

    // Lectura en si misma de 1 registro STORED
    cbReadWrite = (SHORT)fread((LPBYTE)&safRecord, 1, sizeof(safRecord), hfFile );
	// Cursor큦 Current Position
	lcCurrPos = ftell(hfFile);
	// Ciclo de lectura
    while ((cbReadWrite != 0) && (cbReadWrite != RET_INVAL))
        {         
        // EYE-CATCH!           
        if(safRecord.chEyeCatcher != EYE_CATCHER_SYMBOL) 
            continue;
        // Si es STORED...
        if( SAF_RECSTATE_STORED == safRecord.wState) 
		  {
            // Copiar Buffer de Datos generico. Asegurar longitud de copia.
            if((safRecord.cbBufferLen <= (short)cbLen) &&
			   (safRecord.cbBufferLen <= (short)sizeof(safRecord.bBuffer) ))
                {
                /* OUTPUT PARAMETERS */
                memcpy(lpbBuffer, safRecord.bBuffer, safRecord.cbBufferLen);
				if(pcbCopy)
					(*pcbCopy)     = safRecord.cbBufferLen ;
				if(pdwTraceNum)
					(*pdwTraceNum) = safRecord.dwTraceNum ;
                /*******************************************/
				if(pdwRefNum)
					(*pdwRefNum)   = safRecord.dwRefNum ;
                /*******************************************/
				return ((cbReadWrite != 0) && (cbReadWrite != RET_INVAL)) 
					? TRUE 
					: FALSE;
                }            
            else if((short)cbLen < safRecord.cbBufferLen)
                {
                    /* OUTPUT PARAMETERS */
                memcpy(lpbBuffer, safRecord.bBuffer, cbLen);
				if(pcbCopy)
					(*pcbCopy)     = safRecord.cbBufferLen ;
				if(pdwTraceNum)
					(*pdwTraceNum) = safRecord.dwTraceNum ;
                /*******************************************/
				if(pdwRefNum)
					(*pdwRefNum)   = safRecord.dwRefNum ;
                /*******************************************/
				return ((cbReadWrite != 0) && (cbReadWrite != RET_INVAL)) 
					? TRUE 
					: FALSE;
                }
			else
                // Error
				return FALSE;
          };
        // Siguiente registro
        lcPrevPos = lcCurrPos = ftell(hfFile);    
        // Check Positions as Multiple of Record Size
        if( lcCurrPos % sizeof(safRecord) != 0)
        {
            // Re-position in file 
            fseek(hfFile, lcCurrPos - (lcCurrPos % sizeof(safRecord)), SEEK_SET);
    	    // Cursor큦 Current Position
	        lcPrevPos = lcCurrPos = ftell(hfFile);
        };

        cbReadWrite = (SHORT)fread((LPBYTE)&safRecord, 1, sizeof(safRecord), hfFile);
	    // Cursor큦 Current Position
	    lcCurrPos = ftell(hfFile);
        }    
    // Error
    return FALSE;
    }       

// Lectura de siguiente registro STORED como registro SAF
BOOL EXPORT StoreAndForward::ReadStored( SAFREC * psafREC )
    {
        size_t nAuxLen   = 0;
		DWORD  dwAuxNum1 = 0,
		       dwAuxNum2 = 0;
        BOOL   bBaseCall = FALSE;
        // Precondicion : ptr valido
        if( !psafREC )
            return FALSE;
        // Llamada base de lectura
        bBaseCall = this->ReadStored(psafREC->bBuffer, 
                                sizeof psafREC->bBuffer, 
						    	&nAuxLen,
							    &dwAuxNum1,
							    &dwAuxNum2 );
        // Si tuvo exito, copiar registro completo
        if( bBaseCall )
            (*psafREC) = safRecord ;
        // Return !
        return( bBaseCall );
    }       
     
// Escritura de registro STORED
BOOL EXPORT StoreAndForward::WriteStored(LPBYTE lpbBuffer, 
										 SHORT cbLen,
										 DWORD dwTraceNum, 
										 DWORD dwRefNum)
    {    
    // Precondiciones
    if(NULL == hfFile) 
		return FALSE;    
    if(dwStdRecords > dwLimit) 
		return FALSE;
    
    // Armado de Registro SAF                                              
	safRecord.chEyeCatcher = EYE_CATCHER_SYMBOL;  // Marca visual, nada mas
    safRecord.dwRecord     = dwStdRecords + 1L;   // Registro Ordinal Autonumerado    
    safRecord.tStdTime     = time(NULL);          // Hora de STORED (TIME_T Standard ANSI Unix)
    safRecord.tFwdTime     = 0;                   // Hora de FORWARDED a NULL
    safRecord.wState       = SAF_RECSTATE_STORED; // Estado del Registro    
	safRecord.dwTraceNum   = dwTraceNum;          // Numero de auditoria
	safRecord.dwRefNum     = dwRefNum;            // Numero de referencia
    safRecord.cbBufferLen  = cbLen;               // Longitud Buffer Datos
	
    // Buffer de Datos copiado
	if(cbLen > sizeof safRecord.bBuffer) 
        return FALSE;
    
    memset(safRecord.bBuffer, '*'      , sizeof safRecord.bBuffer); 
    memcpy(safRecord.bBuffer, lpbBuffer, cbLen                   ); 
                            
    // Posicionarse al final del archivo
    fseek(hfFile, 0L, SEEK_END);
	// Cursor큦 Current Position
	lcPrevPos = lcCurrPos = ftell(hfFile);

    // Check Positions as Multiple of Record Size
    if( lcCurrPos % sizeof(safRecord) != 0)
    {
        // Re-position in file 
        fseek(hfFile, lcCurrPos - (lcCurrPos % sizeof(safRecord)), SEEK_SET);
    	// Cursor큦 Current Position
	    lcPrevPos = lcCurrPos = ftell(hfFile);
    };

    // Escritura en si misma
    cbReadWrite = (SHORT)fwrite((LPBYTE)&safRecord, 1, sizeof safRecord, hfFile);
	// Cursor큦 Current Position
	lcCurrPos = ftell(hfFile);
	// Asegurar que se baje el buffer del Sistema Operativo a disco	
	fflush(hfFile);

    // Retorno incorrecto?
    if((cbReadWrite == RET_INVAL) || (cbReadWrite == 0)) 
		return FALSE;    

    // Ultimo registro STORED
    dwLastStdRecord = safRecord.dwRecord;
    
    // Un Stored mas
    if(dwStdRecords < ULONG_MAX) 
        dwStdRecords++;
    else 
        return FALSE;

    // Retorno correcto    
    return TRUE;
    }       
    
// Inicializar estadisticas
BOOL EXPORT StoreAndForward::InitStatistics(LPSTR szXFileName,BOOL bUseTodayMask)
    {        
    // Reinicio a cero de Estadisticas
    dwStdRecords     = 0L;    // Registros STORED 
    dwFwdRecords     = 0L;    // Registros FORWARDED
	dwFirstStdRecord = 0L;    // Primer registro STORED    
    dwLastStdRecord  = 0L;    // Ultimo registro STORED    

	/////////////////////////////////////////////////////////////////
	tSAFInit      = time(NULL); // TimeStamp del inicio de SAF
	tLastSAFCheck = 0L;         // TimeStamp del ultimo chequeo SAF    
	/////////////////////////////////////////////////////////////////

    // Precondicion
    if(NULL == hfFile) 
	{
		hfFile = file_open(szXFileName, "r+");
		if(NULL == hfFile) 
			return FALSE;
		else
			strcpy(szFileName, szXFileName);
	}
    if( NULL == szXFileName ) 
		szXFileName = szFileName;    
    
    // Posicionarse al inicio del Archivo
    fseek(hfFile, 0L, SEEK_SET);
	// Cursor큦 Current Position
	lcPrevPos = lcCurrPos = ftell(hfFile);
    // Check Positions as Multiple of Record Size
    if( lcCurrPos % sizeof(safRecord) != 0)
    {
        // Re-position in file 
        fseek(hfFile, lcCurrPos - (lcCurrPos % sizeof(safRecord)), SEEK_SET);
    	// Cursor큦 Current Position
	    lcPrevPos = lcCurrPos = ftell(hfFile);
    };

    // Lectura adelantada
    cbReadWrite = fread((LPBYTE)&safRecord, 1, sizeof safRecord, hfFile);    
	// Cursor큦 Current Position
	lcCurrPos = ftell(hfFile);
	// Ciclo de lectura
    while ((sizeof safRecord == cbReadWrite) && (cbReadWrite != RET_INVAL))
        {         
        // EYE-CATCH!           
        if(safRecord.chEyeCatcher != EYE_CATCHER_SYMBOL) 
            continue;
        // Contadores STORED/FORWARDED           
        if( SAF_RECSTATE_STORED == safRecord.wState ) 
            {             
            // Un Stored mas
            if(dwStdRecords < ULONG_MAX) 
                dwStdRecords++;
            else 
                return FALSE;
            // Primer registro STORED?
            if (dwFirstStdRecord == 0L)
                dwFirstStdRecord = safRecord.dwRecord;
            // Ultimo registro STORED?
            if (dwLastStdRecord < (DWORD)safRecord.dwRecord)
                dwLastStdRecord = safRecord.dwRecord;
            }
        else if( SAF_RECSTATE_FORWARDED == safRecord.wState ) 
            {
            // Un Forwarded mas
            if(dwFwdRecords < ULONG_MAX) 
                dwFwdRecords++;
            else 
                return FALSE;
            }
        else 
            ////////////////////////////////////////////////////
            {
                if( SAF_RECSTATE_FORWARDED == safRecord.wState ) 
                    ;
                else if( SAF_RECSTATE_NULL == safRecord.wState ) 
                    ;
                else
                    ;
            };
            ////////////////////////////////////////////////////
        lcPrevPos = lcCurrPos = ftell(hfFile);    
        // Check Positions as Multiple of Record Size
        if( lcCurrPos % sizeof(safRecord) != 0)
        {
            // Re-position in file 
            fseek(hfFile, lcCurrPos - (lcCurrPos % sizeof(safRecord)), SEEK_SET);
    	    // Cursor큦 Current Position
	        lcPrevPos = lcCurrPos = ftell(hfFile);
        };

        // Siguiente registro
        cbReadWrite = (SHORT)fread((LPBYTE)&safRecord, 1, sizeof safRecord, hfFile);
	    // Cursor큦 Current Position
	    lcCurrPos = ftell(hfFile);
        }        
                                    
    // Si los registros STORED son los mismos que los FORWARDED,
    // o no hay STORED pero si FORWARDED, asegurarse de crear un 
	// nuevo archivo vacio de SAF, para evitar falta de espacio 
	// en disco, eliminando lo que ya fue FORWARDeado...
    if( ( (dwStdRecords == dwFwdRecords) && (dwStdRecords > 0L) )
        || 
        ( (dwStdRecords == 0L) && (dwFwdRecords > 0L) )
      )
        {
		// Cerrarlo, y borrarlo
        Remove();
		// Recrearlo , y verificar nombre archivo externo
		if(szXFileName)
			strcpy(szFileName,szXFileName);
        return ReCreate( bUseTodayMask ); 
        }
    else 
		// Reabrirlo
		return ReOpen();
    }    
    
// Cierre
BOOL EXPORT StoreAndForward::Close()
    {        
    // Precondicion : handle valido
    if (NULL == hfFile) 
		return FALSE;

	// Precondicion : recuperar tamano de archivo
	if(NULL != hfFile)
    {
		fseek( hfFile, 0L, SEEK_END );
        lFileSize = ftell( hfFile );
        fseek( hfFile, 0L, SEEK_SET );
    }
    // Cierre
    if (fclose(hfFile) == RET_INVAL) 
		return FALSE;
    // Asegurar handle en NULL
    hfFile = NULL;

    // Handle paralelo para WRITE/READ...
    if (NULL == hfFileSav) 
		return TRUE;

    // Cierre
    if (fclose(hfFileSav) == RET_INVAL) 
		return FALSE;
    // Asegurar handle en NULL
    hfFileSav = NULL;

    // Retorno Ok
    return TRUE;
    }
    
// Creacion
BOOL EXPORT StoreAndForward::Create(LPSTR szXFileName, BOOL bUseTodayMask )
    {    
    // Precondicion
    if(hfFile != NULL) 
		return FALSE;
    // Formatear nombre segun el dia actual y un pseudo-random prefijado
    if( bUseTodayMask )
	    SetTodayFileName( szXFileName, szFileName );
    else
        ;
	// Creacion    
    return ((hfFile = file_open(szXFileName, "w")) == NULL) 
		? FALSE 
		: TRUE;
    }    
    
// Contador de Stored
DWORD EXPORT StoreAndForward::StoredRecords(void) 
{ 
    return dwStdRecords; 
}

// Contador de Forwarded
DWORD EXPORT StoreAndForward::ForwardedRecords(void) 
{ 
    return dwFwdRecords; 
}    

// Limite de registros
DWORD EXPORT StoreAndForward::RecordsLimit(DWORD dwNewLimit) 
    { 
    // Precondicion: mayor a Cero
    return (dwNewLimit <= 0L) 
        ? (dwLimit)
        : (dwLimit = dwNewLimit);
    }
    
// Re-Apertura, sin reinicializar Estadisticas
BOOL EXPORT StoreAndForward::ReOpen(void)
    {   
    BOOL bRet1 = FALSE,
         bRet2 = FALSE;

	// Cerrarlo
    Close();   
	// Abrirlo, lectura
    bRet1 = ((hfFile = file_open(szFileName, "r+")) == NULL) 
		? FALSE 
		: TRUE ;
	// Abrirlo, escritura
    bRet2 = ((hfFileSav = file_open(szFileName, "r+")) == NULL) 
		? FALSE 
		: TRUE ;
    // Ambos?
    return ( bRet1 && bRet2 );
    }    

// Re-Creacion
BOOL EXPORT StoreAndForward::ReCreate(BOOL bUseTodayMask )
    {    
    char   szLocFileName[SZSAFFNAMELEN];// Nombre adoptado local
    // Cerrar
    Close();
	strcpy( szLocFileName, szFileName );
    // Recrear
    return Create( szLocFileName , bUseTodayMask );
    }

// Liberacion de SAF
BOOL EXPORT StoreAndForward::FreeStored(void)
    {
    // Precondicion
    if(NULL == hfFile) 
        return FALSE;    
    
    // Posicionarse al inicio del Archivo
    fseek(hfFile, 0L, SEEK_SET);
	// Cursor큦 Current Position
	lcPrevPos = lcCurrPos = ftell(hfFile);        
    // Leer cada SAF STORED de este producto        
    cbReadWrite = fread((LPBYTE)&safRecord, 1, sizeof safRecord, hfFile);
	// Cursor큦 Current Position
	lcCurrPos = ftell(hfFile);
	// Ciclo de Lectura
    while ((cbReadWrite != 0) && (cbReadWrite != RET_INVAL))
        {         
        // Si es STORED
        if( SAF_RECSTATE_STORED == safRecord.wState ) 
			{
            // Cancelarlo
            if(!SetCancelled()) 
				return FALSE;
			}
        lcPrevPos = lcCurrPos = ftell(hfFile);    
        // Siguiente registro
        cbReadWrite = (SHORT)fread((LPBYTE)&safRecord, 1, sizeof safRecord, hfFile);
	    // Cursor큦 Current Position
	    lcCurrPos = ftell(hfFile);
        }        

    return TRUE;
    }

// Ir al inicio del primer registro STORED, para leerlo luego
BOOL EXPORT StoreAndForward::SetFirstStored(void)
    {   
    // Precondicion
    if(NULL == hfFile) 
        return FALSE;    

	/////////////////////////////////////////////
	// Grabar fecha-hora de este chequeo
	tLastSAFCheck = time( NULL );
	/////////////////////////////////////////////

	if((dwFirstStdRecord == 0) || (dwFirstStdRecord == 1))
		{
		// Posicionarse al inicio del primer registro STORED
		if (fseek(hfFile, 0L, SEEK_SET) == RET_INVAL)
            return FALSE;
    	// Cursor큦 Current Position
	    lcPrevPos = lcCurrPos = ftell(hfFile);
        // Ok
        return TRUE;	
		}
	else
		{
		// Posicionarse al inicio del primer registro STORED, tentativamente,
		// en forma aproximada, porque no se asegura que sea el primero, pero
		// si que se empieza a leer a partir de ahi en mas:
		if (fseek(hfFile, (dwFirstStdRecord-1L)*sizeof(safRecord), SEEK_SET) 
            == RET_INVAL)
		   return FALSE;
    	// Cursor큦 Current Position
	    lcPrevPos = lcCurrPos = ftell(hfFile);
        // Ok
        return TRUE;	
    	};
    }    

// Nombre de archivo formateado al dia de la fecha
// Esta es la unica funcion no estandard segun Sistema Operativo en que 
// se implemente, debido al formato de los nombres en el File System
BOOL EXPORT StoreAndForward::SetTodayFileName(LPCSTR pszInFName, LPSTR pszOutFName)
	{
	char    szTempFName[SZSAFFNAMELEN];// Nombre de archivo temporal
	char    szTodayDate[SZSAFFNAMELEN];// Fecha ASCII de hoy
	char   *pszExtension = NULL;       // Ptr a CharId de Extension "."
	char   *pszFile      = NULL;       // Ptr a CharId de File "\"
	///////////////////////////////////////////////////////////////////////////////////////////////////
	char	szRandom[10] = {0};		   // Nro, aleatorio en nombre, prefijado, unico por instancia SAF.
	///////////////////////////////////////////////////////////////////////////////////////////////////
		

	// Precondicion: Nombre base
	if(pszInFName == NULL) 
		return FALSE;
	// Precondicion: Extension "." presente
	// Fix .Net 2005 
	if((pszExtension = strrchr( (char*)pszInFName,'.')) == NULL) 
		pszExtension = ".saf"; // default
	
	// Inicio del Nombre de Archivo, denotada por el ultimo "\"
	// Fix .Net 2005 
	pszFile = strrchr( (char*)pszInFName,'\\');	
	// Si no existe... 
	if (pszFile == NULL) 
        pszFile = (LPSTR)pszInFName; // Inicio en 1er caracter alfabetico
	else                 
        pszFile++;                   // sino en siguiente despues de '\'

	// Fecha y Hora actuales y locales del Sistema
	time_t tSystemTime   = time(NULL);               // System Time
	tm     *ptmLocalTime = localtime(&tSystemTime);  // Local System Time
	// Formateo ISO-YYMMDD para fecha
	strftime(szTodayDate, sizeof szTodayDate, "%y%m%d", ptmLocalTime);

	// Copia del Path hasta el Folder/Directorio
	// Verifica longitud de nombre, asegurando "xxYYMMDD.ext"
	if((pszFile-pszInFName) > (SZSAFFNAMELEN-sizeof("xxYYMMDD.ext"))) 
		return FALSE;
	// Copia del Path hasta el Folder/Directorio
	strncpy(szTempFName, pszInFName, (size_t)(pszFile-pszInFName));
	szTempFName[(size_t)(pszFile-pszInFName)] = 0x00;

	// Si existe un Path previo, concatenar el separador de Paths:
	if((size_t)(pszFile-pszInFName) != 0)		
		strcat(szTempFName, "\\");
		
	// Concatenar iniciales, de MessageTracker por default
	// si no hay caracteres iniciales alfabeticos en el nombre del archivo:
	if( isalpha((int)(*pszFile)) && isalpha((int)(*(pszFile+1))) )
		strncat(szTempFName, pszFile, 2);
	else
		// Concatenar prefijo por default "SA"
		strcat(szTempFName, "SA");	

	// Concatenar formato fecha
	strcat(szTempFName, szTodayDate);
	///////////////////////////////////////////////////////////////
	// Concatenar instancia aleatoria fija, dentro del rango 0..999
	// Precondicion : Numero pseudo-random no debe ser negativo	
	if(bUseRandom)
	{
		sprintf( szRandom, ".%i", max(0,iRandomNum) );
		strcat(szTempFName, szRandom);
	};
	///////////////////////////////////////////////////////////////
	// Concatenar extension
	strcat(szTempFName, pszExtension);

	// Copia externa?
	if(pszOutFName != NULL) 
		strcpy(pszOutFName, szTempFName);	
	// Retorno Ok
	return TRUE;
	}

// Esta es una funcion que retorna TRUE si se verifican ciertos parametros
// internos de la clase que indican que cierta accion es necesaria.
BOOL EXPORT StoreAndForward::IsActionRequired()
{
	/* Verificar ultimos N minutos de inactividad */
	if(time( NULL ) >= (tLastSAFCheck+(60*SAF_INACTIVITY_MINUTES)))
	{
		tLastSAFCheck = time( NULL );
		/* verificar si hay registros presentes */
		if ( StoredRecords() > 0 )
			return TRUE;
		else
			/* no se requiere accion */
			return FALSE;
	}
	/* verificar si hay mas de N registros presentes */
	else if ( StoredRecords() >= SAF_RECORDS_ACTION_REQUIRED )
		return TRUE;
	/* sino, no se requiere accion */
	else
		return FALSE;
}

// Usar nombre random ? (filename.random.saf)
BOOL EXPORT StoreAndForward::UseRandomName(BOOL bUse)
{
	return ( bUseRandom = bUse ); 
}

// Borra el archivo si corresponde
BOOL EXPORT StoreAndForward::Remove(void) 
{
	int  iRetval = 0;
	// Precondicion : recuperar tamano de archivo
	if(NULL != hfFile)
    {
		fseek( hfFile, 0L, SEEK_END );
        lFileSize = ftell( hfFile );
        fseek( hfFile, 0L, SEEK_SET );
    }
    // Cierre de archivo
    Close();
////////////////////////////////////////////////////////////////////
#if ( FALSE )
	// Si no tenia ningun registro, borrar el archivo
	if( 0L == dwStdRecords && 0 <= dwFwdRecords && 0L <= lFileSize  )
		iRetval = remove( szFileName );
#endif 
////////////////////////////////////////////////////////////////////
    // Estadisticas a cero
    dwLimit = dwStdRecords = dwFwdRecords = 0L;
	dwFirstStdRecord = dwLastStdRecord = 0L;
	// OK
	return (TRUE);
}
