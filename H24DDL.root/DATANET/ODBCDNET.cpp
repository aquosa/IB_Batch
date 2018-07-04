
///////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A.
// ON2-CONNECTOR (ODBC+MOSAIC) for DATANET system (BANCO CIUDAD DE BS.AS.)
//
//   Clase conexion a base datos via ODBC. Implementacion para XML JERONIMO.
//
// Tarea        Fecha           Autor   Observaciones
// (1.0.1.0)	2006.01.05      mdc     ODBC para ciertos metodos
// (1.0.1.0)    2006.04.17      mdc     En ::GetEncryptionKeys(), se cambia el orden de recuperar claves
// (1.0.1.1)    2007.09.04      mdc     CONTROL ENABLED?
//
//
///////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
#include <qusrinc/odbcdnet.h>
#include <qusrinc/trxdnet.h>
#ifndef _SQL_ODBC_SENTENCES_
#define _SQL_ODBC_SENTENCES_
#endif // _SQL_ODBC_SENTENCES_

#include <sqlext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/////////////////////////////////////////////////

#define IS_VALID_POINTER(x)	(NULL != x && ((void *)0xdddddddd != x) )


#include <qusrinc/datanet.h>
#include <qusrinc/stdiol.h>

#ifndef ODBC_DATANET_DATABASE
#define ODBC_DATANET_DATABASE               "datanet"
#define ODBC_DATANET_USER                   "datanet"
#define ODBC_DATANET_USER_PASSWORD          "datanet"
#define _SQL_INSERT_REQR_SNTC_              ""
#define _SQL_SELECT_SERIAL_SNTC_            ""
#define _SQL_SELECT_RESPONSE_SNTC_          ""
#endif

//////////////////////////////////////////////////////////
// TRACE TO OUTPUT FILE IS PRESENT
#ifndef _DEBUG
#define IS_TRACE_MODE( file, string )	 // DO NOTHING
#define _ODBC_DATANET_TRACE_FILENAME_    "odbc.log"

#else

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Macro de verificacion de codigo de banco emisor/receptor , si es banco propio
#ifndef IS_ON_OUR_BANKCODE
#define IS_ON_OUR_BANKCODE(x)   (BCRA_ID_DEFAULT_I == antoi(x,3)) 
#endif
// Macro simplificada de copia de memoria campo a campo
#define COPYFIELD( to, from )      memcpy( to,from,min(sizeof(to),sizeof(from)) );
#define FULLCOPYFIELD( to, from )  memcpy( to,from,sizeof(from) );
#define EXACTCOPYFIELD( to, from ) memcpy( to,from, sizeof(to) );
#define RIGHTCOPYFIELD( to, from ) sprintf(to,"%-*.*s", sizeof(to), sizeof(to), from);
#define PADCOPYINTEGER( to, from ) sprintf(to,"%0*i" , sizeof(to), atoi(from) );
#define BLANKFIELD( fld )          memset( fld,' ',sizeof(fld) );
#define ZEROESTRING( str )         memset( str,'0',sizeof(str)-1 );
#define BLANKSTRING( str )         memset( str,' ',sizeof(str)-1 );
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// WRITE TO TRACE FILE
//////////////////////////////////////////////////////////////////////////////
void write_to_trace_file( FILE *fFile, void *bBuffer, size_t nLen )
{
	time_t tTime        = time( NULL );
	struct tm *tmLocale = localtime( &tTime );
	char   *szASCII     = asctime( tmLocale );
	char   *pszBuffer   = new char[nLen+1];
	// PRECONDITION
	if( !fFile || !bBuffer || nLen <= 0 )
		return;
	// PRECONDITION
	if(!pszBuffer)
		return;
    // REPLACE NULLS IN BUFFER
	memcpy(pszBuffer,bBuffer,nLen);
	for(int i=0;i<nLen;i++)
	{
		if( 0x00 == pszBuffer[i] ) // dec=0 = NULL
			pszBuffer[i] = 0x20;   // dec=32 = BLANK-SPACE
		else if( pszBuffer[i] < 0x20 ) // dec<32 = ALL CONTROL CHARS
			pszBuffer[i] = 0xff; // dec=255 = NOT USED
	} ;
	// PRINT DEBUG STRING
	fprintf( fFile, "\r\n%s | [%s]", szASCII , pszBuffer );	
	fflush( fFile );	
	// POSTCONDICION
	delete [] pszBuffer;
	pszBuffer = NULL;
}

#define IS_TRACE_MODE( file, string )	write_to_trace_file( file, string )
#define _ODBC_DATANET_TRACE_FILENAME_  "LGODBC.TXT"

#endif // TRACE_MODE

//////////////////////////////////////////////////////////////////////
ODBC_CONNECTOR_DNET::ODBC_CONNECTOR_DNET() 
{
	size_t nLen = 0; /* aux length */
	
	iRetval       = 0;
	szRESULT[0]   = szACCION[0]   = 0x00; // NULL VALUES INITIALLY
	szSQL_USER[0] = szSQL_PASS[0] = 0x00; // NULL VALUES INITIALLY

	achState[0]			       = 0x00; // Default values
	achPostingDate_YYYYMMDD[0] = 0x00; // Default values
	tTimestampPostingDate      = 0L;   // Default values
	/********************************/
	szACCION_ExistingAccount[0]  = 
	szACCION_ExistingBank[0]     =
	szACCION_AccountStatus[0]    =
	szACCION_AccountAssociate[0] =
	szACCION_Authorize[0]        =
	szACCION_PostingDate[0]      =
	szACCION_Refresh[0]          = 0x00;
	/********************************/

	file_hdl = NULL;
	file_hdl = fopen (_ODBC_DATANET_TRACE_FILENAME_, "at") ;
}
//////////////////////////////////////////////////////////////////////
ODBC_CONNECTOR_DNET::ODBC_CONNECTOR_DNET(LPCSTR cszDBUser ,
									LPCSTR cszDBPassword  ) 
{
	file_hdl = NULL;
	ODBC_CONNECTOR_DNET::ODBC_CONNECTOR_DNET();
	ConnectDataBase( "datanet", (PCHAR)cszDBUser, (PCHAR)cszDBPassword );
}
//////////////////////////////////////////////////////////////////////
ODBC_CONNECTOR_DNET::~ODBC_CONNECTOR_DNET()
{
	/* Postconditions */
	if( IS_VALID_POINTER(file_hdl) )
	{
	  fclose (file_hdl) ;
	  file_hdl = NULL;
	}
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Coneccion a base de datos --- si es necesaria
//////////////////////////////////////////////////////////////////////
void ODBC_CONNECTOR_DNET::ConnectDataBase( char *szDBase, char *szDBaseUser, char *szPassw )
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
boolean_t ODBC_CONNECTOR_DNET::LoggTransaction(
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
            "insert into dn_tef_online "
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
			"ERROR_DE_MAC,PEND_ENVIO,SECUENCIA_DEBCRE,COD_ALTERNO_DEBCRE,SECUENCIA_REVERSA,"
			"COD_ALTERNO_REVERSA,ACCION,CLAVE_MENVIADA)"
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
			" '0','S','0','0','0',"
			" '0','0', '%s')"	
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
	  , szFILLER2     //FILLER CLAVE MAC2 ENVIADA
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
boolean_t ODBC_CONNECTOR_DNET::RetrieveTransaction(
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
	/****************/
	char szACCION_IND[] // Accion indicada en la tabla (no la respuesta del metodo)
	/****************/
		) // Retrieve 1er Transaccion DATANET
{
	/* Local variables */
	char szDEFAULT[]  = {0x00} ;
    char szNROLOTE[8] = {0x00} ;
	char szQuery[SZMAXBUFFER_SIZE*2] = {0x00} ;
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

	// Ejecucion de SP
	     
    iSQLSentence = _ODBC_READ_SNTC_ ;		
	Column( szFRNAME , SQL_C_CHAR, 4+1, &iSQLSentence ); //1Nombre nodo emisor, ej."DNET"  //CHAR(4)	
	Column( szTONAME , SQL_C_CHAR, 4+1, &iSQLSentence ); //2Nombre nodo receptor, ej."BSUD" //CHAR(4)
	Column( szRECTYPE, SQL_C_CHAR, 3+1, &iSQLSentence ); //3Tipo de registro, TDE = debito TCE = credito, CHAR(3)
	Column( szBANDEB , SQL_C_CHAR, 3+1, &iSQLSentence ); //4Codigo Banco segun BCRA	ej."067":Bansud, CHAR(3)
	Column( szFECSOL , SQL_C_CHAR, 8+1, &iSQLSentence ); //5Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
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
	Column( szACCION_IND, SQL_C_CHAR, 1+1, &iSQLSentence ); //50-CHAR(1)        
	// 
	//						
	// Ejecucion de SP
    sprintf( szQuery, 
            "select "
            "COD_ORIGEN,COD_DESTINO,TIPO_REGISTRO,BCO_DEBITO,FEC_SOLICITUD,NRO_TRANSFERENCIA,COD_ABONADO,"
			"TIPO_OPERACION,IMPORTE,SUC_DEBITO,NOM_SOLICITANTE,TIPO_CTA_DEB_RED,NRO_CTA_RED,CTA_DEBITO,FEC_ENVIO_DEBITO,"
            "HORA_ENVIO_DEBITO,OPERADOR_DB_1,OPERADOR_DB_2,MOTIVO_RECHAZO_DB,BCO_CREDITO,SUC_CREDITO,"
            "NOM_BENEFICIARIO,TIPO_CTA_CRED_RED,CTA_CREDITO,FEC_ENVIO_CREDITO,HORA_ENVIO_CREDITO,OPERADOR_CR_1,"
            "OPERADOR_CR_2,MOTIVO_RECHAZO_CR,OPERADOR_INGRESO,AUTORIZANTE_1,AUTORIZANTE_2,AUTORIZANTE_3,"
            "FECHA_AUTORIZACION,HORA_AUTORIZACION,ESTADO,FEC_ESTADO,OBSERVACION_1,OBSERVACION_2,"
            "CLAVE_MAC,NRO_REFERENCIA,NRO_ENVIO,DEB_CONSOLIDADO,TIPO_TITULAR,PAGO_PREACORDADO,"
            "TRATA_CR_DIFERIDO,RIESGO_ABONADO,RIESGO_BANCO,ESTADO_TRATAMIENTO,ACCION "
            " from dn_tef_online"
			" where ESTADO_TRATAMIENTO='%s'"
			" order by FEC_SOLICITUD,NRO_TRANSFERENCIA"
			,
			chEstadoTratamiento);

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
// Actualizar la transaccion DATANET en la tabla TEF ONLINE
//////////////////////////////////////////////////////////////////////
boolean_t ODBC_CONNECTOR_DNET::UpdateTransaction(
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
	PCHAR szACCION_IND,// ACCION INDICADA X EL AUTORIZADOR
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
	if( szACCION_IND && szCODRESP && szCODUSUOFI )
	{
		sprintf( szQuery, 
				"update dn_tef_online "
				" set "			
				" ESTADO='%s',PEND_ENVIO='%s',"
				" MOTIVO_RECHAZO_DB='%s',MOTIVO_RECHAZO_CR='%s',"
				" ESTADO_TRATAMIENTO='%s',ACCION='%s',"
				" COD_RESPUESTA='%s',COD_USUARIO_OFICIAL='%s' "
				" where "
				" NRO_TRANSFERENCIA='%s' ",	 
		  szESTADO  ,   //Estado de la transferencia	-Ver tabla IV- CHAR(2)
		  szPENDENVIO,  // Pend. Envio?
		  szRECHDB,     // motivo de rechazo debito
		  szRECHCR,     // motivo de rechazo credito
		  szESTADOTRAT, // Estado tratamiento
		  /************************************************/
		  szACCION_IND,  // ACCION (1)
		  szCODRESP   ,  // Codigo Respuesta (16)
		  szCODUSUOFI ,  // Codigo Usuario Oficial (16)	    
		  /************************************************/
		  szNUMTRA      //Numero de transferencia, CHAR(7)
		  );
	}
	else
	{
		sprintf( szQuery, 
				"update dn_tef_online "
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
boolean_t ODBC_CONNECTOR_DNET::ValidateExistingAccount(char *szNroCuenta,short nTipoCuenta )  // Verificar cuenta existente
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
    sprintf( szQuery, "select COUNT(*) from DN_ABONADOS_CUENTAS where CUENTA='%s'",
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
boolean_t ODBC_CONNECTOR_DNET::ValidateExistingBank(char *szBankNo , char *szBankKey )  
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
    sprintf( szQuery, "select CLAVE from DN_BANCOS_CLAVES where BANCO='%s'",
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
		/* LONGITUD DE CLAVE? */
		if(strlen(szBankKey)<DNET_BANK_KEY_LENGTH)
			strrepchar(szBankKey, DNET_BANK_KEY_LENGTH, 0x00, ' ' );		
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
boolean_t ODBC_CONNECTOR_DNET::ValidateAccountStatus( char *szNroCuenta, char *szEstado,
                                          short nTipoCuenta )
                                           // Verificar estado de la cuenta
{
	/* Local variables */
	char szQuery[SZMAXBUFFER_SIZE*2] = {0x00} ;
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

	// Ejecucion de SP
	     
    iSQLSentence = _ODBC_READ_SNTC_ ;			
	Column( szEstado, SQL_C_CHAR, 1+1, &iSQLSentence );  //CHAR(1)	
	
	//						
	// Ejecucion de SP
    sprintf( szQuery, "select ESTADO from DN_ABONADOS_CUENTAS where CUENTA='%s'",
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
boolean_t ODBC_CONNECTOR_DNET::ValidateAccountAssociate(char *szCodAbonado, char *szClave )
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
	Column( szClave, SQL_C_CHAR, 8+1, &iSQLSentence );  //CHAR(8)		

	//						
	// Ejecucion de SP
    sprintf( szQuery, "select CLAVE from DN_ABONADOS_CLAVES where ABONADO='%s'",
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
		/* LONGITUD DE CLAVE? */
		if(strlen(szClave)<DNET_BANK_KEY_LENGTH)
			strrepchar(szClave, DNET_BANK_KEY_LENGTH, 0x00, ' ' );

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
// Verificar las claves de encripcion
//////////////////////////////////////////////////////////////////////
boolean_t ODBC_CONNECTOR_DNET::GetEncriptionKeys(char *szCodAbonado, char *szCodBanco, 
												 char *uchAsocKEY  , char *uchBankKEY )
{
	boolean_t bBANK, bASSOC ;

    // Debe leerse el BANCO?
    if( szCodBanco && uchBankKEY)
        bBANK  = ValidateExistingBank    ( szCodBanco  , uchBankKEY ) ;
    else
        bBANK  = is_true;
    // Debe leerse el ABONADO - ASOCIADO ?
    if( szCodAbonado && uchAsocKEY)
	    bASSOC = ValidateAccountAssociate( szCodAbonado, uchAsocKEY ) ;
    else
        bASSOC = is_true;

    // Solo si ambos dieron BIEN
	return ( is_true == bBANK && is_true == bASSOC )
		 ? is_true
		 : is_false;

}


////////////////////////////////////////////////////////////////////////////////////////////////
// Recuperar 1er. registro de refresh movimientos online
boolean_t ODBC_CONNECTOR_DNET::RetrieveRefreshMovement( 
				char szBANCO[] ,   //Codigo Banco segun BCRA	ej."067":Bansud, CHAR(3)
				char szNUMCTA[] ,  //Numero de cuenta, formato Banco, CHAR(17)
				char szFECVAL[],  //Fecha Valor -aaaammdd-, CHAR(8)
				char szSECUENCIAL[],//Numero correlativo, CHAR(10)
				char chDEBCRE[],   //Debito-Credito('D'/'C')	, CHAR(1)
				char szIMPORTE[],  //Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
				char szNUMCOMP[],  //Numero de comprobante, CHAR(10)
				char chDIFMOV[],   //Diferido-Movimiento('D'/'M')	, CHAR(1)
				char szCODOPE[],   //Tipo de operacion -	CHAR(3)
				char szHORMOV[],   //Hora de movimiento, Hhmm, CHAR(4)
				char szSUCORI[] ,  //Sucursal, CHAR(5)
				char szFECPRO[],   //Fecha de proceso en el Banco -Aaaammdd-, CHAR(8)
				char chCONTRA[],   //Contraaseinto('1'/'0')	, CHAR(1)
				char szTIPCUE[],   //tipo de cuenta	- CHAR(2)
				char szDEPOSI[],   //DEPOSITO, VARCHAR(8)
				char szDESCRI[],   //Texto libre Opcional, VARCHAR(25)
				char chCODPRO[] ,   //7Debito-Credito('D'/'C')	, CHAR(1)
				char szLastValue[] ) // Last Value of PK
{
	/* Local variables */
	char szDEFAULT[]  = {0x00} ;
    char szNROLOTE[8] = {0x00} ;
	char szQuery[SZMAXBUFFER_SIZE*2] = {0x00} ;
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

	// Ejecucion de SP
	     
    iSQLSentence = _ODBC_READ_SNTC_ ;		
	Column( szBANCO   , SQL_C_CHAR, 3+1, &iSQLSentence ); //1
	Column( szNUMCTA  , SQL_C_CHAR,17+1, &iSQLSentence ); //2
	Column( szFECVAL  , SQL_C_CHAR, 10+1+8+1, &iSQLSentence ); //3
	Column( szSECUENCIAL , SQL_C_CHAR,12+1, &iSQLSentence ); //4
	Column( chDEBCRE  , SQL_C_CHAR, 3+1, &iSQLSentence ); //5
	Column( szIMPORTE , SQL_C_CHAR,17+1, &iSQLSentence ); //6
	Column( szNUMCOMP , SQL_C_CHAR,12+1, &iSQLSentence ); //7
	Column( chDIFMOV  , SQL_C_CHAR, 1+1, &iSQLSentence ); //8
	Column( szCODOPE  , SQL_C_CHAR,3+1, &iSQLSentence );  //9
	Column( szHORMOV  , SQL_C_CHAR,10+1+8+1, &iSQLSentence ); //10
	Column( szSUCORI  , SQL_C_CHAR,5+1, &iSQLSentence );  //11
	Column( szFECPRO  , SQL_C_CHAR,10+1+8+1, &iSQLSentence ); //12
	Column( chCONTRA  , SQL_C_CHAR, 1+1, &iSQLSentence ); //13
	Column( szTIPCUE  , SQL_C_CHAR,2+1, &iSQLSentence ); //14	
	Column( szDEPOSI  , SQL_C_CHAR,8+1, &iSQLSentence ); //15
	Column( szDESCRI  , SQL_C_CHAR,25+1, &iSQLSentence ); //16

	// 
	//						
	// Ejecucion de SP
    sprintf( szQuery, 
            "select "
			" BANCO, NUMCTA, FECVAL, SECUENCIAL, DEBCRE, IMPORTE, NUMCOMP, "
			" DIFMOV, CODOPE, HORMOV, SUCORI, FECPRO, CONTRA, TIPCUE, DEPOSI, DESCRI "
            " from VIEW_DATANET_CTA"
			" where SECUENCIAL > %i"
			" order by SECUENCIAL" ,
			atol(szLastValue) );

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
		/* LAST PK VALUE */
		strcpy( szLastValue, szSECUENCIAL );
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
// Retrieve Transaccion DATANET desde TEF ONLINE, COBRANZAS
//////////////////////////////////////////////////////////////////////
boolean_t ODBC_CONNECTOR_DNET::RetrieveRefreshMovement(
				char szBANCO[] ,       //Codigo Banco segun BCRA	ej."067":Bansud, CHAR(3)
				char szNUMCTA[] ,      //Numero de cuenta, formato Banco, CHAR(17)
				char szNUMCTAEDIT[] ,  //Numero de cuenta editada, formato Banco, CHAR(23)
				char szMONEDA[],  //Moneda, CHAR(3)
				char szTIPOMOV[], //Tipo Mov. CHAR(1)
				char szFECVAL[],  //Fecha Imputacion - Valor -aammdd-, CHAR(6)
				char szCLEARING[],//Clearing , CHAR(1)
				char szNROCHEQUE[],//Nro. Cheque, CHAR(4)
				char szCODCLI[],  //Codigo Cliente, CHAR(10)
				char szNOMCLI[],  //Codigo Cliente, CHAR(24)
				char szCOMPROBANTE[], //Comprobante-Factura-Recibo, CHAR(16)
				char szIMPORTE[],  //Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
				char szSUCURSAL[],//Sucursal, CHAR(3)	
				char szMASMOV[],   //Mas Movimientos - CHAR(1)
				char szBOLETA[],  //BOLETA, VARCHAR(9)
				char szCOBRANZA[], //Cobranza , VARCHAR(9)
				char szLastValue[] ) // Last Value of PK
{
	return is_false;
}

///////////////////////////////////////////////////////////////////////////