///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.             
// DPS-Online DATANET - Process Init Method
// 
// Método de Inicializacion de Procesamientos para transferencias DPS DATANET
//
// Tarea        Fecha           Autor   Observaciones
// (Beta)       2004.01.19		mdc		Metodo 'ProcessInit' pasado a archivo fuente separado
// (Beta)       2004.01.26		mdc		Seguimiento para DEBUG en archivo "debug.out"
// (Beta)       2004.01.28		mdc		Reversos (Estado=80) solo se hacen si los motivos de rechazo son CERO
// (Beta)       2004.05.07		mdc		Se agrega pre-validacion de que no retorne ERROR 19 para FACTURACION 
//                                      si acaso no se halla el ABONADO (especialmente para grupos economicos)
// (Delta)      2005.12.19		mdc		case DPS_PAGOS_AFIP /*NO_ASIGNADO_14*/ : break ;
// (Delta)      2006.02.22		mdc		Ademas de FALTAN DATOS (19), se demora tambien x SISTEMA NO DISP. (13)
// (Delta)      2006.04.03		mdc		_SYSTEM_DATANET_BATCH_ para MAC-2-3
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////


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
// Libreria std. STRING
#include <string.h>
// DPS BATCH
#ifdef _SYSTEM_DATANET_BATCH_	
#include <qusrinc/databatc.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Macro de verificacion de codigo de banco emisor/receptor , si es banco propio
#ifndef  IS_ON_OUR_BANKCODE
#define IS_ON_OUR_BANKCODE(x)   (antoi(x,3)==BCRA_ID_DEFAULT_I) 
#endif

///////////////////////////////////////////////////////////////////////
// Procesamiento Inicial, controles previo de transferencia
// bIsAuth = indica si el origen es el programa AUTORIZADOR (true) o 
//           el programa RECEPTOR (false).
///////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::ProcessInit(boolean_t bIsAuth)	// Proceso base, validacion		
{
    BYTE bMAC_Used = 0;
    return TrxResDATANET::ProcessInit(bIsAuth, &bMAC_Used);	// Proceso base, validacion		
};

///////////////////////////////////////////////////////////////////////
// Procesamiento Inicial, controles previo de transferencia
// bIsAuth = indica si el origen es el programa AUTORIZADOR (true) o 
//           el programa RECEPTOR (false).
///////////////////////////////////////////////////////////////////////
WORD  TrxResDATANET::ProcessInit(boolean_t bIsAuth, PBYTE pbMAC_Used)	// Proceso base, validacion		
{
	e_tipoope_dps toTipoOper = DPS_CUENTAS_PROPIAS;
	WORD  wRetVal            = DPS_APROBADA_SIN_RECHAZO;
	char  szEstadoCta[5]     = {"X"};
    BOOL  bValidateMAC       = _DPS_CONTROL_MAC_123_; // Validar MAC ?
#ifdef _SYSTEM_DATANET_BATCH_
    BOOL  bDPS_Batch_Mac2    = 1 ; // Usa MAC-1,2 O 3 ?
#endif
    // Codigo de retorno para comparar fechas
    e_settlement_retcode esReqDate  = ESTL_TRANSFERENCIA_OK,
                         esSendDate = ESTL_TRANSFERENCIA_OK; 
    // Indicativo de que es BANCO INEXISTENTE
    boolean_t bFALTAN_DATOS_EN_OPERACION = is_false;
    // Estado previo (por BANCO INEX.)
    WORD wESTADO_PREV  = dpsMsg.GetStatusCode(); //  Estado Previo de la transferencia
    //////////////////////////////////////////////////////////////////////////////////

	ResetStatus();
	
	// Pre-Validar datos de la transaccion, 0 por OK
 	if( !dpsMsg.IsDPSTransaction() ) 
		return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );		

    // Valores iniciales antes de iniciar
    szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;    // Transferencia por enviar    

    ///////////////////////////////////////////////////////////////////////////////
	// Si en los débitos el campo FECSOL contiene una fecha distinta a la del día, 
	// se responde con ESTADO="20" y queda la operación demorada hasta el próximo 
	// día de proceso. La aplicación deberá detectar la situación y ejecutarla 
	// automáticamente como si se hubiese recibido en ese momento desde la Red.
	// CONTROL 0: Validar fecha de procesamiento, solo a los debitos
    esReqDate = ESTL_TRANSFERENCIA_OK;
    wRetVal = ValidatePostingDate( &esReqDate , &esSendDate, is_true );
	if( wRetVal != DPS_APROBADA_SIN_RECHAZO )
	{
		// Formar respuesta c/varios motivos de rechazo
		dpsMsg.SetCauseCode( wRetVal, dpsMsg.IsDPSCreditTransfer() );        
		wRetVal = ( dpsMsg.IsDPSCreditTransfer() )
					    ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					    : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
        // Esta ya en un estado FINAL ?
        if( dpsMsg.IsResponseCodeInFinalState() )
        {
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ , szPENDENVIO);
            return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
        }
        else
        {
            // Cambio de Estado
            dpsMsg.SetStatusCode( wRetVal );
            // Loggear Transaccion con motivos informativos
            if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ );
        }
		// Retornar sin rta a la red
		return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED ) ;
	};
    // Es una fecha de solicitud diferida... mayor que hoy ....
    if( ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate )
    {       
        // ?Trata CREDITO diferido ? "SI", entonces enviar la respuesta
        if ( 'S' == dpsMsg.input.data.DIFCRE[0] 
            && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) )
        {
            // Marca de pendiente de envio en general (envio pendiente)
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
        }
        // ?Trata CREDITO diferido ? "NO", entonces diferir la respuesta
        else if ( 'S' != dpsMsg.input.data.DIFCRE[0] 
                  && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) )
        {
            // Marca de envio diferido en gral. 
            szPENDENVIO[0]  = _DPS_PEND_ENV_DIFERIDA_C_;             
        }
        // Sino , por omision
        else
        {
            // Marca de pendiente por omision
            szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_;
        };
    };// end-if-FECHA_SOLICITUD


    ///////////////////////////////////////////////////////////////////////////////////
	// Si el abonado asociado a la cuenta no existe, el banco rechazara el movimiento
	// cambiando a ESTADO a "70" u "80" e integrando el campo RECHDB o RECHCR con el 
	// valor "0001" según corresponda.
	// CONTROL 1: Validar abonado existente para las cuentas
	if( dpsMsg.IsDPSDebitTransfer()
        &&
        (wRetVal = ValidateAccountAssociate()) != DPS_APROBADA_SIN_RECHAZO)
	{
		// Formar respuesta con diferentes motivos de rechazo
		dpsMsg.SetCauseCode( wRetVal, dpsMsg.IsDPSCreditTransfer() );
        if(DPS_FALTAN_DATOS_EN_DETALLE_OPERACION == wRetVal ||
		   ///////////////////////////////////////////////////
		   // Tambien por SISTEMA NO DISPONIBLE se demora:
		   DPS_SISTEMA_NO_DISPONIBLE			 == wRetVal ||
		   DPS_APLICACION_NO_DISPONIBLE		     == wRetVal ||
		   DPS_DEMORADA_NO_ENVIAR_RESPUESTA      == wRetVal )
		   ///////////////////////////////////////////////////
		{
            wRetVal = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO ;
		}
        else
		{
		    wRetVal = ( dpsMsg.IsDPSCreditTransfer() )
			    		    ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
				    	    : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
		}
        ///////////////////////////////////////////////////////////////////////////
        // POST-CONTROL 8 : Si es una transferencia y ya se encuentra 
        // en estado final, no se debe responder.
        if( dpsMsg.IsResponseCodeInFinalState() )
        {
            if(DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO == wRetVal)
                szPENDENVIO[0] = _DPS_PEND_ENV_FALLIDA_C_;
            else
                szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ , szPENDENVIO);
            return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
        };
        // Cambio de Estado, despues de controlar si ya estaba en ESTADO FINAL
        dpsMsg.SetStatusCode( wRetVal );
        ///////////////////////////////////////////////////////////////////////////
        // POST-CONTROL 9: Si no existe BANCO/ABONADO, marcar como FALLIDA
        if(DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO == wRetVal)
        {
            szPENDENVIO[0] = _DPS_PEND_ENV_FALLIDA_C_;
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PENDIENTE_ , szPENDENVIO);
            return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
        } 
        ///////////////////////////////////////////////////////////////////////////
        // POST-CONTROL 7 : Si es una transferencia PERO el banco credito no trata
        // las respuestas, no se debe enviar ninguna respuesta hacia la red.
        else if ( 'S' != dpsMsg.input.data.DIFCRE[0] 
            && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE)
            && ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate)
        {
            // El campo "esSenddate" contiene el resultado de comparar
            // FEC-SOLICITUD vs FEC-ENVIO.
            if(ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate
                &&
               ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esSendDate)            
                 szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;       //  Estado de la respuesta es "N"
            else
                 szPENDENVIO[0]  = _DPS_PEND_ENV_DIFERIDA_C_; //  Estado de la respuesta es "D"
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PEND_PROX_DIA_ , szPENDENVIO);
            return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
        }
        else if ( 'S' != dpsMsg.input.data.DIFCRE[0]
            && IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE)  // Si es nuestro banco
            && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.BANDEB)        // No es nuestro banco
            && ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate )
        {
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PEND_PROX_DIA_ , szPENDENVIO);
            return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
        }
        ///////////////////////////////////////////////////////////////////////////
        else
        {
            dpsMsg.SetStatusCode( wRetVal );
		    // Loggear Transaccion para su posterior autorizacion
		    if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ );
            // Retorno del motivo hacia nivel superior de invocacion
		    return ( DPS_RECHAZO_DEL_BANCO_DE_DEBITO );
        };
	};

    ///////////////////////////////////////////////////////////////////////////////////
	// Cada mensaje recibido, tanto débito como crédito, tiene como control, la 
	// verificación de la clave MAC. Esta deberá ser recalculada con el algoritmo 
	// MAC1, en el caso de un crédito con ESTADO = "00", o MAC2 para el resto.
	// Si la clave calculada no se corresponde con la recibida, se procederá a rechazar
	// la operación cambiando el ESTADO a "70" u "80" e integrando el campo RECHDB o 
	// RECHCR con el valor "0033" según corresponda. 
	// Si no se puede calcular la clave, se demora con estado "20" .	
	// CONTROL 2: Validar MAC de entrada, MAC 1 o 2 segun sea CREDITO o no,
    // unicamente SI NO PROVIENE del autorizador, SI proviniendo del RECEPTOR.
    if( is_false == bIsAuth )
    {
#if (defined(_SYSTEM_DATANET_BATCH_) && FALSE)
		/************************************************************************/
        if( antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_ENVIADO_POR_LA_RED                   ||
			antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_ENVIADA_AL_BANCO_DE_DEBITO           ||
			antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_PENDIENTE_AUTORIZ_CTA_CREDITO        ||
			antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED ||
			antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_RECHAZO_CUENTA_CREDITO )
		{
            wRetVal = DPS_APROBADA_SIN_RECHAZO;
            (*pbMAC_Used) = 0;
		}
		/************************************************************************/
        else if( antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_DEMORADA_POR_EL_BANCO_DE_DEBITO ||
				 antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_VALOR_AL_COBRO				  ||
                 antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_ENVIADA_AL_BANCO_DE_CREDITO	  ||
                 antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_RECHAZO_DEL_BANCO_DE_DEBITO	  ||
                 ( antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_EJECUTADA && dpsMsg.input.data.RIEBCO[0] == ' ')
               )
		{
            wRetVal = ValidateMAC2();
            (*pbMAC_Used) = 2;
		}
        else if( antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_RECHAZO_DEL_BANCO_DE_CREDITO ||
                 ( antoi(dpsMsg.input.data.ESTADO,2)==DPS_B_EJECUTADA && dpsMsg.input.data.RIEBCO[0] != ' ')
               )

		{
            wRetVal = ValidateMAC3();
            (*pbMAC_Used) = 3;
		}
        else
        {
            wRetVal = DPS_SISTEMA_NO_DISPONIBLE;
            (*pbMAC_Used) = 0;
        };
#else // _SYSTEM_DATANET_BATCH_
		/************************************************************************/
        if( dpsMsg.IsDPSDebitTransfer() 
            && 
            antoi(dpsMsg.input.data.ESTADO,2)==DPS_RECHAZO_DEL_BANCO_DE_CREDITO ) 
		{
            wRetVal = ValidateMAC3();
		}
		/************************************************************************/
        else if( dpsMsg.IsDPSCreditTransfer() ) 
		{
            wRetVal = ValidateMAC2();
		}
		/************************************************************************/
        else                                    
		{
            wRetVal = ValidateMAC1();    
		};
#endif // _SYSTEM_DATANET_BATCH_

		/************************************************************************/
	    if( DPS_APROBADA_SIN_RECHAZO != wRetVal && bValidateMAC )
	    {
            // BANCO INEX.?
            if(wRetVal == DPS_FALTAN_DATOS_EN_DETALLE_OPERACION)
                bFALTAN_DATOS_EN_OPERACION = is_true;
            // Excepto reverso por RIESGO RED EXCEDIDO porque no viene el MAC-1/2/3
            if( antoi(dpsMsg.input.data.ESTADO,2) == DPS_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED )
            {
                wRetVal = DPS_APROBADA_SIN_RECHAZO;
            }
            // Excepto que sea FACTURACION, porque no se calcula el MAC-1 por definicion 
            else if( dpsMsg.GetOperType(NULL) == DPS_FACTURACION)
            {
                // A menos que sea un "FALTAN DATOS EN OP" , (BANCO INEX)
                if(wRetVal != DPS_FALTAN_DATOS_EN_DETALLE_OPERACION)
                    wRetVal = DPS_APROBADA_SIN_RECHAZO;
                else      		    
    		        bFALTAN_DATOS_EN_OPERACION = is_true;
            }
            // Excepto EJECUTADA que ya viene en un estado definitivo y se debe aceptar
            else if( antoi(dpsMsg.input.data.ESTADO,2) == DPS_EJECUTADA )
            {
                // A menos que sea un "FALTAN DATOS EN OP" , (BANCO INEX)
                if(wRetVal != DPS_FALTAN_DATOS_EN_DETALLE_OPERACION)
                {
                    //  Motivo de rechazo debito / credito en cero
                    if( antoi(dpsMsg.input.data.DATDEB.RECHDB,4)==0 
                        && 
                        antoi(dpsMsg.input.data.DATCRE.RECHCR,4)==0 ) 
                    {
                        wRetVal = DPS_APROBADA_SIN_RECHAZO;
                    }
                }
                else      		    
    		        bFALTAN_DATOS_EN_OPERACION = is_true;
            }
            // Excepto RECHAZO BANCO CREDITO
            else if( antoi(dpsMsg.input.data.ESTADO,2) == DPS_RECHAZO_DEL_BANCO_DE_CREDITO )
            {
                //  Motivo de rechazo debito / credito
                if( antoi(dpsMsg.input.data.DATDEB.RECHDB,4)==0 
                    && 
                    antoi(dpsMsg.input.data.DATCRE.RECHCR,4)==0 ) //  Motivo de rechazo CR-DB
                {
                    wRetVal = DPS_APROBADA_SIN_RECHAZO;
                }
                //////////////////////////////////////////////////////////////////////
                //  Motivo de rechazo debito / credito
                else if(antoi(dpsMsg.input.data.DATDEB.RECHDB,4)==0 
                        && 
                        antoi(dpsMsg.input.data.DATCRE.RECHCR,4)!=0) //  Motivo de rechazo CR-DB
                {
                    wRetVal = DPS_APROBADA_SIN_RECHAZO;
                };
                //////////////////////////////////////////////////////////////////////
            }
            else
            {
		        // Formar respuesta con diferentes motivos de rechazo
		        dpsMsg.SetCauseCode( wRetVal, dpsMsg.IsDPSCreditTransfer() );
                // Se rechaza por erronea o se demora
                if(DPS_FALTAN_DATOS_EN_DETALLE_OPERACION == wRetVal)
                {
                    wRetVal = DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO ;
                    bFALTAN_DATOS_EN_OPERACION = is_true;
                }
                else
		            wRetVal = ( dpsMsg.IsDPSCreditTransfer() )
			    		            ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
				    	            : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
                ///////////////////////////////////////////////////////////////////////////
                // POST-CONTROL 8 : Si es una transferencia y ya se encuentra 
                // en estado final, no se debe responder.
                if( dpsMsg.IsResponseCodeInFinalState() )
                {                
                    if(DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO == wRetVal)
                        szPENDENVIO[0] = _DPS_PEND_ENV_FALLIDA_C_;
                    else
                        szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		        // Loggear Transaccion c/motivo de rechazo
	    	        if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ , szPENDENVIO);
                    return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
                };
                // Cambio de Estado, despues de controlar si ya estaba en ESTADO FINAL
                dpsMsg.SetStatusCode( wRetVal );
                ///////////////////////////////////////////////////////////////////////////
                // POST-CONTROL 9: Si no existe BANCO/ABONADO, marcar como FALLIDA
                if(DPS_DEMORADA_POR_EL_BANCO_DE_DEBITO == wRetVal)
                {
                    szPENDENVIO[0] = _DPS_PEND_ENV_FALLIDA_C_;
    		        // Loggear Transaccion c/motivo de rechazo
	    	        if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PENDIENTE_ , szPENDENVIO);
                    return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
                }
                ///////////////////////////////////////////////////////////////////////////
                // POST-CONTROL 7 : Si es una transferencia PERO el banco credito no trata
                // las respuestas, no se debe enviar ninguna respuesta hacia la red.
                else if ( 'S' != dpsMsg.input.data.DIFCRE[0] 
                    && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE)
                    && ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate)
                {
                    // Marca de diferida en general 
                    // El campo "esSenddate" contiene el resultado de comparar
                    // FEC-SOLICITUD vs FEC-ENVIO.
                    if(ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate
                        &&
                       ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esSendDate)            
                         szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;       //  Estado de la respuesta es "N"
                    else
                         szPENDENVIO[0]  = _DPS_PEND_ENV_DIFERIDA_C_; //  Estado de la respuesta es "D"
    		        // Loggear Transaccion c/motivo de rechazo
	    	        if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PEND_PROX_DIA_ , szPENDENVIO);
                    return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
                }
                else if ( 'S' != dpsMsg.input.data.DIFCRE[0]
                    && IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE) 
                    && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.BANDEB)        // No es nuestro banco
                    && ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate )
                {
                    szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		        // Loggear Transaccion c/motivo de rechazo
	    	        if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PEND_PROX_DIA_ , szPENDENVIO);
                    return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
                }
                ///////////////////////////////////////////////////////////////////////////
                else
                {
		            // Loggear Transaccion c/motivo de rechazo
		            if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ );
                    // Retorno del motivo hacia nivel superior de invocacion
		            return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
                };//end-if-TRATA-DIFERIDAS
            };//end-if-ERROR
        }; // end-if-VALIDATE-MAC
    };// end-if-FROM-AUTH 

    ///////////////////////////////////////////////////////////////////////////////////
	// Si la cuenta debito o crédito no existe, el banco rechazara el movimiento 
	// cambiando a ESTADO a "70" u "80" e integrando el campo RECHDB o RECHCR con 
	// el valor "0001" según corresponda.
	// CONTROL 3: Validar cuentas existentes
    if( DPS_APROBADA_SIN_RECHAZO == wRetVal )
        wRetVal = ValidateExistingAccount();
	if( DPS_APROBADA_SIN_RECHAZO != wRetVal )
    {   
		
		/// begin-2006.02.23-mdc /////////////////////////////////////////////////
		// Verificar nuevos retornos
		if( DPS_PASADA_A_PROCESO_BATCH				== wRetVal ||
			DPS_DEMORADA_NO_ENVIAR_RESPUESTA		== wRetVal ||
			DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED  == wRetVal )
		{
			if( DPS_DEMORADA_NO_ENVIAR_RESPUESTA == wRetVal )
				dpsMsg.SetCauseCode( DPS_FUERZA_MAYOR, dpsMsg.IsDPSCreditTransfer() );
			else
				dpsMsg.SetCauseCode( DPS_APROBADA_SIN_RECHAZO, dpsMsg.IsDPSCreditTransfer() );			
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;      //  Estado de NO ENVIAR RTA.
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth) LoggTransaction( _DPS_TRAT_PENDIENTE_ , szPENDENVIO);
			if( DPS_DEMORADA_NO_ENVIAR_RESPUESTA == wRetVal )
				return ( DPS_DEMORADA_NO_ENVIAR_RESPUESTA );
			else
				return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
		}
		/// end-2006.02.23-mdc ///////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Formar respuesta c/diferentes motivos de rechazo
		dpsMsg.SetCauseCode( wRetVal, dpsMsg.IsDPSCreditTransfer() );
		wRetVal = ( dpsMsg.IsDPSCreditTransfer() )
					    ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					    : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;        
        ///////////////////////////////////////////////////////////////////////////
        // POST-CONTROL 8 : Si es una transferencia y ya se encuentra 
        // en estado final, no se debe responder. A menos que sea un credito rechazado.
        if(dpsMsg.IsDPSCreditTransfer() && DPS_RECHAZO_DEL_BANCO_DE_CREDITO == wRetVal)
        {
        }
        else if( dpsMsg.IsResponseCodeInFinalState() )
        {
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth) 
            {
                if(!bFALTAN_DATOS_EN_OPERACION)
                    wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ , szPENDENVIO);
                else
                    wRetVal = LoggTransaction( _DPS_TRAT_PENDIENTE_ , szPENDENVIO);
            }
            return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
        };
        // Cambio de Estado, despues de controlar si ya estaba en ESTADO FINAL
        dpsMsg.SetStatusCode( wRetVal );
        //////////////////////////////////////////////////////////////////////////
        // Verificar que para DOLARES jamas se procesen diferidas por norma BCRA
        // aunque la cuenta este en estado invalido (CONTROL REITERADO)
        if( ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate 
            &&
           (antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CUENTA_CORRIENTE_USD ||
            antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CAJA_DE_AHORROS_USD)
            &&
           (antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CUENTA_CORRIENTE_USD ||
            antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CAJA_DE_AHORROS_USD))
        {
            // Diferimiento, no se tratan DOLARES diferidos 
            wRetVal = DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED;
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PEND_PROX_DIA_ , szPENDENVIO);
            return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
        }
        /////////////////////////////////////////////////////////////////////
        // FEC-SOLICITUD vs FEC-ENVIO : difieren a hoy y respuesta pendiente?
        else if(ESTL_TRANSFERENCIA_OK                               == esReqDate
                &&
                ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esSendDate
                &&
                wESTADO_PREV == DPS_VALOR_AL_COBRO  )
        {                  
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;      //  Estado de NO ENVIAR RTA.
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ , szPENDENVIO);
            return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
        }
        ////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////
        // POST-CONTROL 7 : Si es una transferencia PERO el banco credito no trata
        // las respuestas, no se debe enviar ninguna respuesta hacia la red.
        else if ( 'S' != dpsMsg.input.data.DIFCRE[0] 
            && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE)
            && ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate)
        {
            // Marca de diferida en general 
            if(ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate
                &&
               ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esSendDate)            
                 szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;       //  Estado de la respuesta es "N"
            else
                 szPENDENVIO[0]  = _DPS_PEND_ENV_DIFERIDA_C_; //  Estado de la respuesta es "D"
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth) 
            {
                if(!bFALTAN_DATOS_EN_OPERACION)
                    wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ , szPENDENVIO);
                else
                    wRetVal = LoggTransaction( _DPS_TRAT_PENDIENTE_ , szPENDENVIO);
            }
            return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
        }
        else if ( 'S' != dpsMsg.input.data.DIFCRE[0]
            && IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE)  // Si es nuestro banco
            && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.BANDEB)        // No es nuestro banco
            && ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate )
        {
            szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth) 
            {
                if(!bFALTAN_DATOS_EN_OPERACION)
                    wRetVal = LoggTransaction( _DPS_TRAT_PEND_PROX_DIA_ , szPENDENVIO);
                else
                    wRetVal = LoggTransaction( _DPS_TRAT_PENDIENTE_ , szPENDENVIO);
            }
            return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
        }
        ///////////////////////////////////////////////////////////////////////////
        else
        {
    		// Loggear Transaccion c/motivo de rechazo
	    	if(!bIsAuth)
            {
                ///////////////////////////////////////////
                // Por omision, es pendiente de envio
                szPENDENVIO[0] = _DPS_PEND_ENV_DEFAULT_C_; 
                ///////////////////////////////////////////
                if(!bFALTAN_DATOS_EN_OPERACION)
                    wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ , szPENDENVIO);
                else
                    wRetVal = LoggTransaction( _DPS_TRAT_PENDIENTE_ , szPENDENVIO);
            }
    		return ( dpsMsg.IsDPSCreditTransfer() )
					  ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					  : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
        }//end-if-final-state
	}//end-if-valid-account
	
    ///////////////////////////////////////////////////////////////////////////////////
	// Si la cuenta debito o crédito estuviese cerrada o en vías de cierre o embargada
	// o en algún estado no operativo, el banco rechazara el movimiento cambiando a 
	// ESTADO a "70" u "80" e integrando el campo RECHDB o RECHCR con el valor "0011",
	// o "0003", o desde "0005" al "0007", según corresponda.
	// CONTROL 4: Validar cuentas en estado valido
	if( (wRetVal = ValidateAccountStatus(szEstadoCta)) != DPS_APROBADA_SIN_RECHAZO)
	{
        //////////////////////////////////////////////////////////////////////////////
        // Verificar caso de EXCEPCION para creditos con cuentas bloqueadas, estado 40
        if( dpsMsg.IsDPSCreditTransfer()       &&  
            DPS_VALOR_AL_COBRO == wESTADO_PREV &&
            DPS_CUENTA_BLOQUEADA == wRetVal     )
        {            
		    // Formar respuesta con diferentes motivos de rechazo
		    dpsMsg.SetCauseCode( wRetVal, dpsMsg.IsDPSCreditTransfer() ); 
        }
        //////////////////////////////////////////////////////////////////////////////
        else        
        {
		    // Formar respuesta con diferentes motivos de rechazo
		    dpsMsg.SetCauseCode( wRetVal, dpsMsg.IsDPSCreditTransfer() );
		    wRetVal = ( dpsMsg.IsDPSCreditTransfer() )
					        ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					        : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
            ///////////////////////////////////////////////////////////////////////////
            // POST-CONTROL 8 : Si es una transferencia y ya se encuentra 
            // en estado final, no se debe responder.
            if(dpsMsg.IsDPSCreditTransfer() && DPS_RECHAZO_DEL_BANCO_DE_CREDITO == wRetVal)
            {
            }
            else if( dpsMsg.IsResponseCodeInFinalState() && dpsMsg.IsDPSCreditTransfer() )
            {
                szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		    // Loggear Transaccion c/motivo de rechazo
	    	    if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ , szPENDENVIO);
                return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
            };
            // Cambio de Estado, despues de controlar si ya estaba en ESTADO FINAL
            dpsMsg.SetStatusCode( wRetVal );
            //////////////////////////////////////////////////////////////////////////
            // Verificar que para DOLARES jamas se procesen diferidas por norma BCRA
            // aunque la cuenta este en estado invalido (CONTROL REITERADO)
            if( ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate 
                &&
               (antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CUENTA_CORRIENTE_USD ||
                antoi(dpsMsg.input.data.DATDEB.TIPCUE,2 ) == DPS_CAJA_DE_AHORROS_USD)
                &&
               (antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CUENTA_CORRIENTE_USD ||
                antoi(dpsMsg.input.data.DATCRE.TIPCRE,2 ) == DPS_CAJA_DE_AHORROS_USD))
            {
                // Diferimiento, no se tratan DOLARES diferidos 
                wRetVal = DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED;
                szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		    // Loggear Transaccion c/motivo de rechazo
	    	    if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PEND_PROX_DIA_ , szPENDENVIO);
                return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
            }
            /////////////////////////////////////////////////////////////////////
            // FEC-SOLICITUD vs FEC-ENVIO : difieren a hoy y respuesta pendiente?
            else if(ESTL_TRANSFERENCIA_OK                               == esReqDate
                    &&
                    ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esSendDate
                    &&
                    wESTADO_PREV == DPS_VALOR_AL_COBRO  )
            {                  
                szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;      //  Estado de NO ENVIAR RTA.
    		    // Loggear Transaccion c/motivo de rechazo
	    	    if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ , szPENDENVIO);
                return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
            }
            ////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////
            // POST-CONTROL 7 : Si es una transferencia PERO el banco credito no trata
            // las respuestas, no se debe enviar ninguna respuesta hacia la red.
            else if ( 'S' != dpsMsg.input.data.DIFCRE[0] 
                && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE)
                && ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate)
            {
                // Marca de diferida en general 
                if(ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate
                    &&
                   ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esSendDate)            
                     szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;       //  Estado de la respuesta es "N"
                else
                     szPENDENVIO[0]  = _DPS_PEND_ENV_DIFERIDA_C_; //  Estado de la respuesta es "D"
    		    // Loggear Transaccion c/motivo de rechazo
	    	    if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PEND_PROX_DIA_ , szPENDENVIO);
                return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
            }
            else if ( 'S' != dpsMsg.input.data.DIFCRE[0]
                && IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE)  // Si es nuestro banco
                && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.BANDEB)        // No es nuestro banco
                && ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate )
            {
                szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		    // Loggear Transaccion c/motivo de rechazo
	    	    if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PEND_PROX_DIA_ , szPENDENVIO);
                return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
            }
            ///////////////////////////////////////////////////////////////////////////
            else
            {
		        // Loggear Transaccion c/motivo de rechazo
		        if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ );
                // Retorno del motivo hacia nivel superior de invocacion
	            return ( dpsMsg.IsDPSCreditTransfer() )
				          ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
				          : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
            };

        };//end-if-excepcion-valores-al-cobro

	};

    ///////////////////////////////////////////////////////////////////////////////////
	// El primer control ineludible es a nivel contenidos de campos 
	// y validez de los mismos. Es asi que si la operación no es reconocida
	// o es invalida, se procederá a rechazar la misma cambiando el ESTADO a 
	// "70" u "80" e integrando el campo RECHDB o RECHCR con el valor "0020" o 
	// "0021" según corresponda.
	// CONTROL 5: Verificar tipo de operacion
	toTipoOper = (e_tipoope_dps) dpsMsg.GetOperType( NULL );
	switch( toTipoOper )
	{
        /* Las siguientes SI estan soportadas */
		case DPS_CUENTAS_PROPIAS	: break ;
		case DPS_SUELDOS			: break ;
		case DPS_PROVEEDORES		: break ;
		case DPS_ANA_TRADICIONAL	: break ;
		case DPS_ANA_SISTEMA_MARIA	: break ;
		case DPS_DGI				: break ;
		case DPS_FACTURACION		: break ; 
		case DPS_CUENTA_RECAUDADORA	: break ; 
		case DPS_SERVICIOS			: break ;
		case DPS_TESTING			: break ;		
		case DPS_INTERPAGOS			: break ; 
		case DPS_PAGO_DE_SEGUROS	: break ;		
		case DPS_TRANSFERENCIAS_CCI	: break ;
		case DPS_PAGOS_AFIP /*NO_ASIGNADO_14*/ : break ;
        /* Las siguientes NO estan soportadas */
        case DPS_NO_ASIGNADO_11		:         
		case DPS_NO_ASIGNADO_16		: 
		case DPS_NO_ASIGNADO_17		: 
		case DPS_NO_ASIGNADO_18		: 
		case DPS_NO_ASIGNADO_19		: 
		default                     : 	
            // Indicar motivo-cause del rechazo
			dpsMsg.SetCauseCode( DPS_OPERACION_NO_HABILITADA_EN_BANCO, 
				dpsMsg.IsDPSCreditTransfer() );
			wRetVal = ( dpsMsg.IsDPSCreditTransfer() )
				? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
				: DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
            ///////////////////////////////////////////////////////////////////////////
            // POST-CONTROL 8 : Si es una transferencia y ya se encuentra 
            // en estado final, no se debe responder.
            if( dpsMsg.IsResponseCodeInFinalState() )
            {
                strcpy(szPENDENVIO, _DPS_PEND_ENV_NO_ );
    		    // Loggear Transaccion c/motivo de rechazo
	    	    if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ , szPENDENVIO);
                return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
            };
            // Cambio de Estado, despues de controlar si ya estaba en ESTADO FINAL
            dpsMsg.SetStatusCode( wRetVal );
            ///////////////////////////////////////////////////////////////////////////
            // POST-CONTROL 7 : Si es una transferencia PERO el banco credito no trata
            // las respuestas, no se debe enviar ninguna respuesta hacia la red.
            if ( 'S' != dpsMsg.input.data.DIFCRE[0] 
                && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE)
                && ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate)
            {
                // Marca de diferida en general 
                if(ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate
                    &&
                   ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esSendDate)            
                     szPENDENVIO[0]  = _DPS_PEND_ENV_NO_C_;       //  Estado de la respuesta es "N"
                else
                     szPENDENVIO[0]  = _DPS_PEND_ENV_DIFERIDA_C_; //  Estado de la respuesta es "D"
    		    // Loggear Transaccion c/motivo de rechazo
	    	    if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PEND_PROX_DIA_ , szPENDENVIO);
                return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
            }
            // Banco Credito en caso de que seamos nosotros, NO TRATA DIFERIDAS CREDITO
            else if ( 'S' != dpsMsg.input.data.DIFCRE[0]
                && IS_ON_OUR_BANKCODE(dpsMsg.input.data.DATCRE.BANCRE)  // Si es nuestro banco
                && !IS_ON_OUR_BANKCODE(dpsMsg.input.data.BANDEB)        // No es nuestro banco
                && ESTL_FECHA_DE_SOLICITUD_MAYOR_FECHA_DE_HOY_O_MANANA == esReqDate )
            {
                szPENDENVIO[0] = _DPS_PEND_ENV_NO_C_;
    		    // Loggear Transaccion c/motivo de rechazo
	    	    if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PEND_PROX_DIA_ , szPENDENVIO);
                return ( DPS_NO_ENVIAR_RESPUESTA_HACIA_LA_RED );
            }
            ///////////////////////////////////////////////////////////////////////////
            else
            {
		        // Loggear Transaccion c/motivo de rechazo
		        if(!bIsAuth) wRetVal = LoggTransaction( _DPS_TRAT_PROCESADA_ );
				return ( dpsMsg.IsDPSCreditTransfer() )
					    ? DPS_RECHAZO_DEL_BANCO_DE_CREDITO
					    : DPS_RECHAZO_DEL_BANCO_DE_DEBITO ;
            };//end-if-ERROR
            break;
	};//end-SWITCH

    ///////////////////////////////////////////////////////////////////////////////////
	// Validaron todos los controles! OK ! Seguir adelante para autorizar....
	return ( DPS_APROBADA_SIN_RECHAZO );
}
