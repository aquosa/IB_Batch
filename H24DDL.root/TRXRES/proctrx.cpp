////////////////////////////////////////////////////////////////////////////////////////////
// IT24 Sistemas S.A.
// Process Transaction Method
//
//   Metodo de Procesamiento de Transaccion, con discriminacion y especializacion en subclases
// SOAT, PAS, AFJP, ULTMOV, etc. para la resolucion de la misma.
//   Segun la implementacion, el metodo especializado puede ser "dummy", o sea un simple
// retorno en falso sin procesamiento, sin generar el codigo ejecutable actualmente, 
// con el solo motivo de mantener el esqueleto de codigo fuente original y facilitar el 
// posible enlace a futuro de las subclases especializadas en una sola libreria o modulo.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.07.23      mdc     En base a ProcessTransaction de PAS/AFJP/ULTMOV. Solo para SOAT.
// (Inicial)    1999.01.06      mdc     <trxres.h> : Path globalizado
// (Beta)       2001.12.31      mdc     Implementacion de clase ATM
// (Beta)       2002.04.22      mdc     TrxResolution::ProcessTransaction(,,,,enumHostStatus hostStatus);
// (Beta)       2002.08.20      mdc     TrxResolution::ProcessTransaction(,,,,,enumFormatType efExtMsgFormat);
// (Beta)       2002.11.25      mdc     TrxResolution::ProcessTransaction(,,,,,enumHostStatus *pbackendStatus);
// (Beta)       2003.01.21      mdc     if(_SAF_IN_PROGRESS_ == hostStatus)....
// (Beta)       2003.09.17      mdc     Excepcion para _SYSTEM_DATANET_COBIS_
// (Delta)      2004.04.30      mdc     Error de Congestion del Sistema, ABORTAR
// (1.0.1.26)   2004.05.27      MDC     #define _BACKEND_CRITICAL_ERROR_        (1)
// (1.0.1.27)   2005.08.10      MDC     DYNAMIC CAST ATM_POS
//

// Headers ANSI
#include <limits.h>
#include <stdio.h>

// Solo reconocimiento de transacciones para Sistema ATM (para no incluir clases de PAS,ULTMOV,AFJP...)
#ifndef _SYSTEM_ATM_
#define _SYSTEM_ATM_
#endif // _SYSTEM_ATM_

// Header Trx Resolution
#include <qusrinc/trxres.h>

// ERROR DE CONGESTION DEL SISTEMA
#define _BACKEND_ERROR_SYSTEM_BUSY_   (40002)
// ERROR DE PRODUCTO DESHABILITADO / NO PERMITIDO
#define _BACKEND_ERROR_NOT_ALLOWED_     (40003)
#define _BACKEND_ERROR_NOT_AVAILABLE_   (40003)


// Procesamiento de Transacciones.
// Retorna TRUE por procesamiento aprobado o no,
// y FALSE al hallar algun Error en el Sistema o en el Mensaje ISO-8583.
// Verifica tambien si la respuesta es solicitada (OnLine) o no 
// (OffLine) para la copia de datos en el mensaje de retorno.
#ifdef _SYSTEM_DATANET_COBIS_
BOOL TrxResolution::ProcessTransaction( PBYTE  pcbBuffer   , 
                                        INT    iBuffLen    ,
                                        PBYTE  pcbRspBuffer,
                                        PINT   piRspBuffLen,
                                        enumHostStatus hostStatus,
                                        enumFormatType efExtMsgFormat) 
{
    enumHostStatus  backendStatus  = _HOST_READY_ ,
                   *pbackendStatus = &backendStatus;
#else
BOOL TrxResolution::ProcessTransaction( PBYTE  pcbBuffer   , 
                                        INT    iBuffLen    ,
                                        PBYTE  pcbRspBuffer,
                                        PINT   piRspBuffLen,
                                        enumHostStatus hostStatus,
                                        enumFormatType efExtMsgFormat,
                                        enumHostStatus *pbackendStatus) 
{    
#endif // _SYSTEM_DATANET_COBIS_
    // Booleanos de respuesta de o hacia funciones
    // (Pertenece a la clase base TrxBaseResolution)
    bProcResult = is_false;       // Sin procesar inicialmente, default
    ///////////////////////////////////////////////////////////////////
    efFormatMsg = efExtMsgFormat; // Formato de mensaje actual
    ///////////////////////////////////////////////////////////////////

    // Precondicion: Mensaje ISO-8583 valido
    // Import ISO-8583 Message
    if(!isoMsg.Import(pcbBuffer, iBuffLen))
        // Error
        return (FALSE);
        
    // Is Valid Message?
    if(!isoMsg.IsValidMessage())
        // Error
        return (FALSE);

    ///////////////////////////////////////////////////////////////
    // Antes de modificar el requerimiento como respuesta, 
    // guardar un backup original para no perder los datos !
    isoMsgBkp = isoMsg;
    ///////////////////////////////////////////////////////////////


// Compilar para reconocer diversos sistemas segun definiciones previas
#if defined( _SYSTEM_BASE_ ) && !defined( _SYSTEM_SOAT_ )
    //////////////////////////////////////////////////////////////////////////////
    // Bloque discriminador de Tipo de Transaccion
    // Segun producto SOAT/PAS/AFJP/ULTMOV... procesar transaccion con metodo adecuado:    
    //////////////////////////////////////////////////////////////////////////////
    if( isoMsg.IsPASTransaction() ) // PAS?
        {
        // Atencion:
        // No procesar pagos desde terminales POS Teller    
        if( isoMsg.IsPASPaymentFromPOSTerminal() )
            // No es un error, se post-procesa con refresh,
            // y no se genera una respuesta al mensaje...
            return (bProcResult = FALSE);

        // ClassCasting, a clase de resolucion PAS
        TrxResPAS *pTrxRes = dynamic_cast<TrxResPAS*>(this);
        // Procesar
        if(pTrxRes)    bProcResult = pTrxRes->ProcessTransaction();        
        else           bProcResult = FALSE; // error
        }
    else 
    if( isoMsg.IsULTMOVTransaction() ) // ULTMOV?
        {
        // ClassCasting, a clase de resolucion ULTMOV
        TrxResULTMOV *pTrxRes = dynamic_cast<TrxResULTMOV*>(this);
        // Procesar
        if(pTrxRes)    bProcResult = pTrxRes->ProcessTransaction();    
        else           bProcResult = FALSE; // error
        }
    else 
    if( isoMsg.IsAFJPTransaction() ) // AFJP?
        {
        // ClassCasting, a clase de resolucion AFJP
        TrxResAFJP *pTrxRes = dynamic_cast<TrxResAFJP*>(this);
        // Procesar
        if(pTrxRes)    bProcResult = pTrxRes->ProcessTransaction();    
        else           bProcResult = FALSE; // error
        }
    else 
// Sistema SOAT solamente?
#elif defined( _SYSTEM_SOAT_ )    
    if( isoMsg.IsSOATTransaction() ) // SOAT?
        {
        // ClassCasting, a clase de resolucion SOAT
        TrxResSOAT *pTrxRes = dynamic_cast<TrxResSOAT*>(this);
        // Procesar
        if(pTrxRes)    bProcResult = pTrxRes->ProcessTransaction();        
        else           bProcResult = FALSE; // error
        }
    else
///////////////////////////////////////////////////
// Sistema NEAT solamente?
#elif defined( _SYSTEM_POS_NEAT_ )    
    if( isoMsg.IsATMTransaction() || !isoMsg.IsATMTransaction() ) // POS? - ATM?
        {
        // ClassCasting, a clase de resolucion SOAT
        TrxResNEAT *pTrxRes = dynamic_cast<TrxResNEAT*>(this);
        // Procesar
        if(pTrxRes)    bProcResult = pTrxRes->ProcessTransaction();        
        else           bProcResult = is_false; // error
        }
    else
///////////////////////////////////////////////////
// Sistema ATM solamente (incluye COBIS y SIAF) ?
#elif defined( _SYSTEM_ATM_ )    
    if( isoMsg.IsATMTransaction() || !isoMsg.IsATMTransaction() ) // POS? - ATM?
        {
        ////////////////////////////////////////////////////////
        // ClassCasting, a clase de resolucion ATM
        TrxResATM_POS   *pTrxRes      = dynamic_cast<TrxResATM_POS*>(this);        

        // Procesar si el host esta ONLINE y DISPONIBLE
        if(( _HOST_READY_      == hostStatus ||
             _SAF_IN_PROGRESS_ == hostStatus ) && 
             pTrxRes )
        {
            // ATM default
            bProcResult = pTrxRes->ProcessTransaction();
            ////////////////////////////////////////////////////////////////////////////////////////
            // Si hubo error BDATOS... indicarlo para ABORTAR el BackEnd
            if(pbackendStatus && pTrxRes->BackEndAuthStatus() == _BACKEND_DATABASE_ERROR_ )
                (*pbackendStatus) = _NO_COMM_EVENTS_;
            // Si hubo Congestion del Sistema.... indicarlo para ABORTAR el BackEnd
            else if(pbackendStatus && pTrxRes->BackEndAuthStatus() == _BACKEND_ERROR_SYSTEM_BUSY_ )
                (*pbackendStatus) = _NO_COMM_EVENTS_;
            // Si hubo Prod. bancario deshabilitado... indicarlo para ABORTAR el BackEnd
            else if(pbackendStatus && pTrxRes->BackEndAuthStatus() == _BACKEND_ERROR_NOT_ALLOWED_ )
                (*pbackendStatus) = _NO_COMM_EVENTS_;
            // Si hubo error critico... indicarlo para ABORTAR el BackEnd
            else if(pbackendStatus && pTrxRes->BackEndAuthStatus() == _BACKEND_CRITICAL_ERROR_ )
                (*pbackendStatus) = _NO_COMM_EVENTS_;
            // Si hubo error BDATOS... indicarlo para ABORTAR el BackEnd
            else if(pbackendStatus && pTrxRes->BackEndAuthStatus() == _BACKEND_DATABASE_ERROR_ )
                (*pbackendStatus) = _NO_COMM_EVENTS_;
            // Si hubo otros errores de back end
            else if(pbackendStatus && pTrxRes->BackEndAuthStatus() < 0 )
                (*pbackendStatus) = _HOST_UNAVAILABLE_;            
            // Dejar el BackEnd listo a pesar de haber errores generales
            else if(pbackendStatus && pTrxRes->BackEndAuthStatus() >= 0 )
                (*pbackendStatus) = _HOST_READY_;
            ////////////////////////////////////////////////////////////////////////////////////////
        }
        // Si en cambio el host esta en Corte de Red o No Disponible....
        else if(_CUTOVER_IN_PROGRESS_ == hostStatus ||                
                _HOST_UNAVAILABLE_    == hostStatus ||
                _HOST_STOPPED_        == hostStatus ||
                _NO_COMM_EVENTS_      == hostStatus)
        {
            // CUTOVER IN PROGRESS , AUTH-PROC NOT AVAILABLE, DESTINY NOT AVAILABLE
            // Insert Error Response Code { 91=Destino no disponible }
            WORD wRespCodeLen = 2;
            if(!isoMsg.IsValidField(39))
                isoMsg.InsertField( 39, &wRespCodeLen,(PBYTE)CISO_DESTINY_NOT_AVAILABLE, wRespCodeLen);
            else
                isoMsg.SetField( 39, &wRespCodeLen,(PBYTE)CISO_DESTINY_NOT_AVAILABLE, wRespCodeLen);
            wProcRespCde = CISO_DESTINY_NOT_AVAILABLE_I;
        }
        else 
        {
            bProcResult = is_false; // error        
        };
        ////////////////////////////////////////////////////////
        }
    else
#else
#error "Sin definir sistema a compilar"
#endif // _SYSTEM_ATM_    
        {
        // Invalid Transaction 
        // Insert Error Response Code { 40=Transaccion no soportada }
        WORD wRespCodeLen = 2;

        ///////////////////////////////////////////////////////////////////////////////////////
        // Reversals of Balance Inquiry must be replied as APPROVED
        if(isoMsg.IsATMBalanceInquiry() && isoMsg.IsValidReversal())
        {
            isoMsg.SetField( 39, &wRespCodeLen, (PBYTE)CISO_APPROVED, wRespCodeLen);
            wProcRespCde = CISO_APPROVED_I;
        }
        ///////////////////////////////////////////////////////////////////////////////////////
        else if(!isoMsg.IsValidField(39))
        {
            isoMsg.InsertField( 39, &wRespCodeLen, (PBYTE)CISO_TRANSACTION_NOT_SUPP, wRespCodeLen);
            wProcRespCde = CISO_TRANSACTION_NOT_SUPP_I;
        }
        else
        {
            isoMsg.SetField( 39, &wRespCodeLen, (PBYTE)CISO_TRANSACTION_NOT_SUPP, wRespCodeLen);
            wProcRespCde = CISO_TRANSACTION_NOT_SUPP_I;
        };
        

        // Set Message Type Response { 0210,230,430 }
        if( !isoMsg.SetMsgTypeIdResponse() )
            return (FALSE);
        // Procesada, pero con error
        bProcResult = is_true;
        }; // end-if-IsTransaction...
    //////////////////////////////////////////////////////////////////////////////
    // Fin del Bloque discriminador de Tipo de Transaccion    
    //////////////////////////////////////////////////////////////////////////////

    
    // Pudo procesarse?
    if(!bProcResult)
        // Error
        return (FALSE);
        
    // Copiar respuesta, si es copia solicitada en forma OnLine/OffLine:    
    if(pcbRspBuffer != NULL && piRspBuffLen != NULL)
        {
        // Copia external de la respuesta en formato ISO-8583
        WORD wBuffLen = (WORD)(*piRspBuffLen);
        if(!isoMsg.Export(pcbRspBuffer, &wBuffLen))
            {
            // Excepcion de exportacion de mensaje ISO-8583
            throw (int (-1));
            // Error
            return (FALSE);
            }
        else
            // Longitud exportada 
            (*piRspBuffLen) = (INT)wBuffLen;

        }

    // Y retornar verdadero, por transaccion procesada (con error o sin error)
    return (TRUE);
    };//end-ProcessTransaction


/////////////////////////////////////////////////////////////////////
