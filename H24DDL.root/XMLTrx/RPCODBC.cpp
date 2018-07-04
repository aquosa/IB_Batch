///////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A.
// RPC-CONNECTOR (ODBC+RPC+EXTRACT) for DATANET system (BANCO TIERRA DEL FUEGO y otros)
//
// Tarea        Fecha           Autor   Observaciones
// (1.0.1.0)	2007.08.15      mdc     Base tomando como ejemplo lo del BISEL y CIUDAD
// (1.0.1.1)	2007.09.04      mdc     #include <qusrinc/trxdnet.h>
// (1.0.1.2)	2008.09.01      mdc     Operadores 'S','C'.
///////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
#undef DNET_IS_ACTION_OK
#undef DNET_IS_ACTION_ERROR

#include <qusrinc/rpclib.h>
#ifndef _SQL_ODBC_SENTENCES_
#define _SQL_ODBC_SENTENCES_
#endif // _SQL_ODBC_SENTENCES_

#include <sqlext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/////////////////////////////////////////////////

#include <qusrinc/datanet.h>
#include <qusrinc/trxdnet.h>

#ifndef ODBC_DATANET_DATABASE
#define ODBC_DATANET_DATABASE               "datanet"
#define ODBC_DATANET_USER                   "datanet"
#define ODBC_DATANET_USER_PASSWORD          "datanet"
#define _SQL_INSERT_REQR_SNTC_              ""
#define _SQL_SELECT_SERIAL_SNTC_            ""
#define _SQL_SELECT_RESPONSE_SNTC_          ""
#endif

//////////////////////////////////////////////////////////////////////
// Coneccion a base de datos --- si es necesaria
//////////////////////////////////////////////////////////////////////
void RPC_CONNECTOR_DNET::ConnectDataBase( char *szDBase, char *szDBaseUser, char *szPassw )
{
    // Guardar popiedades de conexion a la b.datos
    SetDataSourceName( szDBase );
    SetUserName( szDBaseUser );
    SetPassword( szPassw );
    // Conectarse a la B.datos
    Connect( szDBase );    
}


//////////////////////////////////////////////////////////////////////
// Logg Transaccion DATANET en TEF ONLINE
//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::LoggTransaction(
	char szFRNAME [],// Nombre nodo emisor, ej. "DNET", char(4)
	char szTONAME [], //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
	char szRECTYPE [],// Tipo de registro, TDE = debito TCE = credito, CHAR(3)
	char chACTIVITY[],  //  Actividad, CHAR(1)="N"
    char szFILLER1[],  //  RESERVADO="150"
	char szBANDEB [], //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
	char szFECSOL [], //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	char szNUMTRA [], //  Numero de transferencia,CHAR(7) 
	char szNUMABO [], //  Abonado	, CHAR(7)
	char szTIPOPE [], //  Tipo de operacion -	Ver tabla I - , CHAR(2)
	char szIMPORTE  [],//  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	char szSUCDEB [],  //  Sucursal del Banco que tratara el debito, CHAR(4)
	char szNOMSOL [],  //  Nombre de la cuenta, 
	char szTIPCUEDEB[],//   Tipo de cuenta	- Ver tabla II  - CHAR(2)
	char szNCUECMDEB [], //  Numero de cuenta corto -Interno Datanet- CHAR(2)
	char szNUMCTADEB [], //  Numero de cuenta, formato Banco, CHAR(17)
	char szFSENDB [], //  Fecha de envio de la transf.  al Banco -Aammdd-, CHAR(6)
	char szHSENDB [], // Hora de envio, Hhmm, CHAR(4)
	char szOPEDB1 [], //  Identificacion del operador #1. Primer autorizante del Banco de db. CHAR(2)
	char szOPEDB2 [], //  Identificacion del operador #2. Segundo autorizante, CHAr(2).
	char szRECHDB [], //  Motivo del rechazo del Banco de debito -Ver tabla III - CHAR(4)
	char szBANCRE [], //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
	char szSUCCRE [], //  Sucursal del Banco que tratara el credito. CHAR(4)				
	char szNOMBEN[],  //  Nombre de la cuenta a acreditar, VARCHAR(29)
	char szTIPCRE [], //  Tipo de cuenta -Ver tabla II-, CHAR(2)
	char szCTACRE[], //  Numero de cuenta -Formato Banco-, CHAR(17)
	char szFSENCR[], //  Fecha de envio de la transf.  al Banco	 -Aammdd-, CHAR(6)
	char szHSENCR[], //  Hora de envio -Hhmm-, CHAR(4)
	char szOPECR1[], //  Identificacion del operador #1, primer autorizante del Banco de cr. CHAR(2)
	char szOPECR2[], //  Identificacion del operador #2, segundo autorizante, CHAR(2)
	char szRECHCR[], //  Motivo del rechazo del Banco de credito -ver tabla III-, CHAR(4)
	char szOPECON [], //  Operador que confecciono la transferencia	, CHAR(2)
	char szOPEAU1 [], //  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	char szOPEAU2 [], //  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	char szOPEAU3 [], //  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	char szFECAUT [], //  Fecha de autorizacion (envio del pago a la red) aammdd,  CHAR(6)
	char szHORAUT [], //  Hora de autorizacion (envio del pago a la red) hhmm, CHAR(4)
	char szESTADO [], //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	char szFECEST [], //  Fecha ultima modificacion de ESTADO	, CHAR(6)
	char szOBSER1 [], //  Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
	char szOBSER2 [], //  Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
	char szCODMAC [], //  Message Authentication Code (MAC), CHAR(12)
	char szNUMREF [], //  Numero de transferencia PC	 [], //  nro.lote (N,3)+nro. operacion (N,4) , CHAR(7) 
	char szNUMENV [], //  Numero de envio del abonadochar(3)
	char chCONSOL[], //  "S" / "N" consoildacion (1 lote)
	char chMARTIT[], //  "S" / "N" misma cuenta debito que credito ?
	char chPRIVEZ[], //  "S" / "N" pago previo entre cuentas
	char chDIFCRE[], //  "S" / "N" diferida por credito
	char chRIEABO[], //  "S" / "N" riesgo abonado
	char chRIEBCO[], //  "S" / "N" riesgo banco
    char szTRATAM[],  //  Estado tratamiento
    char szESTADOSANT[], // Estados anteriores
	/***************************************************/
	char szCUITDEB[],    // CUIT debito
	char szCUITCRE[],    // CUIT credito
	char szFILLER2[]     // Relleno 2
	/***************************************************/
		) // Logg Transaccion DATANET
{
	char szDEFAULT[]  = {0x00} ;
    char szNROLOTE[8] = {0x00} ;
	char szQuery[SZMAXBUFFER_SIZE*4] = {0x00} ;
	int  iSQLSentence                = 0;

	/* Precondicion : conexion SQL debe existir */
	if( !IsConnected() )
	{
		/* Conectarse a la B.datos */
		Connect( ODBC_DATANET_DATABASE );    
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		if( !IsConnected() )
			return is_false;
	}
	/* Precondition */
	ResetLastError();

    /* Guardar nro. de lote */
    strncpy(szNROLOTE , szNUMREF , 3);

	// Ejecucion de SP
    sprintf( szQuery, 
            "insert into dbo.dn_tef_online "
            "(COD_ORIGEN,COD_DESTINO,TIPO_REGISTRO,ACTIVIDAD,CONTADOR,RESERVADO,"
            "BCO_DEBITO,FEC_SOLICITUD,NRO_TRANSFERENCIA,COD_ABONADO,TIPO_OPERACION,IMPORTE,"
            "SUC_DEBITO,NOM_SOLICITANTE,TIPO_CTA_DEB_RED,NRO_CTA_RED,CTA_DEBITO,FEC_ENVIO_DEBITO,"
            "HORA_ENVIO_DEBITO,OPERADOR_DB_1,OPERADOR_DB_2,MOTIVO_RECHAZO_DB,BCO_CREDITO,SUC_CREDITO,"
            "NOM_BENEFICIARIO,TIPO_CTA_CRED_RED,CTA_CREDITO,FEC_ENVIO_CREDITO,HORA_ENVIO_CREDITO,OPERADOR_CR_1,"
            "OPERADOR_CR_2,MOTIVO_RECHAZO_CR,OPERADOR_INGRESO,AUTORIZANTE_1,AUTORIZANTE_2,AUTORIZANTE_3,"
            "FECHA_AUTORIZACION,HORA_AUTORIZACION,ESTADO,FEC_ESTADO,OBSERVACION_1,OBSERVACION_2,"
            "CLAVE_MAC,NRO_REFERENCIA,NRO_ENVIO,DEB_CONSOLIDADO,TIPO_TITULAR,PAGO_PREACORDADO,"
            "TRATA_CR_DIFERIDO,RIESGO_ABONADO,RIESGO_BANCO,NRO_LOTE,ESTADO_TRATAMIENTO,ESTADOS_ANTERIORES,"
			"COD_RESPUESTA,CANT_RECLAMOS,HORA_ENVIO_RECLAMO,COD_USUARIO_OFICIAL,COD_OFICIAL_RESP,HORA_RESPUESTA,"
			"ERROR_DE_MAC,CLAVE_MENVIADA,PEND_ENVIO,SECUENCIA_DEBCRE,COD_ALTERNO_DEBCRE,SECUENCIA_REVERSA,"
			"COD_ALTERNO_REVERSA)"
            " values "
            "('%s','%s','%s','%s','%s','%s',"
            " '%s','%s','%s','%s','%s','%s',"
            " '%s','%s','%s','%s','%s','%s',"
            " '%s','%s','%s','%s','%s','%s',"
            " '%s','%s','%s','%s','%s','%s',"
            " '%s','%s','%s','%s','%s','%s',"
            " '%s','%s','%s','%s','%s','%s',"
            " '%s','%s','%s','%s','%s','%s',"
            " '%s','%s','%s','%s','%s','%s',"
			" '0','0','0000','0','0','0000',"
			" '0','0','S','0','0','0',"
			" '0')"	
			,	 
	  szFRNAME   , //Nombre nodo emisor, ej."DNET"  //CHAR(4)	
	  szTONAME   , //Nombre nodo receptor, ej."BSUD" //CHAR(4)
	  szRECTYPE  , //Tipo de registro, TDE = debito TCE = credito, CHAR(3)
	  chACTIVITY , //Tipo de registro, TDE = debito TCE = credito, CHAR(3)
      "1"        , //Contador
      szFILLER1  , //RESERVADO, CHAR(6)
	  
      szBANDEB   ,//Codigo Banco segun BCRA	ej."067":Bansud, CHAR(3)
	  szFECSOL   ,//Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	  szNUMTRA   ,//Numero de transferencia, CHAR(7)
	  szNUMABO   ,//Abonado	, CHAR(7)
	  szTIPOPE   ,//Tipo de operacion -	Ver tabla I - , CHAR(2)
	  szIMPORTE  , //Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	  
      szSUCDEB    ,//Sucursal del Banco que tratara el debito, CHAR(4)
	  szNOMSOL    ,//Nombre de la cuenta, VARCHAR(29)
	  szTIPCUEDEB ,// Tipo de cuenta	- Ver tabla II  - CHAR(2)
	  szNCUECMDEB ,//Numero de cuenta corto -Interno Datanet- CHAR(2)
	  szNUMCTADEB ,//Numero de cuenta, formato Banco, CHAR(17)
	  szFSENDB    ,//Fecha de envio de la transf.  al Banco -Aammdd-, CHAR(6)

	  szHSENDB  , //Hora de envio, Hhmm, CHAR(4)
	  szOPEDB1  , //Identificacion del operador #1. Primer autorizante del Banco de db. CHAR(2)
	  szOPEDB2  , //Identificacion del operador #2. Segundo autorizante, CHAr(2).
	  szRECHDB  , //Motivo del rechazo del Banco de debito -Ver tabla III - CHAR(4)
	  szBANCRE  , //Codigo de Banco segun BCRA, p/credito. CHAR(3)
	  szSUCCRE  , //Sucursal del Banco que tratara el credito. CHAR(4)				
	  
      szNOMBEN  , //Nombre de la cuenta a acreditar, VARCHAR(29)
	  szTIPCRE  , //Tipo de cuenta -Ver tabla II-, CHAR(2)
	  szCTACRE  , //Numero de cuenta -Formato Banco-, CHAR(17)
	  szFSENCR  , //Fecha de envio de la transf.  al Banco	 -Aammdd-, CHAR(6)
	  szHSENCR  , //Hora de envio -Hhmm-, CHAR(4)
	  szOPECR1  , //Identificacion del operador #1, primer autorizante del Banco de cr. CHAR(2)

	  szOPECR2  , //Identificacion del operador #2, segundo autorizante, CHAR(2)
	  szRECHCR  , //Motivo del rechazo del Banco de credito -ver tabla III-, CHAR(4)
	  szOPECON  , //Operador que confecciono la transferencia	, CHAR(2)
	  szOPEAU1  , //Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	  szOPEAU2  , //Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	  szOPEAU3  , //Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)

	  szFECAUT  , //Fecha de autorizacion (envio del pago a la red) aammdd,  CHAR(6)
	  szHORAUT  , //Hora de autorizacion (envio del pago a la red) hhmm, CHAR(4)
	  szESTADO  , //Estado de la transferencia	-Ver tabla IV- CHAR(2)
	  szFECEST  , //Fecha ultima modificacion de ESTADO	, CHAR(6)
	  szOBSER1  , //Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
	  szOBSER2  , //Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)

	  szCODMAC  , //Message Authentication Code (MAC), CHAR(12)
	  szNUMREF  , //Numero de transferencia PC: nro.lote (N,3)+nro. operacion (N,4) , CHAR(7) 
	  szNUMENV  , //Numero de envio del abonado	CHAR(3)
	  chCONSOL  , //Marca de consolidacion de movimientos ("S" o "N")	
	  chMARTIT  , //Indica que la cuenta de debito y la cuenta de credito son de la misma empresa, CHAR(1)
	  chPRIVEZ  , //"S" indica que al momento en que se envia, no existe un pago previo , CHAR(1)

	  chDIFCRE   , //"S" habilita al Banco de debito que asi lo desee a tratar la transferencia. CHAR(1)
	  chRIEABO   , //"S" indica que el pago supera el riesgo fijado"N" el pago no supera el riesgo	CHAR(1)
	  chRIEBCO   , //"S" indica que al ser aceptado el debito la transferencia supero el riesgo ,CHAR(1)    
	  szNROLOTE  , //Numero de transferencia PC: nro.lote (N,3)+nro. operacion (N,4) , CHAR(7) 
      szTRATAM   , //Estado tratamiento, CHAR(2)
      szESTADOSANT //Estados Anteriores enviados desde la RED,CHAR(140) 	
      );
	//
	// Loggear la transaccion en TEF ONLINE para su posterior autorizacion/rechazo
    //
    iSQLSentence = _ODBC_INSERT_SNTC_ ;
    Query( szQuery,&iSQLSentence, true, false, 0, 0 );	
	
	if (GetReturnCode() == SQL_SUCCESS_WITH_INFO ||  GetReturnCode() == SQL_SUCCESS)
	{
	    Commit();
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();		
		/*******************************************/
		szACCION[0] = DNET_ACTION_NO_ERROR;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
		return is_true;		
	}
	else
	{
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_END_OF_FILE;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
		return is_false;		
	};
}

//////////////////////////////////////////////////////////////////////
// Retrieve Transaccion DATANET desde TEF ONLINE
//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::RetrieveTransaction(
	char szFRNAME [],// Nombre nodo emisor, ej. "DNET", char(4)
	char szTONAME [], //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
	char szRECTYPE [],// Tipo de registro, TDE = debito TCE = credito, CHAR(3)
    char chACTIVITY[],// Actividad
    char szFILLER1[], //  RESERVADO, CHAR(6)
	char szBANDEB [], //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
	char szFECSOL [], //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	char szNUMTRA [], //  Numero de transferencia,CHAR(7) 
	char szNUMABO [], //  Abonado	, CHAR(7)
	char szTIPOPE [], //  Tipo de operacion -	Ver tabla I - , CHAR(2)
	char szIMPORTE  [],//  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	char szSUCDEB [],  //  Sucursal del Banco que tratara el debito, CHAR(4)
	char szNOMSOL [],  //  Nombre de la cuenta, 
	char szTIPCUEDEB[],//   Tipo de cuenta	- Ver tabla II  - CHAR(2)
	char szNCUECMDEB [], //  Numero de cuenta corto -Interno Datanet- CHAR(2)
	char szNUMCTADEB [], //  Numero de cuenta, formato Banco, CHAR(17)
	char szFSENDB [], //  Fecha de envio de la transf.  al Banco -Aammdd-, CHAR(6)
	char szHSENDB [], // Hora de envio, Hhmm, CHAR(4)
	char szOPEDB1 [], //  Identificacion del operador #1. Primer autorizante del Banco de db. CHAR(2)
	char szOPEDB2 [], //  Identificacion del operador #2. Segundo autorizante, CHAr(2).
	char szRECHDB [], //  Motivo del rechazo del Banco de debito -Ver tabla III - CHAR(4)
	char szBANCRE [], //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
	char szSUCCRE [], //  Sucursal del Banco que tratara el credito. CHAR(4)				
	char szNOMBEN[],  //  Nombre de la cuenta a acreditar, VARCHAR(29)
	char szTIPCRE [], //  Tipo de cuenta -Ver tabla II-, CHAR(2)
	char szCTACRE[], //  Numero de cuenta -Formato Banco-, CHAR(17)
	char szFSENCR[], //  Fecha de envio de la transf.  al Banco	 -Aammdd-, CHAR(6)
	char szHSENCR[], //  Hora de envio -Hhmm-, CHAR(4)
	char szOPECR1[], //  Identificacion del operador #1, primer autorizante del Banco de cr. CHAR(2)
	char szOPECR2[], //  Identificacion del operador #2, segundo autorizante, CHAR(2)
	char szRECHCR[], //  Motivo del rechazo del Banco de credito -ver tabla III-, CHAR(4)
	char szOPECON [], //  Operador que confecciono la transferencia	, CHAR(2)
	char szOPEAU1 [], //  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	char szOPEAU2 [], //  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	char szOPEAU3 [], //  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	char szFECAUT [], //  Fecha de autorizacion (envio del pago a la red) aammdd,  CHAR(6)
	char szHORAUT [], //  Hora de autorizacion (envio del pago a la red) hhmm, CHAR(4)
	char szESTADO [], //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	char szFECEST [], //  Fecha ultima modificacion de ESTADO	, CHAR(6)
	char szOBSER1 [], //  Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
	char szOBSER2 [], //  Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
	char szCODMAC [], //  Message Authentication Code (MAC), CHAR(12)
	char szNUMREF [], //  Numero de transferencia PC	 [], //  nro.lote (N,3)+nro. operacion (N,4) , CHAR(7) 
	char szNUMENV [], //  Numero de envio del abonadochar(3)
	char chCONSOL[], //  "S" / "N" consoildacion (1 lote)
	char chMARTIT[], //  "S" / "N" misma cuenta debito que credito ?
	char chPRIVEZ[], //  "S" / "N" pago previo entre cuentas
	char chDIFCRE[], //  "S" / "N" diferida por credito
	char chRIEABO[], //  "S" / "N" riesgo abonado
	char chRIEBCO[], //  "S" / "N" riesgo banco
    char chEstadoTratamiento[], // " ", "0", ...etc...
	char szCANTREINT[]
	/****************/
	) // Retrieve 1er Transaccion DATANET
{
	/* Local variables */
	char szDEFAULT[]  = {0x00} ;
    char szNROLOTE[8] = {0x00} ;
	char szQuery[SZMAXBUFFER_SIZE*2] = {0x00} ;
	int  iSQLSentence                = _ODBC_EXECUTE_SNTC_;

	/* Precondicion : conexion SQL debe existir */
	if( !IsConnected() )
	{
		/* Conectarse a la B.datos */
		Connect( ODBC_DATANET_DATABASE );    
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		if( !IsConnected() )
			return is_false;
	}
	/* Precondition */
	ResetLastError();

	// Ejecucion de SP
	     
    iSQLSentence = _ODBC_EXECUTE_SNTC_ ;		
    if( !IsSentenceInUse( iSQLSentence ) )
    {
		/* Precondition */
		ResetLastError();
	    Column( szFRNAME , SQL_C_CHAR, 4+1, &iSQLSentence ); //1Nombre nodo emisor, ej."DNET"  //CHAR(4)	
	    Column( szTONAME , SQL_C_CHAR, 4+1, &iSQLSentence ); //2Nombre nodo receptor, ej."BSUD" //CHAR(4)
	    Column( szRECTYPE, SQL_C_CHAR, 3+1, &iSQLSentence ); //3Tipo de registro, TDE = debito TCE = credito, CHAR(3)
	    Column( szBANDEB , SQL_C_CHAR, 3+1, &iSQLSentence ); //4Codigo Banco segun BCRA	ej."067":Bansud, CHAR(3)
		// FIX FECHA CON SEPARADORES
	    Column( szFECSOL , SQL_C_CHAR, 10+1, &iSQLSentence ); //5Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	    Column( szNUMTRA , SQL_C_CHAR, 7+1, &iSQLSentence ); //6Numero de transferencia, CHAR(7)
	    Column( szNUMABO , SQL_C_CHAR, 7+1, &iSQLSentence ); //7Abonado	, CHAR(7)
	    Column( szTIPOPE , SQL_C_CHAR, 2+1, &iSQLSentence ); //8Tipo de operacion -	Ver tabla I - , CHAR(2)
	    Column( szIMPORTE, SQL_C_CHAR,15+1, &iSQLSentence ); //9Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	    Column( szSUCDEB , SQL_C_CHAR, 4+1, &iSQLSentence ); //10Sucursal del Banco que tratara el debito, CHAR(4)
	    Column( szNOMSOL , SQL_C_CHAR,29+1, &iSQLSentence );  //11Nombre de la cuenta, VARCHAR(29)
	    Column( szTIPCUEDEB, SQL_C_CHAR, 2+1, &iSQLSentence );  //12tipo de cuenta	- Ver tabla II  - CHAR(2)
	    Column( szNCUECMDEB, SQL_C_CHAR, 2+1, &iSQLSentence ); //13Numero de cuenta corto -Interno Datanet- CHAR(2)
	    Column( szNUMCTADEB, SQL_C_CHAR,17+1, &iSQLSentence );  //14Numero de cuenta, formato Banco, CHAR(17)
	    Column( szFSENDB, SQL_C_CHAR, 6+1, &iSQLSentence ); //15Fecha de envio de la transf.  al Banco -Aammdd-, CHAR(6)
	    Column( szHSENDB, SQL_C_CHAR, 4+1, &iSQLSentence ); //16Hora de envio, Hhmm, CHAR(4)
	    Column( szOPEDB1, SQL_C_CHAR, 2+1, &iSQLSentence ); //17Identificacion del operador #1. Primer autorizante del Banco de db. CHAR(2)
	    Column( szOPEDB2, SQL_C_CHAR, 2+1, &iSQLSentence ); //18Identificacion del operador #2. Segundo autorizante, CHAr(2).
	    Column( szRECHDB, SQL_C_CHAR, 4+1, &iSQLSentence ); //19Motivo del rechazo del Banco de debito -Ver tabla III - CHAR(4)
	    Column( szBANCRE, SQL_C_CHAR, 3+1, &iSQLSentence ); //20Codigo de Banco segun BCRA, p/credito. CHAR(3)
	    Column( szSUCCRE, SQL_C_CHAR, 4+1, &iSQLSentence ); //21Sucursal del Banco que tratara el credito. CHAR(4)				
	    Column( szNOMBEN, SQL_C_CHAR,29+1, &iSQLSentence );  //22Nombre de la cuenta a acreditar, VARCHAR(29)
	    Column( szTIPCRE, SQL_C_CHAR, 2+1, &iSQLSentence );  //23Tipo de cuenta -Ver tabla II-, CHAR(2)
	    Column( szCTACRE, SQL_C_CHAR,17+1, &iSQLSentence );  //24Numero de cuenta -Formato Banco-, CHAR(17)
	    Column( szFSENCR, SQL_C_CHAR, 6+1, &iSQLSentence ); //25Fecha de envio de la transf.  al Banco	 -Aammdd-, CHAR(6)
	    Column( szHSENCR, SQL_C_CHAR, 4+1, &iSQLSentence ); //26Hora de envio -Hhmm-, CHAR(4)
	    Column( szOPECR1, SQL_C_CHAR, 2+1, &iSQLSentence ); //17Identificacion del operador #1. Primer autorizante del Banco de db. CHAR(2)
	    Column( szOPECR2, SQL_C_CHAR, 2+1, &iSQLSentence ); //18Identificacion del operador #2. Segundo autorizante, CHAr(2).
	    Column( szRECHCR, SQL_C_CHAR, 4+1, &iSQLSentence ); //29Motivo del rechazo del Banco de credito -ver tabla III-, CHAR(4)
	    Column( szOPECON, SQL_C_CHAR, 2+1, &iSQLSentence ); //30Operador que confecciono la transferencia	, CHAR(2)
	    Column( szOPEAU1, SQL_C_CHAR, 2+1, &iSQLSentence ); //31Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	    Column( szOPEAU2, SQL_C_CHAR, 2+1, &iSQLSentence ); //32Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	    Column( szOPEAU3, SQL_C_CHAR, 2+1, &iSQLSentence ); //33Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	    Column( szFECAUT, SQL_C_CHAR, 6+1, &iSQLSentence ); //34Fecha de autorizacion (envio del pago a la red) aammdd,  CHAR(6)
	    Column( szHORAUT, SQL_C_CHAR, 4+1, &iSQLSentence ); //35Hora de autorizacion (envio del pago a la red) hhmm, CHAR(4)
	    Column( szESTADO, SQL_C_CHAR, 2+1, &iSQLSentence ); //36Estado de la transferencia	-Ver tabla IV- CHAR(2)
	    Column( szFECEST, SQL_C_CHAR, 6+1, &iSQLSentence ); //37Fecha ultima modificacion de ESTADO	, CHAR(6)
	    Column( szOBSER1, SQL_C_CHAR,60+1, &iSQLSentence ); //38Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
	    Column( szOBSER2, SQL_C_CHAR,100+1,&iSQLSentence ); //39Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
	    Column( szCODMAC, SQL_C_CHAR,12+1, &iSQLSentence ); //40Message Authentication Code (MAC), CHAR(12)
	    Column( szNUMREF, SQL_C_CHAR, 7+1, &iSQLSentence ); //41Numero de transferencia PC: nro.lote (N,3)+nro. operacion (N,4) , CHAR(7) 
	    Column( szNUMENV, SQL_C_CHAR, 3+1, &iSQLSentence ); //42Numero de envio del abonado	CHAR(3)
	    Column( chCONSOL, SQL_C_CHAR, 1+1, &iSQLSentence ); //43Marca de consolidacion de movimientos ("S" o "N")	
	    Column( chMARTIT, SQL_C_CHAR, 1+1, &iSQLSentence ); //44Indica que la cuenta de debito y la cuenta de credito son de la misma empresa, CHAR(1)
	    Column( chPRIVEZ, SQL_C_CHAR, 1+1, &iSQLSentence ); //45"S" indica que al momento en que se envia, no existe un pago previo , CHAR(1)
	    Column( chDIFCRE, SQL_C_CHAR, 1+1, &iSQLSentence ); //46"S" habilita al Banco de debito que asi lo desee a tratar la transferencia. CHAR(1)
	    Column( chRIEABO, SQL_C_CHAR, 1+1, &iSQLSentence ); //47"S" indica que el pago supera el riesgo fijado"N" el pago no supera el riesgo	CHAR(1)
	    Column( chRIEBCO, SQL_C_CHAR, 1+1, &iSQLSentence ); //48"S" indica que al ser aceptado el debito la transferencia supero el riesgo ,CHAR(1)    
        Column( chEstadoTratamiento, SQL_C_CHAR, 1+1, &iSQLSentence ); //49-CHAR(1)   
		Column( szCANTREINT, SQL_C_CHAR, 3+1, &iSQLSentence);
		/* POR ERROR, desconectar y reprobar */
		if( Error() )
		{
			Disconnect();
			/*******************************************/
			szACCION[0] = DNET_ACTION_END_OF_FILE;
			/* BOOL FLAG ON ACTION */
			iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
						? TRUE
						: FALSE ;
			/*******************************************/
			return is_false;
		};
    };
	
	// Emisor o Autorizador ?
	if( ( _DPS_TRAT_PROCESADA_C_ + _DPS_PEND_ENV_DEFAULT_C_ ) != chEstadoTratamiento[0] )
	{
		// 
		//						
		// Ejecucion de SP
		sprintf( szQuery, 
				"select TOP 1 "
				"COD_ORIGEN,COD_DESTINO,TIPO_REGISTRO,BCO_DEBITO,FEC_SOLICITUD,NRO_TRANSFERENCIA,COD_ABONADO,"
				"TIPO_OPERACION,IMPORTE,SUC_DEBITO,NOM_SOLICITANTE,TIPO_CTA_DEB_RED,NRO_CTA_RED,CTA_DEBITO,FEC_ENVIO_DEBITO,"
				"HORA_ENVIO_DEBITO,OPERADOR_DB_1,OPERADOR_DB_2,MOTIVO_RECHAZO_DB,BCO_CREDITO,SUC_CREDITO,"
				"NOM_BENEFICIARIO,TIPO_CTA_CRED_RED,CTA_CREDITO,FEC_ENVIO_CREDITO,HORA_ENVIO_CREDITO,OPERADOR_CR_1,"
				"OPERADOR_CR_2,MOTIVO_RECHAZO_CR,OPERADOR_INGRESO,AUTORIZANTE_1,AUTORIZANTE_2,AUTORIZANTE_3,"
				"FECHA_AUTORIZACION,HORA_AUTORIZACION,ESTADO,FEC_ESTADO,OBSERVACION_1,OBSERVACION_2,"
				"CLAVE_MAC,NRO_REFERENCIA,NRO_ENVIO,DEB_CONSOLIDADO,TIPO_TITULAR,PAGO_PREACORDADO,"
				"TRATA_CR_DIFERIDO,RIESGO_ABONADO,RIESGO_BANCO,ESTADO_TRATAMIENTO,CANT_RECLAMOS "
				" from dbo.dn_tef_online"
				" where ESTADO_TRATAMIENTO not in ('3','8','9','S','C') and ESTADO NOT IN ('90') and NRO_TRANSFERENCIA > '%s' "
				//" and ESTADO <> '20' "
				//2008-09-09	MRB Agregado para filtrar por fecha.
				" and fec_solicitud = convert(varchar(10), getdate(), 112) "
				//" and fec_solicitud = '20090616' "
				" order by FEC_SOLICITUD,TIPO_REGISTRO,NRO_TRANSFERENCIA"
				,
				szNUMTRA );

		// Sentencia SQL
		iSQLSentence = _ODBC_EXECUTE_SNTC_ ;
		// Ejecutar sentencia SELECT
		Query(szQuery,&iSQLSentence, true, false, 0, 0 );	
		// Recuperar primer fila de datos
		Fetch( &iSQLSentence );
		
		if (GetReturnCode() == SQL_SUCCESS_WITH_INFO ||  GetReturnCode() == SQL_SUCCESS)
		{		
			/* Internal ODBC status */
			iRetval = ((RODBC *)this)->GetReturnCode();
			/*******************************************/
			szACCION[0] = DNET_ACTION_NO_ERROR;
			/* BOOL FLAG ON ACTION */
			iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
						? TRUE
						: FALSE ;
			/*******************************************/
			/* CLOSE the sentence : NOT UNBIND COLUMNS */
			//FreeSentence( iSQLSentence , TRUE, FALSE );    		
			// UNBIND cols.
			FreeSentence( iSQLSentence , TRUE , TRUE );    		
			/* ok */
			return is_true;		
		}
		else
		{
			/* Internal ODBC status */
			iRetval = ((RODBC *)this)->GetReturnCode();
			/*******************************************/
			szACCION[0] = DNET_ACTION_END_OF_FILE;
			/* BOOL FLAG ON ACTION */
			iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
						? TRUE
						: FALSE ;
			/*******************************************/
			/* Release the sentence */
			FreeSentence( iSQLSentence );    		
			/* EOF */
			return is_false;		
		};
	}
	else
	{

		// Ejecucion de SP
		sprintf( szQuery, 
				"select TOP 1 "
				"COD_ORIGEN,COD_DESTINO,TIPO_REGISTRO,BCO_DEBITO,FEC_SOLICITUD,NRO_TRANSFERENCIA,COD_ABONADO,"
				"TIPO_OPERACION,IMPORTE,SUC_DEBITO,NOM_SOLICITANTE,TIPO_CTA_DEB_RED,NRO_CTA_RED,CTA_DEBITO,FEC_ENVIO_DEBITO,"
				"HORA_ENVIO_DEBITO,OPERADOR_DB_1,OPERADOR_DB_2,MOTIVO_RECHAZO_DB,BCO_CREDITO,SUC_CREDITO,"
				"NOM_BENEFICIARIO,TIPO_CTA_CRED_RED,CTA_CREDITO,FEC_ENVIO_CREDITO,HORA_ENVIO_CREDITO,OPERADOR_CR_1,"
				"OPERADOR_CR_2,MOTIVO_RECHAZO_CR,OPERADOR_INGRESO,AUTORIZANTE_1,AUTORIZANTE_2,AUTORIZANTE_3,"
				"FECHA_AUTORIZACION,HORA_AUTORIZACION,ESTADO,FEC_ESTADO,OBSERVACION_1,OBSERVACION_2,"
				"CLAVE_MAC,NRO_REFERENCIA,NRO_ENVIO,DEB_CONSOLIDADO,TIPO_TITULAR,PAGO_PREACORDADO,"
				"TRATA_CR_DIFERIDO,RIESGO_ABONADO,RIESGO_BANCO,ESTADO_TRATAMIENTO, CANT_RECLAMOS"
				" from dbo.dn_tef_online"
				" where ESTADO_TRATAMIENTO in ('3','9','S','C') and ESTADO NOT IN ('90') and PEND_ENVIO <> 'N' and NRO_TRANSFERENCIA > '%s' "
				" order by FEC_SOLICITUD,TIPO_REGISTRO,NRO_TRANSFERENCIA"
				,
				szNUMTRA );

		// Ejecutar sentencia SELECT
		Query(szQuery,&iSQLSentence, true, false, 0, 0 );	
		// Recuperar primer fila de datos
		Fetch( &iSQLSentence );
		
		if (GetReturnCode() == SQL_SUCCESS_WITH_INFO ||  GetReturnCode() == SQL_SUCCESS)
		{		
			/* Internal ODBC status */
			iRetval = ((RODBC *)this)->GetReturnCode();
			/*******************************************/
			szACCION[0] = DNET_ACTION_NO_ERROR;
			/* BOOL FLAG ON ACTION */
			iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
						? TRUE
						: FALSE ;
			/*******************************************/
			/* CLOSE the sentence : UNBIND COLUMNS */
			FreeSentence( iSQLSentence , TRUE, FALSE );    		
			/* ok */
			return is_true;		
		}
		else
		{
			/* Internal ODBC status */
			iRetval = ((RODBC *)this)->GetReturnCode();
			/*******************************************/
			szACCION[0] = DNET_ACTION_END_OF_FILE;
			/* BOOL FLAG ON ACTION */
			iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
						? TRUE
						: FALSE ;
			/*******************************************/
			/* Release the sentence */
			FreeSentence( iSQLSentence );    		
			/* EOF */
			return is_false;		
		};
	};

};

//////////////////////////////////////////////////////////////////////
// Retrieve Transaccion DATANET desde TEF ONLINE
//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::RetrieveTransactionDemoradas(
	char szFRNAME [],// Nombre nodo emisor, ej. "DNET", char(4)
	char szTONAME [], //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
	char szRECTYPE [],// Tipo de registro, TDE = debito TCE = credito, CHAR(3)
    char chACTIVITY[],// Actividad
    char szFILLER1[], //  RESERVADO, CHAR(6)
	char szBANDEB [], //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
	char szFECSOL [], //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	char szNUMTRA [], //  Numero de transferencia,CHAR(7) 
	char szNUMABO [], //  Abonado	, CHAR(7)
	char szTIPOPE [], //  Tipo de operacion -	Ver tabla I - , CHAR(2)
	char szIMPORTE  [],//  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	char szSUCDEB [],  //  Sucursal del Banco que tratara el debito, CHAR(4)
	char szNOMSOL [],  //  Nombre de la cuenta, 
	char szTIPCUEDEB[],//   Tipo de cuenta	- Ver tabla II  - CHAR(2)
	char szNCUECMDEB [], //  Numero de cuenta corto -Interno Datanet- CHAR(2)
	char szNUMCTADEB [], //  Numero de cuenta, formato Banco, CHAR(17)
	char szFSENDB [], //  Fecha de envio de la transf.  al Banco -Aammdd-, CHAR(6)
	char szHSENDB [], // Hora de envio, Hhmm, CHAR(4)
	char szOPEDB1 [], //  Identificacion del operador #1. Primer autorizante del Banco de db. CHAR(2)
	char szOPEDB2 [], //  Identificacion del operador #2. Segundo autorizante, CHAr(2).
	char szRECHDB [], //  Motivo del rechazo del Banco de debito -Ver tabla III - CHAR(4)
	char szBANCRE [], //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
	char szSUCCRE [], //  Sucursal del Banco que tratara el credito. CHAR(4)				
	char szNOMBEN[],  //  Nombre de la cuenta a acreditar, VARCHAR(29)
	char szTIPCRE [], //  Tipo de cuenta -Ver tabla II-, CHAR(2)
	char szCTACRE[], //  Numero de cuenta -Formato Banco-, CHAR(17)
	char szFSENCR[], //  Fecha de envio de la transf.  al Banco	 -Aammdd-, CHAR(6)
	char szHSENCR[], //  Hora de envio -Hhmm-, CHAR(4)
	char szOPECR1[], //  Identificacion del operador #1, primer autorizante del Banco de cr. CHAR(2)
	char szOPECR2[], //  Identificacion del operador #2, segundo autorizante, CHAR(2)
	char szRECHCR[], //  Motivo del rechazo del Banco de credito -ver tabla III-, CHAR(4)
	char szOPECON [], //  Operador que confecciono la transferencia	, CHAR(2)
	char szOPEAU1 [], //  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	char szOPEAU2 [], //  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	char szOPEAU3 [], //  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	char szFECAUT [], //  Fecha de autorizacion (envio del pago a la red) aammdd,  CHAR(6)
	char szHORAUT [], //  Hora de autorizacion (envio del pago a la red) hhmm, CHAR(4)
	char szESTADO [], //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	char szFECEST [], //  Fecha ultima modificacion de ESTADO	, CHAR(6)
	char szOBSER1 [], //  Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
	char szOBSER2 [], //  Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
	char szCODMAC [], //  Message Authentication Code (MAC), CHAR(12)
	char szNUMREF [], //  Numero de transferencia PC	 [], //  nro.lote (N,3)+nro. operacion (N,4) , CHAR(7) 
	char szNUMENV [], //  Numero de envio del abonadochar(3)
	char chCONSOL[], //  "S" / "N" consoildacion (1 lote)
	char chMARTIT[], //  "S" / "N" misma cuenta debito que credito ?
	char chPRIVEZ[], //  "S" / "N" pago previo entre cuentas
	char chDIFCRE[], //  "S" / "N" diferida por credito
	char chRIEABO[], //  "S" / "N" riesgo abonado
	char chRIEBCO[], //  "S" / "N" riesgo banco
    char chEstadoTratamiento[], // " ", "0", ...etc...
	char szCANTREINT[]
	/****************/
	) // Retrieve 1er Transaccion DATANET
{
	/* Local variables */
	char szDEFAULT[]  = {0x00} ;
    char szNROLOTE[8] = {0x00} ;
	char szQuery[SZMAXBUFFER_SIZE*2] = {0x00} ;
	int  iSQLSentence                = _ODBC_EXECUTE_SNTC_;

	/* Precondicion : conexion SQL debe existir */
	if( !IsConnected() )
	{
		/* Conectarse a la B.datos */
		Connect( ODBC_DATANET_DATABASE );    
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		if( !IsConnected() )
			return is_false;
	}
	/* Precondition */
	ResetLastError();

	// Ejecucion de SP
	     
    iSQLSentence = _ODBC_EXECUTE_SNTC_ ;		
    if( !IsSentenceInUse( iSQLSentence ) )
    {
	    Column( szFRNAME , SQL_C_CHAR, 4+1, &iSQLSentence ); //1Nombre nodo emisor, ej."DNET"  //CHAR(4)	
	    Column( szTONAME , SQL_C_CHAR, 4+1, &iSQLSentence ); //2Nombre nodo receptor, ej."BSUD" //CHAR(4)
	    Column( szRECTYPE, SQL_C_CHAR, 3+1, &iSQLSentence ); //3Tipo de registro, TDE = debito TCE = credito, CHAR(3)
	    Column( szBANDEB , SQL_C_CHAR, 3+1, &iSQLSentence ); //4Codigo Banco segun BCRA	ej."067":Bansud, CHAR(3)
		// FIX FECHA CON SEPARADORES
	    Column( szFECSOL , SQL_C_CHAR, 10+1, &iSQLSentence ); //5Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	    Column( szNUMTRA , SQL_C_CHAR, 7+1, &iSQLSentence ); //6Numero de transferencia, CHAR(7)
	    Column( szNUMABO , SQL_C_CHAR, 7+1, &iSQLSentence ); //7Abonado	, CHAR(7)
	    Column( szTIPOPE , SQL_C_CHAR, 2+1, &iSQLSentence ); //8Tipo de operacion -	Ver tabla I - , CHAR(2)
	    Column( szIMPORTE, SQL_C_CHAR,15+1, &iSQLSentence ); //9Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	    Column( szSUCDEB , SQL_C_CHAR, 4+1, &iSQLSentence ); //10Sucursal del Banco que tratara el debito, CHAR(4)
	    Column( szNOMSOL , SQL_C_CHAR,29+1, &iSQLSentence );  //11Nombre de la cuenta, VARCHAR(29)
	    Column( szTIPCUEDEB, SQL_C_CHAR, 2+1, &iSQLSentence );  //12tipo de cuenta	- Ver tabla II  - CHAR(2)
	    Column( szNCUECMDEB, SQL_C_CHAR, 2+1, &iSQLSentence ); //13Numero de cuenta corto -Interno Datanet- CHAR(2)
	    Column( szNUMCTADEB, SQL_C_CHAR,17+1, &iSQLSentence );  //14Numero de cuenta, formato Banco, CHAR(17)
	    Column( szFSENDB, SQL_C_CHAR, 6+1, &iSQLSentence ); //15Fecha de envio de la transf.  al Banco -Aammdd-, CHAR(6)
	    Column( szHSENDB, SQL_C_CHAR, 4+1, &iSQLSentence ); //16Hora de envio, Hhmm, CHAR(4)
	    Column( szOPEDB1, SQL_C_CHAR, 2+1, &iSQLSentence ); //17Identificacion del operador #1. Primer autorizante del Banco de db. CHAR(2)
	    Column( szOPEDB2, SQL_C_CHAR, 2+1, &iSQLSentence ); //18Identificacion del operador #2. Segundo autorizante, CHAr(2).
	    Column( szRECHDB, SQL_C_CHAR, 4+1, &iSQLSentence ); //19Motivo del rechazo del Banco de debito -Ver tabla III - CHAR(4)
	    Column( szBANCRE, SQL_C_CHAR, 3+1, &iSQLSentence ); //20Codigo de Banco segun BCRA, p/credito. CHAR(3)
	    Column( szSUCCRE, SQL_C_CHAR, 4+1, &iSQLSentence ); //21Sucursal del Banco que tratara el credito. CHAR(4)				
	    Column( szNOMBEN, SQL_C_CHAR,29+1, &iSQLSentence );  //22Nombre de la cuenta a acreditar, VARCHAR(29)
	    Column( szTIPCRE, SQL_C_CHAR, 2+1, &iSQLSentence );  //23Tipo de cuenta -Ver tabla II-, CHAR(2)
	    Column( szCTACRE, SQL_C_CHAR,17+1, &iSQLSentence );  //24Numero de cuenta -Formato Banco-, CHAR(17)
	    Column( szFSENCR, SQL_C_CHAR, 6+1, &iSQLSentence ); //25Fecha de envio de la transf.  al Banco	 -Aammdd-, CHAR(6)
	    Column( szHSENCR, SQL_C_CHAR, 4+1, &iSQLSentence ); //26Hora de envio -Hhmm-, CHAR(4)
	    Column( szOPECR1, SQL_C_CHAR, 2+1, &iSQLSentence ); //17Identificacion del operador #1. Primer autorizante del Banco de db. CHAR(2)
	    Column( szOPECR2, SQL_C_CHAR, 2+1, &iSQLSentence ); //18Identificacion del operador #2. Segundo autorizante, CHAr(2).
	    Column( szRECHCR, SQL_C_CHAR, 4+1, &iSQLSentence ); //29Motivo del rechazo del Banco de credito -ver tabla III-, CHAR(4)
	    Column( szOPECON, SQL_C_CHAR, 2+1, &iSQLSentence ); //30Operador que confecciono la transferencia	, CHAR(2)
	    Column( szOPEAU1, SQL_C_CHAR, 2+1, &iSQLSentence ); //31Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	    Column( szOPEAU2, SQL_C_CHAR, 2+1, &iSQLSentence ); //32Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	    Column( szOPEAU3, SQL_C_CHAR, 2+1, &iSQLSentence ); //33Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	    Column( szFECAUT, SQL_C_CHAR, 6+1, &iSQLSentence ); //34Fecha de autorizacion (envio del pago a la red) aammdd,  CHAR(6)
	    Column( szHORAUT, SQL_C_CHAR, 4+1, &iSQLSentence ); //35Hora de autorizacion (envio del pago a la red) hhmm, CHAR(4)
	    Column( szESTADO, SQL_C_CHAR, 2+1, &iSQLSentence ); //36Estado de la transferencia	-Ver tabla IV- CHAR(2)
	    Column( szFECEST, SQL_C_CHAR, 6+1, &iSQLSentence ); //37Fecha ultima modificacion de ESTADO	, CHAR(6)
	    Column( szOBSER1, SQL_C_CHAR,60+1, &iSQLSentence ); //38Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
	    Column( szOBSER2, SQL_C_CHAR,100+1,&iSQLSentence ); //39Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
	    Column( szCODMAC, SQL_C_CHAR,12+1, &iSQLSentence ); //40Message Authentication Code (MAC), CHAR(12)
	    Column( szNUMREF, SQL_C_CHAR, 7+1, &iSQLSentence ); //41Numero de transferencia PC: nro.lote (N,3)+nro. operacion (N,4) , CHAR(7) 
	    Column( szNUMENV, SQL_C_CHAR, 3+1, &iSQLSentence ); //42Numero de envio del abonado	CHAR(3)
	    Column( chCONSOL, SQL_C_CHAR, 1+1, &iSQLSentence ); //43Marca de consolidacion de movimientos ("S" o "N")	
	    Column( chMARTIT, SQL_C_CHAR, 1+1, &iSQLSentence ); //44Indica que la cuenta de debito y la cuenta de credito son de la misma empresa, CHAR(1)
	    Column( chPRIVEZ, SQL_C_CHAR, 1+1, &iSQLSentence ); //45"S" indica que al momento en que se envia, no existe un pago previo , CHAR(1)
	    Column( chDIFCRE, SQL_C_CHAR, 1+1, &iSQLSentence ); //46"S" habilita al Banco de debito que asi lo desee a tratar la transferencia. CHAR(1)
	    Column( chRIEABO, SQL_C_CHAR, 1+1, &iSQLSentence ); //47"S" indica que el pago supera el riesgo fijado"N" el pago no supera el riesgo	CHAR(1)
	    Column( chRIEBCO, SQL_C_CHAR, 1+1, &iSQLSentence ); //48"S" indica que al ser aceptado el debito la transferencia supero el riesgo ,CHAR(1)    
        Column( chEstadoTratamiento, SQL_C_CHAR, 1+1, &iSQLSentence ); //49-CHAR(1)   
		Column( szCANTREINT, SQL_C_CHAR, 3+1, &iSQLSentence);
    };
	
	sprintf( szQuery, 
			"select TOP 1 "
			"COD_ORIGEN,COD_DESTINO,TIPO_REGISTRO,BCO_DEBITO,FEC_SOLICITUD,NRO_TRANSFERENCIA,COD_ABONADO,"
			"TIPO_OPERACION,IMPORTE,SUC_DEBITO,NOM_SOLICITANTE,TIPO_CTA_DEB_RED,NRO_CTA_RED,CTA_DEBITO,FEC_ENVIO_DEBITO,"
			"HORA_ENVIO_DEBITO,OPERADOR_DB_1,OPERADOR_DB_2,MOTIVO_RECHAZO_DB,BCO_CREDITO,SUC_CREDITO,"
			"NOM_BENEFICIARIO,TIPO_CTA_CRED_RED,CTA_CREDITO,FEC_ENVIO_CREDITO,HORA_ENVIO_CREDITO,OPERADOR_CR_1,"
			"OPERADOR_CR_2,MOTIVO_RECHAZO_CR,OPERADOR_INGRESO,AUTORIZANTE_1,AUTORIZANTE_2,AUTORIZANTE_3,"
			"FECHA_AUTORIZACION,HORA_AUTORIZACION,ESTADO,FEC_ESTADO,OBSERVACION_1,OBSERVACION_2,"
			"CLAVE_MAC,NRO_REFERENCIA,NRO_ENVIO,DEB_CONSOLIDADO,TIPO_TITULAR,PAGO_PREACORDADO,"
			"TRATA_CR_DIFERIDO,RIESGO_ABONADO,RIESGO_BANCO,ESTADO_TRATAMIENTO,CANT_RECLAMOS "
			" from dbo.dn_tef_online"
			" where ESTADO_TRATAMIENTO not in ('3','8','9','S','C') and NRO_TRANSFERENCIA > '%s' "
			" and ESTADO = '20' "
			" order by FEC_SOLICITUD,TIPO_REGISTRO,NRO_TRANSFERENCIA"
			,
			szNUMTRA );

	// Sentencia SQL
	iSQLSentence = _ODBC_EXECUTE_SNTC_ ;
	// Ejecutar sentencia SELECT
	Query(szQuery,&iSQLSentence, true, false, 0, 0 );	
	// Recuperar primer fila de datos
	Fetch( &iSQLSentence );
	
	if (GetReturnCode() == SQL_SUCCESS_WITH_INFO ||  GetReturnCode() == SQL_SUCCESS)
	{		
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_NO_ERROR;
		/* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* CLOSE the sentence : UNBIND COLUMNS */
		FreeSentence( iSQLSentence , TRUE, FALSE );    		
		/* ok */
		return is_true;		
	}
	else
	{
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_END_OF_FILE;
		/* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
		/* EOF */
		return is_false;		
	};
};

//////////////////////////////////////////////////////////////////////
// Actualizar la transaccion DATANET en la tabla TEF ONLINE
//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::UpdateTransaction(
	char szBANDEB [], //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
	char szFECSOL [], //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	char szNUMTRA [], //  Numero de transferencia,CHAR(7) 
	char szNUMABO [], //  Abonado	, CHAR(7)
	char szNUMCTADEB [], //  Numero de cuenta, formato Banco, CHAR(17)
	char szBANCRE [], //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
	char szCTACRE [], //  Numero de cuenta -Formato Banco-, CHAR(17)
	char szESTADO [], //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	char szNUMREF [], //  Numero de transferencia PC	 [], //  nro.lote (N,3)+nro. operacion (N,4) , CHAR(7) 
	char szNUMENV [], //  Numero de envio del abonadochar(3)
    char szPENDENVIO[],// Pend. Envio?
    char szRECHDB[],   // motivo de rechazo debito
    char szRECHCR[],   // motivo de rechazo credito
    char szESTADOTRAT[],  // Estado tratamiento
	/************************************************/
	PCHAR szCODRESP   ,// Codigo Respuesta (16)
	PCHAR szCODUSUOFI  // Codigo Usuario Oficial (16)	     
	/************************************************/
    )   // Autorizar transaccion DATANET
{
	/* Variables Locales */
	char szDEFAULT[]  = {0x00} ;
    char szNROLOTE[8] = {0x00} ;
	char szQuery[SZMAXBUFFER_SIZE*2] = {0x00} ;
	int  iSQLSentence                = 0;

	/* Precondicion : parametros pasados con algun valor cualquiera */
	if( ! (
		szBANDEB     && //  Codigo Banco segun BCRA	ej."067" Bansud && CHAR(3)
		szFECSOL     && //  Fecha de compensacion && de dia o prox. Habil -aaaammdd- && CHAR(8)
		szNUMTRA     && //  Numero de transferencia &&CHAR(7) 
		szNUMABO     && //  Abonado	 && CHAR(7)
		szNUMCTADEB  && //  Numero de cuenta && formato Banco && CHAR(17)
		szBANCRE     && //  Codigo de Banco segun BCRA && p/credito. CHAR(3)
		szCTACRE     && //  Numero de cuenta -Formato Banco- && CHAR(17)
		szESTADO     && //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
		szNUMREF     && //  Numero de transferencia PC	  && //  nro.lote (N &&3)+nro. operacion (N &&4)  && CHAR(7) 
		szNUMENV     && //  Numero de envio del abonadochar(3)
		szPENDENVIO  && // Pend. Envio?
		szRECHDB     && // motivo de rechazo debito
		szRECHCR     && // motivo de rechazo credito
		szESTADOTRAT )  // Estado tratamiento		
		)
		return is_false;

	/* Precondicion : conexion SQL debe existir */
	if( !IsConnected() )
	{
		/* Conectarse a la B.datos */
		Connect( ODBC_DATANET_DATABASE );    
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		if( !IsConnected() )
			return is_false;
	};
	/* Precondition */
	ResetLastError();

    /* Guardar nro. de lote */
    strncpy(szNROLOTE , szNUMREF , 3);

	// Ejecucion de SP
	if(szCODRESP && szCODUSUOFI )
	{
		sprintf( szQuery, 
				"update dbo.dn_tef_online "
				" set "			
				" ESTADO='%s',PEND_ENVIO='%s',"
				" MOTIVO_RECHAZO_DB='%s',MOTIVO_RECHAZO_CR='%s',"
				" ESTADO_TRATAMIENTO='%s',"
				" COD_RESPUESTA='%s',COD_USUARIO_OFICIAL='%s' "
				" where "
				" NRO_TRANSFERENCIA='%s' ",	 
		  szESTADO  ,   //Estado de la transferencia	-Ver tabla IV- CHAR(2)
		  szPENDENVIO,  // Pend. Envio?
		  szRECHDB,     // motivo de rechazo debito
		  szRECHCR,     // motivo de rechazo credito
		  szESTADOTRAT, // Estado tratamiento
		  /************************************************/
		  szCODRESP   ,  // Codigo Respuesta (16)
		  szCODUSUOFI ,  // Codigo Usuario Oficial (16)	    
		  /************************************************/
		  szNUMTRA      //Numero de transferencia, CHAR(7)
		  );
	}
	else
	{
		sprintf( szQuery, 
				"update dbo.dn_tef_online "
				" set "			
				" ESTADO='%s',PEND_ENVIO='%s',MOTIVO_RECHAZO_DB='%s',"
				" MOTIVO_RECHAZO_CR='%s',ESTADO_TRATAMIENTO='%s' "
				" where "
				" NRO_TRANSFERENCIA='%s' ",	 
		  szESTADO    , //Estado de la transferencia	-Ver tabla IV- CHAR(2)
		  szPENDENVIO , // Pend. Envio?
		  szRECHDB    , // motivo de rechazo debito
		  szRECHCR    , // motivo de rechazo credito
		  szESTADOTRAT, // Estado tratamiento
		  szNUMTRA      //Numero de transferencia, CHAR(7)		  
		  );
	};/*end-if*/
	//
	// Loggear la transaccion en TEF ONLINE para su posterior autorizacion/rechazo
    //
    iSQLSentence = _ODBC_UPDATE_SNTC_ ;
    Query( szQuery,&iSQLSentence, true, false, 0, 0 );	
	
	if (GetReturnCode() == SQL_SUCCESS_WITH_INFO ||  GetReturnCode() == SQL_SUCCESS)
	{
	    Commit();
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();		
		/*******************************************/
		szACCION[0] = DNET_ACTION_NO_ERROR;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
		return is_true;		
	}
	else
	{
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_END_OF_FILE;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
		return is_false;		
	};
}

//////////////////////////////////////////////////////////////////////
// Actualizar los contadores en la tabla TEF ONLINE
//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::UpdateTransactionCounters(
		LPCSTR szNUMTRA	,
		PSHORT pshCounter)   // Autorizar transaccion DATANET
{
	/* Variables Locales */
	char szQuery[SZMAXBUFFER_SIZE*2]  = {0x00} ;
    int  iSQLSentence                = 0;
	
	/* Precondicion : parametros pasados con algun valor cualquiera */
	if( ! (
		szNUMTRA     && //  Numero de transferencia &&CHAR(7) 
		pshCounter )  
		)
		return is_false;

	/* Precondicion : conexion SQL debe existir */
	if( !IsConnected() )
	{
		/* Conectarse a la B.datos */
		Connect( ODBC_DATANET_DATABASE );    
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		if( !IsConnected() )
			return is_false;
	};
	/* Precondition */
	ResetLastError();

    sprintf( szQuery, 
				"update dbo.dn_tef_online "
				" set "			
				" CANT_RECLAMOS=CANT_RECLAMOS+1 "
				" where "
				" NRO_TRANSFERENCIA='%s' ",	 
			szNUMTRA
		  );
	
	iSQLSentence = _ODBC_UPDATE_SNTC_ ;

    Query( szQuery,&iSQLSentence, true, false, 0, 0 );	
	
	if (GetReturnCode() == SQL_SUCCESS_WITH_INFO ||  GetReturnCode() == SQL_SUCCESS)
	{
	    Commit();
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();		
		/*******************************************/
		szACCION[0] = DNET_ACTION_END_OF_FILE;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
	}
	else
	{
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_END_OF_FILE;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
	
		return is_false;		
	};

	sprintf(szQuery, "SELECT CANT_RECLAMOS "
					"from dbo.dn_tef_online "
					"where "
					"NRO_TRANSFERENCIA='%s' ",
					szNUMTRA );

	iSQLSentence = _ODBC_READ_SNTC_;

	//Enlazar variable de OUTPUT, default = 0
	Column(pshCounter, SQL_C_ULONG, 4+1, &iSQLSentence);

	Query(szQuery, &iSQLSentence, true, false, 0, 0);

	Fetch(&iSQLSentence);

	if (GetReturnCode() == SQL_SUCCESS_WITH_INFO ||  GetReturnCode() == SQL_SUCCESS)
	{
	  	/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();	
		iRetval = FALSE;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    
		
		return is_true;
	}
	else
	{
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_END_OF_FILE;

		if(0==iRetval)
			iRetval = TRUE;

	    /*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
	
		return is_false;		
	};
}

//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::ValidateExistingAccount(char *szNroCuenta,short nTipoCuenta )  // Verificar cuenta existente
{
	/* Local variables */
	char szQuery[SZMAXBUFFER_SIZE*2] = {0x00} ;
	int  iSQLSentence                = 0;
	BOOL bIS_ENABLE = _DPS_CONTROL_ACCOUNT_EXISTENCE_	;

	/* PRECONDICION */
	if( !bIS_ENABLE )
		return is_true;

	/* Precondicion : conexion SQL debe existir */
	if( !IsConnected() )
	{
		/* Conectarse a la B.datos */
		Connect( ODBC_DATANET_DATABASE );    
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		if( !IsConnected() )
			return is_false;
	}
	/* Precondition */
	ResetLastError();

	// Ejecucion de SP
	     
    iSQLSentence = _ODBC_READ_SNTC_ ;			
	
	//						
	// Ejecucion de SP
    sprintf( szQuery, "select COUNT(*) from dbo.DN_ABONADOS_CUENTAS where CUENTA='%s'",
			 szNroCuenta );

	// Loggear la transaccion en TEF ONLINE para su posterior autorizacion/rechazo
    iSQLSentence = _ODBC_READ_SNTC_ ;
    // Ejecutar sentencia SELECT
	Query(szQuery,&iSQLSentence, true, false, 0, 0 );	
    // Recuperar primer fila de datos
    Fetch( &iSQLSentence );
	
	if (GetReturnCode() == SQL_SUCCESS_WITH_INFO ||  GetReturnCode() == SQL_SUCCESS)
	{		
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_NO_ERROR;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
        /* ok */
		return is_true;		
	}
	else
	{
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_END_OF_FILE;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
		return is_false;		
	};
}

//////////////////////////////////////////////////////////////////////
// Verificar Banco Existente
//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::ValidateExistingBank(char *szBankNo , char *szBankKey )  
{
	/* Local variables */
	char szQuery[SZMAXBUFFER_SIZE*2] = {0x00} ;
	int  iSQLSentence                = 0;
	BOOL bIS_ENABLE = _DPS_CONTROL_BANK_EXISTENCE_	;

	/* PRECONDICION */
	if( !bIS_ENABLE )
		return is_true;

	/* Precondicion : conexion SQL debe existir */
	if( !IsConnected() )
	{
		/* Conectarse a la B.datos */
		Connect( ODBC_DATANET_DATABASE );    
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		if( !IsConnected() )
			return is_false;
	}
	/* Precondition */
	ResetLastError();

	// Ejecucion de SP	     
    iSQLSentence = _ODBC_READ_SNTC_ ;			
	Column( szBankKey , SQL_C_CHAR, 8+1, &iSQLSentence );  //CHAR(8)	
	
	// Ejecucion de SP
    sprintf( szQuery, "select CLAVE from dbo.DN_BANCOS_CLAVES where BANCO='%s'",
			 szBankNo );

	// Loggear la transaccion en TEF ONLINE para su posterior autorizacion/rechazo
    iSQLSentence = _ODBC_READ_SNTC_ ;

    // Ejecutar sentencia SELECT
	Query(szQuery,&iSQLSentence, true, false, 0, 0 );	
    // Recuperar primer fila de datos
    Fetch( &iSQLSentence );
	
	if (GetReturnCode() == SQL_SUCCESS_WITH_INFO ||  GetReturnCode() == SQL_SUCCESS)
	{		
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_NO_ERROR;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
        /* ok */
		return is_true;		
	}
	else
	{
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_END_OF_FILE;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
		return is_false;		
	};
}

//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::ValidateAccountStatus( char *szNroCuenta, char *szEstado,
                                          short nTipoCuenta )
                                           // Verificar estado de la cuenta
{
	/* Local variables */
	char szQuery[SZMAXBUFFER_SIZE*2] = {0x00} ;
	int  iSQLSentence                = 0;
	BOOL bIS_ENABLE = _DPS_CONTROL_ACCOUNT_STATUS_	;

	/* PRECONDICION */
	if( !bIS_ENABLE )
		return is_true;

	/* Precondicion : conexion SQL debe existir */
	if( !IsConnected() )
	{
		/* Conectarse a la B.datos */
		Connect( ODBC_DATANET_DATABASE );    
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		if( !IsConnected() )
			return is_false;
	}
	/* Precondition */
	ResetLastError();

	// Ejecucion de SP
	     
    iSQLSentence = _ODBC_READ_SNTC_ ;			
	Column( szEstado, SQL_C_CHAR, 1+1, &iSQLSentence );  //CHAR(1)	
	
	//						
	// Ejecucion de SP
    sprintf( szQuery, "select ESTADO from dbo.DN_ABONADOS_CUENTAS where CUENTA='%s'",
			 szNroCuenta );

	// Loggear la transaccion en TEF ONLINE para su posterior autorizacion/rechazo
    iSQLSentence = _ODBC_READ_SNTC_ ;
    // Ejecutar sentencia SELECT
	Query(szQuery,&iSQLSentence, true, false, 0, 0 );	
    // Recuperar primer fila de datos
    Fetch( &iSQLSentence );
	
	if (GetReturnCode() == SQL_SUCCESS_WITH_INFO ||  GetReturnCode() == SQL_SUCCESS)
	{		
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_NO_ERROR;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
        /* ok */
		return is_true;		
	}
	else
	{
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_END_OF_FILE;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
		return is_false;		
	};
}

//////////////////////////////////////////////////////////////////////
// Verificar abonado de la cuenta
//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::ValidateAccountAssociate(char *szCodAbonado, char *szClave )
{
	/* Local variables */
	char szQuery[SZMAXBUFFER_SIZE*2] = {0x00} ;
	int  iSQLSentence                = 0;
	BOOL bIS_ENABLE = _DPS_CONTROL_ASSOCIATE_EXISTENCE_	;

	/* PRECONDICION */
	if( !bIS_ENABLE )
		return is_true;

	/* Precondicion : conexion SQL debe existir */
	if( !IsConnected() )
	{
		/* Conectarse a la B.datos */
		Connect( ODBC_DATANET_DATABASE );    
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		if( !IsConnected() )
			return is_false;
	}
	/* Precondition */
	ResetLastError();

	// Ejecucion de SP	     
    iSQLSentence = _ODBC_READ_SNTC_ ;			
	Column( szClave, SQL_C_CHAR, 8+1, &iSQLSentence );  //CHAR(8)		

	//						
	// Ejecucion de SP
    sprintf( szQuery, "select CLAVE from dbo.DN_ABONADOS_CLAVES where ABONADO='%s'",
			 szCodAbonado );

	// Loggear la transaccion en TEF ONLINE para su posterior autorizacion/rechazo
    iSQLSentence = _ODBC_READ_SNTC_ ;
    // Ejecutar sentencia SELECT
	Query(szQuery,&iSQLSentence, true, false, 0, 0 );	
    // Recuperar primer fila de datos
    Fetch( &iSQLSentence );
	
	if (GetReturnCode() == SQL_SUCCESS_WITH_INFO ||  GetReturnCode() == SQL_SUCCESS)
	{		
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_NO_ERROR;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
        /* ok */
		return is_true;		
	}
	else
	{
		/* Internal ODBC status */
		iRetval = ((RODBC *)this)->GetReturnCode();
		/*******************************************/
		szACCION[0] = DNET_ACTION_END_OF_FILE;
	    /* BOOL FLAG ON ACTION */
		iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
					? TRUE
					: FALSE ;
		/*******************************************/
		/* Release the sentence */
		FreeSentence( iSQLSentence );    		
		return is_false;		
	};
};


