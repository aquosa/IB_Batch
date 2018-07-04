/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 Sistemas S.A.
// Transaction Base Resolution Class
// Transaction Resolution Class
//
//        Esta es la Clase de Resolucion de Transacciones.
//        Aplicada en principio para PAS, puede soportar las subsiguientes
//  logicas de resolucion de transacciones de futuros proyectos, como
//  ULTMOV, AFJP, MEP, SOAT, etc, ya que simplemente se discriminan
//  por tipo de trx y se aplican distintos metodos con diversos Stored Procedures.
//        La misma se plantea como instancias multiples creadas por un Servidor 
//  de Atencion de Transacciones. Como puede tener multiples transacciones
//  resolviendose, no se plantea como clase estatica, y es por esto mismo
//  que no deben establecerse variables miembro estaticas, sino locales al metodo.
//      El siguiente grafico da una idea de la 'familia' de clases obtenidas:
//
//
//                           TrxBaseResolution                    1) Esta es la base
//                                  |
//                                  |
//               +------------+-----+------+-------------+
//               |            |            |             |
//               |            |            |             |
//           TrxResPAS   TrxResULTMOV   TrxResAFJP   TrxResSOAT   2) Esta la especializacion
//               |            |            |             |
//               |            |            |             |
//               +------------+-----+------+-------------+
//                                  |
//                                  |
//                             TrxResolution                      3) Y este el controlador
//
//   Segun la implementacion, los metodos especializados puede ser "dummy", o sea un 
// simple retorno en falso sin procesamiento, sin generar el codigo ejecutable 
// actualmente,  con el solo motivo de mantener el esqueleto de codigo fuente 
// original y facilitar el posible enlace a futuro de las subclases especializadas 
// en una sola libreria o modulo.
//
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.07.23      mdc     En base a TrxRes de PAS/AFJP/ULTMOV
// (Inicial)    1999.07.08      mdc     Assume time-out and retry (ConnectDataBase(...))
// (Beta)       2001.12.31      mdc     ATM
// (Beta)       2002.01.25      mdc     BOOL TrxBaseResolution::ConnectDataBase(PSTR pszServer,PSTR pszDBase,PSTR pszUser);
// (Beta)       2002.02.18      mdc     TrxBaseResolution::SetOnOffLine(boolean_t boolXOnline)
// (Beta)       2002.11.22      mdc     wAuthRespCde   = TCP4U_SUCCESS;       // OK por default
// (Beta)       2003.05.20      mdc     BOOL TrxBaseResolution::FormatTrxDateTime_WMask(char *szYYYYMMDD, ...);
// (Beta)       2004.01.05      mdc     Cuando el MES de fecha de negocios es MENOR al de fecha de transaccion, 
//                                      debe sumarse 1 al año actual.
// (1.0.1.3)    2004.06.07      mdc     shAuthRespCde inicializado en 0
// (1.0.2.0)    2005.07.28      mdc     BANELCO, Base24 Release 6. Tambien agrega campo ISO #2.
// (1.0.2.1a)   2005.08.25      mdc     OPERATION CODE x PRODUCTO.
// (1.0.2.1b)   2005.08.31      mdc     TIPO DE CAMBIO US$ DESDE CAMPO 126 - CAMPO 127
// (1.0.2.1c)   2006.03.01      mdc     ::TranslateTranCode_ISO_B24() ... case B24_OPCODE_CREDIT_ORDER_XFER	: shTran = CEXTR_OPCODE_CREDIT_ORDER_XFER;
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Header Type Definitions
#include <qusrinc/typedefs.h>    
// Headers ANSI 
#include <time.h>    
#include <stdlib.h>    
#include <ctype.h>    
// Headers propios
#include <qusrinc/trxres.h>
#include <qusrinc/stdiol.h>
#include <qusrinc/extrctst.h>


// Macro de asignacion de String, con NULL al final
#define COPYSTRING(dst,src) strcpy(dst,src) 
// Macro de asignacion de String, con NULL al final
#define ASSIGNSTRING(dst,src) \
    { strncpy(dst,src,sizeof(src)); dst[sizeof(src)]=0x00; }; 
// Macro de asignacion de bytes
#define ASSIGNFIELD(dst,src) \
    { memset(dst,' ',sizeof(dst)); memcpy(dst,src,min(sizeof(src),sizeof(dst)));  }; 

///////////////////////////////////////////////////////////////////////////////////
// TrxBaseResolution
///////////////////////////////////////////////////////////////////////////////////

// Clase Resolucion Base de Transaccion
// Constructor default
EXPORT TrxBaseResolution::TrxBaseResolution(void)
    {
    boolOnlineMode = is_true;    // En linea por default
    /////////////////////////////////////////////////////////////////////////
    bProcResult    = is_false;    // Sin resultado de procesamiento por default
    wProcRespCde   = CISO_SYSTEM_ERROR_I; // Desaprobada por default
    shAuthRespCde  = 0;                   // OK por default
    efFormatMsg    = CNTL_FORMAT_ISO8583; // ISO8583 por default
    srand( time( NULL ) );        // Inicializar criba de pseudorandoms
    /////////////////////////////////////////////////////////////////////////

    // Inicializacion: Bitmap strings parametricas?    
    isoMsg.CheckExternParameters();

    }

// Constructor opcional
EXPORT TrxBaseResolution::TrxBaseResolution(PSTR pszDBase,PSTR pszUser)
    {
    // Initialization    
    boolOnlineMode = is_true;    // En linea por default
    /////////////////////////////////////////////////////////////////////////
    bProcResult    = is_false;    // Sin resultado de procesamiento por default
    wProcRespCde   = CISO_SYSTEM_ERROR_I; // Desaprobada por default
    shAuthRespCde  = 0;                   // OK por default
    efFormatMsg    = CNTL_FORMAT_ISO8583; // ISO8583 por default
    srand( time( NULL ) );        // Inicializar criba de pseudorandoms
    /////////////////////////////////////////////////////////////////////////

    // Inicializacion: Bitmap strings parametricas?    
    isoMsg.CheckExternParameters();

    }

EXPORT TrxBaseResolution::~TrxBaseResolution(void)
    {    
    // Sin formato de mensaje al finalizar
    efFormatMsg = CNTL_FORMAT_NULL; 
    // Offline al finalizar
    boolOnlineMode = is_false;    
    }

BOOL EXPORT TrxBaseResolution::SetOnOffLine(boolean_t boolXOnline)
    {
    // Senal de modo ON u OFF que implica informar fechas en forma distinta segun corresponda
    boolOnlineMode = boolXOnline;
    return TRUE;
    }

////////////////////////////////////////////////////////////////////////////
// Para loggear errores internos, se disponibilizan estos metodos publicas:
WORD EXPORT TrxBaseResolution::TransactionStatus(void)
    {
#if FALSE
    // Ok?
    return ( bProcResult ) 
        ? wProcRespCde
        : 0;
#else
    // Ok? Error?
    if ( bProcResult ) 
		{
        return ( wProcRespCde );
		}
	else
		{
        return( wProcRespCde >= 00 && wProcRespCde <= 99 )
			? wProcRespCde
			: (wProcRespCde = CISO_EXTERNAL_DECLINE_I);
		}
#endif
    }

////////////////////////////////////////////////////////////////////////////
// Para loggear errores internos, se disponibilizan estos metodos publicas:
short EXPORT TrxBaseResolution::BackEndAuthStatus(void)
    {
    // Ok? {-32768....+32767}
    return shAuthRespCde;
    }

////////////////////////////////////////////////////////////////////////////
// Para verificar tipo de mensajes ISO para enviar a SAF
BOOL EXPORT TrxBaseResolution::IsValidSAFTransaction(void)
    {
    // Las transacciones validas para enviar a SAF son aquellas notificaciones 
    // (ISO 220) y reversos (ISO 420) o forzados de los mismos. El resto no se
    // guarda en el SAF local, sino en el SAF del switch financiero.
    if(  isoMsgBkp.IsFinancialAdvice() ||     // Forzados
         isoMsgBkp.IsValidReversal() )        // Reversos
         return TRUE;
    else
        return FALSE;

    }

///////////////////////////////////////////////////////////////////////////////////
// TrxResolution
///////////////////////////////////////////////////////////////////////////////////

// Verificacion de TrxAdvice/SAFAdvice (0220)
BOOL EXPORT TrxResolution::IsTrxAdvice(void)
    {
#if ( _ISO8583_FORMAT_TO_USE_ & _ISO8583_FORMAT_FOR_REDLINK_ )
    // Actualmente solo se da en el Sistema PAS
    return isoMsg.IsPASPayment();
#endif
	return isoMsgBkp.IsFinancialAdvice() ;
    }

// Verificacion de TrxAdviceResp/SAFAdviceResp (0230)
BOOL EXPORT TrxResolution::IsTrxAdviceResponse(void)
    {
#if ( _ISO8583_FORMAT_TO_USE_ & _ISO8583_FORMAT_FOR_REDLINK_ )
    // Actualmente solo se da en el Sistema PAS
    return isoMsg.IsPASPaymentResponse();
#endif
	return false;
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatea el monto agregandole o quitando punto decimal y millares,
// y optativamente aplica el tipo de cambio si correspondiera.
// szAmmount - monto en formato display, a convertir y formatear con decimales
// shDecPos - posiciones decimales a fijar
// szNew - variable de recepcion del monto formateado
// shAlignWidth - cantidad de posiciones a alinear y rellenar
// bApplyChangeRate - aplica tipo de cambio, si o no?
// szExchangeRate - tipo de cambio aplicado
SHORT TrxBaseResolution::Format_ISO_Ammount(const char *szAmmount, short shDecPos, 
                                      char *szNew, short shAlignWidth,
                                      boolean_t bApplyChangeRate ,
                                      char *szExchangeRate)
{
    /******************************************************************/
    /* maximo de longitud a tomar */
    const short cwAmmountLen     = sizeof(isoFields.field4.chAmmount); 
	const short cwIntegerDigits  = 8;       /* Might be 8 or 9 */	
	const short cwDecimalDigits  = 2;       /* Might be 3 or 2 */	
    /******************************************************************/    
    short  shLen           = 0;       /* long de cadena */
    char   *pszDecPoint    = NULL; /* ptr a pto. decimal en la cadena */
    short  shQPoints       = 0;    /* cantidad de reemplazos efectuados */
    double dblAmmount      = 0.0f; /* monto en binario interno */           
    const char chMilePoint = ',';  /* punto millar buscado */
    double dblCnvRate      = 0.0f, /* tasa de cambio-conversion DOLARES-PESOS-etc */
           dblAmmountCnv   = 0.0f; /* monto convertido a tasa de cambio */    
    /* Recuperar en forma binaria los tipos de cuenta DESDE y HACIA */
    int iFromAccType = antoi(isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ,2) ;
    int iToAccType   = antoi(isoFields.field3.uProcCde.stTranCde.chTo_Acct_Typ  ,2) ;
	boolean_t        bAvailExchgRate = is_false;
	/*********************************************************************************/

    /* precondicion */
    if(szAmmount == NULL || szAmmount[0] == 0x00 || szNew == NULL || 
        shAlignWidth < 0 || shDecPos < 0)
		/* error */
        return -1;

	/**********************************************************************/
	/* precondicion :tipos de cambio en 2 posibles campos : unificar en 1 */
	if( isoMsg.IsValidField( 127 ) )
		 bAvailExchgRate = is_true;  /* ACI Base24 R4 : sin problemas */
	else
		 bAvailExchgRate = is_false; 
	/**********************************************************************/


    /******************************************************************/
    /* conversion de formato ISO a uso interno con decimales ? */
    /* posicion decimal invalida para poner punto decimal ? */
    if(shDecPos > 0)    
    {
        /******************************************************************/
        /* copia local y longitud de la cadena que representa al MONTO TRX*/    
        strncpy(szNew, szAmmount, cwAmmountLen);
        szNew[cwAmmountLen] = 0x00 ;
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
        dblCnvRate = 1.0f; /* default : NO hay conversion a ningun tipo de cambio */ 
        /* si el tipo de cuenta difiere de la moneda de la transaccion */
        if(_B24_CURRENCY_ARGENTINA_ == antoi(isoFields.field49.chData,3) 
            && 
            bApplyChangeRate == is_true)
        {
            /* En funcion del tipo de cuenta, establecer la tasa de cambio */
            if((_B24_SAVINGS_ACCOUNT_USD_    == iFromAccType ||            
                _B24_CHECKINGS_ACCOUNT_USD_  == iFromAccType )  &&				
                antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C,
					cwIntegerDigits,cwDecimalDigits) > 0.0f)
            {
                dblCnvRate = antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C,
								cwIntegerDigits, cwDecimalDigits);
                isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='2';
                /* copio el tipo de cambio aplicado */
                if(szExchangeRate)
                    sprintf(szExchangeRate,"%*.*f", cwIntegerDigits,cwDecimalDigits,dblCnvRate);
                /* aplicacion del tipo de cambio SI CORRESPONDIERA */
                if(dblCnvRate != 0.0f && dblCnvRate != 1.0f)
                    dblAmmountCnv = dblAmmount * dblCnvRate;
            }
            else 
            if((_B24_SAVINGS_ACCOUNT_USD_    == iToAccType ||            
                _B24_CHECKINGS_ACCOUNT_USD_  == iToAccType )    &&
                antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V,
					cwIntegerDigits,cwDecimalDigits) >0.0f)
            {
                dblCnvRate = antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V,
									cwIntegerDigits,cwDecimalDigits);
                isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='1';
                /* copio el tipo de cambio aplicado */
                if(szExchangeRate)
                    sprintf(szExchangeRate,"%*.*f", cwIntegerDigits,cwDecimalDigits,dblCnvRate);
                /* aplicacion del tipo de cambio SI CORRESPONDIERA */
                if(dblCnvRate != 0.0f && dblCnvRate != 1.0f)
                    dblAmmountCnv = dblAmmount / dblCnvRate;
            }
            else
            {
                /* aplicacion del tipo de cambio SI CORRESPONDIERA */
                if(dblCnvRate != 0.0f && dblCnvRate != 1.0f)
                    dblAmmountCnv = dblAmmount * dblCnvRate;        
                else
                    dblAmmountCnv = dblAmmount;
            }
        }
        else if(_B24_CURRENCY_USA_ == antoi(isoFields.field49.chData,3) 
                && 
                bApplyChangeRate == is_true)
        {        
            /* En funcion del tipo de cuenta, establecer la tasa de cambio */
            if((_B24_SAVINGS_ACCOUNT_         == iFromAccType ||            
                _B24_CHECKINGS_ACCOUNT_       == iFromAccType )    &&
                antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V,
					cwIntegerDigits,cwDecimalDigits) >0.0f)
            {
                dblCnvRate = antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V,
									cwIntegerDigits,cwDecimalDigits);
                isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='1';
                /* copio el tipo de cambio aplicado */
                if(szExchangeRate)
                    sprintf(szExchangeRate,"%*.*f", cwIntegerDigits,cwDecimalDigits,dblCnvRate);
                /* aplicacion del tipo de cambio SI CORRESPONDIERA */
                if(dblCnvRate != 0.0f && dblCnvRate != 1.0f)
                    dblAmmountCnv = dblAmmount / dblCnvRate;

            }
            else if((_B24_SAVINGS_ACCOUNT_    == iToAccType ||            
                _B24_CHECKINGS_ACCOUNT_        == iToAccType ) &&
                antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C,
					cwIntegerDigits,cwDecimalDigits) >0.0f)
            {
                dblCnvRate = antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C,
									cwIntegerDigits,cwDecimalDigits);
                isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='2';
                /* copio el tipo de cambio aplicado */
                if(szExchangeRate)
                    sprintf(szExchangeRate,"%8.*f", cwDecimalDigits, dblCnvRate);
                /* aplicacion del tipo de cambio SI CORRESPONDIERA */
                if(dblCnvRate != 0.0f && dblCnvRate != 1.0f)
                    dblAmmountCnv = dblAmmount * dblCnvRate;

            }
            else
            {
                /* aplicacion del tipo de cambio SI CORRESPONDIERA */
                if(dblCnvRate != 0.0f && dblCnvRate != 1.0f)
                    dblAmmountCnv = dblAmmount / dblCnvRate;
                else
                    dblAmmountCnv = dblAmmount;
            };
        }
        else if((_B24_CURRENCY_BRAZIL_ == antoi(isoFields.field49.chData,3) ||
                _B24_CURRENCY_URUGUAY_ == antoi(isoFields.field49.chData,3)) && 
                bApplyChangeRate == is_true)
        {        
            /* En funcion del tipo de cuenta, establecer la tasa de cambio */
            if((_B24_SAVINGS_ACCOUNT_         == iFromAccType ||            
                _B24_CHECKINGS_ACCOUNT_       == iFromAccType )    &&
                antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1,
					cwIntegerDigits,cwDecimalDigits) >0.0f)
            {
                dblCnvRate = antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1,
									cwIntegerDigits,cwDecimalDigits);
                isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='3';
                /* copio el tipo de cambio aplicado */
                if(szExchangeRate)
                    sprintf(szExchangeRate,"%*.*f", cwIntegerDigits,cwDecimalDigits,dblCnvRate);
                /* aplicacion del tipo de cambio SI CORRESPONDIERA */
                if(dblCnvRate != 0.0f && dblCnvRate != 1.0f)
                    dblAmmountCnv = dblAmmount / dblCnvRate;

            }
            else if((_B24_SAVINGS_ACCOUNT_    == iToAccType ||            
                _B24_CHECKINGS_ACCOUNT_        == iToAccType ) &&
                antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2,
						cwIntegerDigits,cwDecimalDigits) >0.0f)
            {
                dblCnvRate = antof(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2,
									cwIntegerDigits,cwDecimalDigits);
                isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='4';
                /* copio el tipo de cambio aplicado */
                if(szExchangeRate)
                    sprintf(szExchangeRate,"%*.*f",cwIntegerDigits,cwDecimalDigits,dblCnvRate);
                /* aplicacion del tipo de cambio SI CORRESPONDIERA */
                if(dblCnvRate != 0.0f && dblCnvRate != 1.0f)
                    dblAmmountCnv = dblAmmount * dblCnvRate;

            }
            else
            {
                /* aplicacion del tipo de cambio SI CORRESPONDIERA */
                if(dblCnvRate != 0.0f && dblCnvRate != 1.0f)
                    dblAmmountCnv = dblAmmount / dblCnvRate;
                else
                    dblAmmountCnv = dblAmmount;
            };
        };
        /* El monto de la transaccion ya viene expresada en la moneda de la cuenta ORIGEN */
        /* a menos que sea un DEPOSITO. Por lo que no es necesaria la conversion con tasa */
        /* de cambio, sino solo a titulo verificador, EXCEPTO para DEPOSITOS.             */
        /* Conversion a ASCII sin necesidad de alineamiento */
        if(dblAmmountCnv == dblAmmount)        
            /* monto convertido es identico a monto de transaccion */
            sprintf( szNew, "%.2f", dblAmmountCnv ); 
        /* excepto para depositos en efectivo */
        else if ((dblAmmountCnv != dblAmmount)          &&
                 (0 == iFromAccType )                   &&
                 (_B24_SAVINGS_ACCOUNT_   == iToAccType ||            
                  _B24_CHECKINGS_ACCOUNT_ == iToAccType ))
        {
            /* monto convertido con tipo de cambio, para DEPOSITOS*/
            if(dblAmmountCnv != 0.0f)
                sprintf( szNew, "%.2f", dblAmmountCnv ); 
            else
                sprintf( szNew, "%.2f", dblAmmount ); 
        }
        else if (dblAmmountCnv != dblAmmount)
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
// Traduce el codigo de transaccion de ISO hacia Base24 interno
char *TrxBaseResolution::TranslateTranCode_ISO_B24(char szTranCode[], char bPRODUCT)
{
    // Casteo a estructura
    PROCCDE_ISO    *pProcCde = (PROCCDE_ISO *)szTranCode;
    // Processing Code, From Acct, To Acct.
    SHORT       shTran    = 0, 
                shFromAcc = 0, 
                shToAcc   = 0;
    // Variable de soporte, estatica y local, ya que se retorna a la salida de la funcion
    static PROCCDE_ISO    stB24TranCode;

    // Copia estatica local
    stB24TranCode.uProcCde    = pProcCde->uProcCde ;
    stB24TranCode.chFiller[0] = 0x00; // Null-ending string
    // Inicio de conversion de codigos ISO8583 a internos de Base24 R4 y R6 
	// ***(RED LINK -Argentina- se aleja del estandar al incorporar TRAN-CDE alfanumericos)***
    // Ambos digitos numericos ?
    if( isdigit(pProcCde->uProcCde.stTranCde.chTran_Cde[0]) &&
        isdigit(pProcCde->uProcCde.stTranCde.chTran_Cde[1]) )
        shTran = antoi(pProcCde->uProcCde.stTranCde.chTran_Cde,2);
    // Solo el primer digito es numerico ?
    else if (isdigit(pProcCde->uProcCde.stTranCde.chTran_Cde[0]) &&
             !isdigit(pProcCde->uProcCde.stTranCde.chTran_Cde[1]) )
        shTran = -1; // Un valor negativo es un valor que NO SE CONVIERTE, se traslada identico    
    // En todo otro caso
    else                
        shTran = -1; // Un valor negativo es un valor que NO SE CONVIERTE, se traslada identico    
    // Cuenta desde-hacia son numericos obligatorios        
    shFromAcc = antoi(pProcCde->uProcCde.stTranCde.chFrom_Acct_Typ,2);
    shToAcc   = antoi(pProcCde->uProcCde.stTranCde.chTo_Acct_Typ,2);

	// DEPENDING ON THE PRODUCT CODE... (ATM,POS,...)
	if( _ISO8583_FORMAT_FOR_ATM_ == bPRODUCT )
		// ATM-OPERATION CODES
		switch( shTran )
		{
			case B24_OPCODE_WITHDRAWAL			: shTran = CEXTR_OPCODE_WITHDRAWAL      ; break; // Extraccion de cuenta
			case B24_OPCODE_DEBIT_ORDER_XFER	: shTran = CEXTR_OPCODE_DEBIT_ORDER_XFER; break; // Orden de debito en cta. por transferencias        
			/*****************************/
#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )
			case B24_OPCODE_CREDIT_ORDER_XFER	: shTran = CEXTR_OPCODE_CREDIT_ORDER_XFER; break; // Orden de credito en cta. por transferencias        
#endif
			/*****************************/
			case B24_OPCODE_DEPOSIT				: shTran = CEXTR_OPCODE_DEPOSIT         ; break; // Depositos en cuenta        
			case B24_OPCODE_BALANCE_INQUIRY		: shTran = CEXTR_OPCODE_BALANCE_INQUIRY ; break; // Consultas de saldos vs.
			case B24_OPCODE_PIN_CHANGE			: shTran = CEXTR_OPCODE_PIN_CHANGE      ; break; // Cambio de PIN        
			case B24_OPCODE_PAYMENT_W_CASH		: shTran = CEXTR_OPCODE_PAYMENT_W_CASH  ; break; // Pago Servicios EFEVO.
			case B24_OPCODE_WITHDRAWAL_FRGN_CURRENCY: shTran = CEXTR_OPCODE_WITHDRAWAL_FRGN_CURRENCY ; break; // Extraccion de cuenta
			case B24_OPCODE_TRANSFER_FRGN_CURRENCY  : shTran = CEXTR_OPCODE_TRANSFER_FRGN_CURRENCY   ; break; // Extraccion de cuenta
			/********************************************************/
			/* JUST IN CASE... CONSIDER NON-REPEATING POS-CODES...  */
			/********************************************************/
			case B24_OPCODE_PAYMENT_W_DEBIT           : shTran = CEXTR_OPCODE_PAYMENT_W_DEBIT     ; break; // Pago con debito en CTA.c/deuda
			case B24_OPCODE_PAYMENT_W_STATEMENT       : shTran = CEXTR_OPCODE_PAYMENT_W_STATEMENT ; break; // Pago con debito en CTA.s/deuda
			case B24_OPCODE_BUYING_W_ACCT_DEBIT       : shTran = CEXTR_OPCODE_BUYING_W_ACCT_DEBIT ; break; // Compra con Debito			
			case B24_OPCODE_BUYING_DEVOLUTION         : shTran = CEXTR_OPCODE_BUYING_DEVOLUTION       ; break; // Devolucion compra "credito NO se procesa"
			case B24_OPCODE_BUYING_DEVOLUTION_CANCEL  : shTran = CEXTR_OPCODE_BUYING_DEVOLUTION_CANCEL; break; // Anul. Devolucion compra "debito NO se procesa"
			case B24_OPCODE_BUYING_CASH_BACK          : shTran = CEXTR_OPCODE_BUYING_CASH_BACK        ; break; // Compra cash-back
			case B24_OPCODE_BUYING_CASH_BACK_CANCEL   : shTran = CEXTR_OPCODE_BUYING_CASH_BACK_CANCEL ; break; // Anulacion Compra cash-back
			/* Ooops ... B24_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL:  repeated!.... */
			default : break; // permanece sin cambios, puede ser alfanumerico....
		}
	else
		// POS-OPERATION CODES
		switch( shTran )
		{
			case B24_OPCODE_BALANCE_INQUIRY		: shTran = CEXTR_OPCODE_BALANCE_INQUIRY ; break; // Consultas de saldos vs.
			case B24_OPCODE_PAYMENT_W_DEBIT     : shTran = CEXTR_OPCODE_PAYMENT_W_DEBIT     ; break; // Pago con debito en CTA.c/deuda
			case B24_OPCODE_PAYMENT_W_STATEMENT : shTran = CEXTR_OPCODE_PAYMENT_W_STATEMENT ; break; // Pago con debito en CTA.s/deuda
			case B24_OPCODE_BUYING_W_ACCT_DEBIT : shTran = CEXTR_OPCODE_BUYING_W_ACCT_DEBIT ; break; // Compra con Debito
			case B24_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL: shTran = CEXTR_OPCODE_BUYING_W_ACCT_DEBIT_CANCEL ; break; // Anulacion compra "credito"
			case B24_OPCODE_BUYING_DEVOLUTION         : shTran = CEXTR_OPCODE_BUYING_DEVOLUTION       ; break; // Devolucion compra "credito NO se procesa"
			case B24_OPCODE_BUYING_DEVOLUTION_CANCEL  : shTran = CEXTR_OPCODE_BUYING_DEVOLUTION_CANCEL; break; // Anul. Devolucion compra "debito NO se procesa"
			case B24_OPCODE_BUYING_CASH_BACK          : shTran = CEXTR_OPCODE_BUYING_CASH_BACK        ; break; // Compra cash-back
			case B24_OPCODE_BUYING_CASH_BACK_CANCEL   : shTran = CEXTR_OPCODE_BUYING_CASH_BACK_CANCEL ; break; // Anulacion Compra cash-back
			/********************************************************/
			/* JUST IN CASE... CONSIDER NON-REPEATING ATM-CODES...  */
			/********************************************************/
			case B24_OPCODE_WITHDRAWAL			   : shTran = CEXTR_OPCODE_WITHDRAWAL      ; break; // Extraccion de cuenta
			case B24_OPCODE_DEBIT_ORDER_XFER	   : shTran = CEXTR_OPCODE_DEBIT_ORDER_XFER; break; // Orden de debito en cta. por transferencias        
			case B24_OPCODE_DEPOSIT				   : shTran = CEXTR_OPCODE_DEPOSIT         ; break; // Depositos en cuenta        
			case B24_OPCODE_PIN_CHANGE			   : shTran = CEXTR_OPCODE_PIN_CHANGE      ; break; // Cambio de PIN        
			case B24_OPCODE_PAYMENT_W_CASH		   : shTran = CEXTR_OPCODE_PAYMENT_W_CASH  ; break; // Pago Servicios EFEVO.
			case B24_OPCODE_TRANSFER_FRGN_CURRENCY : shTran = CEXTR_OPCODE_TRANSFER_FRGN_CURRENCY   ; break; // Extraccion de cuenta
			/* Ooops B24_OPCODE_WITHDRAWAL_FRGN_CURRENCY: repeated ! */			
			default : break; // permanece sin cambios, puede ser alfanumerico....
		};
	// FROM-ACCOUNT TYPE
    switch( shFromAcc )
    {
        case _B24_CHECKINGS_ACCOUNT_USD_ : shFromAcc = _CEXTR_CHECKINGS_ACCOUNT_USD_; break; // CUENTA CORRIENTE EN DOLARES
        case _B24_SAVINGS_ACCOUNT_       : shFromAcc = _CEXTR_SAVINGS_ACCOUNT_      ; break; // CAJA DE AHORROS EN PESOS
        case _B24_SAVINGS_ACCOUNT_USD_   : shFromAcc = _CEXTR_SAVINGS_ACCOUNT_USD_  ; break; // CAJA DE AHORROS EN DOLARES
        case _B24_CHECKINGS_ACCOUNT_     : shFromAcc = _CEXTR_CHECKINGS_ACCOUNT_    ; break; // CUENTA CORRIENTE EN PESOS        
        case _B24_CREDIT_CARD_ACCOUNT_   : shFromAcc = _CEXTR_CREDIT_CARD_ACCOUNT_  ; break; // CUENTA TARJETA DE CREDITO EN PESOS
        case _B24_CREDIT_ACCOUNT_        : shFromAcc = _CEXTR_CREDIT_ACCOUNT_       ; break; // CUENTA CREDITO VISTA EN PESOS
		case _B24_TRANSFER_ACCOUNT_CBU_  : shFromAcc = _CEXTR_CREDIT_ACCOUNT_       ; break; // CTA. TRANSF. CBU-ACH
        default : break; // permanece sin cambios
    };
	// TO-ACCOUNT TYPE
    switch( shToAcc )
    {
        case _B24_CHECKINGS_ACCOUNT_USD_ : shToAcc = _CEXTR_CHECKINGS_ACCOUNT_USD_; break; // CUENTA CORRIENTE EN DOLARES
        case _B24_SAVINGS_ACCOUNT_       : shToAcc = _CEXTR_SAVINGS_ACCOUNT_      ; break; // CAJA DE AHORROS EN PESOS
        case _B24_SAVINGS_ACCOUNT_USD_   : shToAcc = _CEXTR_SAVINGS_ACCOUNT_USD_  ; break; // CAJA DE AHORROS EN DOLARES
        case _B24_CHECKINGS_ACCOUNT_     : shToAcc = _CEXTR_CHECKINGS_ACCOUNT_    ; break; // CUENTA CORRIENTE EN PESOS
        case _B24_CREDIT_CARD_ACCOUNT_   : shToAcc = _CEXTR_CREDIT_CARD_ACCOUNT_  ; break; // CUENTA TARJETA DE CREDITO EN PESOS
        case _B24_CREDIT_ACCOUNT_        : shToAcc = _CEXTR_CREDIT_ACCOUNT_       ; break; // CUENTA CREDITO VISTA EN PESOS
		case _B24_TRANSFER_ACCOUNT_CBU_  : shFromAcc = _CEXTR_CREDIT_ACCOUNT_       ; break; // CTA. TRANSF. CBU-ACH
        default : break; // permanece sin cambios
    };
    // En ASCII-DISPLAY
    // Si son todos numericos....
    if(shTran > 0)
        sprintf( stB24TranCode.uProcCde.stTranCde.chTran_Cde, "%02d%02d%02d", 
                shTran, shFromAcc, shToAcc );
    // Sino, si alguno es alfanumerico....
    else
        sprintf( stB24TranCode.uProcCde.stTranCde.chTran_Cde, "%2.2s%02d%02d", 
                pProcCde->uProcCde.stTranCde.chTran_Cde, shFromAcc, shToAcc );
    // Retorno de valor convertido, en forma estatica
    return ( stB24TranCode.uProcCde.chData );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formateo especial de campos fecha y hora
BOOL TrxBaseResolution::FormatTrxDateTime(char *szYYYYMMDD, char *szHHMMSS,    
                                    char *szYYYYMMDD_Trx, char *szHHMMSS_Trx,
                                    char *szYYYYMMDD_Cap )    
{
    // Precondicion
    if(!(szYYYYMMDD && szHHMMSS && szYYYYMMDD_Trx && szHHMMSS_Trx && szYYYYMMDD_Cap))
        return FALSE;
    /* Invocar a funcion base con defaults */
    return FormatTrxDateTime_WMask( szYYYYMMDD, szHHMMSS,    
                                    szYYYYMMDD_Trx, szHHMMSS_Trx,
                                    szYYYYMMDD_Cap,
                                    is_true , is_true, is_true, is_true, 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formateo especial de campos fecha y hora, segun mascara y segun ano de 4 digitos
BOOL TrxBaseResolution::FormatTrxDateTime_WMask(char *szYYYYMMDD, char *szHHMMSS,    
                                    char *szYYYYMMDD_Trx, char *szHHMMSS_Trx,
                                    char *szYYYYMMDD_Cap,
                                    boolean_t bUseSeparators ,
                                    boolean_t bUse4DigitsYear,
                                    boolean_t bFormatIsMMDDYY,
                                    boolean_t bAlwaysTimeIsDateAndTime,
                                    short     nTimestampLength,                                    
                                    short     nHourLength)    
{
    const time_t tTimestamp         = time( NULL );
    const tm *ptmLocal              = localtime( &tTimestamp );
    short nYear                     = ptmLocal->tm_year + 1900 ;   /* default es ano actual */
    const short cnYearDigits        = ( bUse4DigitsYear ) ? 4 : 2; /* default es 4 digitos  */
    short nLength                   = 0;
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
    short nMonth                    = ptmLocal->tm_mon + 1;  /* default es mes actual */
    short nYearTranDat              = nYear ;               /* default es ano actual */
    short nYearPostDat              = nYear ;               /* default es ano actual */
    /***********************************************************************/

    // No hay Precondicion esepcifica, ya que pueden o no informarse todos los campos

    //////////////////////////////////////////////////////////////////////
    // Segun mascara y ano de 4 digitos
    //////////////////////////////////////////////////////////////////////
    // ptmLocal->tm_year + 1900; /* years since 1900 */
    // ptmLocal->tm_mon + 1;     /* months since January - [0;11] */
    // ptmLocal->tm_mday;        /* day of the month - [1;31] */
    // ptmLocal->tm_hour;        /* hours since midnight - [0;23] */        
    // ptmLocal->tm_min;         /* minutes after the hour - [0;59] */
    // ptmLocal->tm_sec;         /* seconds after the minute - [0;59] */
    //////////////////////////////////////////////////////////////////////
    if(bUse4DigitsYear)
        nYear = ptmLocal->tm_year + 1900 ;
    else
        nYear = ptmLocal->tm_year % 100;

    //////////////////////////////////////////////////////////////////////////
    // Cuando el MES de POSTING-DATE es MENOR al de TRANSACTION-DATE,
    // y es IGUAL al MES actual del sistema, ha cambiado el año !!!
    // Ej. Es 01-01-2002 y entra SAF del 31-12-2001, debe restarse 1 al año actual.
    if( strncmp(isoFields.field17.chMMDD, isoFields.field13.chMMDD,2) < 0 
        &&
        antoi(isoFields.field17.chMMDD,2) == nMonth )
            nYearTranDat = nYear -  1;
    else
            nYearTranDat = nYear ;
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // Cuando el MES de POSTING-DATE es MENOR al de TRANSACTION-DATE,
    // y es MENOR al MES actual del sistema, ha cambiado el año !!!
    // Ej. Es 31-12-2001 y ya ingresan del 01-01-2002, debe sumarse 1 al año actual.
    if( strncmp(isoFields.field17.chMMDD, isoFields.field13.chMMDD,2) < 0 
        &&
        antoi(isoFields.field17.chMMDD,2) < nMonth )
            nYearPostDat = nYear +  1;
    else
            nYearPostDat = nYear ;
    //////////////////////////////////////////////////////////////////////////

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

    // Fecha de Transmision, formato MM/DD/YYYY (XMIT-DATE), ISO-FIELD-07
    if(szYYYYMMDD)
    {
        if(bFormatIsMMDDYY)
            sprintf(szYYYYMMDD,pszDATE_Mask, isoFields.field7.chMMDD, isoFields.field7.chMMDD+2, 
                cnYearDigits , nYear );
        else
            sprintf(szYYYYMMDD,pszDATE_Mask, cnYearDigits , nYear, isoFields.field7.chMMDD, 
                isoFields.field7.chMMDD+2 );
    }
    // Hora de Transmision formato HH:MM:SS (XMIT-TIME), ISO-FIELD-07
    if(szHHMMSS)
    {
        if(bAlwaysTimeIsDateAndTime)
        {
            sprintf(szHHMMSS  ,pszTIMESTAMP_Mask , szYYYYMMDD, 
                isoFields.field7.chHHMMSS,isoFields.field7.chHHMMSS+2,isoFields.field7.chHHMMSS+4 );
            // Paddear lo restante con ceros
            nLength = strlen(szHHMMSS);
            if(nTimestampLength > nLength)
                memset( szHHMMSS+nLength, '0', nTimestampLength-nLength);
        }
        else
            sprintf(szHHMMSS  ,pszTIME_Mask , 
                isoFields.field7.chHHMMSS,isoFields.field7.chHHMMSS+2,isoFields.field7.chHHMMSS+4 );
    };
    // Fecha de Transaccion, formato MM/DD/YYYY (TRX-DATE), ISO-FIELD-13
    // El ano se obtiene , o bien del sistema, o bien de la fecha de transaccion :
    // Mismo ano de transmision que la fecha de negocios? (comparar 2 ultimos digitos)
    if(szYYYYMMDD_Trx)
    {
        if(bFormatIsMMDDYY)
            sprintf(szYYYYMMDD_Trx,pszDATE_Mask, isoFields.field13.chMMDD, isoFields.field13.chMMDD+2, 
                cnYearDigits , nYearTranDat );
        else
            sprintf(szYYYYMMDD_Trx,pszDATE_Mask, cnYearDigits , nYear, isoFields.field13.chMMDD, 
                isoFields.field13.chMMDD+2 );
    }
    // Hora de Transaccion formato HH:MM:SS (TRX-TIME = XMIT-TIME), ISO-FIELD-12
    if(szHHMMSS_Trx)
    {
        if(bAlwaysTimeIsDateAndTime)
            sprintf(szHHMMSS_Trx  ,pszTIMESTAMP_Mask , szYYYYMMDD_Trx,
                isoFields.field12.chHHMMSS,isoFields.field12.chHHMMSS+2,isoFields.field12.chHHMMSS+4 );
        else
            sprintf(szHHMMSS_Trx  ,pszTIME_Mask , 
                isoFields.field12.chHHMMSS,isoFields.field12.chHHMMSS+2,isoFields.field12.chHHMMSS+4 );
        // Paddear lo restante con ceros, si corresponde, a los milisegundos....
        nLength = strlen(szHHMMSS_Trx);
        if(nHourLength > nLength)
            memset( szHHMMSS_Trx+nLength, '0', nHourLength-nLength);
    };
    // Fecha de Negocios (capture date)(CAP-DATE), ISO-FIELD-17
    if(szYYYYMMDD_Cap)
    {
        if(bFormatIsMMDDYY)
            sprintf(szYYYYMMDD_Cap,pszDATE_Mask, isoFields.field17.chMMDD, isoFields.field17.chMMDD+2, 
                cnYearDigits , nYearPostDat );
        else
            sprintf(szYYYYMMDD_Cap,pszDATE_Mask, cnYearDigits , nYear, isoFields.field17.chMMDD, 
                isoFields.field17.chMMDD+2 );
    }
    // Ok, returnar
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Verifica si el reverso es por rta. tardia de nuestro host hacia el switch financiero
WORD TrxBaseResolution::VerifyReversalForLateResponse(boolean_t bIsReversal)
{
    // Es reverso ?
    if( (bIsReversal == is_true) && isoMsg.IsValidReversal() )
    {
        // Codigo de reverso si hubiera
        WORD wRevCode = antoi(isoFields.field39.chRespCde,2);
        // Verificar campos ISO cargados
        if(is_false == isoFields.bLoaded)
            ProcessInit(); // Parsear y cargar campos ISO en estructuras            
        ///////////////////////////////////////////////////////////////////////////////
        // REGLA DE REVERSOS : Si existe mas de 1 reverso : 
        // El 1ro es siempre por timeout (68) y se aplica al requerimiento 0200,
        // pero no a los forzados 220
        // El 2do, si el codigo es distinto de 68, se aplica al forzado 220.
        // El 2do, si el codigo es igual a 68 y es "REVERSAL FOR LATE", se aplica
        // al 0210, pero si no contiene "REVERSAL FOR LATE", se aplica al forzado, 220.
        // El 3er reverso se aplica.
        ///////////////////////////////////////////////////////////////////////////////
        if((strstr(isoFields.field43.chData, 
            "REVERSAL FOR LATE/UNSOL RESPONSE") != NULL)
            && 
            (wRevCode == CISO_REV_TIMEOUT))
            // TIMEOUT para REQUERIMIENTOS (200)
            return CISO_REV_TIMEOUT; // 68
        else if((strstr(isoFields.field43.chData, 
            "REVERSAL FOR LATE/UNSOL RESPONSE") == NULL)
            && 
            (wRevCode == CISO_REV_TIMEOUT))
            // TIMEOUT para FORZADO (220)
            return CISO_REV_TIMEOUT_ADVICE ; // 168
        else
            // OTROS CODIGOS DE REVERSOS
            return wRevCode ;
    }
    else
        // NO ES REVERSO
        return 0;
};

///////////////////////////////////////////////////////////////////////
// Extraer el TRACK-2 desde el campo TRACK-2 ISO, hasta el FIELD SEPARATOR
PCHAR TrxBaseResolution::ExtractCardnumFromTrack2( const TRACK2_ISO *pstTrack2ISO)
{    
    char *pszSeparator    = NULL,
         *pszBegin        = (char *)pstTrack2ISO,
         chAfterSeparator = 'N'; /* reemplaza despues del separador ? */
    // Variable de soporte, estatica y local, ya que se retorna a la salida de la funcion
    static TRACK2_ISO    stTrack2Cardnum;

    // Precondicion
    if(!pstTrack2ISO)
        return NULL;


    ///////////////////////////////////////////////////////////////////////////////////
    // Buscar separador NO NUMERICO, primer digito despues del nro. de tarjeta
    // Limpiar con blancos los digitos DETRAS del separador no numerico
    for( pszSeparator = (char *)pstTrack2ISO;
        // Mientras haya caracteres y la longitud no exceda el maximo ISO de 37 digitos
        pszSeparator != NULL       &&    /* no sea nulo */
        pszSeparator >= pszBegin   &&    /* sea mayor al inicio del track-2 */
        (pszSeparator - pszBegin) <= 37; /* hasta 37 digitos */
        pszSeparator++ )
    {
        // Si es separador o digito numerico, reemplazarlo con BLANCO
        if((*pszSeparator >= '0' && *pszSeparator <= '9'))        // ASCII-48 a 57
        {
            /* Si ya paso el separador de TRACK, reemplazar igual */
            if( 'Y' == chAfterSeparator )
                (*pszSeparator) = ' ';
            else
                continue;
        }
        else if((*pszSeparator >= 'A' && *pszSeparator <= 'Z')) // ASCII-65 a 90
        {
            chAfterSeparator = 'Y';
            (*pszSeparator) = ' ';
        }
        else if((*pszSeparator >= '!' && *pszSeparator <= '/')) // ASCII-33 a 48
        {
            chAfterSeparator = 'Y';
            (*pszSeparator) = ' ';
        }
        else if((*pszSeparator) == '=') // ASCII-61
        {
            chAfterSeparator = 'Y';
            (*pszSeparator) = ' ';
        }
        else
        {
            chAfterSeparator = 'Y';
            (*pszSeparator) = ' ';
        }; /* end-if */
    }; /* end-for */
    ///////////////////////////////////////////////////////////////////////////////////
    // Retornar el CARD-NUM desde el TRACK-2
    stTrack2Cardnum = (*pstTrack2ISO);
    stTrack2Cardnum.stTrack2.chFiller[0] = 0x00;
    return (stTrack2Cardnum.stTrack2.chData);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inicio de Procesamiento de transacciones en general
WORD TrxBaseResolution::ProcessInit(void)
{
	/**************************************************************************/
	// DO THE ISO8583 FIELD EXPANSION (CALL PROPIETARY METHOD)
	return isoMsg.ExpandFields ( isoFields );
	/**************************************************************************/
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// 2006.03.01 mdc
int TrxBaseResolution::SetISOMessage(PBYTE pMsg,WORD iLen)   // ISO8583 msg
{
	return isoMsg.Import(pMsg,iLen);
}
int TrxBaseResolution::GetISOMessage(PBYTE pMsg,PWORD piLen)   // ISO8583 msg
{
	return isoMsg.Export(pMsg, piLen);
}
////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////


