///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.             
// Process DATANET Transaction Method
// 
// Método de Procesamiento de Transacción DATANET, con manejo local de parsing del mensaje DPS
// y seteo del código de respuesta en el mensaje saliente.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2003.03.21		mdc		Base
// (Inicial)    2003.04.14		mdc		Controles generales de transacciones debito-credito
// (Alfa)       2003.09.29      mdc     ::RetrieveTransaction()
// (Alfa)       2003.10.18      mdc     ::CalculateMAC() ahora con claves parametricas
// (Alfa)       2003.10.20		mdc	    Calculo de MAC-2: Se fuerza "99" en el operador de debito 1
// (Alfa)       2003.11.07		mdc		ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate
// (Beta)       2003.11.12		mdc		EstadoPendiente segun resultado del procesamiento CREDITO/DEBITO 
// (Beta)       2003.11.21		mdc		Se modifica el RECEPTOR para que simplemente GRABE las transferencias
// (Beta)       2003.11.26		mdc		Mas controles en variable DIFCRE y FECHA DE SOLICITUD para no enviar rtas.
// (Beta)       2003.12.04		mdc		Correccion de la copia de FECHA DE ENVIO segun tamaño de 6 yy/mm/dd.
// (Beta)       2004.01.05		mdc		Dolares diferidas con cuenta invalida
// (Beta)       2004.01.07		mdc		Banco No en Tabas que emite Valores al Cobro
// (Beta)       2004.01.09		mdc		Error de NO inicializacion de variable "szPENDENVIO"
// (Beta)       2004.01.26		mdc		Precondicion : FUNCION DE VALIDACION MAC CARGADA de libreria EDS.
// (Delta)      2005.10.21		mdc		Recupear el año de la transaccion y convertirlo a base segun ANSI C.
// (Delta)      2005.11.14		mdc		bCONTROL_ENABLED = _DPS_CONTROL_POSTING_DATE_ ;
// (Delta)      2005.12.31		mdc		/* FIX 31/12 AL 01/01 IN LITERAL ENGLISH LETTERS **/
// (Delta)      2006.04.11		mdc		ProcessCreditNote(...) ADMITE ESTADOS 80 Y 90
// (Delta)      2006.05.08		mdc		Separador de fecha tambien es el menos '-' . FIX cuando el año de INPUT es de 4 y el de OUTPUT de 2 .
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Headers ANSI C/C++
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Header Type Definitions
#include <qusrinc/typedefs.h>    
// Header Transaction Resolution
#include <qusrinc/trxdnet.h>
// DPS Msg Redefinition
#include <qusrinc/datanet.h>
// DPS SYSTEM
#include <qusrinc/dpcsys.h>
// Libreria de utilidades estandards
#include <qusrinc/stdiol.h>

///////////////////////////////////////////////////////////////////////////////

// Macro de verificacion de codigo de banco emisor/receptor , si es banco propio
#ifndef  IS_ON_OUR_BANKCODE
#define IS_ON_OUR_BANKCODE(x)   (antoi(x,3)==BCRA_ID_DEFAULT_I) 
#endif 

// Macro de control de ejecucion de funcion "NDES" desde libreria o desde 
// funcion propia "NDESX" incorporada
#define _NDES_FROM_LIBRARY_		(TRUE)
#define _NDES_OPER_MAC_EXT_     (7)
#define _NDES_SHIFTS_COUNT_     (2)	

// Libreria de utilidades estandards
#include <windows.h>
#include <windef.h>
// TIPIFICACION DE FUNCION "NDES" si se usa la libreria de DATANET
typedef char * (WINAPI *DNDESPROC) (char texto[],unsigned char clave[],
				  		    short operacion,short shiftkey,short bloques);

#define _TIMESTAMPLENGTH_       (14) 
#define _DATE_STR_LENGTH_       (11) 
#define _TIME_STR_LENGTH_       (17) 

/**********************************************/
#ifndef DPS_B_RECHAZO_CUENTA_CREDITO
#define	 DPS_B_RECHAZO_CUENTA_CREDITO	 90 
#define	_DPS_B_RECHAZO_CUENTA_CREDITO_	"90"
#endif // DPS_B_RECHAZO_CUENTA_CREDITO
/**********************************************/


///////////////////////////////////////////////////////////////////////////////////
// _TRX_BASE_RESOLUTION_
///////////////////////////////////////////////////////////////////////////////////

// Clase Resolucion Base de Transacción
// Constructor default
EXPORT  _TRX_BASE_RESOLUTION_::_TRX_BASE_RESOLUTION_(void)
	{
	boolOnlineMode = is_true;	// En linea por default
	///////////////////////////////////////////////////////////////////////////////////////////
	bProcResult    = is_false;						 // Sin resultado de procesamiento x/default
	wProcRespCde   = DPS_RECHAZO_DEL_BANCO_DE_DEBITO;// Desaprobada por default
	shAuthRespCde  = 0;								 // OK por default
	efFormatMsg    = CNTL_FORMAT_DATANET;			 // DATANET por default
	srand( time( NULL ) );							 // Inicializar criba de pseudorandoms
	///////////////////////////////////////////////////////////////////////////////////////////
	}

EXPORT  _TRX_BASE_RESOLUTION_::~_TRX_BASE_RESOLUTION_(void)
	{	
	// Sin formato de mensaje al finalizar
	efFormatMsg = CNTL_FORMAT_NULL; 
	// Offline al finalizar
	boolOnlineMode = is_false;	
	}

BOOL EXPORT  _TRX_BASE_RESOLUTION_::SetOnOffLine(boolean_t boolXOnline)
	{
	// Señal de modo ON u OFF que implica informar fechas en forma distinta segun corresponda
	boolOnlineMode = boolXOnline;
	return TRUE;
	}

////////////////////////////////////////////////////////////////////////////
// Para loggear errores internos, se disponibilizan estos metodos publicas:
WORD EXPORT  _TRX_BASE_RESOLUTION_::TransactionStatus(void)
	{
	// Ok?
	return ( bProcResult ) 
		? wProcRespCde
		: 0;
	}

////////////////////////////////////////////////////////////////////////////
// Para loggear errores internos, se disponibilizan estos metodos publicas:
short EXPORT  _TRX_BASE_RESOLUTION_::BackEndAuthStatus(void)
{
	// Ok? {-32768....+32767}
	return shAuthRespCde;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inicio de Procesamiento de transacciones en general
// A implementar en subclases que heredan esta clase y metodo virtual
WORD  _TRX_BASE_RESOLUTION_::ProcessInit(void)
{
	// Pre-Validar datos de la transaccion, siempre OK
    // Postcondicion : OK
	return ( DPS_APROBADA_SIN_RECHAZO );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formateo especial de campos fecha y hora, segun mascara y segun ano de 4 digitos
BOOL  _TRX_BASE_RESOLUTION_::FormatTrxDateTime_WMask(char *szYYYYMMDD, char *szHHMMSS,    
                                    char *szYYYYMMDD_Trx, char *szHHMMSS_Trx,
                                    boolean_t bUseSeparators ,
                                    boolean_t bUse4DigitsYear,
                                    boolean_t bFormatIsMMDDYY,
                                    boolean_t bAlwaysTimeIsDateAndTime,
                                    short     nTimestampLength,
                                    short     nHourLength,
                                    short     nOptionalInputYearDigits)    
{
    const time_t tTimestamp         = time( NULL );                /* TimeStamp actual */
    const tm *ptmLocal              = localtime( &tTimestamp );    /* Fecha-Hora ASCII actual */
    const short cnYearDigits        = ( bUse4DigitsYear ) ? 4 : 2; /* default es 4 digitos */
    short nYear                     = ptmLocal->tm_year + 1900 ;   /* default es AÑO actual  */    
    short nLength                   = 0;                           /* Longitud de cadena */
    /***********************************************************************/
    const char *cszMMDDYY_Mask         = "%.2s/%.2s/%0*i";
    const char *cszMMDDYY_MaskNoSep    = "%.2s%.2s%0*i";
    const char *cszHHMMSS_Mask         = "%.2s:%.2s:%.2s";
    const char *cszHHMMSS_MaskNoSep    = "%.2s%.2s%.2s";
    const char *cszTIMESTAMP_Mask      = "%s %.2s:%.2s:%.2s";
    const char *cszTIMESTAMP_MaskNoSep = "%s%.2s%.2s%.2s";
    const char *cszYYMMDD_Mask         = "%0*i/%.2s/%.2s";
    const char *cszYYMMDD_MaskNoSep    = "%0*i%.2s%.2s";
    char       *pszDATE_Mask           = (char *)cszMMDDYY_Mask ;    /* default c/separadores y MMDDYYYY*/
    char       *pszTIME_Mask           = (char *)cszHHMMSS_Mask ;    /* default c/separadores y HHMMSS*/
    char       *pszTIMESTAMP_Mask      = (char *)cszTIMESTAMP_Mask ; /* default c/separadores y YYYY*/
    /***********************************************************************/
    // Para fechas del formato "Jan 1 2000" en ingles, se definen los valores:
    const char  *aszMonths[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" } ;
    const char  *aszNMonth[] = {"01", "02", "03", "04", "05", "06", 
                                "07", "08", "09", "10", "11", "12" } ;
    boolean_t   bMonthInEnglish = is_false;          // Indicador de MES en ingles detectado
    short       iMonth      = 0;                     // Posicion del MES en el arreglo
    char       *pStr        = NULL;                  // MES en ingles fue detectado?
    char       *pSeparators = NULL;                  // Separadores detectados?
    short       nSeparators = 0;                     // En 1 para suma aritmetica de separadores 
    short       nInputYearDigits = cnYearDigits ;    // Longitud del AÑO de entrada    
    short       nCount           = 0;                // Contador general
    boolean_t   bFlag            = is_false;         // Indicador general 
    /***********************************************************************/
	const short nSysMonth       = ptmLocal->tm_mon + 1 ;     /* default es MES actual  */    
	const short nSysYear        = ptmLocal->tm_year + 1900 ; /* default es AÑO actual  */    
	short       nTrxYear        = ptmLocal->tm_year + 1900 ; /* default es AÑO actual  */    
	/***********************************************************************/


    /////////////////////////////////////////////////////////////////////////
    // Precondiciones
    if( (NULL == szYYYYMMDD     && NULL == szHHMMSS)     || 
        (NULL == szYYYYMMDD_Trx && NULL == szHHMMSS_Trx) ||
        nTimestampLength < 0 || nTimestampLength > 32    ||
        nHourLength < 0 || nHourLength > 32 )    
        return FALSE;
    /////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Precondicion : formato de entrada es alfanumerico descrptivo en INGLES?
    /////////////////////////////////////////////////////////////////////////
    pStr = szYYYYMMDD_Trx;
    if(!pStr) 
        pStr = szHHMMSS_Trx;
    for(iMonth = 0; 
        pStr && iMonth < 12 && bMonthInEnglish != is_true; 
        iMonth++)
    {
        if(strncmp(pStr,aszMonths[iMonth],3)==0)
        {
            bMonthInEnglish = is_true;
            break;
        };
    }; // end-for
    //////////////////////////////////////////////////////////////////////////
    // Tiene separadores de fecha-hora el campo de entrada?
    /////////////////////////////////////////////////////////////////////////
    pStr = szYYYYMMDD_Trx;
	/*************************************/
    if(!pStr) 
		{
        pStr		   = szHHMMSS_Trx;
		szYYYYMMDD_Trx = szHHMMSS_Trx;
		}
	/*************************************/
    pSeparators = strchr(pStr, '/');
	/*************************************/
    if(!pSeparators)
        pSeparators = strchr(pStr, '-');
	/*************************************/
    if(!pSeparators)
        pSeparators = strchr(pStr, ':');
    if(pSeparators && (pSeparators-pStr) <= 8) // hasta 8 digitos
        nSeparators = 1;
    else
        nSeparators = 0;

    /////////////////////////////////////////////////////////////////////////
    // La cifra del año de entrada, es de 4 o 2 digitos?
    // Verificar longitud de decimales, si es que no se especifico ya....
    /////////////////////////////////////////////////////////////////////////    
    if( 2 == nOptionalInputYearDigits || 4 == nOptionalInputYearDigits )
    {
        nInputYearDigits = nOptionalInputYearDigits;
    }
    else
    {
        // Contar cantidad de digitos, aunque no es un metodo fiable
        // ya que si es una estructura y el campo siguiente es numerico,
        // pues se solaparia. Peromejor que nada es.
        for(nCount = 0, bFlag=is_true, pStr = szYYYYMMDD_Trx;
            nCount < 8 && bFlag; 
            nCount++, pStr++)
            {
                if(!isdigit(*pStr))
                {
                    bFlag = is_false; 
                    break;
                };/*end-if*/
            };/*end-for*/
        // Segun la cantidad de digitos y el Indicador general....
        if( bFlag && 8 == nCount || bMonthInEnglish )
            nInputYearDigits = 4;
        else if ( 6 == nCount )
            nInputYearDigits = 2;
        else 
            nInputYearDigits = 2;
    };/*end-if-optional-input-year-digits*/
    /////////////////////////////////////////////////////////////////////////

	/***************************************************************************/
    // Elegir la mascara y el año, segun este ultimo sea de 2/4 digitos
    if( bUse4DigitsYear )
        nYear = ptmLocal->tm_year + 1900; // Año de 4 digitos, base año 1900 y hasta 9999
    else
        nYear = ptmLocal->tm_year % 100 ; // Año de 2 digitos, base 100 y hasta 99
	/* FIX 31/12 AL 01/01 - begin **********************************************/
    if( bUse4DigitsYear )
	{
        nTrxYear = antoi(szYYYYMMDD_Trx,cnYearDigits);       // Año de 4 digitos
		nYear    = antoi(szYYYYMMDD_Trx,cnYearDigits);       // Año de 4 digitos
	}
    else
	{
		/* FIX cuando el año de INPUT es de 4 y el de OUTPUT de 2 - begin ******/
        nTrxYear = antoi(szYYYYMMDD_Trx + (nInputYearDigits-cnYearDigits),
						 cnYearDigits) % 100; // Año de 2 digitos
		nYear    = antoi(szYYYYMMDD_Trx + (nInputYearDigits-cnYearDigits),
						 cnYearDigits) % 100; // Año de 2 digitos
		/* FIX cuando el año de INPUT es de 4 y el de OUTPUT de 2 - begin ******/
	};
	/**************************************************/
	/* FIX 31/12 AL 01/01 IN LITERAL ENGLISH LETTERS **/
	/**************************************************/
	if(bMonthInEnglish)
	{
		nCount = min(11,strlen(szYYYYMMDD_Trx)); /* LEN("Jan 01 1970") = 11 */
		nYear = antoi(szYYYYMMDD_Trx+nCount-cnYearDigits,cnYearDigits);  
	}
	/**************************************************/
	/* FIX 31/12 AL 01/01 - end *************************************************/
    if(bUseSeparators)
    {
        if(bFormatIsMMDDYY)
            pszDATE_Mask  = (char *)cszMMDDYY_Mask ;
        else
            pszDATE_Mask  = (char *)cszYYMMDD_Mask ;
        pszTIMESTAMP_Mask = (char *)cszTIMESTAMP_Mask ;
    }
    else
    {
        if(bFormatIsMMDDYY)
            pszDATE_Mask  = (char *)cszMMDDYY_MaskNoSep ;
        else
            pszDATE_Mask  = (char *)cszYYMMDD_MaskNoSep ;
        if(bAlwaysTimeIsDateAndTime)
            pszTIME_Mask  = (char *)cszTIMESTAMP_MaskNoSep ;
        else
            pszTIME_Mask  = (char *)cszHHMMSS_MaskNoSep ;
        pszTIMESTAMP_Mask = (char *)cszTIMESTAMP_MaskNoSep ;
    };

    /////////////////////////////////////////////////////////////////////
    // Verificar si la fecha es TODO BLANCOS, y asumir una fecha DUMMY
    if(szYYYYMMDD_Trx && strncmp(szYYYYMMDD_Trx,"        ",cnYearDigits+2+2)==0)
    {
        if(4 == cnYearDigits) 
        {
            szYYYYMMDD_Trx = "19000101";
            nInputYearDigits = 4;
        }
        else                
        {
            szYYYYMMDD_Trx = "000101";
            nInputYearDigits = 2;
        };
    };
    // Verificar si la hora es TODO BLANCOS, y asumir una hora DUMMY
    if(szHHMMSS_Trx && strncmp(szHHMMSS_Trx,"      ",6)==0)
        szHHMMSS_Trx = "000000";
    else if(szHHMMSS_Trx && strncmp(szHHMMSS_Trx,"    ",4)==0)
        szHHMMSS_Trx = "000000";
    /////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////
    // Fecha de Transaccion, formato MM/DD/YYYY (TRX-DATE)
    // El ano se obtiene , o bien del sistema, o bien de la fecha de transaccion :
    // Mismo ano de transmision que la fecha de negocios? (comparar 2 ultimos digitos)
    //////////////////////////////////////////////////////////////////////////////////
    if(szYYYYMMDD)
    {
        ///////////////////////////////////////////////////////////////////////////
        // Si se usa "YYMMDD" pero el formato de entrada tiene el año de 4 digitos,
        // se agrega un offset para contemplarlo:
        if( bFormatIsMMDDYY && !bUse4DigitsYear && 
            nInputYearDigits > cnYearDigits && 
            nTimestampLength == _DATE_STR_LENGTH_ ) 
            nLength = nInputYearDigits - cnYearDigits;
        else
            nLength = 0;
        ///////////////////////////////////////////////////////////////////////////
        if(bFormatIsMMDDYY && szYYYYMMDD_Trx && !bMonthInEnglish)
		{
            sprintf(szYYYYMMDD,pszDATE_Mask, szYYYYMMDD_Trx+cnYearDigits+nLength, 
                szYYYYMMDD_Trx+nInputYearDigits+2, cnYearDigits , nYear );
		}
        else if (szYYYYMMDD_Trx && !bMonthInEnglish)
		{
            sprintf(szYYYYMMDD,pszDATE_Mask, cnYearDigits , nYear, 
                szYYYYMMDD_Trx+nInputYearDigits+nSeparators, 
                szYYYYMMDD_Trx+nInputYearDigits+2*nSeparators+2 );
		}
        else if(bFormatIsMMDDYY && bMonthInEnglish)
		{
            sprintf(szYYYYMMDD,pszDATE_Mask, aszNMonth[iMonth],
                szYYYYMMDD_Trx+3+nSeparators, cnYearDigits , nYear );
		}
        else if (szYYYYMMDD_Trx && bMonthInEnglish)
        {
            sprintf(szYYYYMMDD,pszDATE_Mask, cnYearDigits , nYear, 
                aszNMonth[iMonth], szYYYYMMDD_Trx+3+1 );
            // Verificar que el CERO del dia este correctamente reemplazado            
            if((pStr = strchr(szYYYYMMDD, ' ')) != NULL) 
                (*pStr) = '0';
        }
        else
            return (FALSE);
        // Paddear lo restante con blancos, si corresponde...
        nLength = strlen(szYYYYMMDD);
        if(nTimestampLength > nLength && nTimestampLength <= 32)
            memset( szYYYYMMDD+nLength, ' ', nTimestampLength-nLength);
    }
    // Hora de Transaccion formato HH:MM:SS
    if(szHHMMSS)
    {
        /*******************************************************************************/
        if(!bAlwaysTimeIsDateAndTime && szHHMMSS_Trx && !bMonthInEnglish && !szYYYYMMDD)
            sprintf(szHHMMSS  ,pszTIME_Mask , 
                szHHMMSS_Trx+nInputYearDigits+2+2+2*nSeparators+1,                
                szHHMMSS_Trx+nInputYearDigits+2+2+2*nSeparators+1+2+1,                
                "00" );
        /*******************************************************************************/
        else if(bAlwaysTimeIsDateAndTime && szHHMMSS_Trx && !bMonthInEnglish)
            sprintf(szHHMMSS  ,pszTIMESTAMP_Mask , szYYYYMMDD,
                szHHMMSS_Trx, szHHMMSS_Trx+2, "00" );
        else if (szHHMMSS_Trx && !bMonthInEnglish)
            sprintf(szHHMMSS  ,pszTIME_Mask , 
                szHHMMSS_Trx,szHHMMSS_Trx+2,"00" );
        else if(bAlwaysTimeIsDateAndTime && szHHMMSS_Trx && bMonthInEnglish)
            sprintf(szHHMMSS  ,pszTIMESTAMP_Mask , szYYYYMMDD,
                szHHMMSS_Trx+3+2*nSeparators,
                szHHMMSS_Trx+3+2*nSeparators+2+nSeparators, 
                "00" );
        else if(szHHMMSS_Trx && bMonthInEnglish)
            sprintf(szHHMMSS  ,pszTIME_Mask , 
                szHHMMSS_Trx+3+2*nSeparators,
                szHHMMSS_Trx+3+2*nSeparators+2+nSeparators, 
                "00" );
        else
            return (FALSE);
        // Paddear lo restante con ceros, si corresponde, a los milisegundos....
        nLength = strlen(szHHMMSS);
        if(nHourLength > nLength && nHourLength <= 32)
            memset( szHHMMSS+nLength, '0', nHourLength-nLength);
        if(nHourLength < nLength && nHourLength > 0 && nHourLength <= 32)
            memset( szHHMMSS+nHourLength, ' ', nLength-nHourLength);
    };
    // Ok, retornar
    return (TRUE);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Implementacion de clase TrxResDATANET
/////////////////////////////////////////////////////////////////////////////
// Constructor
TrxResDATANET::TrxResDATANET(void)
{
    time_t  tNOW         = time(NULL);          // Current Time
    struct  tm stCutover = *localtime( &tNOW ); // Current TimeStamp

    // Valores iniciales de atributos propios
    szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;  // Transferencia por enviar
    szPENDENVIO[1] = 0x00 ;                     // End-Of-Text , NULL
    bExecStatus_ok = is_false;                  // Resultado, sin ejecutar

    // Hora de Corte por Omision 23:59:59
    stCutover.tm_hour = 23;
    stCutover.tm_min  = 59;
    stCutover.tm_sec  = 59;
    // Rearmar TimeSatmp para medianoche
    tNOW = mktime( &stCutover );
    stCutover = *localtime( &tNOW ); // Guardarlo

    // Guardarlo en la instancia
    stCutoverTime = stCutover;
}

/////////////////////////////////////////////////////////////////////////////
// Procesamiento de Transacciones DATANET.
// Retorna is_true por procesamiento aprobado o no,
// y is_false al hallar algún Error en el Sistema o en el Mensaje ISO-8583.
// Verifica también si la respuesta es solicitada (OnLine) o no 
// (OffLine) para la copia de datos en el mensaje de retorno.
/////////////////////////////////////////////////////////////////////////////
boolean_t TrxResDATANET::ProcessTransaction(void)
	{	
	// Estructuras locales de parsing de Datos desde el Mensaje o BDatos
	// Variables para extracción de Campos del Mensaje ISO-8583/POS/...	
    WORD            wRespCode      = 0; // Codigo de respuesta hacia nivel de invocacion superior
    WORD            wRetVal        = 0; // Codigo de respuesta desde nivel de invocacion inferior
	// Ok o Error?
	boolean_t		bProcResult = is_false;// Procesamiento OK?	
	// MAC's
	CHAR			szMAC[DNET_MAC_LENGTH+1] = {0}; // calculada para debitos/creditos
    //////////////////////////////////////////////////////////////////////////////////

    // procesar toda transacción de DPS INTERBANKING 
	if( dpsMsg.IsDPSTransaction() ) 
		{

        // Valores iniciales por omision para la transferencia en curso
        wRespCode      = DPS_RECHAZO_DEL_BANCO_DE_DEBITO;// Transferencia rechazada       
        szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;    // Transferencia por enviar    
        bExecStatus_ok = is_false;                    // Resultado, sin ejecutar
		
		///////////////////////////////////////////////////////////////////////////////
		if( dpsMsg.IsValidReversal()            || // Es reversa generica 
		    dpsMsg.IsDPSDebitTransfer()		    || // Transferencia debito
		    dpsMsg.IsDPSCreditTransfer()	    || // Transferencia credito
		    dpsMsg.IsDPSCreditAndDebitTransfer()|| // Transferencia ambas db/cr
		    dpsMsg.IsDPSOtherMessages()         )  // Otros mensajes al banco
        {
            // Simplemente loggear la transferencia tal como viene, CAUSAL=0,AUTH=FALSE
			wRespCode = ProcessOtherMessages(is_false,is_false);
        }
		else
        {
			// Todo lo demas NO SE RECONOCE, SON "TRANSACCIONES INVALIDAS"
			wRespCode = DPS_RECHAZO_DEL_BANCO_DE_DEBITO;
        };
		///////////////////////////////////////////////////////////////////////////////

		// Backup de RESULTADO de ejecucion hacia la clase base
		wProcRespCde   = wRespCode;		
        bProcResult    = is_true; // Procesamiento OK
        bExecStatus_ok = is_true; // Resultado EJECUTADA        

		
		}
	else
		{

        ////////////////////////////////////////////////////////////////////
        // Calcular MACs de respuesta
        wRetVal = CalculateMAC( IS_ON_OUR_BANKCODE(dpsMsg.input.data.BANDEB) ? "2" : "3",
                                szMAC );
        if( DPS_APROBADA_SIN_RECHAZO != wRetVal ) 
        {
            //  Motivo de rechazo debito/credito
            dpsMsg.SetCauseCode( wRetVal, dpsMsg.IsDPSCreditTransfer() );
            wRetVal   = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO ;
            dpsMsg.SetStatusCode( wRetVal );
            wRetVal   = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO;
            wRespCode = DPS_DEMORADA_NO_ENVIAR_RESPUESTA;
        };
        ////////////////////////////////////////////////////////////////////
		
		// Código de Respuesta erróneo
		dpsMsg.SetStatusCode( DPS_RECHAZO_DEL_BANCO_DE_DEBITO );
		// Formatear respuesta
		if(!dpsMsg.FormatResponseMessage( szMAC, szMAC ))
			return (is_false);

        bProcResult = is_true;// Procesamiento OK

		};//end-if-then-else-TranCode

	 // Ok por procesado
	return (is_true);
	}//end-ProcessATMTransaction

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de reversos
WORD TrxResDATANET::ProcessReversal(WORD wCause, boolean_t bIsAuth)
{
	WORD wRespCde = ProcessInit( bIsAuth );	
	// Precondicion
	if(wRespCde != DPS_APROBADA_SIN_RECHAZO)
		return wRespCde;

	//////////////////////////////////////////////////////////////////////////
    dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer());    
	// Loggear Transaccion para su posterior autorizacion
	if(is_false == bIsAuth) wRespCde = LoggTransaction( _DPS_TRAT_PENDIENTE_  );
	// Formar respuesta segun resultado
	dpsMsg.SetRecType( dpsMsg.IsDPSCreditTransfer() 
		? DPS_RECTYPE_CREDITO_RESPUESTA 
		: DPS_RECTYPE_DEBITO_RESPUESTA );	
    // Retorno del motivo hacia nivel superior de invocacion
	return (!bIsAuth)
        ? wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED
        : wRespCde = DPS_APROBADA_SIN_RECHAZO;
	//////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResDATANET::ProcessAccountVerification(WORD wCause,boolean_t bIsAuth)// Verificacion cuenta destino
{
	WORD wRespCde = ProcessInit( bIsAuth );
	// Precondicion
	if(wRespCde != DPS_APROBADA_SIN_RECHAZO)
		return wRespCde;

	//////////////////////////////////////////////////////////////////////////
    dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer());
	// Loggear Transaccion para su posterior autorizacion
	if(is_false == bIsAuth) wRespCde = LoggTransaction( _DPS_TRAT_PENDIENTE_  );
	// Formar respuesta segun resultado
	dpsMsg.SetRecType( dpsMsg.IsDPSCreditTransfer() 
		? DPS_RECTYPE_CREDITO_RESPUESTA 
		: DPS_RECTYPE_DEBITO_RESPUESTA );	
    // Retorno del motivo hacia nivel superior de invocacion
	return (wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED);
	//////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResDATANET::ProcessOtherMessages(WORD wCause,boolean_t bIsAuth)			
{
	WORD wRespCde = ProcessInit( bIsAuth );
    ///////////////////////////////////////////////////////////////

	// Precondicion
	if(wRespCde != DPS_APROBADA_SIN_RECHAZO)
		return wRespCde;
    

	//////////////////////////////////////////////////////////////////////////
    dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer());
	// Loggear Transaccion para su posterior autorizacion
	if(is_false == bIsAuth) wRespCde = LoggTransaction( _DPS_TRAT_PENDIENTE_ );
	// Formar respuesta segun resultado
	dpsMsg.SetRecType( dpsMsg.IsDPSCreditTransfer() 
		? DPS_RECTYPE_CREDITO_RESPUESTA 
		: DPS_RECTYPE_DEBITO_RESPUESTA );	
    // Retorno del motivo hacia nivel superior de invocacion
	return (!bIsAuth)
        ? wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED
        : wRespCde = DPS_APROBADA_SIN_RECHAZO;
	//////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResDATANET::ProcessCreditDebitNote(WORD wCause,boolean_t bIsAuth)			
{
    // Siempre esta PENDIENTE DE ENVIO a menos que el banco credito difiera los creditos    
	WORD  wRespCde       = ProcessInit( bIsAuth ),
          wRetVal        = 0;
	
    // Precondicion
    if(wRespCde != DPS_APROBADA_SIN_RECHAZO)
		return ( wRespCde );


	// Solo se aceptarán los mensajes que ingresen con ESTADO="00", "30" u "80", 
	// un valor distinto originará una respuesta con RECTYPE="TDR", ESTADO="70", 
	// RECHDB="0020" y CODMAC recalculado (algoritmo MAC2).
	// Verificacion de campos
	if( strncmp( dpsMsg.input.data.ESTADO,_DPS_ENVIADO_POR_EL_ABONADO_,2)!=0               &&
		strncmp( dpsMsg.input.data.ESTADO,_DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED_,2)!=0 &&
		strncmp( dpsMsg.input.data.ESTADO,_DPS_RECHAZO_DEL_BANCO_DE_CREDITO_,2)!=0)
	{
		// Motivo de rechazo	
		dpsMsg.SetCauseCode( DPS_OPERACION_INEXISTENTE, dpsMsg.IsDPSCreditTransfer());
        // Cambio de Estado
        dpsMsg.SetStatusCode( wRespCde = DPS_RECHAZO_DEL_BANCO_DE_DEBITO );
    	// Loggear Transaccion para su posterior autorizacion
	    if(is_false == bIsAuth) wRespCde = LoggTransaction( _DPS_TRAT_PROCESADA_ , szPENDENVIO );
        // Tipo de registro de respuesta despues de LOGGEAR en TEF
        dpsMsg.SetRecType( DPS_RECTYPE_DEBITO_RESPUESTA );
        // Retorno del motivo hacia nivel superior de invocacion
		return (wRespCde = DPS_RECHAZO_DEL_BANCO_DE_DEBITO);
	};

    //////////////////////////////////////////////////////////////////////////
	// Loggear Transaccion para su posterior autorizacion
    dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer());
	if(is_false == bIsAuth) wRespCde = LoggTransaction( _DPS_TRAT_PENDIENTE_ , szPENDENVIO );
	// Formar respuesta segun resultado
	dpsMsg.SetRecType( dpsMsg.IsDPSCreditTransfer() 
		? DPS_RECTYPE_CREDITO_RESPUESTA 
		: DPS_RECTYPE_DEBITO_RESPUESTA );	

    // Retorno del motivo hacia nivel superior de invocacion
	return (!bIsAuth)
        ? wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED
        : wRespCde = DPS_APROBADA_SIN_RECHAZO;
	//////////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResDATANET::ProcessDebitNote(WORD wCause,boolean_t bIsAuth)			
{
	WORD wRespCde = ProcessInit( bIsAuth );
	// Precondicion
	if(wRespCde != DPS_APROBADA_SIN_RECHAZO)
		return ( wRespCde );

    //////////////////////////////////////////////////////////////////////////////////////
	// Solo se aceptarán los mensajes que ingresen con ESTADO="00", "30" u "80", 
	// un valor distinto originará una respuesta con RECTYPE="TDR", ESTADO="70", 
	// RECHDB="0020" y CODMAC recalculado (algoritmo MAC2).
	// CONTROL 1 : 
	if( strncmp( dpsMsg.input.data.ESTADO,_DPS_ENVIADO_POR_EL_ABONADO_,2)!=0               &&
		strncmp( dpsMsg.input.data.ESTADO,_DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED_,2)!=0 &&
		strncmp( dpsMsg.input.data.ESTADO,_DPS_RECHAZO_DEL_BANCO_DE_CREDITO_,2)!=0         &&
        strncmp( dpsMsg.input.data.ESTADO,_DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO_,2)!=0)
	{
        // Motivo de rechazo
		dpsMsg.SetCauseCode( DPS_OPERACION_INEXISTENTE, dpsMsg.IsDPSCreditTransfer());
        // Cambio de Estado
        dpsMsg.SetStatusCode( wRespCde = DPS_RECHAZO_DEL_BANCO_DE_DEBITO );
	    // Loggear Transaccion para su posterior autorizacion
        if(is_false == bIsAuth) wRespCde = LoggTransaction( _DPS_TRAT_PROCESADA_ );
        // Tipo de registro de respuesta
		dpsMsg.SetRecType( DPS_RECTYPE_DEBITO_RESPUESTA );
        // Retorno del motivo hacia nivel superior de invocacion
		return (wRespCde = DPS_RECHAZO_DEL_BANCO_DE_DEBITO);
	};


    //////////////////////////////////////////////////////////////////////////////////////
	// Si ESTADO="00" o "20", se enviará el débito al SISTEMA AUTORIZADOR DEL BANCO, si 
    // este lo aplica se generará un registro en la cola de datos  con formato OUTPUT para 
    // que el programa  envíe la respuesta a la Red, con RECTYPE="TDR", ESTADO="60" y 
    // CODMAC recalculado (algoritmo MAC2).
    // CONTROL 2 : 
	if( strncmp( dpsMsg.input.data.ESTADO,_DPS_ENVIADO_POR_EL_ABONADO_,2)==0 
        ||
        strncmp( dpsMsg.input.data.ESTADO,_DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO_,2)==0)
	{
        // Siempre es ENVIADA POR EL ABONADO, asi que si llega una DEMORADA
        // se fuerza un ENVIADA POR EL ABONADO porque es un re-intento por
        // banco inexistente .
        if(strncmp( dpsMsg.input.data.ESTADO,_DPS_ENVIADO_POR_EL_ABONADO_,2)!=0)
            strncpy( dpsMsg.input.data.ESTADO,_DPS_ENVIADO_POR_EL_ABONADO_,2);
		// Enviar para autorizar....
        dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer());
	    // Loggear Transaccion para su posterior autorizacion
	    if(is_false == bIsAuth) wRespCde = LoggTransaction( _DPS_TRAT_PENDIENTE_ );
		// Respuesta por omision, NO es inmediata
	    dpsMsg.SetRecType( DPS_RECTYPE_DEBITO_RESPUESTA );	    
        // Retorno del motivo hacia nivel superior de invocacion
	    return (!bIsAuth)
            ? wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED
            : wRespCde = DPS_APROBADA_SIN_RECHAZO;
	}
    else
    {
        ////////////////////////////////////////////////////////////////////////////////
	    // Si el SISTEMA AUTORIZADOR DEL BANCO rechazara el débito, se generará la rta. 
        // a la Red, con RECTYPE="TDR", ESTADO="20", RECHCR con el código que se ajuste
        // al rechazo y CODMAC recalculado (algoritmo MAC2). 
	    // Esta operación ingresa al circuito de autorización.
        // CONTROL 3 : 	    
        dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer());
	    // Loggear Transaccion para su posterior autorizacion
        // con estado tratamiento PENDIENTE y tambien ENVIO PENDIENTE
	    if(is_false == bIsAuth) wRespCde = LoggTransaction( _DPS_TRAT_PENDIENTE_ );
	    // Respuesta por omision, NO es inmediata
	    dpsMsg.SetRecType( DPS_RECTYPE_DEBITO_RESPUESTA );	    
        // Retorno del motivo hacia nivel superior de invocacion
    	return (!bIsAuth)
            ? wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED
            : wRespCde = DPS_APROBADA_SIN_RECHAZO;
        ////////////////////////////////////////////////////////////////////////////////
    }; // end-if-ESTADO=ENVIADO-por-el-abonado
	//////////////////////////////////////////////////////////////////////////////////

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResDATANET::ProcessCreditNote(WORD wCause,boolean_t bIsAuth)			
{    
    // Procesamiento inicial
	WORD  wRespCde = ProcessInit( bIsAuth );

    // Precondicion : Aprobada sin rechazo
	if(wRespCde != DPS_APROBADA_SIN_RECHAZO)
		return ( wRespCde );
	
    ////////////////////////////////////////////////////////////////////////////////////
	// Por definición del Banco, serán aplicados en firme, en el momento de recibirlos.
	// Solo se aceptarán los mensajes que ingresen con ESTADO="40", "60" o "70", un valor
	// distinto originará una respuesta con RECTYPE="TCR", ESTADO="80", RECHCR="0020" y 
	// CODMAC recalculado (algoritmo MAC2).
    // CONTROL 1 :
	if( strncmp( dpsMsg.input.data.ESTADO,_DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO_,2)!=0 &&
        strncmp( dpsMsg.input.data.ESTADO,_DPS_VALOR_AL_COBRO_,2)!=0 &&
		strncmp( dpsMsg.input.data.ESTADO,_DPS_EJECUTADA_,2)!=0 &&
		strncmp( dpsMsg.input.data.ESTADO,_DPS_RECHAZO_DEL_BANCO_DE_DEBITO_,2)!=0 &&
		/**2006.04.11-mdc********************************************************/
		strncmp( dpsMsg.input.data.ESTADO,_DPS_RECHAZO_DEL_BANCO_DE_CREDITO_,2)!=0 &&
		strncmp( dpsMsg.input.data.ESTADO,_DPS_B_RECHAZO_CUENTA_CREDITO_    ,2)!=0)		
		/**2006.04.11-mdc********************************************************/
	{
        // Motivo de rechazo
		dpsMsg.SetCauseCode( DPS_OPERACION_INEXISTENTE, dpsMsg.IsDPSCreditTransfer());
        // Cambio de Estado
        dpsMsg.SetStatusCode( wRespCde = DPS_RECHAZO_DEL_BANCO_DE_CREDITO );
		// Loggear Transaccion c/motivo informativo
		if(is_false == bIsAuth) wRespCde = LoggTransaction( _DPS_TRAT_PROCESADA_ );
		dpsMsg.SetRecType( DPS_RECTYPE_CREDITO_RESPUESTA );
        // Retorno del motivo hacia nivel superior de invocacion
		return (wRespCde = DPS_RECHAZO_DEL_BANCO_DE_CREDITO);
	};


    ////////////////////////////////////////////////////////////////////////////////////
    // CONTROL 2 :
    // Si ESTADO="40" o RIEBCO="S", deberá quedar pendiente, a la espera que Banca Electrónica
    // lo active.  Genera aviso a Banca Electrónica, no requiere intervención del oficial de 
    // créditos.
    if(strncmp( dpsMsg.input.data.ESTADO,_DPS_VALOR_AL_COBRO_,2)==0 
        ||  
        'S' == dpsMsg.input.data.RIEBCO[0] )      
    {
        // Nada, grabar como pendiente, ir al pie de la funcion
    }
	// Si ESTADO="40" + RIEBCO="S", deberá quedar pendiente, a la espera que Banca 
	// Electrónica lo active. Genera aviso a Banca Electrónica, no requiere intervención
	// del oficial de créditos.
	// NOTA: la posibilidad ESTADO="40"+RIEBCO="N" que existe, no será recibida por el 
	// Banco por su modalidad de operación.
	else if( strncmp( dpsMsg.input.data.ESTADO,_DPS_VALOR_AL_COBRO_,2)==0 
          &&
		'N' == dpsMsg.input.data.RIEBCO[0] )
	{
        // Motivo de rechazo
		dpsMsg.SetCauseCode( DPS_OPERACION_NO_HABILITADA_EN_BANCO, dpsMsg.IsDPSCreditTransfer());
        // Cambio de Estado
        dpsMsg.SetStatusCode( wRespCde = DPS_RECHAZO_DEL_BANCO_DE_CREDITO );
		// Loggear Transaccion c/motivo informativo
		if(is_false == bIsAuth) wRespCde = LoggTransaction( _DPS_TRAT_PROCESADA_ );
		// Formar respuesta segun resultado
		dpsMsg.SetRecType( DPS_RECTYPE_CREDITO_RESPUESTA );
        // Retorno del motivo hacia nivel superior de invocacion
    	return (!bIsAuth)
            ? wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED
            : wRespCde = DPS_RECHAZO_DEL_BANCO_DE_CREDITO;
	};

	// Al convalidarse por parte de Banca Electrónica, el proceso se comportará como sigue:
	// Se enviará el crédito al SISTEMA AUTORIZADOR DEL BANCO, si este lo aplica se generará
	// un registro en la cola de datos con formato OUTPUT para que el programa  envíe la 
	// respuesta a la Red, con RECTYPE="TCR", ESTADO="60" y CODMAC recalculado .

	//////////////////////////////////////////////////////////////////////////
    dpsMsg.SetCauseCode( wRespCde, dpsMsg.IsDPSCreditTransfer());
	// Loggear Transaccion para su posterior autorizacion
	if(is_false == bIsAuth) wRespCde = LoggTransaction( _DPS_TRAT_PENDIENTE_ );
	// Formar respuesta segun resultado
	dpsMsg.SetRecType( DPS_RECTYPE_CREDITO_RESPUESTA );	
    return (!bIsAuth)
        ? wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED
        : wRespCde = DPS_APROBADA_SIN_RECHAZO;
	//////////////////////////////////////////////////////////////////////////

    
    ///////////////////////////////////////////////////////////////////////////////////
	// Si el SISTEMA AUTORIZADOR DEL BANCO rechazara el crédito, se generará una rta.
	// a la Red, con RECTYPE="TCR", ESTADO="80", RECHCR con el código que se ajuste 
	// al rechazo y CODMAC recalculado (algoritmo MAC2).
	// Si ESTADO="60", corresponde aplicar el crédito. No se genera respuesta a la Red.
    // CONTROL 3 :
	if( strncmp( dpsMsg.input.data.ESTADO,_DPS_EJECUTADA_,2) == 0)
	{
		// Autorizar....
        // No formar respuesta, porque no se envia nada.
        // Retorno del motivo hacia nivel superior de invocacion
		return (wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED);
	}

    ///////////////////////////////////////////////////////////////////////////////////
	// Si ESTADO="70", es un mensaje informativo. No se aplica ni se genera respuesta. 
    // CONTROL 4 :
	if( strncmp( dpsMsg.input.data.ESTADO,_DPS_RECHAZO_DEL_BANCO_DE_DEBITO_,2) == 0)
	{
		// No formar respuesta, porque no se envia nada.
        // Retorno del motivo hacia nivel superior de invocacion
		return (wRespCde = DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED);
	}

	// Caso contrario...
    // Retorno del motivo hacia nivel superior de invocacion
	return (wRespCde);
}

///////////////////////////////////////////////////////////////////////
// Estado de la ultima ejecucion RPC/ODBC
boolean_t TrxResDATANET::TransactionExecutionStatus()
{
	return ( bExecStatus_ok );
}


///////////////////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::CalculateMAC(char  *szMACTYPE, char*szExtMAC) // Calcular MAC-n
{
	////////////////////////////////////////////////////
	// A implementar en subclase, con acceso a b.datos
	////////////////////////////////////////////////////
	return ( dpsMsg.IsDPSCreditTransfer() )
					  ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					  : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
}

///////////////////////////////////////////////////////////////////////////
// Calculo de MAC-1-2-3 segun clave ya obtenida de BANCO y USUARIO-ABONADO
///////////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::CalculateMAC(char *szMACTYPE, char *szOutMAC, // Calcular MAC-2 o 3
                                  UCHAR uchAssocKEY[], UCHAR uchBankKEY[])
{
	CHAR  szMAC[DNET_MAC_LENGTH+1] = {0},    // MAC a verificar/calcular
		  szCodAbonado[10]     = {"0000000"},// Abonado Default PRUEBA-EDS
		  szCodBanco[10]       = {"000"},    // Banco Default PRUEBA-EDS
		  szMACinputNDES[1024] = {0}; // Parametros de entrada a la rutina NDES
    CONST SHORT bFromDLL   = _NDES_FROM_LIBRARY_ ;
	SHORT		shMAC      = _NDES_OPER_MAC_EXT_;       // Default operacion 7
	SHORT		shShifts   = _NDES_SHIFTS_COUNT_;       // Default 2 shifts
	SHORT		shBlocks   = sizeof(szMACinputNDES) / 8;// Default 128 bloques	
	HINSTANCE   hModule    = LoadLibrary( "DNLIB32.DLL" );		
	DNDESPROC   fProc      = (DNDESPROC)GetProcAddress( hModule, "DNDES" );
	CHAR        *achResult = NULL;
    short       shMACType  = 0;

	FILE		*hFile = fopen("debug.mac", "at");

		// Precondicion
	if( NULL == szMACTYPE || NULL == hModule || NULL == fProc || 
        NULL == uchAssocKEY || NULL == uchBankKEY)
		return DPS_FUERZA_MAYOR;
	// Pre-Validar datos de la transaccion, 0 por OK
	if( !dpsMsg.IsDPSTransaction() ) 
		return DPS_OPERACION_INEXISTENTE;	
	// MAC 2 o 3 en binario
    shMACType = szMACTYPE[0] - '0';
	if(shMACType != 2 && shMACType != 3)
		return DPS_MAC_ERRONEO;

	// Recuperar MAC
	dpsMsg.GetMAC( szMAC );
	// Formatear los parametros para calcular la MAC-x ("Calculo")
	dpsMsg.GetMACTextToNDES(szMACinputNDES, sizeof(szMACinputNDES), 
                            shMACType , &shBlocks , 'C' );
    ////////////////////////////////////////////////////////////////////////////
	// Calcular la MAC 2 o 3 con la funcion DNDES de DATANET/INTERBANKING
    // Función DNDES
    // des = DNDES(texto,clave,operación,shiftkey,bloques)
    // donde:	
    //	 texto: (string) texto para el cálculo del DES
    //	 clave: (string) clave para el cálculo del DES
    //	 operación: (int) operación a realizar
    //				 7 - MAC extendido
    //	 shiftkey: (int) shift sobre la clave
    //				 2 - hace shift sobre la clave
    //	 bloques: (int) cantidad de bloques de texto 
    //				1 bloque = 8 bytes
    //	 des: (string) DES en 12 bytes  
    // Nota: La operación  7 utiliza los 12 primeros bytes del texto.	
	/////////////////////////////////////////////////////////////
	if (hFile)
	{
		fprintf(hFile,
			"\r\n**************************************\r\n"
			"\r\n   ARGUMENTOS ANTES DE CALCULAR MAC-%d\r\n"
			"\r\n**************************************\r\n"			
			"TEXTO [%s]\r\n"
			"TIPO MAC [%d] - CLAVE [%8.8s]\r\n"
			"OPERACION [%i] SHIFTS [%i] BLOQUES [%i]\r\n"
			"\r\n**************************************\r\n"
			,		
			(WORD)shMACType,
			szMACinputNDES, 
			(WORD)shMACType,
			(  1 == shMACType ) 
				? uchAssocKEY 
				: uchBankKEY , 
			shMAC, 
			shShifts, 
			shBlocks );
		fflush(hFile);        
	}
	/////////////////////////////////////////////////////////////


	achResult = (*fProc)(szMACinputNDES, uchBankKEY, shMAC, shShifts, shBlocks);
    ////////////////////////////////////////////////////////////////////////////
	// Copiar MAC de salida
	memcpy(szOutMAC, achResult, DNET_MAC_LENGTH);

	/////////////////////////////////////////////////////////////
	if (hFile)
	{
		fprintf(hFile,"\r\nVALOR MAC :"
			"[%12.12s]\r\n"
			"\r\n**************************************\r\n\r\n"
			,
			achResult);
	    
		fclose(hFile);        
	}
	
	// OK
	return DPS_APROBADA_SIN_RECHAZO;
}
///////////////////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::ValidateMAC(char  *szMACTYPE) // Verificacion de MAC-n
{
	////////////////////////////////////////////////////
	// A implementar en subclase, con acceso a b.datos
	////////////////////////////////////////////////////
	return ( dpsMsg.IsDPSCreditTransfer() )
					  ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					  : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
}

///////////////////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::ValidateMAC(char  *szMACTYPE , // Verificacion cuenta y MAC-n
                                 UCHAR uchAssocKEY[], 
                                 UCHAR uchBankKEY[] )
///////////////////////////////////////////////////////////////////////////////////
{
	CHAR  szMAC[DNET_MAC_LENGTH+1] = {0},        // MAC a verificar
		  szCodAbonado[10]         = {"0000000"},// Abonado Default PRUEBA-EDS
		  szCodBanco[10]           = {"000"},    // Banco Default PRUEBA-EDS
		  szMACinputNDES[1024]     = {0};        // Parametros de entrada a la rutina NDES
    CONST SHORT bFromDLL   = _NDES_FROM_LIBRARY_ ;
	SHORT		shMAC      = _NDES_OPER_MAC_EXT_;       // Default operacion 7
	SHORT		shShifts   = _NDES_SHIFTS_COUNT_;       // Default 2 shifts
	SHORT		shBlocks   = sizeof(szMACinputNDES) / 8;// Default 128 bloques	
	HINSTANCE   hModule    = LoadLibrary( "DNLIB32.DLL" );		
	DNDESPROC   fProc      = (DNDESPROC)GetProcAddress( hModule, "DNDES" );
	CHAR        *achResult = NULL;
    BOOL        bValidateMAC = _DPS_CONTROL_MAC_123_;
	FILE		*hFile = fopen("debug.mac", "at");
    BYTE        nMACTYPE_BATCH = 0,
                nMACTYPE_BIN   = 0;


	// Precondicion : punteros de parametros 
	if( NULL == szMACTYPE   || NULL == uchAssocKEY || NULL == uchAssocKEY  )
		return DPS_FUERZA_MAYOR;
    // Precondicion : FUNCION DE VALIDACION MAC CARGADA
	if( NULL == fProc && _NDES_FROM_LIBRARY_ == bFromDLL  )
		return DPS_FUERZA_MAYOR;
	// Pre-Validar datos de la transaccion, 0 por OK
	if( !dpsMsg.IsDPSTransaction() ) 
		return DPS_OPERACION_INEXISTENTE;	
#ifdef _VERIFY_MSG_TYPE_IN_MAC
	if( !dpsMsg.IsDPSTransaction() ) 
		return DPS_OPERACION_INEXISTENTE;	
#endif 
	// MAC 1, 2 o 3
	if(atoi(szMACTYPE) != 1 && atoi(szMACTYPE) != 2 && atoi(szMACTYPE) != 3)
		return DPS_MAC_ERRONEO;


    nMACTYPE_BATCH = atoi(szMACTYPE);
    nMACTYPE_BIN   = atoi(szMACTYPE);
#if ( defined( _SYSTEM_DATANET_BATCH_ ) )
    if( atoi(szMACTYPE) == 2 )
    {
        // MAC-2 en BATCH implica MAC-1 en ONLINE
        nMACTYPE_BATCH = 2;
        nMACTYPE_BIN   = 2; /* 1 */
    };
#endif // _SYSTEM_DATANET_BATCH_


    // Recuperar MAC
	dpsMsg.GetMAC( szMAC );
	// Formatear los parametros para validar la MAC-x ("Validacion")
	dpsMsg.GetMACTextToNDES( szMACinputNDES, sizeof(szMACinputNDES), 
            nMACTYPE_BIN , &shBlocks , 'V' );
    /////////////////////////////////////////////////////////////////////////////////
	// Validar la MAC
	// Calcular la MAC 2 o 3 con la funcion DNDES de DATANET/INTERBANKING
    // Función DNDES
    // des = DNDES(texto,clave,operación,shiftkey,bloques)
    // donde:	
    //	 texto: (string) texto para el cálculo del DES
    //	 clave: (string) clave para el cálculo del DES
    //	 operación: (int) operación a realizar
    //				 7 - MAC extendido
    //	 shiftkey: (int) shift sobre la clave
    //				 2 - hace shift sobre la clave
    //	 bloques: (int) cantidad de bloques de texto 
    //				1 bloque = 8 bytes
    //	 des: (string) DES en 12 bytes  
    // Nota: La operación  7 utiliza los 12 primeros bytes del texto.	

	/////////////////////////////////////////////////////////////
	if (hFile)
	{
		fprintf(hFile,
			"\r\n**************************************\r\n"
			"\r\n   ARGUMENTOS ANTES DE CALCULAR MAC-%d\r\n"
			"\r\n**************************************\r\n"			
			"TEXTO [%s]\r\n"
			"TIPO MAC [%d] - CLAVE [%8.8s]\r\n"
			"OPERACION [%i] SHIFTS [%i] BLOQUES [%i]\r\n"
			"\r\n**************************************\r\n"
			,		
			(WORD)nMACTYPE_BATCH,
			szMACinputNDES, 
			(WORD)nMACTYPE_BATCH,
			(  1 == nMACTYPE_BIN ) 
				? uchAssocKEY 
				: uchBankKEY , 
			shMAC, 
			shShifts, 
			shBlocks );
		fflush(hFile);        
	}
	/////////////////////////////////////////////////////////////

    if( 1 == nMACTYPE_BIN )
	    achResult = (*fProc)(szMACinputNDES, uchAssocKEY, shMAC, shShifts, shBlocks);
    else
        achResult = (*fProc)(szMACinputNDES, uchBankKEY, shMAC, shShifts, shBlocks);
    /////////////////////////////////////////////////////////////////////////////////
    // Las MAC son diferentes y la validacion esta activada ?
	if(strcmp(achResult,szMAC)!=0 && bValidateMAC )
	{
		/////////////////////////////////////////////////////////////
		if (hFile)
		{
			fprintf(hFile,"\r\nRESULTADO INVALIDO DE COMPARAR LAS MAC :"
				"[%12.12s] vs. [%12.12s]\r\n"
				"\r\n**************************************\r\n\r\n"
				,
				achResult,szMAC);
		    fclose(hFile);        
		}
		/////////////////////////////////////////////////////////////
		return DPS_MAC_ERRONEO;
	}
	else
	{
		/////////////////////////////////////////////////////////////
		if (hFile)
		{
			fprintf(hFile,"\r\nVALIDACION OK DE LA MAC\r\n"				
				"\r\n**************************************\r\n\r\n");
		    fclose(hFile);        
		}
		/////////////////////////////////////////////////////////////		
		// OK
		return DPS_APROBADA_SIN_RECHAZO;
	};    
}
///////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::ValidateMAC1()              // Verificacion cuenta y MAC1
{
	return ValidateMAC( "1" );
}
///////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::ValidateMAC2()              // Verificacion cuenta y MAC2
{
	return ValidateMAC( "2" );
}
///////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::ValidateMAC3()              // Verificacion cuenta y MAC3
{
	return ValidateMAC( "3" );
}
///////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::ValidateExistingAccount()   // Verificar cuenta existente
{
	////////////////////////////////////////////////////
	// A implementar en subclase, con acceso a b.datos
	////////////////////////////////////////////////////
	return ( dpsMsg.IsDPSCreditTransfer() )
					  ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					  : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
}
///////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::ValidateAccountStatus(char *pszEstado)  // Verificar estado de la cuenta
{
	////////////////////////////////////////////////////////////
	// A implementar en subclase, con acceso a b.datos
	////////////////////////////////////////////////////////////
	// Por omision, solo verifica contenidos del campo nro. cuenta
	CHAR  szNroCuenta[sizeof(dpsMsg.input.data.DATDEB.NUMCTA)+1] = {0},// Cuenta Nro.
		  szEstadoCuenta[5] = {0};// Estado Cta.
	// Pre-Validar datos de la transaccion, 0 por OK
	if( !dpsMsg.IsDPSTransaction() ) 
		return DPS_OPERACION_INEXISTENTE;	
	// Verificacion de campos                    ....:....1....:....2
	if(strncmp( dpsMsg.input.data.DATDEB.NUMCTA,"                    ",
		sizeof(dpsMsg.input.data.DATDEB.NUMCTA))==0)
		return DPS_FALTAN_DATOS_EN_DETALLE_OPERACION;
	// Verificacion de campos                    ....:....1....:....2
	if(strncmp( dpsMsg.input.data.DATCRE.CTACRE,"                    ",
		sizeof(dpsMsg.input.data.DATCRE.CTACRE))==0)
		return DPS_FALTAN_DATOS_EN_DETALLE_OPERACION;
	// OK
	return DPS_APROBADA_SIN_RECHAZO;
}
///////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::ValidateAccountAssociate()  // Verificar abonado de la cuenta
{
	////////////////////////////////////////////////////////////
	// A implementar en subclase, con acceso a b.datos
	////////////////////////////////////////////////////////////
	return ( dpsMsg.IsDPSCreditTransfer() )
					  ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					  : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
}

//////////////////////////////////////////////////////////////////////////////////
// Verificar si la fecha de posting es igual a hoy, sino postergar transaccion
// Contemplar que si es CREDITO y el banco debito ya aplico el mismo, la respuesta
// no puede diferirse sino que debe tratarse en el mismo dia del banco debito.
// RcToday  : FECHA_SOLICITUD versus SYSTEM TIME
// RcStatus : FECHA_SOLICITUD versus FECHA_ESTADO
//////////////////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::ValidatePostingDate(e_settlement_retcode *pesRcToday,
                                         e_settlement_retcode *pesRcStatus,
                                         boolean_t bValidate_CR_DB)   
{
	time_t tSystemTime   = time(NULL);               // System Time, in seconds since 1-Jan-70
	tm     *ptmLocalTime = localtime(&tSystemTime);  // Local System Time (GMT - h)
	CHAR   szDATE[10+1]  = {"20011231"};             // Posting Date "YYYYMMDD"
    WORD   wRespCde      = DPS_APROBADA_SIN_RECHAZO; // Retorno a instancia superior
    short  nOffset       = 0;                        // Offset de fechas
    // Controles ENABLE/DISABLE via archivo CFG
	BOOL   bCONTROL_ENABLED = dpsMsg.bCONTROL_POSTING_DATE ;

    // Precondicion
    if(NULL == pesRcToday || NULL == pesRcStatus)
        return (wRespCde = DPS_FUERZA_MAYOR );	

    // Pre-Validar datos de la transaccion, 0 por OK . Solo debitos a menos 
    // que se indique lo contrario !!!!
	if( !dpsMsg.IsDPSDebitTransfer() && is_false == bValidate_CR_DB) 
		return (wRespCde = DPS_APROBADA_SIN_RECHAZO);	

	// CONTROL HABILITADO ?
	if(!bCONTROL_ENABLED)
		return DPS_APROBADA_SIN_RECHAZO;


	// Formateo YYYYMMDD para fecha
	strftime(szDATE, sizeof szDATE, "%Y%m%d", ptmLocalTime);
    ///////////////////////////////////////////////////////////////////////////////////////
	// Pre-Verificacion de campo FECHA-SOLICITUD
	if(strncmp( dpsMsg.input.data.FECSOL,"        ", sizeof(dpsMsg.input.data.FECSOL))==0
        ||
	   strncmp( dpsMsg.input.data.FECSOL,"00000000", sizeof(dpsMsg.input.data.FECSOL))==0 )
    {
        (*pesRcToday) = ESTL_ERROR_DEL_SISTEMA;
		return (wRespCde = DPS_FALTAN_DATOS_EN_DETALLE_OPERACION);
    }

	///////////////////////////////////////////////////////////////////////////////////////
    // Pre-Verificacion de campo FECHA-ENVIO-DEBITO y copiar fecha de hoy si es necesario
	if( strncmp( dpsMsg.input.data.DATDEB.FSENDB,"        ",
                 sizeof(dpsMsg.input.data.DATDEB.FSENDB))==0 
        ||
	    strncmp( dpsMsg.input.data.DATDEB.FSENDB,"00000000",
                 sizeof(dpsMsg.input.data.DATDEB.FSENDB))==0 )
    {
        // Desplazamiento de N caracteres segun año de 4 o 2 digitos = {0,2} 
        nOffset = sizeof(dpsMsg.input.data.FECSOL) - sizeof(dpsMsg.input.data.DATDEB.FSENDB);
    	strncpy( dpsMsg.input.data.DATDEB.FSENDB, szDATE + nOffset,
            sizeof( dpsMsg.input.data.DATDEB.FSENDB) );
    }
    
    /////////////////////////////////////////////////////////////////////////////////////
	// Verificacion de campos con respecto a la fecha del dia
    // FECHA-SOLICITUD IGUAL A HOY
	if(strncmp( dpsMsg.input.data.FECSOL, szDATE,sizeof(dpsMsg.input.data.FECSOL))==0)
    {
        (*pesRcToday) = ESTL_TRANSFERENCIA_OK;
		wRespCde = DPS_APROBADA_SIN_RECHAZO;
    }
    // FECHA-SOLICITUD MAYOR A HOY , DIFERIDA
	else if( strncmp( dpsMsg.input.data.FECSOL, szDATE, sizeof(dpsMsg.input.data.FECSOL))>0)
    {        
        //////////////////////////////////////////////////////////////////////////
        // Pero si el banco debito ya la trato, no debe demorarse la respuesta 
        // sino que debe tratarse como si fuera del dia, y responderse.
        if(strncmp( dpsMsg.input.data.DATDEB.FSENDB, szDATE, 
            sizeof(dpsMsg.input.data.DATDEB.FSENDB))==0)
        {
            (*pesRcToday) = ESTL_TRANSFERENCIA_OK;    
        }
        else 
        {
            (*pesRcToday) = ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA;
        }
        //////////////////////////////////////////////////////////////////////////
		wRespCde = DPS_APROBADA_SIN_RECHAZO;
    }
    else
    {
        ///////////////////////////////////////////////////////////////////////////////////////
        // SINO, ES ERROR Y SE RECHAZA	
        (*pesRcToday) = ESTL_FECHA_DE_SOLICITUD_MENOR_A_FECHA_DEL_DIA;
	    wRespCde = DPS_APLICACION_NO_DISPONIBLE;
    }; // end-if-strncmp()

    // Verificar si se retorna OK u ERROR antes de continuar con la siguiente fecha
    if( DPS_APROBADA_SIN_RECHAZO != wRespCde )
        return (wRespCde);

    /////////////////////////////////////////////////////////////////////////////////////
	// Verificacion de campos con respecto a la fecha del ESTADO
    // FECHA-SOLICITUD IGUAL A FECHA DEL ESTADO
    nOffset = sizeof(dpsMsg.input.data.FECSOL) - sizeof (dpsMsg.input.data.FECEST) ;
	if(strncmp( dpsMsg.input.data.FECSOL+nOffset, 
                dpsMsg.input.data.FECEST,
                sizeof(dpsMsg.input.data.FECEST)) == 0 )
    {
        (*pesRcStatus) = ESTL_TRANSFERENCIA_OK;
		wRespCde = DPS_APROBADA_SIN_RECHAZO;
    }
    // FECHA-SOLICITUD MAYOR A LA DEL ESTADO , DIFERIDA
	else if( strncmp( dpsMsg.input.data.FECSOL+nOffset, 
                      dpsMsg.input.data.FECEST,
                      sizeof(dpsMsg.input.data.FECEST))>0)
    {        
        (*pesRcStatus) = ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA;
		wRespCde = DPS_APROBADA_SIN_RECHAZO;
    }
    else
    {
        ///////////////////////////////////////////////////////////////////////////////////////
        // SINO, ES ERROR Y SE RECHAZA	
        (*pesRcStatus) = ESTL_FECHA_DE_SOLICITUD_MENOR_A_FECHA_DEL_DIA;
	    wRespCde = DPS_APLICACION_NO_DISPONIBLE;
    }; // end-if-strncmp()

    // Retorno OK u ERROR
    return (wRespCde);

}

//////////////////////////////////////////////////////////////////////////////
// Autorizar transaccion
//////////////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::AuthorizeTransaction() // Autorizar trx
{
	////////////////////////////////////////////////////
	// A implementar en subclase, con acceso a b.datos
	////////////////////////////////////////////////////
	return ( dpsMsg.IsDPSCreditTransfer() )
					  ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					  : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
}

//////////////////////////////////////////////////////////////////////////////
void  TrxResDATANET::ResetStatus()
{
}

//////////////////////////////////////////////////////////////////////////////
// Logg transaccion
//////////////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::LoggTransaction(char *szESTADOTRAT , // Loggear trx
                                     char *szPENDENVIO )
{
	////////////////////////////////////////////////////
	// A implementar en subclase, con acceso a b.datos
	////////////////////////////////////////////////////
	return ( dpsMsg.IsDPSCreditTransfer() )
					  ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					  : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
}

//////////////////////////////////////////////////////////////////////////////
// Retrieve transaccion
//////////////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::RetrieveTransaction() // Leer 1er trx
{
	////////////////////////////////////////////////////
	// A implementar en subclase, con acceso a b.datos
	////////////////////////////////////////////////////
	return ( DPS_RECHAZO_DEL_BANCO_DE_DEBITO );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatea el monto agregandole o quitando punto decimal y millares,
// y optativamente aplica el tipo de cambio si correspondiera.
// szAmmount - monto en formato display, a convertir y formatear con decimales
// shDecPos - posiciones decimales a fijar
// szNew - variable de recepcion del monto formateado
// shAlignWidth - cantidad de posiciones a alinear y rellenar
// bApplyChangeRate - aplica tipo de cambio, si o no?
// szExchangeRate - tipo de cambio aplicado
SHORT  _TRX_BASE_RESOLUTION_::Format_ISO_Ammount(const char *szAmmount, short shDecPos, 
                                      char *szNew, short shAlignWidth,
                                      boolean_t bApplyChangeRate ,
                                      char *szExchangeRate)
{
    /******************************************************************/
    /* maximo de longitud a tomar */
    const short wAmmountLen = DPS_MAX_AMMOUNT_LENGTH; 
    /******************************************************************/    
    short  shLen           = 0;       /* long de cadena */
    char   *pszDecPoint    = NULL; /* ptr a pto. decimal en la cadena */
    short  shQPoints       = 0;    /* cantidad de reemplazos efectuados */
    double dblAmmount      = 0.0f; /* monto en binario interno */           
    const char chMilePoint = ',';  /* punto millar buscado */

    /* precondicion */
    if(szAmmount == NULL || szAmmount[0] == 0x00 || szNew == NULL || 
        shAlignWidth < 0 || shDecPos < 0)
        return -1;
    /******************************************************************/
    /* conversion de formato ISO a uso interno con decimales ? */
    /* posicion decimal invalida para poner punto decimal ? */
    if(shDecPos > 0)    
    {
        /******************************************************************/
        /* copia local y longitud de la cadena que representa al MONTO TRX*/    
        strncpy(szNew, szAmmount, wAmmountLen);
        szNew[wAmmountLen] = 0x00 ;
        /* longitud en base al maximo ISO posible */
        shLen = strlen(szNew);
        /******************************************************************/

        /* conversion a ASCII con pto. decimal */
        sprintf(szNew, "%*.*s.%*.*s", 
            shLen - shDecPos, shLen - shDecPos, szAmmount, 
            shDecPos, shDecPos, szAmmount + shLen - shDecPos);                    
        shLen = strlen(szNew);
        /* conversion a binario y verificacion del tipo de cambio si corresponde */
        dblAmmount = atof( szNew );
        sprintf( szNew, "%.2f", dblAmmount ); /* monto original de trx */
        /* retornar conversion con pto. decimal incluido */
        return (shLen);
    }
    /******************************************************************/
    else
    {
        /* copia local y longitud de la cadena que representa al MONTO TRX*/    
        strcpy(szNew, szAmmount );        
        /* longitud */
        shLen = strlen(szNew);        
    };
    /******************************************************************/

    /* de lo contrario, se convierte de uso interno a formato ISO con 2 decimales */
    /* ubicacion del pto. millar */
    for(pszDecPoint = strchr(szNew, (int)chMilePoint),
        shQPoints = 0;
        pszDecPoint != NULL ;                
        pszDecPoint++,
        pszDecPoint = strchr(pszDecPoint, (int)chMilePoint),
        shLen = strlen(szNew) )
        {
            memmove( pszDecPoint,pszDecPoint+1, shLen - (pszDecPoint - szNew) - 1);
            shQPoints++;
        }; /* end for */
    szNew[shLen - shQPoints] = 0x00;
    shLen = strlen(szNew);

    /* conversion a binario y verificacion del tipo de cambio si corresponde */
    dblAmmount = atof( szNew );
    /* reconversion a ASCII con 2 decimales y N digitos representativos */
    if(shAlignWidth > 0)
        /* alineamiento + un espacio para contempletar el pto. decimal que luego desaparece */
        sprintf( szNew, "%0*.2f", shAlignWidth + 1, dblAmmount );
    else
        /* sin necesidad de alineamiento....*/
        sprintf( szNew, "%.2f", dblAmmount );
    /* buscar el pto. decimal y eliminarlo (formato ISO pto. implicito 2 decimales )*/
    pszDecPoint = strchr(szNew, (int)'.');
    if(pszDecPoint)
    {
        shLen = strlen(szNew);
        memmove( pszDecPoint,pszDecPoint+1, shLen - (pszDecPoint - szNew) - 1);
        szNew[shLen - 1] = 0x00;
        shLen = strlen(szNew);
    };    
    /* retornar longitud de nuevo string */
    return shLen;
        
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Verificar si la marca de estado de envio debe ser actualizada a "NO"
// esReqDate : Indicativo de FECHA_SOLICITUD versus SYSTEM TIME
// esSendDate: Indicativo de FECHA_SOLICITUD versus FECHA_ESTADO
// bESTADO_VALOR_AL_COBRO : Indicativo de Valor al Cobro (Estado 40)
// Postcondicion : true, false
//////////////////////////////////////////////////////////////////////////////////
WORD TrxResDATANET::VerifySendFlag(e_settlement_retcode esReqDate,
                                          e_settlement_retcode esSendDate,
                                          boolean_t bESTADO_VALOR_AL_COBRO)   
{
    /////////////////////////////////////////////////////////////////////
    // FEC-SOLICITUD vs FEC-ENVIO : difieren a hoy y respuesta pendiente?
    if( ESTL_TRANSFERENCIA_OK                               == esReqDate
        &&
        ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esSendDate
        &&
        _DPS_PEND_ENV_DEFAULT_C_ == szPENDENVIO[0] 
        &&
        is_true == bESTADO_VALOR_AL_COBRO )
    {      
        szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_; //  Estado de NO ENVIAR RTA.
        return TRUE; // Postcondicion 
    }
    else
        return FALSE; // Postcondicion 
    /////////////////////////////////////////////////////////////////////
}

// Vencio la hora de corte ?
boolean_t TrxResDATANET::IsCutoverTimeElapsed( void ) // Hora de Corte
{
    time_t  tNOW     = time(NULL);          // Current Time
    struct  tm stNOW = *localtime( &tNOW ); // Current TimeStamp

    ///////////////////////////////////////////////
    // ES LA HORA DE CORTE ??????????????????????
    return (stNOW.tm_hour > stCutoverTime.tm_hour       // La hora es MAYOR ?
            ||                                          // o
            (   stNOW.tm_hour == stCutoverTime.tm_hour  // La hora es IGUAL  
                &&                                      // y
                stNOW.tm_min  >  stCutoverTime.tm_min)  // Los minutos son MAYORES
            ||                                          // o
            (   stNOW.tm_hour == stCutoverTime.tm_hour  // La hora es IGUAL
                &&                                      // y
                stNOW.tm_min  == stCutoverTime.tm_min   // Los minutos son IGUALES
                &&                                      // y
                stNOW.tm_sec  >  stCutoverTime.tm_sec)  // Los segundos son MAYORES
           )
        ? is_true
        : is_false ;
    ///////////////////////////////////////////////
}

// Establecer la hora de corte ?
boolean_t TrxResDATANET::SetCutoverTime( struct tm *pstCutoverTime ) // Hora de Corte
{
    if( pstCutoverTime )
        stCutoverTime = (*pstCutoverTime);
    else
        return is_false;
    // Ok
    return is_true;
}

//////////////////////////////////////////////////////////////////////////////
// Autorizar transaccion
//////////////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::AuthorizeTransaction( long *lResult ) // Autorizar trx
{
	////////////////////////////////////////////////////
	// A implementar en subclase, con acceso a b.datos
	////////////////////////////////////////////////////
    *lResult = -1;
	return ( dpsMsg.IsDPSCreditTransfer() )
					  ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					  : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
}

//////////////////////////////////////////////////////////////////////////////

