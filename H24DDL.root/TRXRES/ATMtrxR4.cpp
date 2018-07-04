///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// Process ATM Transaction Method, Base24 R4
//
//   Metodo de Procesamiento de Transaccion ATM, con conexion y desconexion a base de
// datos para cada transaccion entrante, con manejo local de parsing del mensaje ISO
// y seteo del codigo de respuesta en el mensaje saliente.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2001.12.31      mdc     Base 24 Release 4.0
// (Inicial)    2002.01.30      mdc     Base 24 Release 4.0 
// (Inicial)    2002.02.18      mdc     Nuevos queries
// (Alfa)       2002.03.12      mdc     RED LINK pide "DESTINO O AUTORIZADOR NO DISPONIBLE" para Ult.Mov.
// (Alfa)       2002.05.27      mdc     SetOnOffLine( isoMsg.IsValid220()  || isoMsg.IsValid221() );
// (Alfa)       2002.05.30      mdc     STATEMENT PRINT RESPONSE para un FINANCIAL REQUEST. RED LINK S.A.
// (Alfa)       2002.07.23      mdc     if( isoMsg.IsReversalOrAdviceRepeat()  )....
// (Beta)       2002.10.10      mdc     En vez de "AUTORIZADOR NO DISPONIBLE", se responde "NO SOPORTADA" cuando es reversa
// (Beta)       2003.01.21      mdc     'Extract and Verify RefNum validity'
// (Release-1)  2003.07.24      mdc     Reversos de consultas se responden aprobadas : "CISO_APPROVED_I"
// (Release-1)  2004.11.25      mdc     if(CISO_TRANSACTION_NOT_SUPP_I == wRespCode )....ProcessGeneric()
// (Release-2)  2005.08.23      mdc     chCREDIT_AMMOUNT en cuentas con acuerdo / adelanto.
// (Release-2b) 2006.02.08      mdc     STATEMENT_PRINT_FORCED=TRUE segun implementacion
// (Release-2c) 2006.02.28      mdc     PIN CHANGE suma campos formateados
// (Release-2d) 2006.07.12      mdc     RED LINK requiere siempre 00 en lugar de 12
// (Release-2e) 2007.03.28      mdc     Eror no aprobado si NUMREF no es numerico
// (Release-2f) 2007.09.25      mdc     FIX : retorno FALSO ante errores grave (96 y 88)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Headers ANSI C/C++
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#ifndef _SYSTEM_ATM_   
#define _SYSTEM_ATM_DEFINED_   
#define _SYSTEM_ATM_   
#endif // _SYSTEM_ATM_DEFINED_

// Header Transaction Resolution
#include <qusrinc/trxres.h>

// Header ISO Msg Redefinition
#include <qusrinc/isoredef.h>

#ifdef _SYSTEM_ATM_DEFINED_
#undef _SYSTEM_ATM_
#endif // _SYSTEM_ATM_DEFINED_
// Libreria criptografica
//#include <qusrinc/crypto.h>
// Libreria de utilidades estandards
#include <qusrinc/stdiol.h>
// Libreria de conversion de base 36 a 10 y viceversa
#include <qusrinc/unicode.h>
// Extensiones SQL
//#include <sql.h>
//#include <sqlext.h>

//////////////////////////////////////////////////////////////////////////
// Toma el valor de la clave de encripcion
// char *=(input) buffer de 24 caracteres + NULL
// retorno=direccion del buffer de entrada
//////////////////////////////////////////////////////////////////////////
extern "C" char *get_encryption_key(char *);

///////////////////////////////////////////////////////////////////////////
// Macro de invocacion de copia de valores campo a campo
#define COPYVALUE(destiny,source)            \
if(source && destiny)                        \
    {                                        \
    memset(destiny,' ',sizeof(destiny));    \
    memcpy(destiny,source,min(strlen(source),sizeof(destiny)));    \
    }
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Macro de invocacion de copia de valores exacta campo a campo
#define COPYVALUEEXACT(destiny,source)        \
if(source && destiny)                        \
    {                                        \
    memset(destiny,' ',sizeof(destiny));    \
    memcpy(destiny,source,min(sizeof(source),sizeof(destiny)));    \
    }
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Macro de actualizacion de valor en cero
#define SETVALUEZEROE(destiny)                \
if(destiny)                                    \
    {                                        \
    memset(destiny,'0',sizeof(destiny));    \
    }                                            
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// Macro de asignacion de causal segun sea OFF y ONLINE
#define ISONOFFLINECAUSE(on,off) (boolOnlineMode == is_true) ? on : off
///////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
// Implementacion
/////////////////////////////////////////////////////////////////////////////////////
// Constructor
TrxResATM_Base24_R4::TrxResATM_Base24_R4(void)
{
    // Sin cargar campos ISO en estructuras parseadas
    isoFields.bLoaded = is_false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de Transacciones ATM.
// Retorna is_true por procesamiento aprobado o no,
// y is_false al hallar algun Error en el Sistema o en el Mensaje ISO-8583.
// Verifica tambien si la respuesta es solicitada (OnLine) o no 
// (OffLine) para la copia de datos en el mensaje de retorno.
////////////////////////////////////////////////////////////////////////////////////////////////
boolean_t TrxResATM_Base24_R4::ProcessTransaction(void)
    {    
    // Estructuras locales de parsing de Datos desde el Mensaje o BDatos
    // Variables para extraccion de Campos del Mensaje ISO-8583/POS/...    
    BYTE            szRespCode[8]   = {0},// Resp Code
                    szTranTyp[8]    = {0};// Trx Type (para ATMTRX)
    WORD            wRespCode       = 0,
                    wTranTypLen     = 0,
                    wRspCdeLen      = 0;
    CONST WORD      cwDefaultCause  = 0;
    // Ok o Error?
    boolean_t        bProcResult       = is_false; // Procesamiento OK?
    boolean_t        bIsReversal       = is_false; // Es reverso?
    const boolean_t  bStmtPrint_forced = is_false;  // Forzar STATEMENT PRINT (0215)
    const boolean_t  bIgnoreAdvRepeat  = is_false; // No ignorar ADVICE REEPAT (0221)
    ///////////////////////////////////////////////////////////////
    BYTE            szRefNum[sizeof(FIELD37)+1] = {0}; // Reference Number
    WORD            wLen  = 0;             // Reference Number length
    ///////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////
    // Antes de modificar el requerimiento como respuesta, 
    // guardar un backup original para no perder los datos !
    isoMsgBkp = isoMsg;
    ///////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////////////////////////////
    // Extract TranTyp
    wTranTypLen = sizeof(szTranTyp)-1;
    if(!isoMsg.GetMsgTypeId( (PBYTE)&szTranTyp, wTranTypLen ))
        {
        // On error, set Message Type Response 
        if(!isoMsg.SetMsgTypeIdResponse())
            return (is_false);
        // Codigo de Respuesta erroneo
        isoMsg.InsertField(39, &(wRspCdeLen=2), (PBYTE)CISO_INVALID_FORMAT,    2 );
        // error no aprobado
        return (is_false);
        // Ok por procesado, aunque no por aprobado
        return (is_true);
        };//end-if-trantype
    ////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////
    // Extract and Verify RefNum validity
    wLen = sizeof(szRefNum)-1;
    if(isoMsg.GetField(37, &wLen, szRefNum, wLen ))
        {
        // Verify NUMERIC content
        if( strnumlenpad( (char *)szRefNum, sizeof(FIELD37), ' ') != sizeof(FIELD37) )
            {
            // On error, set Message Type Response 
            isoMsg.SetMsgTypeIdResponse();
            // Codigo de Respuesta erroneo, segun sea SAF o no
            if( isoMsg.IsFinancialAdvice() || isoMsg.IsReversalOrAdviceRepeat() )
                isoMsg.InsertField(39, &(wRspCdeLen=2), (PBYTE)CISO_EXTERNAL_DECLINE,    2 );
            else
                isoMsg.InsertField(39, &(wRspCdeLen=2), (PBYTE)CISO_INVALID_FORMAT,    2 );
            // Ok por procesado, aunque no por aprobado
            return (is_true);
            }
        };//end-if-refnum
    ////////////////////////////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////////////////////////////
    // Conexion y desconexion a Base de Datos por cada Transaccion,
    // accediendo con usuario-odbc y contrasena respectiva
    // OBSERVACIONES : EN ESTA CASO NO CORRESPONDE YA QUE EXISTE SESION RPC PREVIA
    ////////////////////////////////////////////////////////////////////////////////////

    // Procesar toda otra transaccion de ATM
	if( isoMsg.IsATMTransaction() || !isoMsg.IsATMTransaction() ) 
    {

        // Por default e inicialmente, el codigo de respuesta es ERROR,
        // para la transaccion en curso
        wRespCode   = CISO_DESTINY_NOT_AVAILABLE_I;
        // Es reverso? Verificar en el mensaje ISO y pasar luego un booleano
        bIsReversal = ( isoMsg.IsValidReversal() ) 
                        ? is_true 
                        : is_false;  // Reversal or Rvsl.Repeat?
        
        // Si es FORZADO (Fin.Adv.0220 o 0221), habilitar el MODO OFFLINE del HOST-DPC
        // SetOnOffLine(TRUE : ONLINE, FALSE : OFFLINE)
        if(!SetOnOffLine( isoMsg.IsFinancialAdvice() ? is_false : is_true ))
        ///////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
        {
            isoMsg.SetMsgTypeIdResponse();            
            isoMsg.InsertField(39, &(wRspCdeLen=2), (PBYTE)CISO_INVALID_TRANSACTION, 2 );
            return (is_true);     
        };
        ///////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////////////
        // Segun el tipo de transaccion, procesar...
        // Repeticion de Forzado, siempre y cuando no deba ignorarse....
        ///////////////////////////////////////////////////////////////////////////////////
        if( is_true == bIgnoreAdvRepeat && isoMsg.IsReversalOrAdviceRepeat()  )
        {
            // No soportada
            wRespCode = CISO_INVALID_TRANSACTION_I;
        }
        ///////////////////////////////////////////////////////////////////////////////////
        else if(is_true == bIsReversal)    // Es reversa generica para cualquier transaccion?
        {
            //////////////////////////////////////////////////////////////
            // No procesar reversos de consultas
            if( isoMsg.IsATMLastMovInquiry() || 
                isoMsg.IsATMBalanceInquiry() ||
                isoMsg.IsATMOthersMsgs() )
            {
                // Completar campos de respuesta en BLANCOS y RECHAZAR TRX.
                // Formatear campo ISO #44, RESP_DATA, saldo y disponible
                FormatField44_RESP_DATA( NULL,NULL, NULL );
                // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
                FormatField122_SECNDRY_RSRVD3_PRVT();
                // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
                FormatField123_SECNDRY_RSRVD4_PRVT();
                // No soportada, pero informar APROBADA
                wRespCode = CISO_APPROVED_I;
            }
            else
                wRespCode = ProcessReversal(cwDefaultCause); 
            //////////////////////////////////////////////////////////////
        }
        else if(isoMsg.IsATMBalanceInquiry())            // Consulta de saldos
            wRespCode = ProcessBalanceInquiry(cwDefaultCause,bIsReversal);
        else if(isoMsg.IsATMTransfer())                // Transferencia cuentas relacionadas
            wRespCode = ProcessTransfer(cwDefaultCause,bIsReversal);
        else if(isoMsg.IsATMTransferInterbank())    // Transferencia interbancaria
            wRespCode = ProcessTransferInterbank(cwDefaultCause,bIsReversal);
        else if(isoMsg.IsATMTransferCBU())            // Transferencia minorista x CBU
            wRespCode = ProcessIndividualTransfer(cwDefaultCause,bIsReversal);
        else if(isoMsg.IsATMWithdrawal())            // Extraccion
            wRespCode = ProcessWithdrawal(cwDefaultCause,bIsReversal);
        else if(isoMsg.IsATMDeposit())                // Deposito
            wRespCode = ProcessDeposit(cwDefaultCause,bIsReversal);
        else if(isoMsg.IsATMFastCash())                // FastCash o compra
            wRespCode = ProcessFastCash(cwDefaultCause,bIsReversal);
        else if(isoMsg.IsATMAccountVerification())    // Verificacion cuenta destino
            wRespCode = ProcessAccountVerification(cwDefaultCause,bIsReversal);
        else if(isoMsg.IsATMPayment())                // Pagos
            wRespCode = ProcessPayment(cwDefaultCause,bIsReversal);
        else if(isoMsg.IsATMBuying())                // Compras y relacionados
            wRespCode = ProcessBuying(cwDefaultCause,bIsReversal);
        else if(isoMsg.IsATMAutServicePayment())    // Pagos Automatico de Servs.
            wRespCode = ProcessAutServicePayment(cwDefaultCause,bIsReversal);
        else if(isoMsg.IsATMLastMovInquiry())        // Consulta ultimos movimientos
        {    
            // Completar campos de respuesta en BLANCOS y RECHAZAR TRX.
            // Formatear campo ISO #44, RESP_DATA, saldo y disponible
            FormatField44_RESP_DATA( NULL,NULL, NULL );
            // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
            FormatField122_SECNDRY_RSRVD3_PRVT();
            // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
            FormatField123_SECNDRY_RSRVD4_PRVT();
            // A pedido de RED LINK, se retorna "DESTINO O AUTORIZADOR NO DISPONIBLE"
            if(is_true == bIsReversal)
                wRespCode = CISO_APPROVED_I;
            else 
                wRespCode = CISO_DESTINY_NOT_AVAILABLE_I;
        }
        else if (isoMsg.IsATMPinChange())           // Cambio de PIN
        {
			/////////////////////////////////////////////////////////////////////////
            // Completar campos de respuesta en BLANCOS y RECHAZAR TRX.
            // Formatear campo ISO #44, RESP_DATA, saldo y disponible
            FormatField44_RESP_DATA( NULL,NULL, NULL );
            // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
            FormatField122_SECNDRY_RSRVD3_PRVT();
            // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
            FormatField123_SECNDRY_RSRVD4_PRVT();
			/////////////////////////////////////////////////////////////////////////
            // A pedido de RED LINK, se retorna "DESTINO O AUTORIZADOR NO DISPONIBLE"
            if(is_true == bIsReversal)
                // wRespCode = CISO_TRANSACTION_NOT_SUPP_I;
                // RED LINK requiere siempre 00
                wRespCode = CISO_APPROVED_I;
            else 
                wRespCode = CISO_DESTINY_NOT_AVAILABLE_I;

            //////////////////////////////////////////
            // Loggear el movimiento para comisionar
            wRespCode = ProcessGeneric();		
            //////////////////////////////////////////

        }
        else if (isoMsg.IsATMAssignHomeBankUser())  // Asignacion usuario HB
        {
            // A pedido de RED LINK, se retorna "DESTINO O AUTORIZADOR NO DISPONIBLE"
            if(is_true == bIsReversal)
                // wRespCode = CISO_TRANSACTION_NOT_SUPP_I;
                // RED LINK requiere siempre 00
                wRespCode = CISO_APPROVED_I;
            else 
                wRespCode = CISO_DESTINY_NOT_AVAILABLE_I;
        }
        else if (isoMsg.IsATMExchangeRateInquiry())     // Consulta tipo de cambio
        {    
            // Completar campos de respuesta en BLANCOS y RECHAZAR TRX.
            // Formatear campo ISO #44, RESP_DATA, saldo y disponible
            FormatField44_RESP_DATA( NULL,NULL, NULL );
            // Formatear campo ISO #54 de rta., stB24_0210_Def_Data
            FormatField54_B24_0210_Def_Data( NULL, NULL, NULL, 
                NULL, NULL, NULL, NULL, NULL, NULL, NULL ); 
            // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
            FormatField122_SECNDRY_RSRVD3_PRVT();
            // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
            FormatField123_SECNDRY_RSRVD4_PRVT();
            // A pedido de RED LINK, se retorna "DESTINO O AUTORIZADOR NO DISPONIBLE"
            if(is_true == bIsReversal)
                // wRespCode = CISO_TRANSACTION_NOT_SUPP_I;
                // RED LINK requiere siempre 00
                wRespCode = CISO_APPROVED_I;
            else 
                wRespCode = CISO_DESTINY_NOT_AVAILABLE_I;
        }
		/*********************************************************************/
        else if(isoMsg.IsATMOthersMsgs())            // Otros mensajes al banco
            wRespCode = ProcessOtherMsgs(cwDefaultCause,bIsReversal);
		/*********************************************************************/
        else
            // Todo lo demas NO SE RECONOCE, SON "TRANSACCIONES INVALIDAS"
            wRespCode = CISO_INVALID_TRANSACTION_I;

#ifdef _PROCESS_UNKNOWN_TRX_
		//////////////////////////////////////////////////////////////////
		if(CISO_TRANSACTION_NOT_SUPP_I == wRespCode 
			||
		   CISO_INVALID_TRANSACTION_I  == wRespCode)
            // Todo lo demas SE RECONOCE COMO GENERICO
            wRespCode = ProcessGeneric();		
		//////////////////////////////////////////////////////////////////
#endif

        // Backup de resultado de ejecucion hacia la clase base
        wProcRespCde = wRespCode;        


        // Establecer codigo de mensaje ISO de respuesta
        // Como RED LINK tiene "ciertas particularidades".....
        if((!isoMsg.IsATMLastMovInquiry()) && is_true == bStmtPrint_forced)
        {
            // FINANCIAL RESPONSE para un FINANCIAL REQUEST, o lo que corresponda
            if(!isoMsg.SetMsgTypeIdResponse())
                return (is_false);
        }
        else if(is_true == bStmtPrint_forced)  // Statement Print forzado ?
        {
            // STATEMENT PRINT RESPONSE para un FINANCIAL REQUEST. RED LINK S.A.
            if(!isoMsg.SetMsgTypeId((PBYTE)CISO_MSGTYPE_STMT_PRINT_RESP,4))
                return (is_false);
        }
        else
        {
            // FINANCIAL RESPONSE para un FINANCIAL REQUEST, o lo que corresponda
            if(!isoMsg.SetMsgTypeIdResponse())
                return (is_false);
        };
        

        
        // Resultado de Respuesta parametrico, en campo #39 del mensaje ISO
        sprintf((PSTR)szRespCode, "%0*i", 2, wRespCode);
        if(!isoMsg.IsValidField(39))
            isoMsg.InsertField(39, &(wRspCdeLen=2), (PBYTE)szRespCode,    2 );
        else
            isoMsg.SetField(39, &(wRspCdeLen=2), (PBYTE)szRespCode,    2 );
        
        }
    else
        {
        
        // Set Message Type Response 
        if(!isoMsg.SetMsgTypeIdResponse())
            return (is_false);
        // Codigo de Respuesta erroneo, en campo #39 del mensaje ISO
        isoMsg.InsertField(39, &(wRspCdeLen=2), (PBYTE)CISO_INVALID_TRANSACTION, 2 );
        };//end-if-then-else-TranCode

    //////////////////////////////////////////////////////////////////////    
    // Commit o Rollback de transaccion segun el resultado obtenido, y
    // desconectarse a Base de Datos .
    // OBSERVACIONES : EN ESTA CASO NO CORRESPONDE YA QUE EXISTE SESION RPC PREVIA    
    //////////////////////////////////////////////////////////////////////

	// FIX : FALSO ante errores graves
	if ( CISO_SYSTEM_FAILURE_I == wRespCode 
		 ||
	     CISO_SYSTEM_ERROR_I == wRespCode )
	{
		bProcResult = is_false;
		return ( is_false );
		
	}
	// FIX

     // Ok por procesado
    return (is_true);
    }//end-ProcessATMTransaction

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de reversos
WORD TrxResATM_Base24_R4::ProcessReversal(WORD wCause)
{
    WORD wRespCde = ProcessInit();
    // Retornar cod.rta.
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I)   ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento generico
WORD TrxResATM_Base24_R4::ProcessGeneric(void)
{
    WORD wRespCde = ProcessInit();
    // Retornar cod.rta.
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I)   ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessBalanceInquiry(WORD wCause,boolean_t bIsRvsl)        // Consulta de saldos
{
    WORD wRespCde = ProcessInit();
    // Retornar cod.rta.
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I)   ;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessTransfer(WORD wCause,boolean_t bIsRvsl)            // Transferencia ctas. relacionadas
{
    WORD wRespCde = ProcessInit();
    // Retornar cod.rta.
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I)     ;
}
// Procesamiento de transacciones xfer. interbancarias
WORD TrxResATM_Base24_R4::ProcessTransferInterbank(WORD wCause,boolean_t bIsRvsl)            // Transferencia Interbancaria
{
    WORD wRespCde = ProcessInit();
    // Retornar cod.rta.
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I)     ;
}

// Procesamiento de transacciones de xfer. minorista, o CBU
WORD TrxResATM_Base24_R4::ProcessIndividualTransfer(WORD wCause,boolean_t bIsRvsl)            // Transferencia minorista
{
    WORD wRespCde = ProcessInit();
    // Retornar cod.rta.
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I)    ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessWithdrawal(WORD wCause,boolean_t bIsRvsl)            // Extraccion
{    
    WORD wRespCde = ProcessInit();
    // Retornar cod.rta.
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessDeposit(WORD wCause,boolean_t bIsRvsl)            // Deposito
{
    WORD wRespCde = ProcessInit();
    // Retornar cod.rta.
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessFastCash(WORD wCause,boolean_t bIsRvsl)            // FastCash o compra
{
    WORD wRespCde = ProcessInit();
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessAccountVerification(WORD wCause,boolean_t bIsRvsl)// Verificacion cuenta destino
{
    WORD wRespCde = ProcessInit();
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessPayment(WORD wCause,boolean_t bIsRvsl)            // Pagos
{
    WORD wRespCde = ProcessInit();
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessBuying(WORD wCause,boolean_t bIsRvsl)                // Compras y relacionados
{
    WORD wRespCde = ProcessInit();
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessAutServicePayment(WORD wCause,boolean_t bIsRvsl)    // Pagos Automatico de Servs.
{
    WORD wRespCde = ProcessInit();
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessOtherMsgs(WORD wCause,boolean_t bIsRvsl)            
{
    WORD wRespCde = ProcessInit();
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessDebitNote(WORD wCause,boolean_t bIsRvsl)            
{
    WORD wRespCde = ProcessInit();
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Procesamiento de transacciones en particular
WORD TrxResATM_Base24_R4::ProcessCreditNote(WORD wCause,boolean_t bIsRvsl)            
{
    WORD wRespCde = ProcessInit();
    return (wRespCde = CISO_TRANSACTION_NOT_SUPP_I);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #54 de rta., stB24_0210_Def_Data
void TrxResATM_Base24_R4::FormatField54_B24_0210_Def_Data( 
     char *chWITH_ADV_AVAIL
    ,char *chINT_OWE_AUSTRAL
    ,char *chCASH_AVAIL
    ,char *chMIN_PAYMENT
    ,char *chPAYMENT_DAT
    ,char *chINTEREST_RATE
    ,char *chOWE_DOLAR
    ,char *chMIN_PAYMENT_DOLAR
    ,char *chPURCHASE_DOLAR
    ,char *chCASH_FEE )
{
    WORD wRspCdeLen=0;

    // Formatear campo ISO #54 de rta., B24_0210_DEF, pero no para Xfer. x CBUs.
    if( isoMsg.IsATMTransferCBU() )        
        return ;

    // Formatear campo ISO #54 de rta., stB24_0210_Def_Data    
    if(NULL == chWITH_ADV_AVAIL)    chWITH_ADV_AVAIL    = "99"; // Sin limites de extracciones
    if(NULL == chINT_OWE_AUSTRAL)   chINT_OWE_AUSTRAL   = "000000000000";
    if(NULL == chCASH_AVAIL)        chCASH_AVAIL        = "000000000000";    
    if(NULL == chMIN_PAYMENT)       chMIN_PAYMENT       = "000000000000";
    if(NULL == chPAYMENT_DAT)       chPAYMENT_DAT       = "000000";
    if(NULL == chINTEREST_RATE)     chINTEREST_RATE     = "000000";
    if(NULL == chOWE_DOLAR)         chOWE_DOLAR         = "0000000000";
    if(NULL == chMIN_PAYMENT_DOLAR) chMIN_PAYMENT_DOLAR = "0000000000";    
    if(NULL == chPURCHASE_DOLAR)    chPURCHASE_DOLAR    = "0000000000";    
    if(NULL == chCASH_FEE)          chCASH_FEE          = "00000000";        
    
    // No se blanquea inicialmente porque ya viene informado por el switch financiero
    COPYVALUE(isoFields.field54.stB24_0210_Def_Data.chAMT, NULL ); // 12 + reseved 108 bytes
    COPYVALUE(isoFields.field54.stB24_0210_Def_Data.chWITH_ADV_AVAIL,chWITH_ADV_AVAIL);    // contador extracciones o adelantos
    COPYVALUE(isoFields.field54.stB24_0210_Def_Data.chINT_OWE_AUSTRAL,chINT_OWE_AUSTRAL); // intereses ganados o deuda anterior
    COPYVALUE(isoFields.field54.stB24_0210_Def_Data.chCASH_AVAIL,chCASH_AVAIL); // saldo disponible
    COPYVALUE(isoFields.field54.stB24_0210_Def_Data.chMIN_PAYMENT,chMIN_PAYMENT); // pago minimo
    COPYVALUE(isoFields.field54.stB24_0210_Def_Data.chPAYMENT_DAT,chPAYMENT_DAT); // dia de pago
    COPYVALUE(isoFields.field54.stB24_0210_Def_Data.chINTEREST_RATE,chINTEREST_RATE); // tasa de interes
    COPYVALUE(isoFields.field54.stB24_0210_Def_Data.chOWE_DOLAR,chOWE_DOLAR); // deuda en dolares
    COPYVALUE(isoFields.field54.stB24_0210_Def_Data.chMIN_PAYMENT_DOLAR,chMIN_PAYMENT_DOLAR); // pago minimo en dolares
    COPYVALUE(isoFields.field54.stB24_0210_Def_Data.chPURCHASE_DOLAR,chPURCHASE_DOLAR);    // compra en dolares
    COPYVALUE(isoFields.field54.stB24_0210_Def_Data.chCASH_FEE,chCASH_FEE); // arancel de usuario
    isoFields.field54.stB24_0210_Def_Data.chFILLER[0] = 0x00;
    // Insertar el campo #54 solo si no existe
    if(isoMsg.IsValidField( 54 ))        
        isoMsg.DeleteField( 54 );
    // Respuesta parametrica en campo #54        
    wRspCdeLen=min(sizeof(isoFields.field54),strlen(isoFields.field54.chData));
    isoMsg.InsertField(54, &wRspCdeLen, (PBYTE)&isoFields.field54, wRspCdeLen );

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #55 de rta., PRIRSRVD1_ISO
void TrxResATM_Base24_R4::FormatField55_PRIRSRVD1_ISO( 
    char *chTrack2,
    char *chCA,
    char *chFiid_1,
    char *chTyp_1,
    char *chAcct_num_1,
    char *chFiid_2,
    char *chTyp_2,
    char *chAcct_num_2
    )
{
    WORD wRspCdeLen=0;

    // Formatear campo ISO #55 de rta., PRIRSRVD1_ISO, solamente para interbancarias.
    if( !( isoMsg.IsATMTransferInterbank() || isoMsg.IsATMAccountVerification() )  )    
    {
        isoMsg.DeleteField ( 55 );
        return ;
    };
    // Formatear campo ISO #55 de rta., PRIRSRVD1_ISO
    // No se blanquea inicialmente porque ya viene informado por el switch financiero
    if( chTrack2        != NULL &&
        chCA            != NULL &&
        chFiid_1        != NULL &&
        chTyp_1         != NULL &&
        chAcct_num_1    != NULL &&
        chFiid_2        != NULL &&
        chTyp_2         != NULL &&
        chAcct_num_2    != NULL )
    {
        COPYVALUE(isoFields.field55.stInterbancarias.chTrack2,chTrack2);
        COPYVALUE(isoFields.field55.stInterbancarias.chCA, chCA);
        COPYVALUE(isoFields.field55.stInterbancarias.stFrom_Acct.chFiid, chFiid_1);
        COPYVALUE(isoFields.field55.stInterbancarias.stFrom_Acct.chTyp, chTyp_1);
        COPYVALUE(isoFields.field55.stInterbancarias.stFrom_Acct.chAcct_num, chAcct_num_1);
        COPYVALUE(isoFields.field55.stInterbancarias.stTo_Acct.chFiid, chFiid_2);
        COPYVALUE(isoFields.field55.stInterbancarias.stTo_Acct.chTyp, chTyp_2 );
        COPYVALUE(isoFields.field55.stInterbancarias.stTo_Acct.chAcct_num, chAcct_num_2);
        COPYVALUE(isoFields.field55.stInterbancarias.filler, "--------------------");
        // Respuesta parametrica en campo #55
        // Insertar el campo #55 solo si no existe
        if(isoMsg.IsValidField( 55 ))
            isoMsg.DeleteField( 55 );
        wRspCdeLen=min(sizeof(isoFields.field55),strlen(isoFields.field55.chData));
        isoMsg.InsertField(55, &wRspCdeLen, (PBYTE)&isoFields.field55,    wRspCdeLen );
    };

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #15 y #61 que POR ERRORES DEL PREHOMO DE LINK, a veces no vienen en el 0200
void TrxResATM_Base24_R4::FormatFields15_61( void )
{
    WORD wRspCdeLen=0;
    if(isoMsg.IsValidField( 15 ) && isoMsg.IsValidField( 61 ))
        return;

    // Formatear campo ISO #15 de rta.    SETTLEMENT-DATE como POSTING DATE, #17
    COPYVALUEEXACT(isoFields.field15.chMMDD , isoFields.field17.chMMDD ); 
    // Respuesta parametrica en campo #15    
    wRspCdeLen=sizeof(isoFields.field15);
    isoMsg.InsertField(15, &wRspCdeLen, (PBYTE)&isoFields.field15,    wRspCdeLen );
    
    // Formatear campo ISO #61 de rta.    , como TERM #60 y valores por defecto    
    COPYVALUE(isoFields.field61.stATM_Data.chCrdFIID     , isoFields.field60.stATMTerm.chTermFIID);       
    COPYVALUE(isoFields.field61.stATM_Data.chCrdLNet     ,  isoFields.field60.stATMTerm.chTermLNet);        
    COPYVALUE(isoFields.field61.stATM_Data.chSaveAcctType, "0000");    
    isoFields.field61.stATM_Data.chAutorizer             = 'P';        
    isoFields.field61.stATM_Data.chOrigTranCde[0] = 0x00;    

    // Respuesta parametrica en campo #61
    wRspCdeLen=min(sizeof(isoFields.field61),strlen(isoFields.field61.chData));
    isoMsg.InsertField(61, &wRspCdeLen, (PBYTE)&isoFields.field61,    wRspCdeLen );

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #44 de rta., PRIRSRVD1_ISO
void TrxResATM_Base24_R4::FormatField44_RESP_DATA( char *chOPEN_AMMOUNT, 
												   char *chDAILY_LIMIT ,
												   char *chCREDIT_AMMOUNT)
{    
    const boolean_t bNetworkDailyLimit   = is_true;    
    WORD            wRspCdeLen           = 0;
    // Cuantos montos se informan ?
    // "1" Solo se informa saldo.
    // "2" Solo se informa interés. (sin uso)
    // "3" Ambos.
	const char      chDEFINED_AVAIL_TYPE = '2';

    // Formatear campo ISO #44 de rta., RESP_DATA    
    memset(&isoFields.field44,' ',sizeof(isoFields.field44));
    // Cuantos montos se informan ?
    // "1" Solo se informa saldo.
    // "2" Solo se informa interés. (sin uso)
    // "3" Ambos.
    if( chOPEN_AMMOUNT && chDAILY_LIMIT)
        isoFields.field44.stAvail.chType = '3'; // '0','1','2','3','4' o ' '
    else if( chOPEN_AMMOUNT || chDAILY_LIMIT)
        isoFields.field44.stAvail.chType = '1'; // '0','1','2','3','4' o ' '
    else 
        isoFields.field44.stAvail.chType = '2'; // '0','1','2','3','4' o ' ' 

	//////////////////////////////////////////////////////////////////////////////////////
	isoFields.field44.stAvail.chType = chDEFINED_AVAIL_TYPE; // '1' Solo se informa saldo.
	//////////////////////////////////////////////////////////////////////////////////////

    // Segun la cantidad de montos informados...
    if( chOPEN_AMMOUNT && chDAILY_LIMIT &&
        strcmp(chOPEN_AMMOUNT,chDAILY_LIMIT)>0 && bNetworkDailyLimit)
    {
        COPYVALUE(isoFields.field44.stAvail.chOpenAmmount ,chOPEN_AMMOUNT);  /* 9(10).99 : saldo apertura */
        COPYVALUE(isoFields.field44.stAvail.chAvailAmmount,chOPEN_AMMOUNT); /* 9(10).99 : disponible     */
    }
    else if( chOPEN_AMMOUNT && chDAILY_LIMIT &&
            strcmp(chOPEN_AMMOUNT,chDAILY_LIMIT)<0 && bNetworkDailyLimit)
    {
        COPYVALUE(isoFields.field44.stAvail.chOpenAmmount ,chDAILY_LIMIT);  /* 9(10).99 : saldo apertura */
        COPYVALUE(isoFields.field44.stAvail.chAvailAmmount,chDAILY_LIMIT); /* 9(10).99 : disponible     */
    }
    else if( NULL == chOPEN_AMMOUNT && NULL == chDAILY_LIMIT )
    {
        isoFields.field44.stAvail.chType = '2'; /* '0','1','2','3','4' o ' ' *** forzado LINK = 2 */        
        SETVALUEZEROE(isoFields.field44.stAvail.chOpenAmmount );  /* 9(10).99 : saldo apertura */
        SETVALUEZEROE(isoFields.field44.stAvail.chAvailAmmount); /* 9(10).99 : disponible     */
    }
    else // Todo otro caso...
    {
        COPYVALUE(isoFields.field44.stAvail.chOpenAmmount ,chOPEN_AMMOUNT);  /* 9(10).99 : saldo apertura */
        COPYVALUE(isoFields.field44.stAvail.chAvailAmmount,chDAILY_LIMIT); /* 9(10).99 : disponible     */
    };


    // Respuesta parametrica en campo #44        
    // Insertar el campo #44 solo si no existe
    if(isoMsg.IsValidField( 44 ))
        isoMsg.DeleteField( 44 );
    wRspCdeLen=min(sizeof(isoFields.field44),strlen(isoFields.field44.chData));

    isoMsg.InsertField(44, &wRspCdeLen, (PBYTE)&isoFields.field44,    wRspCdeLen );

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #122 de rta., SECNDRY_RSRVD3_PRVT
void TrxResATM_Base24_R4::FormatField122_SECNDRY_RSRVD3_PRVT( void )
{
    WORD wRspCdeLen=0;
    // Insertar el campo #122 solo si no existe
    if(isoMsg.IsValidField( 122 ))
        return;
    // Formatear campo ISO #122 de rta., SECNDRY_RSRVD3_PRVT
    memset(&isoFields.field122,' ',sizeof(isoFields.field122));
    COPYVALUE(isoFields.field122.chCardIssuerIdentCode, "00000000000");
    // Respuesta parametrica en campo #122        
    wRspCdeLen=min(sizeof(isoFields.field122),strlen(isoFields.field122.chCardIssuerIdentCode));
    isoMsg.InsertField(122, &wRspCdeLen, (PBYTE)&isoFields.field122, wRspCdeLen );

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #123 de rta., SECNDRY_RSRVD4_PRVT
void TrxResATM_Base24_R4::FormatField123_SECNDRY_RSRVD4_PRVT( void )
{    
    WORD wRspCdeLen=0;
    // Insertar el campo #123 solo si no existe
    if(isoMsg.IsValidField( 123 ))
        return;
    // Formatear campo ISO #123 de rta., SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
    memset(&isoFields.field123,' ',sizeof(isoFields.field123));
    COPYVALUE(isoFields.field123.chDepositCreditAmmount, "000000000000");
    // Respuesta parametrica en campo #123        
    wRspCdeLen=min(sizeof(isoFields.field123),strlen(isoFields.field123.chDepositCreditAmmount));
    isoMsg.InsertField(123, &wRspCdeLen, (PBYTE)&isoFields.field123, wRspCdeLen );

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #127 de rta., SECNDRY_RSRVD7_PRVT
void TrxResATM_Base24_R4::FormatField127_SECNDRY_RSRVD8_PRVT( void )
{    
    WORD wRspCdeLen=0;    

    // Actualizar o insertar segun requiera RED LINK
    if(isoMsg.IsValidField( 127 ))
    {
        // Actualizar los tipos de cambio segun informa el switch financiero
        // Copiar la cotizacion informada hacia la cotizacion aplicada
        FIELD127 resp127;
        // Copia local de trabajo
        resp127.stATM.stATM_Dl_Resp = isoFields.field127.stATM.stATM_Dl_Resp;
        // Copiar las cotizaciones del requerimiento hacia las cotizaciones de respuesta
        resp127.stATM.stATM_Dl_Resp.chCamb_Aplic = '0'; /* FORZADO LINK */
        COPYVALUEEXACT(resp127.stATM.stATM_Dl_Resp.chCambio_C,
            isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_C);
        COPYVALUEEXACT(resp127.stATM.stATM_Dl_Resp.chCambio_V,
            isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_V);
        COPYVALUEEXACT(resp127.stATM.stATM_Dl_Resp.chCambio_1,
            isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_1);
        COPYVALUEEXACT(resp127.stATM.stATM_Dl_Resp.chCambio_2,
            isoFields.field127.stATM.stATM_Dl_Rqst.chCambio_2);
        COPYVALUE(resp127.stATM.stATM_Dl_Resp.chFiller,"0000000000");            
        // Sobreescitura del campo existente
        isoFields.field127.stATM.stATM_Dl_Resp = resp127.stATM.stATM_Dl_Resp;
        // Actualizacion en mensaje ISO
        wRspCdeLen=sizeof(isoFields.field127.stATM.chMicr_Data); // Longitud fija de 43 bytes
        isoMsg.SetField(127, &wRspCdeLen, (PBYTE)&isoFields.field127, wRspCdeLen );
    }
    else
    {
        // Formatear campo ISO #127 de rta., SECNDRY_RSRVD8_PRVT    
        memset(&isoFields.field127,' ',sizeof(isoFields.field127));
        isoFields.field127.stATM.stATM_Dl_Resp.chCamb_Aplic = '0'; /* FORZADO LINK */
        COPYVALUE(isoFields.field127.stATM.stATM_Dl_Resp.chCambio_C, "00000000");
        COPYVALUE(isoFields.field127.stATM.stATM_Dl_Resp.chCambio_V, "00000000");
        COPYVALUE(isoFields.field127.stATM.stATM_Dl_Resp.chCambio_1, "00000000");
        COPYVALUE(isoFields.field127.stATM.stATM_Dl_Resp.chCambio_2, "00000000");
        COPYVALUE(isoFields.field127.stATM.stATM_Dl_Resp.chFiller, "          ");
        wRspCdeLen=sizeof(isoFields.field127.stATM.chMicr_Data); // Longitud fija de 43 bytes
        isoMsg.InsertField(127, &wRspCdeLen, (PBYTE)&isoFields.field127, wRspCdeLen );
    };

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #125 de rta., SECNDRY_RSRVD6_PRVT
void TrxResATM_Base24_R4::FormatField125_SECNDRY_RSRVD6_PRVT( char chProc_Acct_Ind )
{
    WORD wRspCdeLen=0;    
    // Insertar el campo #125 solo si no existe
    if(isoMsg.IsValidField( 125 ))
        return;
    // Formatear campo ISO #125 de rta., SECNDRY_RSRVD6_PRVT, "ACCOUNT INDICATOR"
    isoFields.field125.stATM_STMT.stATM.chProc_Acct_Ind = chProc_Acct_Ind;
    isoFields.field125.stATM_STMT.stATM.chFiller[0]     = 0x00;
    // Respuesta parametrica en campo #125        
    wRspCdeLen=min(sizeof(isoFields.field125),strlen(isoFields.field125.chData));
    isoMsg.InsertField(125, &wRspCdeLen, (PBYTE)&isoFields.field125, wRspCdeLen );

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Estado de la ultima ejecucion RPC/ODBC
boolean_t TrxResATM_Base24_R4::TransactionExecutionStatus()
{
    return bExecStatus_ok;
}
///////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #126 de rta., SECNDRY_RSRVD7_PRVT
void TrxResATM_Base24_R4::FormatField126_SECNDRY_RSRVD7_PRVT( char *szUSER_DATA )
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #60 
void TrxResATM_Base24_R4::FormatField60( void )
{
    WORD wRspCdeLen=0;
    if( isoMsg.IsValidField( 60 ) )
        return;

    
    // Formatear campo ISO #60 de rta.    , como TERM #61 y valores por defecto    
    COPYVALUE(isoFields.field60.stATMTerm.chTermFIID , isoFields.field61.stATM_Data.chCrdFIID);       
    COPYVALUE(isoFields.field60.stATMTerm.chTermLNet , isoFields.field61.stATM_Data.chCrdLNet );            

    // Respuesta parametrica en campo #60
    wRspCdeLen=sizeof(isoFields.field60);
    isoMsg.InsertField(60, &wRspCdeLen, (PBYTE)&isoFields.field60,    wRspCdeLen );

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatear campo ISO #124 de rta., SECNDRY_RSRVD5_PRVT
void TrxResATM_Base24_R4::FormatField124_SECNDRY_RSRVD5_PRVT( void )
{
    WORD wRspCdeLen=0;
    if( isoMsg.IsValidField( 124 ) )
        return;

    
    // DEPOSITORY TYPE
    isoFields.field124.chDepositoryType = '1'; // 1-Cash, 2-Check

    // Respuesta parametrica en campo #124
    wRspCdeLen=sizeof(isoFields.field124);
    isoMsg.InsertField(124, &wRspCdeLen, (PBYTE)&isoFields.field124, wRspCdeLen );

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////



