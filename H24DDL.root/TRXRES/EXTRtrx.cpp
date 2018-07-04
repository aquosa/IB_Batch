////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// Process ATM Transaction Method
//
//   Metodo de Procesamiento de Transaccion ATM, con conexion y desconexion a base de
// datos para cada transaccion entrante, con manejo local de parsing del mensaje ISO
// y seteo del codigo de respuesta en el mensaje saliente.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2003.05.09      mdc     Base
// (Inicial)    2003.06.04      mdc     REVERSAL REASONS
// (Inicial)    2003.06.10      mdc     Para formatos EXTRACT, clase especializada TrxResEXTRACT
// (Release-1)  2003.07.23      mdc     Arbitraje no multiplica AMMT-1 : bArbitraje = is_false;
// (Release-2)  2003.08.26      mdc     Señales de uso o no de transacciones CBU y ANUL.COMPRAS.
// (Release-2)  2003.10.30      mdc     NO SE REVERSA PARCIALMENTE EL FAST-CASH.
// (Release-3)  2004.04.29      mdc     Fix a multiplicacion de cotizacion x/Banco Industrial.
// (Release-4)  2006.02.14      mdc     AMT-2 en cero para reversos TOTALES
// (Release-5A) 2006.03.27      mdc     Devol.Compra y Anul.Devol. Compra retornan 91 y 00 (normal y SAF)
// (Release-5B) 2006.03.28      mdc     Fast Cash lo mismo que anterior.
// (Release-5C) 2006-05-15      mdc     VISUAL .NET EXCEPTION : TrxResEXTRACT::~TrxResEXTRACT()
// (Release-5D) 2006-07-15      mdc     Jamas cambiar el FLAG de multiplicacion por TIPO DE CAMBIO  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Headers ANSI C/C++
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#ifndef _SYSTEM_ATM_   
#define _SYSTEM_ATM_DEFINED_   
#define _SYSTEM_ATM_   
#endif // _SYSTEM_ATM_DEFINED_

// Header Transaction Resolution
#include <qusrinc/trxres.h>
// Libreria de utilidades estandards
#include <qusrinc/stdiol.h>
// Header ISO Msg Redefinition
#include <qusrinc/isoredef.h>
// Header ISO 8583
#include <qusrinc/iso8583.h>
// Header EXTRACT BASE24 RED LINK
#include <qusrinc/extrctst.h>

#ifdef _SYSTEM_ATM_DEFINED_
#undef _SYSTEM_ATM_
#endif // _SYSTEM_ATM_DEFINED_

// Librerias propias de TCP/IP v4
#include <qusrinc/tcp4u.h>

/////////////////////////////////////////////////////////////////////////////////////////
// Prototipo de funcion utilitaria que retorna la cantidad de bytes numericos en string
/////////////////////////////////////////////////////////////////////////////////////////
short strnumlen( char *pszValue , const short nMax );
/////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////
// Definiciones y Macros de expansion MUY utiles para simplificar el codigo
/////////////////////////////////////////////////////////////////////////////////////////
// Macro de asignacion de String, con NULL al final
#define COPYSTRING(dst,src) strcpy(dst,src) 
// Macro de asignacion de String, con NULL al final
#define ASSIGNSTRING(dst,src) \
    { strncpy(dst,src,sizeof(src)); dst[sizeof(src)]=0x00; }
// Macro de asignacion de bytes a campo de estructura
#define ASSIGNFIELD(dst,src) \
    { memset(dst,' ',sizeof(dst)); memcpy(dst,src,min(sizeof(src),sizeof(dst)));  }
// Macro de asignacion de ceros a campo de estructura
#define SETFIELDZEROED(dst)   memset(dst ,'0',sizeof(dst))
#define SETFIELDBLANKED(dst)  memset(dst ,' ',sizeof(dst))
#define SETSTRUCTZEROED(dst)   memset(&dst,'0',sizeof(dst))
#define SETSTRUCTBLANKED(dst)  memset(&dst,' ',sizeof(dst))
#define SETSTRUCTNULL(dst)     memset(&dst, 0x00,sizeof(dst))
/////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// Nombre local al modulo para el archivo de logg del EXTRACT
static const char _LOGFILENAME_[] = "h24log.txt";
// ISO8583 Msg. Header
static const char _ISO_MSG_HEADER_[] = "ISO";
// Longitud maxima de tarjeta en TACK-2
static const short _MAX_CARDNUM_LEN_ = 19;
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// Funcion utilitaria que retorna la cantidad de bytes numericos en string
///////////////////////////////////////////////////////////////////////////
short strnumlen( char *pszValue , const short nMax );


/////////////////////////////////////////////////////////////////////////////////////
// Implementacion
/////////////////////////////////////////////////////////////////////////////////////
// Constructor
TrxResEXTRACT::TrxResEXTRACT()
{
    // Apertura del archivo de logging
    glLogg.Open(_LOGFILENAME_);
}

/////////////////////////////////////////////////////////////////////////////////////
// Constructor opcional con parametros para el back-end
TrxResEXTRACT::TrxResEXTRACT(char szIPBackEnd[], unsigned short usPortBackEnd)
{
    // Apertura del archivo de logging
    glLogg.Open(_LOGFILENAME_);
}


// Transacciones individualizadas, que luego, se procesan genericamente
WORD TrxResEXTRACT::ProcessBalanceInquiry(WORD wCause,boolean_t bIsRvsl)    // Consulta de saldos
{
    // Metodo generico de procesamiento de transaccion
    return ProcessGeneric();
}
WORD TrxResEXTRACT::ProcessTransfer(WORD wCause,boolean_t bIsRvsl)    // Transferencia Ctas. relacionadas
{
    // Metodo generico de procesamiento de transaccion
    return ProcessGeneric();
}
WORD TrxResEXTRACT::ProcessTransferInterbank(WORD wCause,boolean_t bIsRvsl)    // Transferencia Interbancaria
{
    // Metodo generico de procesamiento de transaccion
    return ProcessGeneric();
}
WORD TrxResEXTRACT::ProcessIndividualTransfer(WORD wCause,boolean_t bIsRvsl)// Transferencia CBU de individuo
{
	////////////////////////////////////////////////////
	// FLAG que controla el procesamiento OFF-ON del CBU
	boolean_t bIsOfflineIndivXfer = is_false;
	////////////////////////////////////////////////////

    // Respuesta default
    WORD wRespCde = CISO_EXTERNAL_DECLINE_I;
    // Precondicion = Descomponer mensaje ISO-8583 en campos estructurados 
    wRespCde = ProcessInit();
    // Denegada, no se procesa, segun sea ON/OFF, sera 91 o 06
    wRespCde = ( isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() )
                        ? CISO_SYSTEM_FAILURE_I
                        : CISO_DESTINY_NOT_AVAILABLE_I;
    ///////////////////////////////////////////////////////////////////////////
    // Verificar transaccion XFER POR CBU    
    if( isoMsg.IsATMTransferCBU() 
		&& is_true == bIsOfflineIndivXfer )
    {
        glLogg.Put("############# PARSEO DE TRX EXTRACT-ATM-POS #############\n");    
        glLogg.Put("Transferencia por CBU, se procesa BATCH, resultado:<%i>\n", wRespCde );            
        ///////////////////////////////////////////////////////////////////////
        // Formatear campo ISO #44, RESP_DATA, saldo y disponible
        FormatField44_RESP_DATA( NULL, NULL , NULL);
        // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
        FormatField122_SECNDRY_RSRVD3_PRVT();
        // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
        FormatField123_SECNDRY_RSRVD4_PRVT();
        // Borrar campos no validos en la rta. : 43,48,52,63
        isoMsg.DeleteField ( 43 );
        isoMsg.DeleteField ( 48 );
        isoMsg.DeleteField ( 52 );
        isoMsg.DeleteField ( 63 );
        // Borrar campo 120, OPCIONALMENTE PODRIAN NO VENIR EN UN REQUERIMIENTO 
        isoMsg.DeleteField ( 120 );        
        glLogg.Put("### Fin ###\n");
        // Denegada, no se procesa, segun sea ON/OFF, sera 91 o 06
        return (wRespCde);        
        ///////////////////////////////////////////////////////////////////////
    }    
    else
        // Metodo generico de procesamiento de transaccion
        return ProcessGeneric();
}
WORD TrxResEXTRACT::ProcessWithdrawal(WORD wCause,boolean_t bIsRvsl)        // Extraccion
{
    // Metodo generico de procesamiento de transaccion
    return ProcessGeneric();
}
WORD TrxResEXTRACT::ProcessDeposit(WORD wCause,boolean_t bIsRvsl)            // Deposito
{
    // Metodo generico de procesamiento de transaccion
    return ProcessGeneric();
}
WORD TrxResEXTRACT::ProcessFastCash(WORD wCause,boolean_t bIsRvsl)            // FastCash o compra
{
	//////////////////////////////////////////////////////////////////////////
	// Este BOOLEANO controla si se procesan online o no las compras fast-cash
	boolean_t bIsOfflineFCash = is_true;
	//////////////////////////////////////////////////////////////////////////

    // Respuesta default
    WORD wRespCde = CISO_EXTERNAL_DECLINE_I;    
    // Precondicion = Descomponer mensaje ISO-8583 en campos estructurados 
    wRespCde = ProcessInit();
    // Denegada, no se procesa, segun sea ON/OFF, sera 91-Autoriza RED, o 00-Aprobada sin DEBITO
    wRespCde = ( isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() )
                    ? CISO_APPROVED_I
                    : CISO_DESTINY_NOT_AVAILABLE_I;    
    // Verificar transaccion CASH BACK Y SU ANULACION
    if((antoi(isoFields.field3.uProcCde.stTranCde.chTran_Cde,2)==B24_OPCODE_BUYING_CASH_BACK        || 
        antoi(isoFields.field3.uProcCde.stTranCde.chTran_Cde,2)==B24_OPCODE_BUYING_CASH_BACK_CANCEL  )  
		&& is_true == bIsOfflineFCash )
    {
        glLogg.Put("############# PARSEO DE TRX EXTRACT-ATM-POS #############\n");    
        glLogg.Put("FAST-CASH se procesan BATCH, resultado:<%i>\n", wRespCde);            
        ///////////////////////////////////////////////////////////////////////
        // Formatear campo ISO #44, RESP_DATA, saldo y disponible
        FormatField44_RESP_DATA( NULL, NULL , NULL);
        // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
        FormatField122_SECNDRY_RSRVD3_PRVT();
        // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
        FormatField123_SECNDRY_RSRVD4_PRVT();
        // Borrar campos no validos en la rta. : 43,48,52,63
        isoMsg.DeleteField ( 43 );
        isoMsg.DeleteField ( 48 );
        isoMsg.DeleteField ( 52 );
        isoMsg.DeleteField ( 63 );
        // Borrar campo 120, OPCIONALMENTE PODRIAN NO VENIR EN UN REQUERIMIENTO 
        isoMsg.DeleteField ( 120 );        
        glLogg.Put("### Fin ###\n");
        // Denegada, no se procesa, segun sea ON/OFF, sera 91 o 06
        return (wRespCde);        
        ///////////////////////////////////////////////////////////////////////
    }    
    else
        return ProcessGeneric();
    /////////////////////////////////////////////////////////
}
WORD TrxResEXTRACT::ProcessAccountVerification(WORD wCause,boolean_t bIsRvsl)    // Verificacion cuenta destino
{
    // Metodo generico de procesamiento de transaccion
    return ProcessGeneric();
}
WORD TrxResEXTRACT::ProcessPayment(WORD wCause,boolean_t bIsRvsl)            // Pagos
{
    // Metodo generico de procesamiento de transaccion
    return ProcessGeneric();
}
WORD TrxResEXTRACT::ProcessBuying(WORD wCause,boolean_t bIsRvsl)            // Compras y relacionados
{        
	//////////////////////////////////////////////////////////////////////////
	// Este BOOLEANO controla si se procesan online o no las compras devueltas
	boolean_t bIsOfflineBuying = is_true;
	//////////////////////////////////////////////////////////////////////////

    // Respuesta default
    WORD wRespCde = CISO_EXTERNAL_DECLINE_I;    
    // Precondicion = Descomponer mensaje ISO-8583 en campos estructurados 
    wRespCde = ProcessInit();
    // Denegada, no se procesa, segun sea ON/OFF, sera 91-Autoriza RED, o 00-Aprobada sin DEBITO
    wRespCde = ( isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() )
                    ? CISO_APPROVED_I
                    : CISO_DESTINY_NOT_AVAILABLE_I;    
    // Verificar transaccion DEVOLUCION DE COMPRA/ANULACION DEV.COMPRA/PREAUTORIZACION/...
		// Devolucion compra (credito NO se procesa)
    if((antoi(isoFields.field3.uProcCde.stTranCde.chTran_Cde,2)==B24_OPCODE_BUYING_DEVOLUTION        
        || 
		// Anul. Devolucion compra (debito NO se procesa)
        antoi(isoFields.field3.uProcCde.stTranCde.chTran_Cde,2)==B24_OPCODE_BUYING_DEVOLUTION_CANCEL  
#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )
        ||
		// Preautorizacion NO se procesa
        antoi(isoFields.field3.uProcCde.stTranCde.chTran_Cde,2)==B24_OPCODE_BUYING_PREAUTH            
        ||
		// Anul. Preautorizacion NO se procesa
        antoi(isoFields.field3.uProcCde.stTranCde.chTran_Cde,2)==B24_OPCODE_BUYING_PREAUTH_CANCEL   
#endif
        ) 
        && 
        is_true == bIsOfflineBuying )
    {
        glLogg.Put("############# PARSEO DE TRX EXTRACT-ATM-POS #############\n");    
        glLogg.Put("Devolucion/Anul.Devol.Compra se procesan BATCH, resultado:<%i>\n", wRespCde);            
        ///////////////////////////////////////////////////////////////////////
        // Formatear campo ISO #44, RESP_DATA, saldo y disponible
        FormatField44_RESP_DATA( NULL, NULL , NULL);
        // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
        FormatField122_SECNDRY_RSRVD3_PRVT();
        // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
        FormatField123_SECNDRY_RSRVD4_PRVT();
        // Borrar campos no validos en la rta. : 43,48,52,63
        isoMsg.DeleteField ( 43 );
        isoMsg.DeleteField ( 48 );
        isoMsg.DeleteField ( 52 );
        isoMsg.DeleteField ( 63 );
        // Borrar campo 120, OPCIONALMENTE PODRIAN NO VENIR EN UN REQUERIMIENTO 
        isoMsg.DeleteField ( 120 );        
        glLogg.Put("### Fin ###\n");
        // Denegada, no se procesa, segun sea ON/OFF, sera 91 o 06
        return (wRespCde);        
        ///////////////////////////////////////////////////////////////////////
    }    
    else
        return ProcessGeneric();
    /////////////////////////////////////////////////////////
}
WORD TrxResEXTRACT::ProcessAutServicePayment(WORD wCause,boolean_t bIsRvsl)    // Pagos Automatico de Servs.
{
    // Metodo generico de procesamiento de transaccion
    return ProcessGeneric();
}
WORD TrxResEXTRACT::ProcessCreditNote(WORD wCause,boolean_t bIsRvsl)        // Nota de credito
{
    // Metodo generico de procesamiento de transaccion
    return ProcessGeneric();
}
WORD TrxResEXTRACT::ProcessDebitNote(WORD wCause,boolean_t bIsRvsl)            // Nota de debito
{
    // Metodo generico de procesamiento de transaccion
    return ProcessGeneric();
}
WORD TrxResEXTRACT::ProcessReversal(WORD wCause)            // Reverso generico        
{
    //////////////////////////////////////////////////////////////////////
    // Respuesta default
    WORD wRespCde   = CISO_EXTERNAL_DECLINE_I;    
    WORD wRspCdeLen = 2;
    CHAR szRespCode[3] = {"21"}; // CISO_REV_HARDWARE_ERROR=21
    // Por default, ante reversos parciales, reversa completo el original y 
    // genera un nuevo movimiento por el monto realmente dispensado.
    // Si este comportamiento es distinto en el sistema siendo implementado,
    // simplemente re-definir este metodo virtual en la superclase que hereda
    // a esta clase,  y omitir este nuevo movimiento.
    const boolean_t bIMPACT_PARTIAL_REV = is_false;

    // Precondicion = Descomponer mensaje ISO-8583 en campos estructurados 
    wRespCde = ProcessInit();
    // Verificar TIMEOUT para FORZADO (220), y aplicar nuevo codigo de reversa (reverso triple)
    // Control de Reverso Triple : Similar al Reverso Doble, solo que se agregar el time-out 
    // del ATM, caso en que BASE24 vuelve a enviar un tercer reverso pero sobre el forzado
    // 220, con motivo de reverso en 68 y con leyenda "*** REVERSAL FOR LATE/UNSOL RESPONSE ***".
    // Controlado por HOST24, este motivo de reverso se cambia a 168, para diferenciarlo de un
    // time-out comun.
    if(VerifyReversalForLateResponse( is_true )==CISO_REV_TIMEOUT_ADVICE)
    {
        // Modificar codigo de reversa para que impacte el forzado
        // por CCISO_REV_HARDWARE_ERROR = 21
        isoMsg.SetField(39, &(wRspCdeLen=2), (PBYTE)szRespCode,    2 );
    };

    //////////////////////////////////////////////////
    // Metodo generico de procesamiento de transaccion
    wRespCde = ProcessGeneric();
    //////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////
    // Si es REVERSO PARCIAL, y solamente en ese caso, debe re-generarse un movimiento
    // solo por el monto realmente dispensado, debitado o acreditado : Eso lo dice el 
    // campo #95 del mensaje ISO8583. El monto original se informa en el campo #4.
    // Entonces : Si el reverso completo fue OK...
    ///////////////////////////////////////////////////////////////////////////////////
    if( (CISO_APPROVED_I == wRespCde && is_true == bIMPACT_PARTIAL_REV)
        &&
        // Si es REVERSO PARCIAL, resp-cde=32
        (antoi(isoFields.field39.chRespCde,2)==CISO_REV_PARTIAL)            
        && 
        ////////////////////////////////////////////////////////////////////////////////////////////
        // NO SE REVERSA PARCIALMENTE EL FAST-CASH
        antoi(isoFields.field3.uProcCde.stTranCde.chTran_Cde,2)!=B24_OPCODE_BUYING_CASH_BACK 
        &&
        antoi(isoFields.field3.uProcCde.stTranCde.chTran_Cde,2)!=B24_OPCODE_BUYING_CASH_BACK_CANCEL
        &&
        ////////////////////////////////////////////////////////////////////////////////////////////
        // Si DIFIERE el monto original del realmente dispensado... (PARTIAL RVSL)
        // (monto original es MAYOR o IGUAL al dispensado)
        (strncmp(isoFields.field4.chAmmount,isoFields.field95.chReversalAmmount,
                sizeof(isoFields.field4.chAmmount))>=0)            
        &&
        // Y el monto efectivamente dispensado NO es blancos
        (strncmp(isoFields.field95.chReversalAmmount, "            ",
                sizeof(isoFields.field95.chReversalAmmount))!=0) 
        &&
        // y NO sea ceros
        (strncmp(isoFields.field95.chReversalAmmount, "000000000000",
                sizeof(isoFields.field95.chReversalAmmount))!=0) 
      )
    {            
        // Copiar el monto dispensado como monto original, y...
        wRspCdeLen = 12;
        isoMsg.SetField(4,&(wRspCdeLen=12), 
            (PBYTE)isoFields.field95.chReversalAmmount,    12 );
        // ...indicar que es un FORZADO (220), y...
        isoMsg.SetMsgTypeId((PBYTE)CISO_MSGTYPE_FINANCIAL_ADV,4);
        // ...aplicar la transaccion por el monto realmente dispensado
        wRespCde = ProcessGeneric();
    };
    ///////////////////////////////////////////////////////////////////////////////////
    return (wRespCde);

}

///////////////////////////////////////////////////////////////////////
// Procesamiento de Transacciones ATM EXTRACT genericas, no en particular.
///////////////////////////////////////////////////////////////////////
WORD TrxResEXTRACT::ProcessGeneric(void)
{    
    // Respuesta default, metodo base siempre RECHAZA la transaccion
    // Quien debe implementar la resolucion de la misma, es la clase
    // especializada....
    WORD wRespCode = CISO_EXTERNAL_DECLINE_I;

    ///////////////////////////////////////////////////////////////////
    // Verificar si fue rechazada y era un SAF que debia impactarse
    if ( (isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() ) 
         && 
         ( wRespCode != CISO_SYSTEM_FAILURE_I && wRespCode != CISO_APPROVED_I)
        )
    {
        // Forzar el envio a SAF LOCAL cuando es SAF online que no 
        // pudo impactarse debido a cualquier tipo de error en el HOST.
        wRespCode = CISO_SYSTEM_FAILURE_I;
    };
    ///////////////////////////////////////////////////////////////////
    // Backup de resultado de ejecucion hacia la clase base
    wProcRespCde = wRespCode;        
    // Retornar codigo de rta.
    return (wRespCode);
}//end-ProcessGeneric



///////////////////////////////////////////////////////////////////////////////////
// Mapear codigo de respuesta EXTRACT hacia RED LINK    
// El codigo EXTRACT es de 7 digitos, tal que si todos son blancos es aprobada,
// y sino es un error de codigo alfanumerico que se mapea segun indican esta tablas
///////////////////////////////////////////////////////////////////////////////////
short TrxResEXTRACT::Mapear_RespCde_EXTRACT_REDLINK(char *szEXTRACT, char *szLINK, short nLen)
{
    // Precondicion     
    if(NULL == szEXTRACT || NULL == szLINK || nLen <= 0)
        // Postcondicion : 0=OK, -1=ERROR
        return (-1);
    // Segun codigo de error EXTRACT, mapear el respectivo Link en ISO8583:    
    switch( antoi(szEXTRACT,min(nLen,3)) )    
    {
        case 0  : /*  Falta de fondos en la cuenta  */ 
            strcpy( szLINK, CISO_APPROVED ); break; 
        default:   /* DEFAULT */
            strcpy( szLINK, CISO_EXTERNAL_DECLINE);break; 
    };
    ////////////////////////////////////////////////////////////////
    // Postcondicion : 0=OK, -1=ERROR
    return (0);
};

//////////////////////////////////////////////////////////////////////////////////
// Mapeo de codigo de transaccion Red Link ISO hacia EXTRACT Red Link
//////////////////////////////////////////////////////////////////////////////////
short TrxResEXTRACT::Transformar_TranCde_EXTRACT_REDLINK(  msgEXTRACT *pextractBuff )
{
    // Variable de soporte, estatica y local, ya que se retorna a la salida de la funcion
    static char chBuffer[sizeof(PROCCDE_ISO)+1]= { 0x00 };
    // Puntero para struct-casting
    PROCCDE_ISO    *pstB24TranCode = (PROCCDE_ISO *)chBuffer;

    /* precondicion */
    if(!pextractBuff)
        return (-1);
    
    /*************************************************************************/
    /* Mapeo de codigo de transaccion Red Link ISO hacia EXTRACT Red Link    */
    /* utilizando un metodo de la clase base : "TranslateTranCode_ISO_B24"   */
    /*************************************************************************/
    strncpy( chBuffer,  pextractBuff->cuerpo.stTRAN_CDE.T_CDE, sizeof(pextractBuff->cuerpo.stTRAN_CDE));
    /* Pasar a STRING ALFANUMERICO, invocando metodo base "TranslateTranCode_ISO_B24" */
    strncpy( pextractBuff->cuerpo.stTRAN_CDE.T_CDE,              
             TranslateTranCode_ISO_B24( chBuffer , _ISO8583_FORMAT_FOR_ATM_), 
			 sizeof(pextractBuff->cuerpo.stTRAN_CDE) );

    /* Postcondicion : Codigo de Transaccion Mapeado : 0=OK, -1=ERROR */
    return ( 0 );
};

///////////////////////////////////////////////////////////
// Verificar moneda de transaccion y cotizacion
// boolean_t *pbCrossXfer = es transferencia cruzada ?
// char chChangeOper  = 'C'=compra, 'V'=venta de moneda ?
// return boolean_t = Aplica tipo de cambio 'S','N' ?
///////////////////////////////////////////////////////////
boolean_t TrxResEXTRACT::Verificar_Cotizacion_y_Moneda(msgEXTRACT *pextractBuff , 
                                                    boolean_t *pbCrossXfer,
                                                    char *chChangeOper )
{
    boolean_t bApplyChange = is_false; /* inicialmente, no aplica tipo de cambio */
    boolean_t bCrossXfer   = is_false; /* transferencia cruzada ? */
    /* precondicion */
    if(!pextractBuff || !pbCrossXfer)
        return (is_false);


    /***********************************************************************************/
    /* ¿ Moneda de Trx. distinta a Moneda Cuenta en una extraccion o transferencia ? */
    /***********************************************************************************/    
    if( isoMsg.IsATMTransfer()          || 
        isoMsg.IsATMWithdrawal()        || 
        isoMsg.IsATMPayment()           ||
        isoMsg.IsATMBuying()            ||
        isoMsg.IsATMAutServicePayment()    )
    {
        /* Moneda de cuenta es PESOS pero moneda de transa NO lo es... */
        if ( (antoi(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,2)==_CEXTR_SAVINGS_ACCOUNT_ ||
              antoi(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,2)==_CEXTR_CHECKINGS_ACCOUNT_ )
            && 
            antoi(pextractBuff->cuerpo.ORIG_CRNCY_CDE,3) != _B24_CURRENCY_ARGENTINA_ )
        {
            bApplyChange = is_true; /* se aplica tipo de cambio */
            (*chChangeOper) = 'V'; /* vendedor */
        }
        /* Moneda de cuenta NO es PESOS pero moneda de transa SI lo es... */
        else if ( (antoi(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,2)==_CEXTR_SAVINGS_ACCOUNT_USD_ ||
                   antoi(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,2)==_CEXTR_CHECKINGS_ACCOUNT_USD_ )
                && 
                antoi(pextractBuff->cuerpo.ORIG_CRNCY_CDE,3) == _B24_CURRENCY_ARGENTINA_ )
        {
            bApplyChange = is_true; /* se aplica tipo de cambio */
            (*chChangeOper) = 'C'; /* comprador */
        }
        /* Moneda de cuenta destino NO es PESOS pero moneda de transa SI lo es... */
        else if ( (antoi(pextractBuff->cuerpo.stTRAN_CDE.T_TO,2)==_CEXTR_SAVINGS_ACCOUNT_USD_ ||
                   antoi(pextractBuff->cuerpo.stTRAN_CDE.T_TO,2)==_CEXTR_CHECKINGS_ACCOUNT_USD_ )
                && 
                antoi(pextractBuff->cuerpo.ORIG_CRNCY_CDE,3) == _B24_CURRENCY_ARGENTINA_ )
        {
            bApplyChange = is_false; /* NO se aplica tipo de cambio */
            bCrossXfer   = is_true;  /* Transferencia cruzada */
            (*chChangeOper) = 'V'; /* vendedor */
        }
        /* Moneda de cuenta destino es PESOS pero moneda de transa NO lo es... */
        else if ( (antoi(pextractBuff->cuerpo.stTRAN_CDE.T_TO,2)==_CEXTR_SAVINGS_ACCOUNT_ ||
                   antoi(pextractBuff->cuerpo.stTRAN_CDE.T_TO,2)==_CEXTR_CHECKINGS_ACCOUNT_ )
                && 
                antoi(pextractBuff->cuerpo.ORIG_CRNCY_CDE,3) != _B24_CURRENCY_ARGENTINA_ )
        {
            bApplyChange = is_false; /* NO se aplica tipo de cambio */
            bCrossXfer   = is_true;  /* Transferencia cruzada */
            (*chChangeOper) = 'C'; /* comprador */
        }
        /******************************************************************************/
        /* Moneda de cuenta NO es PESOS y moneda de transa TAMPOCO lo es... */
        else if ( (antoi(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,2)==_CEXTR_SAVINGS_ACCOUNT_USD_ ||
                   antoi(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,2)==_CEXTR_CHECKINGS_ACCOUNT_USD_ )
                && 
                antoi(pextractBuff->cuerpo.ORIG_CRNCY_CDE,3) != _B24_CURRENCY_USA_ )
        {
            bApplyChange = is_true; /* se aplica tipo de cambio */
            (*chChangeOper) = 'V'; /* vendedor para arbitraje */
        }
        /* Moneda de cuenta destino NO es PESOS y moneda de transa TAMPOCO lo es... */
        else if ( (antoi(pextractBuff->cuerpo.stTRAN_CDE.T_TO,2)==_CEXTR_SAVINGS_ACCOUNT_USD_ ||
                   antoi(pextractBuff->cuerpo.stTRAN_CDE.T_TO,2)==_CEXTR_CHECKINGS_ACCOUNT_USD_ )
                && 
                antoi(pextractBuff->cuerpo.ORIG_CRNCY_CDE,3) != _B24_CURRENCY_USA_ )
        {
            bApplyChange = is_false; /* NO se aplica tipo de cambio */
            bCrossXfer   = is_true;  /* Transferencia cruzada */
            (*chChangeOper) = 'C'; /* comprador para arbitraje */
        }
        /******************************************************************************/
        else
        {
            bApplyChange = is_false; /* no se aplica tipo de cambio */
        };
    };
    /* Postcondicion : aplica el tipo de cambio, S/N ?  */
    (*pbCrossXfer)= bCrossXfer;
    return ( bApplyChange );
};

///////////////////////////////////////////////////////////
// Transformar tipos de cuentas Red Link ISO hacia EXTRACT
///////////////////////////////////////////////////////////
short TrxResEXTRACT::Transformar_AcctTypes_EXTRACT_REDLINK( msgEXTRACT *pextractBuff )
{
    /***********************************************************************************/
    /* Conversion Tipo de Operacion y Tipos de Cuentas EXTRACT hacia LINK */
    /***********************************************************************************/
    /* Precondicion */
    if(!pextractBuff)
        return (-1);
    /*    Codigo de Operacion  */
       strncpy( pextractBuff->cuerpo.stTRAN_CDE.T_CDE, 
        isoFields.field3.uProcCde.stTranCde.chTran_Cde, 2);
    /*    Tipo cta Debito  */
    strncpy( pextractBuff->cuerpo.stTRAN_CDE.T_FROM,  
        isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ, 2);    
    /*  Validacion */
    if( strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,B24_CHECKINGS_ACCOUNT_USD,2) ==0|| 
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,B24_SAVINGS_ACCOUNT_USD,2) ==0  || 
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,B24_SAVINGS_ACCOUNT,2) ==0      || 
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,B24_CHECKINGS_ACCOUNT,2) ==0    ||
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,B24_CREDIT_CARD_ACCOUNT,2) ==0  ||
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,B24_CREDIT_LINE_ACCOUNT,2) ==0  ||
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,"00",2) ==0                      )
            ;
    else
        /* ERROR */
        return (-1);

    /*    Tipo cta Credito  */
    strncpy( pextractBuff->cuerpo.stTRAN_CDE.T_TO,  isoFields.field3.uProcCde.stTranCde.chTo_Acct_Typ, 2);    
    /*  Validacion  */
    if( strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_TO,B24_CHECKINGS_ACCOUNT_USD,2) ==0|| 
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_TO,B24_SAVINGS_ACCOUNT_USD,2) ==0  || 
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_TO,B24_SAVINGS_ACCOUNT,2) ==0      || 
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_TO,B24_CHECKINGS_ACCOUNT,2) ==0    ||
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_TO,B24_CREDIT_CARD_ACCOUNT,2) ==0  ||
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_TO,B24_CREDIT_LINE_ACCOUNT,2) ==0  ||
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_TO,"00",2) ==0                     ||
        strncmp(pextractBuff->cuerpo.stTRAN_CDE.T_TO,"02",2) ==0) /* caso especial RED LINK */
            ; 
    else
        /* ERROR */
        return (-1);

    /* OK ! */
    return (0);
};

///////////////////////////////////////////////////////////
// Es valido el monto de reversa parcial ? Copiarlo....
///////////////////////////////////////////////////////////
boolean_t TrxResEXTRACT::Completar_PartialRvslAmmount_EXTRACT_LINK( msgEXTRACT *pextractBuff )
{
    // Precondicion
    if(!pextractBuff)
        return is_false;
    // Si es reverso parcial, con resp-cde=32, y tiene un monto inferior al original
    if( isoMsg.IsValidReversal()  
        &&
        antoi(isoFields.field39.chRespCde,2)==CISO_REV_PARTIAL
        &&
        // Si DIFIERE el monto original del realmente dispensado... (PARTIAL RVSL)
        (strncmp(isoFields.field4.chAmmount,isoFields.field95.chReversalAmmount,
            sizeof(isoFields.field4.chAmmount))!=0)            
        &&
        // Y el monto efectivamente dispensado NO es blancos
        (strncmp(isoFields.field95.chReversalAmmount, "            ",
            sizeof(isoFields.field95.chReversalAmmount))!=0) 
        &&
        // y NO sea ceros
        (strncmp(isoFields.field95.chReversalAmmount, "000000000000",
            sizeof(isoFields.field95.chReversalAmmount))!=0) )
    {
        // Copiar monto de trx al monto original 
        strnset(pextractBuff->cuerpo.AMT_1, '0', sizeof pextractBuff->cuerpo.AMT_1);
        strncpy(pextractBuff->cuerpo.AMT_1+7, isoFields.field4.chAmmount, 12);
        // Copiar monto dispensado al monto de reversa
        strnset(pextractBuff->cuerpo.AMT_2, '0', sizeof pextractBuff->cuerpo.AMT_2);
        strncpy(pextractBuff->cuerpo.AMT_2+7, isoFields.field95.chReversalAmmount, 12);
        // Copiar ceros al disponible
        strnset(pextractBuff->cuerpo.AMT_3, '0', sizeof pextractBuff->cuerpo.AMT_3);
    }
    else if ( isoMsg.IsValidReversal() )
    {
        // Copiar monto de trx al monto original 
        strnset(pextractBuff->cuerpo.AMT_1, '0', sizeof pextractBuff->cuerpo.AMT_1);
        strncpy(pextractBuff->cuerpo.AMT_1+7, isoFields.field4.chAmmount, 12);
        // Copiar monto dispensado al monto de reversa
        strnset(pextractBuff->cuerpo.AMT_2, '0', sizeof pextractBuff->cuerpo.AMT_2);
		
		// 2006.02.14-mdc ------------------------------------------------------
		// Esta mal que si es un REVERSO TOTAL, copie un monto parcial al AMT-2,
		// ya que el mismo deberia ser cero :
        // strncpy(pextractBuff->cuerpo.AMT_2+7, isoFields.field4.chAmmount, 12);
		// 2006.02.14-mdc ------------------------------------------------------

        // Copiar ceros al disponible
        strnset(pextractBuff->cuerpo.AMT_3, '0', sizeof(pextractBuff->cuerpo.AMT_3));
    }
    else
    {
        // Copiar monto de trx al monto original 
        strnset(pextractBuff->cuerpo.AMT_1, '0', sizeof pextractBuff->cuerpo.AMT_1);
        strncpy(pextractBuff->cuerpo.AMT_1+7, isoFields.field4.chAmmount, 12);
        // Copiar ceros al saldo
        strnset(pextractBuff->cuerpo.AMT_2, '0', sizeof pextractBuff->cuerpo.AMT_2);
        // Copiar ceros al disponible
        strnset(pextractBuff->cuerpo.AMT_3, '0', sizeof pextractBuff->cuerpo.AMT_3);
    };

    // OK !
    return is_true;
};

///////////////////////////////////////////////////////////
// Completar montos cotizados - contrapartida
///////////////////////////////////////////////////////////
boolean_t TrxResEXTRACT::Completar_CrossXfer_Ammounts(msgEXTRACT *pextractBuff,
                                                   boolean_t bCrossWithd,
                                                   boolean_t bCrossXfer  ,
                                                   char chChangeOper )
{
    /* Auxiliares */
    double      dblCotizacion    = 1.0f,/* cotizacion comprador-vendedor */
                dblMontoOrig     = 0.0f;/* monto original binary */
    char        szOrigAmmt[19+1] = {0};     /* monto original ASCII */    
    char        szTempAmmt[19+1] = {0};     /* monto temporal ASCII */    
    char        szExchgRate[19+1]= {0};     /* tasa de conversion   */        
    boolean_t   bArbitraje       = is_false;/* usa arbitraje? */        

    /* Precondicion    */
    if(!pextractBuff)
        return is_false;
    /* Si es venta de dolares-reales-uruguayos en extraccion cruzada */
    if('V'==chChangeOper && bCrossWithd)
    {
        /******************** DOLARES o URUGUAYOS-REALES ? **********/
        if(_B24_CURRENCY_USA_ == antoi(isoFields.field49.chData,3))
        {
            isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='1';
            dblCotizacion = antof( pextractBuff->cuerpo.TIP_EXCHA_VEND,8,3);
        }
        /******************** PESOS? ********************************/
        else if(_B24_CURRENCY_ARGENTINA_ == antoi(isoFields.field49.chData,3))
        {
            isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='2';
            dblCotizacion = antof( pextractBuff->cuerpo.uDATOS.stDATOS2.TIP_EXCHA_COMP,8,3);
            chChangeOper = 'C';
        }
        /************************************************************/
        else
        {
            /* usa arbitraje */
            bArbitraje = is_true;
            isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='3';
            dblCotizacion = antof( pextractBuff->cuerpo.uDATOS.stDATOS2.ARBITRAJE,8,3);
        };
        /************************************************************/
    }        
    /* Si es compra de dolares-reales-uruguayos en extraccion cruzada */
    else if('C'==chChangeOper && bCrossWithd)
    {
        /******************** DOLARES o URUGUAYOS-REALES ? **********/
        if(_B24_CURRENCY_USA_ == antoi(isoFields.field49.chData,3))
        {
            isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='2';
            dblCotizacion = antof( pextractBuff->cuerpo.uDATOS.stDATOS2.TIP_EXCHA_COMP,8,3);
        }
        /******************** PESOS? ********************************/
        else if(_B24_CURRENCY_ARGENTINA_ == antoi(isoFields.field49.chData,3))
        {
            isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='1';
            dblCotizacion = antof( pextractBuff->cuerpo.TIP_EXCHA_VEND,8,3);
            chChangeOper = 'V';
        }
        /************************************************************/
        else
        {
            /* usa arbitraje */
            bArbitraje = is_true;
            isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic='4';
            dblCotizacion = antof( pextractBuff->cuerpo.uDATOS.stDATOS2.ARBITRAJE,8,3);
        };
        /************************************************************/        
    }
    else
        dblCotizacion = 0.0f;
    /* Monto original */
    dblMontoOrig  = antof( pextractBuff->cuerpo.AMT_1, 19, 2);
    /* Aplicar al monto original el tipo de cambio utilizado , */
    /* ya que en lainterfase EXTRACT el monto 1 no viene       */
    /* multiplicado por la cotizacion                          */
    if( dblCotizacion > 0.0f && dblMontoOrig > 0.0f && is_true == bCrossWithd )
    {                    
        /* Monto original convertido a binario para poder dividir por cotizacion */
        if('V'==chChangeOper)
            sprintf(szOrigAmmt, "%12.2f", dblMontoOrig * dblCotizacion );
        else
            sprintf(szOrigAmmt, "%12.2f", dblMontoOrig / dblCotizacion );        
        /* Formatear contrapartida en formato ASCII ISO */        
        Format_ISO_Ammount( szOrigAmmt, 0,szTempAmmt,sizeof(szTempAmmt)-1,is_true,szExchgRate);
        if(bArbitraje)
            strncpy(pextractBuff->cuerpo.AMT_1,szTempAmmt,sizeof(szTempAmmt)-1);
        /****************************************************/
    }
    else if( dblCotizacion > 0.0f && dblMontoOrig > 0.0f && is_true == bCrossXfer )
    {                    
        /* Monto original convertido a binario para poder dividir por cotizacion */
        /* Es Vendedor ? */
        if('V'==chChangeOper)
            sprintf(szOrigAmmt, "%12.2f", dblMontoOrig * dblCotizacion );
        /* Es Comprador ? */
        else
            sprintf(szOrigAmmt, "%12.2f", dblMontoOrig / dblCotizacion );
        /* Formatear contrapartida en formato ASCII ISO */        
        Format_ISO_Ammount( szOrigAmmt, 0,szTempAmmt,sizeof(szTempAmmt)-1,is_true,szExchgRate);
        if(bArbitraje)
            strncpy(pextractBuff->cuerpo.AMT_1,szTempAmmt,sizeof(szTempAmmt)-1);
    }
    else
    {
        /* Nada, no hay cotizaciones.... 1 a 1 en moneda local */
    };
    /* OK */
    return is_true;    
};


///////////////////////////////////////////////////////////////////////////////////////////////
// Completar campos de la estructura EXTRACT desde el mensaje ISO 
// Este es un procedimiento por default, puede variar en superclases que lo hereden
////////////////////////////////////////////////////////////////////////////////////////////////
WORD TrxResEXTRACT::Completar_MSG_EXTRACT(msgEXTRACT *pextractBuff)
    {    
    // Variables de soporte
    INT         iLeng     = 0,
                iRetval   = 0;
    WORD        wBuff     = 0,
                wRespCode = CISO_EXTERNAL_DECLINE_I ; /* respuesta default */
    char        szMsgTypeID[4+1]  = {0},
                szRespCode[8]     = { CISO_EXTERNAL_DECLINE }, /* preasignado error="05" */
                szISOHeader[16+1] = { "ISO0000000550000" }; /* header ISO8583 default */
    char        *pszCard      = NULL;
    boolean_t   bApplyChange      = is_false; /* inicialmente, no aplica tipo de cambio */
    boolean_t   bCrossXfer        = is_false; /* transferencia cruzada ? */
    char        chChangeOper      = 0x00;     /* 'C'=Compra, 'V'=venta de moneda ? */

    /************************************************/
    /* blanqueo inicial con CEROS display           */
    /************************************************/
	memset(pextractBuff,'0',sizeof(msgEXTRACT) );
    pextractBuff->chETX = 0x03; /* END-OF-TEXT */
    pextractBuff->chEOF = 0x0A; /* END-OF-FILE */
    pextractBuff->chNUL = 0x00;
    /************************************************/
    
    /***********************************************************************************/
    /* copia de campos header EXTRACT */
    /***********************************************************************************/    
	// cambiar host24 por header o footer segun corresponda.
	

    ASSIGNFIELD(pextractBuff->header.PREFIX1   ,"HOST24" );
    ASSIGNFIELD(pextractBuff->header.PREFIX2   ,"99"     ); /* QUE NO SEA CEROS */
    FormatTrxDateTime_WMask( pextractBuff->header.DAT_TIM, pextractBuff->header.DAT_TIM, 
                            NULL, NULL, NULL, is_false , is_true, is_false, is_true,
                            sizeof pextractBuff->header.DAT_TIM);    
    ASSIGNFIELD(pextractBuff->header.REC_TYP   ,"01"      ); 
    ASSIGNFIELD(pextractBuff->header.AUTH_PPD  ,"H24 "    );
    ASSIGNFIELD(pextractBuff->header.TERM_LN     ,isoFields.field60.stATMTerm.chTermLNet );
    ASSIGNFIELD(pextractBuff->header.TERM_FIID   ,isoFields.field60.stATMTerm.chTermFIID );
    ASSIGNFIELD(pextractBuff->header.TERM_ID     ,isoFields.field41.chData               );
    ASSIGNFIELD(pextractBuff->header.CARD_LN     ,isoFields.field61.stATM_Data.chCrdLNet );
    ASSIGNFIELD(pextractBuff->header.CARD_FIID   ,isoFields.field61.stATM_Data.chCrdFIID );
    ASSIGNFIELD(pextractBuff->header.CARD_PAN    ,isoFields.field34.chNum                );
    /***********************************************************************************/
    /* Longitud y contenido de TARJETA EXTRACT                                         */
    /***********************************************************************************/
    pszCard = ExtractCardnumFromTrack2( &isoFields.field35 );        
    if(pszCard)    iLeng = strnumlen ( pszCard , _MAX_CARDNUM_LEN_ );
    else           iLeng = 0;
    if(NULL != pszCard && iLeng > 0)
        strncpy( pextractBuff->header.CARD_PAN, pszCard, min(_MAX_CARDNUM_LEN_,iLeng));  
    /***********************************************************************************/    
    ASSIGNFIELD(pextractBuff->header.CARD_MBR_NUM,isoFields.field23.chData  );
    sprintf(pextractBuff->header.CARD_MBR_NUM,"%03i", 
            antoi( isoFields.field23.chData, sizeof(isoFields.field23)) ); /* MEMBER_NUM */
    ASSIGNFIELD(pextractBuff->header.BRCH_ID     ,isoFields.field120.stATM.chBrch_Id );
    ASSIGNFIELD(pextractBuff->header.REGN_ID     ,isoFields.field120.stATM.chRegn_Id );
    ASSIGNFIELD(pextractBuff->header.USER_FLD1X  ,"00"                               );    
    
    /***********************************************************************************/
    /* cuerpo */
    /***********************************************************************************/        
    isoMsg.GetMsgTypeId((PBYTE)szMsgTypeID,sizeof(szMsgTypeID));
    ASSIGNFIELD(pextractBuff->cuerpo.TYP_CDE,"31" );
    /* tipo de mensaje ISO : requerimiento, reversa o forzada ? */
    if(strcmp(szMsgTypeID,CISO_MSGTYPE_FINANCIAL_RQST)==0)
        /* Se procesan los 210, formato EXTRACT */
        COPYSTRING(szMsgTypeID,CISO_MSGTYPE_FINANCIAL_RESP);
    else if(strcmp(szMsgTypeID,CISO_MSGTYPE_FINANCIAL_ADV_REP)==0 ||
            strcmp(szMsgTypeID,CISO_MSGTYPE_FINANCIAL_ADV)==0)
        /* Se procesan los 221 como 220, formato EXTRACT */
        COPYSTRING(szMsgTypeID,CISO_MSGTYPE_FINANCIAL_ADV);
    else if(strcmp(szMsgTypeID,CISO_MSGTYPE_REVERSAL_REP)==0 ||
            strcmp(szMsgTypeID,CISO_MSGTYPE_REVERSAL)==0)
        /* Se procesan los 421 como 420, formato EXTRACT */
        COPYSTRING(szMsgTypeID,CISO_MSGTYPE_REVERSAL);
    else if(strcmp(szMsgTypeID,CISO_MSGTYPE_FINANCIAL_ADV)    !=0 &&
            strcmp(szMsgTypeID,CISO_MSGTYPE_FINANCIAL_ADV_REP)!=0 &&
            strcmp(szMsgTypeID,CISO_MSGTYPE_REVERSAL)         !=0 &&
            strcmp(szMsgTypeID,CISO_MSGTYPE_REVERSAL_REP)     !=0 )
    {    
        glLogg.Put("Tipo de Mensaje Transaccional INVALIDO:<%s> \n", szMsgTypeID );            
        /* Transa no sooprtada, codigo de error para ISO8583 */
        return (wRespCode = CISO_TRANSACTION_NOT_SUPP_I);
    };
    ASSIGNFIELD(pextractBuff->cuerpo.TYP,szMsgTypeID );
    ASSIGNFIELD(pextractBuff->cuerpo.RTE_STAT,"00"   );    
    isoMsg.GetHeader( szISOHeader, sizeof szISOHeader );
    pextractBuff->cuerpo.ORIGINATOR = szISOHeader[10] ;
    pextractBuff->cuerpo.RESPONDER  = szISOHeader[11] ;        
    /* Entry-TimeStamp con formato YYYYMMDDHHMMSS00000 */
    FormatTrxDateTime_WMask( pextractBuff->cuerpo.ENTRY_TIM, pextractBuff->cuerpo.ENTRY_TIM, 
                            NULL, NULL, NULL, is_false , is_true, is_false, is_true,
                            sizeof pextractBuff->cuerpo.ENTRY_TIM);
    ASSIGNFIELD(pextractBuff->cuerpo.EXIT_TIM    ,pextractBuff->cuerpo.ENTRY_TIM );
    ASSIGNFIELD(pextractBuff->cuerpo.RE_ENTRY_TIM,pextractBuff->cuerpo.ENTRY_TIM );
    /* Trx-Date y Post-Date con formato YYMMDD */
    FormatTrxDateTime_WMask( NULL, NULL, 
                            pextractBuff->cuerpo.TRAN_DAT, pextractBuff->cuerpo.TRAN_TIM, 
                            pextractBuff->cuerpo.POST_DAT, 
                            is_false , is_false, is_false, is_false,
                            0 , sizeof pextractBuff->cuerpo.TRAN_TIM);    
    ASSIGNFIELD(pextractBuff->cuerpo.ACQ_ICHG_SETL_DAT,pextractBuff->cuerpo.POST_DAT );
    ASSIGNFIELD(pextractBuff->cuerpo.ISS_ICHG_SETL_DAT,pextractBuff->cuerpo.POST_DAT );
    /********************************************************************************************/
    /* Correccion de error cuando el nro. de referencia es superior a 9 digitos numericos       */
    /* FIXINTEGER tiene mascara "%I64i" para enteros de 64 bits Microsoft Visual C/C++          */
    /* FIXINTEGER tiene mascara "%.0f"  para reales  de 64/80 bits, cualquier C/C++             */
    /********************************************************************************************/
#ifdef _WIN32 
    sprintf( pextractBuff->cuerpo.SEQ_NUM, "%012I64i", antofix(isoFields.field37.chNum,12));
#else
    sprintf( pextractBuff->cuerpo.SEQ_NUM, "%012.0f" , antofix(isoFields.field37.chNum,12));
#endif     // _WIN32
    /********************************************************************************************/
    ASSIGNFIELD(pextractBuff->cuerpo.TERM_TYP,"00" );
    SETFIELDZEROED(pextractBuff->cuerpo.TIM_OFST);
    /* time offset expresado en minutos */
    sprintf( pextractBuff->cuerpo.TIM_OFST, "%05i",         
        antoi( isoFields.field60.stATMTerm.chTermTimeOffset + 1,
                sizeof isoFields.field60.stATMTerm.chTermTimeOffset ) * 60 );     
    if( isoMsg.IsValidField(32) ) 
        sprintf( pextractBuff->cuerpo.ACQ_INST_ID_NUM, "%11.11s", isoFields.field32.chNum );    
    else
        SETFIELDZEROED(pextractBuff->cuerpo.ACQ_INST_ID_NUM);
    if( isoMsg.IsValidField(33) ) 
        sprintf( pextractBuff->cuerpo.RCV_INST_ID_NUM, "%11.11s", isoFields.field33.chNum );        
    else
        SETFIELDZEROED(pextractBuff->cuerpo.RCV_INST_ID_NUM);
    /**************************************************************************************************/
    ASSIGNFIELD(pextractBuff->cuerpo.stTRAN_CDE.T_CDE ,isoFields.field3.uProcCde.stTranCde.chTran_Cde     );
    ASSIGNFIELD(pextractBuff->cuerpo.stTRAN_CDE.T_FROM,isoFields.field3.uProcCde.stTranCde.chFrom_Acct_Typ);
    ASSIGNFIELD(pextractBuff->cuerpo.stTRAN_CDE.T_TO  ,isoFields.field3.uProcCde.stTranCde.chTo_Acct_Typ  );
    /**************************************************************************************************/
    /* Conversion Tipo de Operacion y Tipos de Cuentas EXTRACT hacia LINK */
    /**************************************************************************************************/
    if(Transformar_AcctTypes_EXTRACT_REDLINK( pextractBuff )!=0)
    {    
        glLogg.Put("Cod.Transaccion INVALIDO:<%6.6s> \n", pextractBuff->cuerpo.stTRAN_CDE.T_CDE );            
        /* Transa no soportada, codigo de error para ISO8583 */
        return (wRespCode = CISO_TRANSACTION_NOT_SUPP_I);
    };
    /**************************************************************************************************/
    if( isoMsg.IsValidField(102) ) {
        ASSIGNFIELD(pextractBuff->cuerpo.FROM_ACCT,isoFields.field102.chNum );
    } else {
        SETFIELDZEROED( pextractBuff->cuerpo.FROM_ACCT );    
    };
    pextractBuff->cuerpo.TIPO_DEP = isoFields.field54.stB24_0200_Def_Data.chTIPO_DEP;
    if( isoMsg.IsValidField(103) ) {
        ASSIGNFIELD(pextractBuff->cuerpo.TO_ACCT  ,isoFields.field103.chNum );
    } else {
        SETFIELDZEROED( pextractBuff->cuerpo.TO_ACCT );    
    };
    pextractBuff->cuerpo.MULT_ACCT = 'N';
    /********************************************************************************************/
    /* Monto de transaccion : desde longitud ISO de 12 digitos a longitud EXTRACT de 19 digitos */
    if( isoMsg.IsValidField(4) ) {
        strnset( pextractBuff->cuerpo.AMT_1  , '0', sizeof pextractBuff->cuerpo.AMT_1 );
        strncpy( pextractBuff->cuerpo.AMT_1+7, isoFields.field4.chAmmount, 12);
    } else {
        SETFIELDZEROED( pextractBuff->cuerpo.AMT_1 );    
    }
    SETFIELDZEROED( pextractBuff->cuerpo.AMT_2 );    
    SETFIELDZEROED( pextractBuff->cuerpo.AMT_3 );    
    /********************************************************************************************/
    SETFIELDZEROED(pextractBuff->cuerpo.FILLER1);
    pextractBuff->cuerpo.DEP_TYP = '0'; /*  Deposito con sobre, siempre que corresponda */
    ASSIGNFIELD(pextractBuff->cuerpo.RESP_CDE,"000"); /* valores originalmente EN CERO */
    if ( isoMsg.IsValidField(120) ) {
        ASSIGNFIELD(pextractBuff->cuerpo.uTERM_NAME_LOC.TERM_NAME_LOC , 
            isoFields.field120.stATM.chTerm_Nam_Loc );        
    } else {
        SETFIELDBLANKED(pextractBuff->cuerpo.uTERM_NAME_LOC.TERM_NAME_LOC);
    }
    ASSIGNFIELD(pextractBuff->cuerpo.uTERM_OWNER_NAME.TERM_OWNER_NAME, isoFields.field43.stCrdAccptNameLoc.chOwner);
    if( isoMsg.IsATMTransferCBU() ) {
        ASSIGNFIELD(pextractBuff->cuerpo.uTERM_OWNER_NAME.NUM_CBU, isoFields.field54.stTransferenciaCBU.chCBU);
    };
    ASSIGNFIELD(pextractBuff->cuerpo.TERM_CITY   ,isoFields.field43.stCrdAccptNameLoc.chCity );
    ASSIGNFIELD(pextractBuff->cuerpo.TERM_ST_X   ,isoFields.field43.stCrdAccptNameLoc.chState);
    ASSIGNFIELD(pextractBuff->cuerpo.TERM_CNTRY_X,isoFields.field43.stCrdAccptNameLoc.chCountry);
    if ( isoMsg.IsValidField(90) )
    {
        ASSIGNFIELD(pextractBuff->cuerpo.OSEQ_NUM    ,isoFields.field90.stB24Orig.chSeq_Num );
        ASSIGNFIELD(pextractBuff->cuerpo.OTRAN_DAT   ,isoFields.field90.stB24Orig.chXmit_Dat );
        ASSIGNFIELD(pextractBuff->cuerpo.OTRAN_TIM   ,isoFields.field90.stB24Orig.chXmit_Tim );
        ASSIGNFIELD(pextractBuff->cuerpo.B24_POST_DAY,isoFields.field90.stB24Orig.chPosting_Dat );
    }
    else
    {
        SETFIELDBLANKED( pextractBuff->cuerpo.OSEQ_NUM );
        SETFIELDBLANKED( pextractBuff->cuerpo.OTRAN_DAT );
        SETFIELDBLANKED( pextractBuff->cuerpo.OTRAN_TIM );
        SETFIELDBLANKED( pextractBuff->cuerpo.B24_POST_DAY );
    };
    ASSIGNFIELD(pextractBuff->cuerpo.ORIG_CRNCY_CDE,isoFields.field49.chData );
    /***********************************************************************************/
    /* Codigo de moneda de la transaccion VALIDOS */
    if(_B24_CURRENCY_USA_       == antoi(isoFields.field49.chData,3) || 
       _B24_CURRENCY_ARGENTINA_ == antoi(isoFields.field49.chData,3) ||
       _B24_CURRENCY_BRAZIL_    == antoi(isoFields.field49.chData,3) ||
       _B24_CURRENCY_URUGUAY_   == antoi(isoFields.field49.chData,3)  )
        /* MONEDAS VALIDAS SON ARGENTINOS, DOLARES EE.UU., REALES y URUGUAYOS */
    {
        ASSIGNFIELD( pextractBuff->cuerpo.ORIG_CRNCY_CDE, isoFields.field49.chData  );
    }
    else
        {
        //////////////////////////////////////////////////////////////////////////
        // Formatear campo ISO #44, RESP_DATA, saldo y disponible
        FormatField44_RESP_DATA( NULL, NULL , NULL);
        // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
        FormatField122_SECNDRY_RSRVD3_PRVT();
        // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
        FormatField123_SECNDRY_RSRVD4_PRVT();
        // Borrar campos no validos en la rta. : 43,48,52,63
        isoMsg.DeleteField ( 43 );
        isoMsg.DeleteField ( 48 );
        isoMsg.DeleteField ( 52 );
        isoMsg.DeleteField ( 63 );
        // Borrar campo 120, OPCIONALMENTE PODRIAN NO VENIR EN UN REQUERIMIENTO 
        isoMsg.DeleteField ( 120 );
        //////////////////////////////////////////////////////////////////////////
        COPYSTRING(szRespCode, CISO_TRANSACTION_NOT_SUPP);
        if(isoMsg.IsValidField( 39 ))
            isoMsg.SetField(39,&(wBuff=2),(PBYTE)szRespCode, 2); 
        else
            isoMsg.InsertField(39,&(wBuff=2),(PBYTE)szRespCode, 2); 
        /* Codigo de moneda invalido */
        glLogg.Put("Cod.de Moneda es INVALIDO : %3.3s\n", isoFields.field49.chData);            
        /* Sino, trx. no soportada / error generico */
        return (wRespCode = CISO_TRANSACTION_NOT_SUPP_I);
        };
    /***********************************************************************************/

    /* Clearing por default en 48hs */
    ASSIGNFIELD(pextractBuff->cuerpo.uDATOS.stDATOS2.TIPO_CLEARING,"48" );
    ASSIGNFIELD(pextractBuff->cuerpo.uDATOS.stDATOS2.FILLER2      ,"0000" );
    /* Tipos de Cambio por default , luego se ajustan segun moneda */
    ASSIGNFIELD(pextractBuff->cuerpo.uDATOS.stDATOS2.TIP_EXCHA_COMP,
        isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C );
    /* Segun sea comprador o vendedor, se copia el ARBITRAJE */
    /* Comparar hasta 8 digitos "999.99999" */
    if( antoi(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1,
        sizeof isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1) != 0) {
        ASSIGNFIELD(pextractBuff->cuerpo.uDATOS.stDATOS2.ARBITRAJE,
            isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1 );
    } else if( antoi(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2,
        sizeof isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2) != 0) {
        ASSIGNFIELD(pextractBuff->cuerpo.uDATOS.stDATOS2.ARBITRAJE,
            isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2 );
    } else {
        SETFIELDZEROED(pextractBuff->cuerpo.uDATOS.stDATOS2.ARBITRAJE);
    };
    ASSIGNFIELD(pextractBuff->cuerpo.TIP_EXCHA_VEND,
        isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V );
    SETFIELDZEROED( pextractBuff->cuerpo.FILLER3 );    
    /**********************************************************************************/
    /* Codigo de Respuesta o Causal de Reversa, segun corresponda                     */
    /**********************************************************************************/
    if( isoMsg.IsValidReversal() )    /* Es un Reverso valido ? */
    { 
        /* Recuperar codigo de reverso ISO */
        isoMsg.GetField(39, &(wBuff=2), (PBYTE)szRespCode, 2);
        iRetval = antoi(szRespCode,2);
        /********************************************************************/
        /* Mapear a REVERSAL REASON en el EXTRACT, ver Capitulo V.7 de LINK */
        /********************************************************************/
        if(CISO_REV_TIMEOUT == iRetval) {
            ASSIGNFIELD(pextractBuff->cuerpo.RVSL_RSN,CEXTR_REV_TIMEOUT);
        } else if(CISO_REV_COMMAND_REJECTED == iRetval) {
            ASSIGNFIELD(pextractBuff->cuerpo.RVSL_RSN,CEXTR_REV_COMMAND_REJECTED);
        } else if(CISO_REV_DEST_NOT_AVAIL == iRetval) {
            ASSIGNFIELD(pextractBuff->cuerpo.RVSL_RSN,CEXTR_REV_DEST_NOT_AVAIL);
        } else if(CISO_REV_CANCELLED_TRX == iRetval) {
            ASSIGNFIELD(pextractBuff->cuerpo.RVSL_RSN,CEXTR_REV_CANCELLED_TRX);
        } else if(CISO_REV_HARDWARE_ERROR == iRetval) {
            ASSIGNFIELD(pextractBuff->cuerpo.RVSL_RSN,CEXTR_REV_HARDWARE_ERROR);
        } else if(CISO_REV_SUSPICIUS_TRX == iRetval) {
            ASSIGNFIELD(pextractBuff->cuerpo.RVSL_RSN,CEXTR_REV_SUSPICIUS_TRX);
        } else {
            ASSIGNFIELD(pextractBuff->cuerpo.RVSL_RSN,isoFields.field39.chRespCde );
        };
    } else {
        ASSIGNFIELD(pextractBuff->cuerpo.RVSL_RSN,"00" );
    };
    ASSIGNFIELD(pextractBuff->cuerpo.PIN_OFST,isoFields.field52.chData );
    pextractBuff->cuerpo.SHRG_GRP = isoFields.field48.ATM.chSHRG_GRP[0];
    SETFIELDZEROED(pextractBuff->cuerpo.tCAMPOS_NUEVOS_3.FILLER4);
	// Agrego letra inicio de filler - 2013.08.13
	// Fixear y mandar marca consolidado
	//pextractBuff->cuerpo.tCAMPOS_NUEVOS_3.FILLER4[0] = isoFields.field25.chData[0];
	//for(int i=0, j=1; i<22; ++i, ++j)
	//	pextractBuff->cuerpo.tCAMPOS_NUEVOS_3.FILLER4[j] = isoFields.field125.chData[i];

	//Marca consolidado
	pextractBuff->cuerpo.tCAMPOS_NUEVOS_3.stDATOS3.CONSOLIDADO[0] = isoFields.field25.chData[0];
	//CUIT DEBITO
	for(int i=0; i<11; i++)
		pextractBuff->cuerpo.tCAMPOS_NUEVOS_3.stDATOS3.CUIT_DEBITO[i] = isoFields.field125.chData[i];
	//CUIT CREDITO
	for(int j=0, i=11; i<22; i++, j++)
		pextractBuff->cuerpo.tCAMPOS_NUEVOS_3.stDATOS3.CUIT_CREDITO[j] = isoFields.field125.chData[i];
	//NOMBRE DE SOLICITANTE
	for(int j=0, i=22; i<22+24; i++, j++)
		pextractBuff->cuerpo.tCAMPOS_NUEVOS_3.stDATOS3.NOMBRE_SOLICITANTE[j] = isoFields.field125.chData[i];
	//NOMBRE DE BENEFICIARIO
	for(int j=0, i=24+22; i<22+24+24; i++, j++)
		pextractBuff->cuerpo.tCAMPOS_NUEVOS_3.stDATOS3.NOMBRE_BENEFICIARIO[j] = isoFields.field125.chData[i];

	

    pextractBuff->cuerpo.uUSER_FLD2X.tCAMPOS_NUEVOS_2.TIPO_DEP = 
        isoFields.field54.stB24_0200_Def_Data.chTIPO_DEP;
    ASSIGNFIELD(pextractBuff->cuerpo.uUSER_FLD2X.tCAMPOS_NUEVOS_2.ISSUER_FIID, 
        isoFields.field61.stATM_Data.chCrdFIID );
    ASSIGNFIELD(pextractBuff->cuerpo.uUSER_FLD2X.tCAMPOS_NUEVOS_2.INTEREST_RATE,"0000"   );
    ASSIGNFIELD(pextractBuff->cuerpo.uUSER_FLD2X.tCAMPOS_NUEVOS_2.CASH_FEE     ,"000000" );
    ASSIGNFIELD(pextractBuff->cuerpo.uUSER_FLD2X.tCAMPOS_NUEVOS_2.CARD_TYPE    ,"00"     );
    pextractBuff->cuerpo.uUSER_FLD2X.tCAMPOS_NUEVOS_2.SITUACION_IVA = '0';

	// 2013.08.15 - JAF - copio numero de referencia
	pextractBuff->cuerpo.uUSER_FLD2X.tCAMPOS_NUEVOS_2.FILLER = '0' ;
	sprintf( pextractBuff->cuerpo.uUSER_FLD2X.tCAMPOS_NUEVOS_2.NRO_REFERENCIA, 
			"%11.11s", 
			isoFields.field32.chNum );    

	/***********************************************************************************/
    /* Transformar codigo de transaccion ISO-Red Link hacia EXTRACT-Red Link           */
    /***********************************************************************************/
    if(Transformar_TranCde_EXTRACT_REDLINK( pextractBuff )!=0)
        {
        //////////////////////////////////////////////////////////////////////////
        // Formatear campo ISO #44, RESP_DATA, saldo y disponible
        FormatField44_RESP_DATA( NULL, NULL , NULL);
        // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
        FormatField122_SECNDRY_RSRVD3_PRVT();
        // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
        FormatField123_SECNDRY_RSRVD4_PRVT();
        // Borrar campos no validos en la rta. : 43,48,52,63
        isoMsg.DeleteField ( 43 );
        isoMsg.DeleteField ( 48 );
        isoMsg.DeleteField ( 52 );
        isoMsg.DeleteField ( 63 );
        // Borrar campo 120, OPCIONALMENTE PODRIAN NO VENIR EN UN REQUERIMIENTO 
        isoMsg.DeleteField ( 120 );
        //////////////////////////////////////////////////////////////////////////
        COPYSTRING(szRespCode, CISO_INVALID_TRANSACTION);
        if(isoMsg.IsValidField( 39 ))
            isoMsg.SetField(39,&(wBuff=2),(PBYTE)szRespCode, 2); 
        else
            isoMsg.InsertField(39,&(wBuff=2),(PBYTE)szRespCode, 2); 
        /* Sino, transaccion no soportada / error generico */
        return (wRespCode = CISO_INVALID_TRANSACTION_I);
        };


    /***********************************************************************************/
    /* ¿ Moneda distinta a moneda cuenta en una extraccion o transferencia ? */
    /***********************************************************************************/
    /* bApplyChange = Verificar_Cotizacion_y_Moneda( pextractBuff , &bCrossXfer, &chChangeOper ); */
    /* Jamas cambiar el FLAG de multiplicacion por TIPO DE CAMBIO  */
    Verificar_Cotizacion_y_Moneda( pextractBuff , &bCrossXfer, &chChangeOper );


    /************************************************************************************/
    /* Tipos de Cambio comprador y vendedor                                             */
    /************************************************************************************/
    if( isoMsg.IsValidField(127) )
    {
        /*******************************************************************************/
        /* Segun moneda, tomar cotizacion                                              */
        /*******************************************************************************/
        if(_B24_CURRENCY_USA_ == antoi(isoFields.field49.chData,3))        
        {
            ASSIGNFIELD( pextractBuff->cuerpo.uDATOS.stDATOS2.TIP_EXCHA_COMP, 
                isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C  );
        }
        else if(_B24_CURRENCY_BRAZIL_ == antoi(isoFields.field49.chData,3) ||
                _B24_CURRENCY_URUGUAY_ == antoi(isoFields.field49.chData,3))
        {
            if('C'==chChangeOper &&
               antoi(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1,
                 sizeof isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1)!=0)
            {
                ASSIGNFIELD( pextractBuff->cuerpo.uDATOS.stDATOS2.ARBITRAJE, 
                    isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1  );
            } else {
                ASSIGNFIELD( pextractBuff->cuerpo.uDATOS.stDATOS2.ARBITRAJE, 
                    isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C  );
            };
        }
        else 
        {
            ASSIGNFIELD( pextractBuff->cuerpo.uDATOS.stDATOS2.TIP_EXCHA_COMP, 
                isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C  );
        };
        /*******************************************************************************/
    };
    
    if( isoMsg.IsValidField(127) )
    {
        /*******************************************************************************/
        /* Segun moneda, tomar cotizacion                                              */
        /*******************************************************************************/
        if(_B24_CURRENCY_USA_ == antoi(isoFields.field49.chData,3))        
        {
            ASSIGNFIELD( pextractBuff->cuerpo.TIP_EXCHA_VEND, 
                isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V  );
        }
        else if(_B24_CURRENCY_BRAZIL_ == antoi(isoFields.field49.chData,3) ||
                _B24_CURRENCY_URUGUAY_ == antoi(isoFields.field49.chData,3)) 
        {
            if('V'==chChangeOper &&
               antoi(isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2,
                 sizeof isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2)!=0)
            {
                ASSIGNFIELD( pextractBuff->cuerpo.uDATOS.stDATOS2.ARBITRAJE, 
                    isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2  );
            } else {
                ASSIGNFIELD( pextractBuff->cuerpo.uDATOS.stDATOS2.ARBITRAJE, 
                    isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V  );
            };
        }
        else 
        {
            ASSIGNFIELD( pextractBuff->cuerpo.TIP_EXCHA_VEND, 
                isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V  );
        };
        /*******************************************************************************/
    };
    
    /************************************************************************************/
    /* Montos de reversos parciales y totales, si hubiere                               */
    /************************************************************************************/
    Completar_PartialRvslAmmount_EXTRACT_LINK( pextractBuff );

    /**********************************************************************************/
    /* Monto en moneda del exterior, dividido el tipo de cambio vendedor              */     
    /**********************************************************************************/    
    Completar_CrossXfer_Ammounts( pextractBuff, bApplyChange, bCrossXfer, chChangeOper );
    /**********************************************************************************/
    
    
    /**********************************************************************************/
    /* ultimo campo es ETX, end of text */    
    /**********************************************************************************/
    pextractBuff->chETX = 0x03; /* END-OF-TEXT */
    pextractBuff->chEOF = 0x0A; /* END-OF-FILE */
    pextractBuff->chNUL = 0x00;
    /**********************************************************************************/

    /* Ok ! */
    return (wRespCode = CISO_APPROVED_I);
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
// Verificaciones varias al procesamiento ONLINE : reversos dobles, triples e 
// impacto previa, ademas de chequear si un SAF fue rechazado y debe ir al SAF local.
// Algunos metodos deberan redefinirse segun la conexion a la base de datos que se
// posea, sino, deberan implementarse en el autorizador de c/institucion.
/////////////////////////////////////////////////////////////////////////////////////
boolean_t TrxResEXTRACT::Verify_Duplicated_Trx(msgEXTRACT *extractBuff)
{
    return is_false;
};
///////////////////////////////////////////////////////////////////////////////////
// Excepciones para los reversos 420 y 421
boolean_t TrxResEXTRACT::Verify_Reversal_Exceptions(msgEXTRACT *extractBuff)
{
    // Situacion de excepcion de Mensaje 420 o 421 (reverso y repeticion de reverso) : 
    // verificar si el mensaje corresponde  a la fecha de posting date. 
    // Si no corresponde, no aplicar mensaje. 
    // Si corresponde, verificar la existencia previa de un 420 ya aplicado por 
    // el mismo motivo de reverso, tal que si existe, NO debe aplicarse nuevamente. 
    // Si no existiera, verificar si el motivo del reverso es time-out (68) . 
    // Si no es asi, aplicar el reverso. 
    // Si en cambio es time-out, verificar si la transaccion original fue respondida y 
    // aprobada con un 210, tal que si es asi, reversar la misma aplicando el mensaje 420 
    // en curso. 
    // Si no, no aplicarlo.
    return is_false;
};
///////////////////////////////////////////////////////////////////////////////////
// Excepciones para los avisos forzados 220 y 221
boolean_t TrxResEXTRACT::Verify_Advice_Exceptions(msgEXTRACT *extractBuff)
{
    // Situacion de excepcion de Mensaje 220 o 221 (forzado y repeticion de forzado) : 
    // verificar si el mensaje corresponde  a la fecha de posting date. 
    // Si no corresponde, no aplicar mensaje. Si corresponde, verificar la existencia 
    // previa de un 200 ya aplicado, tal que si existe, NO debe aplicarse nuevamente. 
    // Si no existe, verificar si hay un mensaje requerimiento 200 previo y si el mismo 
    // fue aprobado (210) y no reversado (420). 
    // Si existe, reversalo o cancelarlo y aplicar el 220 en curso. 
    // Si no existe una respuesta 210 o no fue reversado, aplicar el 220 en curso.
    return is_false;
};
///////////////////////////////////////////////////////////////////////////////////
// Verificar si un aviso forzado fue rechazado (en base al RESP-CDE)
boolean_t TrxResEXTRACT::Verify_Rejected_SAF( WORD wPrevRespCode )
{
    ///////////////////////////////////////////////////////////////////
    // Verificar si fue rechazada y era un SAF que debia impactarse:
    // Todo SAF debe impactarse PERO si por algun motivo el RESP-CDE
    // es erroneo (no se impacto) se manda al SAF local y se reintenta
    // una vez mas (para evitar congestiones temporales y deadlocks).
    if ( (isoMsg.IsFinancialAdvice() || isoMsg.IsValidReversal() ) 
         && 
         ( wPrevRespCode != CISO_SYSTEM_FAILURE_I 
           && 
           wPrevRespCode != CISO_APPROVED_I ) )
        return is_true;
    else
        return is_false;
    ///////////////////////////////////////////////////////////////////
};
///////////////////////////////////////////////////////////////////////////////////
// Conversion de signo del monto del EXTRACT (positivo y negativo)
void TrxResEXTRACT::Convert_Extract_Ammount( char *szExtAmmount , const short cEXTR_AMT_LEN)
{
    short nAuxLen    = 0;
    char  chNegative = FALSE;

    // Precondicion
    if( NULL != szExtAmmount && cEXTR_AMT_LEN > 0)
        nAuxLen    = min(strlen(szExtAmmount),cEXTR_AMT_LEN);

    // Segun ultimo caracter de cadena alfanumerica    
    switch(szExtAmmount[nAuxLen-1])
     {
        // Positivos
        case '{' : szExtAmmount[nAuxLen-1] = '0'; break;
        case 'A' : szExtAmmount[nAuxLen-1] = '1'; break;
        case 'B' : szExtAmmount[nAuxLen-1] = '2'; break;
        case 'C' : szExtAmmount[nAuxLen-1] = '3'; break;
        case 'D' : szExtAmmount[nAuxLen-1] = '4'; break;
        case 'E' : szExtAmmount[nAuxLen-1] = '5'; break;
        case 'F' : szExtAmmount[nAuxLen-1] = '6'; break;
        case 'G' : szExtAmmount[nAuxLen-1] = '7'; break;
        case 'H' : szExtAmmount[nAuxLen-1] = '8'; break;
        case 'I' : szExtAmmount[nAuxLen-1] = '9'; break;
        // Negativos
        case '}' : szExtAmmount[nAuxLen-1] = '0'; chNegative = TRUE; break;
        case 'J' : szExtAmmount[nAuxLen-1] = '1'; chNegative = TRUE; break;
        case 'K' : szExtAmmount[nAuxLen-1] = '2'; chNegative = TRUE; break;
        case 'L' : szExtAmmount[nAuxLen-1] = '3'; chNegative = TRUE; break;
        case 'M' : szExtAmmount[nAuxLen-1] = '4'; chNegative = TRUE; break;
        case 'N' : szExtAmmount[nAuxLen-1] = '5'; chNegative = TRUE; break;
        case 'O' : szExtAmmount[nAuxLen-1] = '6'; chNegative = TRUE; break;
        case 'P' : szExtAmmount[nAuxLen-1] = '7'; chNegative = TRUE; break;
        case 'Q' : szExtAmmount[nAuxLen-1] = '8'; chNegative = TRUE; break;
        case 'R' : szExtAmmount[nAuxLen-1] = '9'; chNegative = TRUE; break;
        default  : break;
     };

    // Adicionar signo negativo cuando corresponda
    if (TRUE == chNegative && '0'==szExtAmmount[0])
        szExtAmmount[0] = '-';
};
///////////////////////////////////////////////////////////////////////////////////
