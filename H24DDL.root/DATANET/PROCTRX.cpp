///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// Process DATANET Transaction Method
//
//   Método de Procesamiento de Transacción DATANET, con manejo local de parsing del mensaje DPS
// y seteo del código de respuesta en el mensaje saliente.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2003.03.21		mdc		Base
// (Inicial)    2003.04.07		mdc		ValidateMAC1, 2, 3
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Headers ANSI C/C++
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

// Header Type Definitions
#include <qusrinc/typedefs.h>    
// Header Transaction Resolution
#include <qusrinc/trxdnet.h>
// DPS Msg Redefinition
#include <qusrinc/datanet.h>
// DPS SYSTEM
#include <qusrinc/dpcsys.h>

///////////////////////////////////////////////////////////////////////
// Procesamiento de Transacciones.en general
// Retorna TRUE por procesamiento aprobado o no,
// y FALSE al hallar algún Error en el Sistema o en el Mensaje ISO-8583.
// Verifica también si la respuesta es solicitada (OnLine) o no 
// (OffLine) para la copia de datos en el mensaje de retorno.
BOOL _TRX_RESOLUTION_::ProcessTransaction( PBYTE  pcbBuffer   , 
										INT    iBuffLen    ,
										PBYTE  pcbRspBuffer,
										PINT   piRspBuffLen,
										enumHostStatus hostStatus,
										enumFormatType efExtMsgFormat,
										enumHostStatus *pbackendStatus) 
{	
	CHAR szMAC2[DNET_MAC_LENGTH+1] = {"            "}, 
         szMAC3[DNET_MAC_LENGTH+1] = {"            "};
	// Booleanos de respuesta de o hacia funciones
	// (Pertenece a la clase base TrxBaseResolution)
	bProcResult = is_false;       // Sin procesar inicialmente, default
	///////////////////////////////////////////////////////////////////
	efFormatMsg = efExtMsgFormat; // Formato de mensaje actual
	///////////////////////////////////////////////////////////////////

    // Precondicion: Ptrs validos
    if(!pcbBuffer || iBuffLen <= 0 || !pcbRspBuffer || !piRspBuffLen || *piRspBuffLen <= 0)
		// Error
		return (FALSE);

    // Precondicion: Mensaje DPS valido
	// Import DPS Message
	if(!dpsMsg.Import(pcbBuffer, iBuffLen))
		// Error
		return (FALSE);
		
	// Is Valid Message?
	if(!dpsMsg.IsValidMessage())
		// Error
		return (FALSE);

	///////////////////////////////////////////////////////////////
	// Antes de modificar el requerimiento como respuesta, 
	// guardar un backup original para no perder los datos !
	dpsMsgBkp = dpsMsg;
	// Import DPS Message Backup (EBCDIC backup)
	if(!dpsMsgBkp.Import(pcbBuffer, iBuffLen))
		// Error
		return (FALSE);
	///////////////////////////////////////////////////////////////


#if defined( _SYSTEM_DATANET_COBIS_ )	
	if( dpsMsg.IsDPSTransaction() ) // DPS-DATANET?
		{
		////////////////////////////////////////////////////////
		// ClassCasting, a clase de resolución ATM
		TrxResCOBIS   *pTrxRes      = dynamic_cast<TrxResCOBIS*>(this);		

		// Procesar si el host esta ONLINE y DISPONIBLE
		if(( _HOST_READY_ == hostStatus ) && pTrxRes )
		{
			// DPS default
			bProcResult = pTrxRes->ProcessTransaction();
			(*pbackendStatus) = _HOST_READY_;
			/////////////////////////////////////////////////////
		}
		// Si en cambio el host esta en Corte de Red o No Disponible....
		else if(_CUTOVER_IN_PROGRESS_ == hostStatus ||			    
			    _HOST_UNAVAILABLE_    == hostStatus ||
				_HOST_STOPPED_        == hostStatus ||
				_NO_COMM_EVENTS_      == hostStatus)
		{
			// CUTOVER IN PROGRESS , AUTH-PROC NOT AVAILABLE, DESTINY NOT AVAILABLE
			// Insert Error Response Code { 91=Destino no disponible }
			dpsMsg.SetStatusCode( (WORD)DPS_SISTEMA_NO_DISPONIBLE);
			wProcRespCde = (WORD)DPS_SISTEMA_NO_DISPONIBLE;
		}
		else 
		{
			bProcResult = is_false; // error		
		};
		////////////////////////////////////////////////////////
		}
	else
		{
		// Set Message Type Response { 0210,230,430 }
		if( !dpsMsg.FormatResponseMessage( szMAC2, szMAC3 ) )
			return (FALSE);
		// Invalid Transaction 
		// Insert Error Response Code { 40=Transacción no soportada }
		dpsMsg.SetStatusCode( (WORD)DPS_OPERACION_INEXISTENTE );
		wProcRespCde = (WORD)DPS_OPERACION_INEXISTENTE;
		// Procesada, pero con error
		bProcResult = is_true;
		}; // end-if-IsTransaction...
#else 
	if( dpsMsg.IsDPSTransaction() ) // DPS-DATANET?
		{
		////////////////////////////////////////////////////////
		// ClassCasting, a clase de resolución DPS
#if defined( _SYSTEM_DATANET_JERONIMO_ )	
		TrxResJERONIMO   *pTrxRes      = dynamic_cast<TrxResJERONIMO*>(this);		
#elif defined( _SYSTEM_DATANET_ON2_ )	
		TrxResON2   *pTrxRes      = dynamic_cast<TrxResON2*>(this);		
#elif defined( _SYSTEM_DATANET_BATCH_ )	
		TrxResDATANET   *pTrxRes      = dynamic_cast<TrxResDATANET*>(this);		
#else
#error "Sin definir sistema a compilar"
#endif

		// Procesar si el host esta ONLINE y DISPONIBLE
		if(( _HOST_READY_ == hostStatus ) && pTrxRes )
		{
			// DPS default
			bProcResult = pTrxRes->ProcessTransaction();
			(*pbackendStatus) = _HOST_READY_;
			/////////////////////////////////////////////////////
		}
		else 
		{
			bProcResult = is_false; // error		
		};
		////////////////////////////////////////////////////////
		}
	else
		{
		// Set Message Type Response { 0210,230,430 }
		if( !dpsMsg.FormatResponseMessage( szMAC2, szMAC3 ) )
			return (FALSE);
		// Invalid Transaction 
		// Insert Error Response Code { 40=Transacción no soportada }
		dpsMsg.SetStatusCode( (WORD)DPS_OPERACION_INEXISTENTE );
		wProcRespCde = (WORD)DPS_OPERACION_INEXISTENTE;
		// Procesada, pero con error
		bProcResult = is_true;
		}; // end-if-IsTransaction...
#endif // _SYSTEM_DATANET_COBIS_	
	//////////////////////////////////////////////////////////////////////////////
	// Fin del Bloque discriminador de Tipo de Transacción	
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
		if(!dpsMsg.Export(pcbRspBuffer, &wBuffLen))
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

	// Y retornar verdadero, por transacción procesada (con error o sin error)
	return (TRUE);
	};//end-ProcessTransaction



////////////////////////////////////////////////////////////////////////////////////////