/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IT24 SISTEMAS S.A.
// RPC-CONNECTOR (ODBC+RPC) for DATANET system (BANCO TIERRA DEL FUEGO y otros - IB)
//
// Tarea        Fecha           Autor   Observaciones
// (Inicio)		2007.08.15		mrb		BASE, tomando como ejemplo lo del BISEL con SFB
// (Inicio)		2007.09.25		mrb		Conversion TCE/TDE hacia ISO-8583 hacia EXTRACT-TLF 
// (Inicio)		2007.11.21		mdc		FIX : Debit or Credit? (optional)
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <qusrinc/rpclib.h>
#include <qusrinc/datanet.h>
#include <qusrinc/stdiol.h>
#include <qusrinc/msgtrk.h>
#include <qusrinc/copymacr.h>
#include <qusrinc/databatc.h>

/////////////////////////////////////////////////
// ANSI STL string 
#include <string>
using namespace std;
/////////////////////////////////////////////////


#if defined( _SYSTEM_DATANET_ )
#include <qusrinc/datanet.h>
#endif // _SYSTEM_DATANET_

//////////////////////////////////////////////////////////
// TRACE TO OUTPUT FILE IS PRESENT
#ifndef _DEBUG
#define _RPC_TRACE_FILENAME_ ""
#else

#define _RPC_TRACE_FILENAME_  "RPCLOGG.TXT"

#define _RPC_CHAR_CUT_ 0xFF

#endif // TRACE_MODE
//////////////////////////////////////////////////////////

/*********************************************/
#ifdef _SYSTEM_DATANET_TDFUEGO_
#define _DTN_ACCT_PICTURE_LENGHT_   (3+2+9)   /* verificado : suc.logica + numero       */
#define _ISO_ACCT_PICTURE_LENGHT_   (3+2+9+1) /* y se agrega la suc.fisica como prefijo */
#define _ACCT_INTERNAL_LENGHT_      (3+2+9)   /* unicamente al momento del db/cr        */
/*********************************************/
#else
#error 'FALTA DEFINIR LA LONGITUD DE CUENTA, EXTERNA E INTERNA'
#endif
//*************************************************************************//

///////////////////////////////////////////////////////////////////////////////////
// DEFAULT CONSTRUCTOR
RPC_CONNECTOR_DNET::RPC_CONNECTOR_DNET() 
{
		/********************************/
		achState[0]			       = 0x00; // Default values
		achPostingDate_YYYYMMDD[0] = 0x00; // Default values
		tTimestampPostingDate      = 0L;   // Default values
		strcpy(szIP    ,"127.0.0.1");    // BACKOFFICE's IP ADDRESS BY DEFAULT
		strcpy(szIP_bkp,"127.0.0.1");    // BACKOFFICE's IP ADDRESS BY DEFAULT
		ushPORT     = 80 ;           // TCP PORT (commonly used '80') BY DEFAULT
		ushPostLen  = ushGetLen = 0;      		
		iRetval     = 0;
		szRESULT[0] = szACCION[0] = 0x00;  // NULL VALUES INITIALLY		
		/********************************/
		szACCION_ExistingAccount[0]  = 
		szACCION_ExistingBank[0]     =
		szACCION_AccountStatus[0]    =
		szACCION_AccountAssociate[0] =
		szACCION_Authorize[0]        =
		szACCION_PostingDate[0]      =
		szACCION_Refresh[0]          = 0x00;
		/********************************/
		srand( time( NULL ) );
		/********************************/
};

///////////////////////////////////////////////////////////////////////////////////////
// ALTERNATE CONSTRUCTOR
RPC_CONNECTOR_DNET::RPC_CONNECTOR_DNET( const char *szExternalIP, 
                                        const unsigned usExtPort, 
									    BOOL bUseTrace		,
										LPCSTR cszDBUser    ,
										LPCSTR cszDBPassword
										)
			// DERIVED CLASSES CONSTRUCTORS
			: RPC () ,
			  RODBC         ( ODBC_DATANET_DATABASE , (char *)cszDBUser, (char *)cszDBPassword ) 
{
	size_t nLen = 0; /* aux length */
	
	/* Precondition */
	if(!szExternalIP || 0 == usExtPort)
		throw(-1);

	strcpy(szIP,szExternalIP);    // BACKOFFICE's IP ADDRESS
	strcpy(szIP_bkp,szIP);        // BACKOFFICE's IP ADDRESS BKP
	
    ushPORT       = usExtPort ;   // TCP PORT 
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

	/* USE TRACE FILE ? */
	if(bUseTrace)
	{
	  msgLogg.Open( _RPC_TRACE_FILENAME_ ) ;
	};
};

////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
RPC_CONNECTOR_DNET::~RPC_CONNECTOR_DNET() 
{
	/* Postconditions */
    msgLogg.Close() ;	  
};

static char *FormatInputDate(ULONG ulFEC)
{
    /* FIX WARNING .NET 'LOCAL TEMPORARY' RETURN */
	static char strAux[8+1];
	static char strMes[4+1];

    sprintf(strAux, "%0.2d0.2d%0.2d", ulFEC % 100, (ulFEC / 100)%100, ulFEC / 10000);

	return strAux;
}

/////////////////////////////////////////////////////////////////////
// Validar MAC
// TXT : texto de entrada a funcion NDES
// szAffiliate : Codigo de abonado
// szBankCode  : Codigo de banco
// szMACType   : Tipo de MAC : 1, 2, o 3
// szMACOutput : MAC verificada
//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::ValidateMAC(char *szTXT, char *szAffiliateCode, 
													char *szBankCode, char *szMACtype, 
													char *szMACoutput )
{
   /* ERROR BY DEFAULT */
   szACCION[0] = DNET_ACTION_END_OF_FILE;
   /* BOOL FLAG ON ACTION */
   iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
				? FALSE
				: TRUE ;
	/* not implemented */
	return is_false;
};


/////////////////////////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::GetEncriptionKeys( char *szCodAbonado, char *szCodBanco, 
				                                 char *uchAssocKEY , char *uchBankKEY )
{
	/* GET THE ASSOCIATE KEY */
	boolean_t bAssociate = ( NULL == szCodAbonado ) 
                               ? is_true 
                               : ValidateAccountAssociate( szCodAbonado, uchAssocKEY) ;
	/* GET THE BANK KEY */
	boolean_t bBank      = ( NULL == szCodBanco )
                            ? is_true
                            : ValidateExistingBank( szCodBanco, uchBankKEY ) ;
	/* RETURN OK IF BOTH GET WERE OK */
	return ( bBank && bAssociate ) ? is_true : is_false ;
}

//////////////////////////////////////////////////////////////////////
// Autorizacion general de transaccion DATANET
//////////////////////////////////////////////////////////////////////
WORD RPC_CONNECTOR_DNET::AuthorizeTransaction(
	char szRECTYPE [],// Tipo de registro, TDE = debito TCE = credito, CHAR(3)
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
	/*************************************************/
	char szCODRESP[]   , // Codigo Respuesta
	char szCODUSUOFI[] ,  // Codigo Usuario Oficial
	/*************************************************/
	char szTIPCTADEB [], //  Tipo de cuenta DB, CHAR(2)
	char szTIPCTACRE[] , //  Tipo de cuenta CR, CHAR(2)
	char szSUCDEB []   , //  Sucursal cuenta DB, CHAR(4)
	char szSUCCRE[]    , //  Sucursal cuenta CR, CHAR(4)
	char szAMMOUNT[]   , //  Monto , CHAR(17)
	/*************************************************/
	char chMARTIT	   , //  Marca de mismo titular
	char szTIPOPE[]    , // Tipo Operacion
	//
	PBYTE	pbDPSMSG     // ptr al mensaje DPS en si mismo
	//
	) // Autorizar transaccion DATANET
{
	/////////////////////////////////////////////////////////////
	// Conversion TDE/TCE hacia ISO8583 hacia EXTRACT-TLF
	// mediante la reutilizacion de funciones del producto BATCH.
	/////////////////////////////////////////////////////////////
	DPS_i_batch	  dpsBatchTRA( this->szIP , this->ushPORT );		
	WORD          wRespCde = 0;
	
	// Precondicion : ptrs validos
	if(!(
		szRECTYPE   &&  // Tipo de registro, TDE = debito TCE = credito, CHAR(3)
		szBANDEB    &&  //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
		szFECSOL    &&  //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
		szNUMTRA    &&  //  Numero de transferencia,CHAR(7) 
		szNUMABO    &&  //  Abonado	, CHAR(7)
		szNUMCTADEB &&  //  Numero de cuenta, formato Banco, CHAR(17)	
		szBANCRE    &&  //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
		szCTACRE    &&  //  Numero de cuenta -Formato Banco-, CHAR(17)	
		szESTADO    &&  //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
		szNUMREF    &&  //  Numero de transferencia PC	   &&  //  nro.lote (N,3)+nro. operacion (N,4) , CHAR(7) 
		szNUMENV    &&  //  Numero de envio del abonadochar(3)
		szCODRESP   &&  // Codigo Respuesta
		szCODUSUOFI &&  // Codigo Usuario Oficial
		szTIPCTADEB &&  //  Tipo de cuenta DB, CHAR(2)
		szTIPCTACRE &&  //  Tipo de cuenta CR, CHAR(2)
		szSUCDEB    &&  //  Sucursal cuenta DB, CHAR(4)
		szSUCCRE    &&  //  Sucursal cuenta CR, CHAR(4)
		szAMMOUNT   &&  //  Monto , CHAR(17)
		szTIPOPE        // Tipo Operacion
		) )
	{ 
		iRetval = TCP4U_EMPTYBUFFER;
		// Error
		return (iRetval );
	} 	

	// Precondicion : PTR valido
	if( !pbDPSMSG )
	{ 
		iRetval = TCP4U_EMPTYBUFFER;
		// Error
		return (iRetval );
	} 	

	// Precondicion : SESION DE USUARIO+CONTRASEÑA, verificar estado de envio
	if ( !Login() )
	{ 
		iRetval = TCP4U_CONNECTFAILED;
		// Error
		return (iRetval );
	} 	
	//////////////////////////////////////////
	COPYSTRING( szTransfInt , szRECTYPE );	
	ushPostLen = strlen(szTransfInt);
	//////////////////////////////////////////
	ushPostLen = strlen((PSTR)pbDPSMSG);
	COPYSTRING( szBuffer, (PSTR)pbDPSMSG );
	szBuffer[ushPostLen] = 0x00; 
	//////////////////////////////////////////

	// Escribir lo enviado a un archivo de trace	
	msgLogg.Write( (PBYTE)szBuffer , ushPostLen );

	/////////////////////////////////////////////////////////////
	// Conversion TDE/TCE hacia ISO8583 hacia EXTRACT-TLF
	// mediante la reutilizacion de funciones del producto BATCH.
	/////////////////////////////////////////////////////////////
	iRetval = dpsBatchTRA.DPRE_ISO8583_Convert(szBuffer, ushPostLen        ,
					  						   szBuffer, sizeof(szBuffer)-1,
											   _ISO_ACCT_PICTURE_LENGHT_, _DTN_ACCT_PICTURE_LENGHT_, 
											   'R' ,
											   // FIX : Debit or Credit? (optional)
											   !strcmp( szRECTYPE, "TDE" ) 
												? 'D'
												: 'C' );
	// Escribir lo enviado a un archivo de trace	
	msgLogg.Write( (PBYTE)szBuffer , strlen(szBuffer) );
	
	iRetval = dpsBatchTRA.ISO8583_Authorize( szBuffer , &wRespCde);
	
	// Escribir lo recibido a un archivo de trace	
	msgLogg.Write( (PBYTE)szBuffer , strlen(szBuffer));
	////////////////////////////////////////////////////////

#if FALSE
	// FIX , NO SE USA PROTOCOLO TCP-PP (2 BYTES ANTERIORES INDICAN LONGITUD) SINO
	// TCP PLANO, ASI NOMAS, SIN CONTROL DE LONGITUD.
	// Enviar y esperar respuesta via TCP
	ushPostLen = strlen(szBuffer);
	iRetval = TCPSendAndReceive( szIP, ushPORT, szBuffer, &ushPostLen, ushGetLen, 
							FALSE /*TRUE*/, 30 ,
							FALSE /* KEEP SESSION ALIVE */ );

	// Escribir lo enviado a un archivo de trace
	msgLogg.Write( (PBYTE)szBuffer , ushPostLen );
#endif

	// Verificar retorno
    if( TCP4U_SUCCESS == iRetval )
    {		
		// LOGOUT RPC
		Logout();
		// Save specific error
		iRetval = wRespCde;
		// Return specific error
		return (iRetval = wRespCde);

    }
    else
	{
		// LOGOUT RPC
		Logout();
        // Return 'NOT PROCESSED'
        return (iRetval = -1);
	};
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Logg Transaccion DATANET en TEF ONLINE, sin estados anteriores
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
    char szTRATAM[]  //  Estado tratamiento
		) // Logg Transaccion DATANET
{
    // Lista de ESTADOS ANTERIORES informados por la red financiera
    CHAR szESTADOSANT[]={" "};
	/***************************/
	CHAR szCUITDEB[]={"00000000000"};
	CHAR szCUITCRE[]={"00000000000"};
    CHAR szFILLER2[]={" "};
	/***************************/
    // Invocar al mismo metodo PERO con estados anteriores en BLANCOS
    return LoggTransaction(
	    szFRNAME ,// Nombre nodo emisor, ej. "DNET", char(4)
	    szTONAME , //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
	    szRECTYPE ,// Tipo de registro, TDE = debito TCE = credito, CHAR(3)
	    chACTIVITY,  //  Actividad, CHAR(1)="N"
        szFILLER1,  //  RESERVADO="150"
	    szBANDEB , //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
	    szFECSOL , //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
	    szNUMTRA , //  Numero de transferencia,CHAR(7) 
	    szNUMABO , //  Abonado	, CHAR(7)
	    szTIPOPE , //  Tipo de operacion -	Ver tabla I - , CHAR(2)
	    szIMPORTE  ,//  Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	    szSUCDEB ,  //  Sucursal del Banco que tratara el debito, CHAR(4)
	    szNOMSOL ,  //  Nombre de la cuenta, 
	    szTIPCUEDEB,//   Tipo de cuenta	- Ver tabla II  - CHAR(2)
	    szNCUECMDEB , //  Numero de cuenta corto -Interno Datanet- CHAR(2)
	    szNUMCTADEB , //  Numero de cuenta, formato Banco, CHAR(17)
	    szFSENDB , //  Fecha de envio de la transf.  al Banco -Aammdd-, CHAR(6)
	    szHSENDB , // Hora de envio, Hhmm, CHAR(4)
	    szOPEDB1 , //  Identificacion del operador #1. Primer autorizante del Banco de db. CHAR(2)
	    szOPEDB2 , //  Identificacion del operador #2. Segundo autorizante, CHAr(2).
	    szRECHDB , //  Motivo del rechazo del Banco de debito -Ver tabla III - CHAR(4)
	    szBANCRE , //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
	    szSUCCRE , //  Sucursal del Banco que tratara el credito. CHAR(4)				
	    szNOMBEN,  //  Nombre de la cuenta a acreditar, VARCHAR(29)
	    szTIPCRE , //  Tipo de cuenta -Ver tabla II-, CHAR(2)
	    szCTACRE, //  Numero de cuenta -Formato Banco-, CHAR(17)
	    szFSENCR, //  Fecha de envio de la transf.  al Banco	 -Aammdd-, CHAR(6)
	    szHSENCR, //  Hora de envio -Hhmm-, CHAR(4)
	    szOPECR1, //  Identificacion del operador #1, primer autorizante del Banco de cr. CHAR(2)
	    szOPECR2, //  Identificacion del operador #2, segundo autorizante, CHAR(2)
	    szRECHCR, //  Motivo del rechazo del Banco de credito -ver tabla III-, CHAR(4)
	    szOPECON , //  Operador que confecciono la transferencia	, CHAR(2)
	    szOPEAU1 , //  Id. Firmante #1 (primer autorizante de la Empresa	, CHAR(2)
	    szOPEAU2 , //  Id. Firmante #2 (segundo autorizante de la Empresa	, CHAR(2)
	    szOPEAU3 , //  Id. Firmante #3 (tercer autorizante de la Empresa	, CHAR(2)
	    szFECAUT , //  Fecha de autorizacion (envio del pago a la red) aammdd,  CHAR(6)
	    szHORAUT , //  Hora de autorizacion (envio del pago a la red) hhmm, CHAR(4)
	    szESTADO , //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
	    szFECEST , //  Fecha ultima modificacion de ESTADO	, CHAR(6)
	    szOBSER1 , //  Texto libre ingresado por el abonado	Opcional, VARCHAR(60)
	    szOBSER2 , //  Datos adicionales ingresados por el abonado	Opcional, VARCHAR(100)
	    szCODMAC , //  Message Authentication Code (MAC), CHAR(12)
	    szNUMREF , //  Numero de transferencia PC	 , //  nro.lote (N,3)+nro. operacion (N,4) , CHAR(7) 
	    szNUMENV , //  Numero de envio del abonadochar(3)
	    chCONSOL, //  "S" / "N" consoildacion (1 lote)
	    chMARTIT, //  "S" / "N" misma cuenta debito que credito ?
	    chPRIVEZ, //  "S" / "N" pago previo entre cuentas
	    chDIFCRE, //  "S" / "N" diferida por credito
	    chRIEABO, //  "S" / "N" riesgo abonado
	    chRIEBCO, //  "S" / "N" riesgo banco
        szTRATAM,  //  Estado tratamiento
        szESTADOSANT, // Estados anteriores
		/************************************/
		szCUITDEB,
		szCUITCRE,
		szFILLER2
		/************************************/
		); 
};


///////////////////////////////////////////////////////////////////////////
// Reversar la transaccion DATANET en la tabla TEF ONLINE / LOG MOVIMIENTOS
///////////////////////////////////////////////////////////////////////////
WORD RPC_CONNECTOR_DNET::ReverseTransaction(
	char szRECTYPE [],// Tipo de registro, TDE = debito TCE = credito, CHAR(3)
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
	/*************************************************/
	char szCODRESP[]   , // Codigo Respuesta
	char szCODUSUOFI[] ,  // Codigo Usuario Oficial
	/*************************************************/
	char szTIPCTADEB [], //  Tipo de cuenta DB, CHAR(2)
	char szTIPCTACRE[] , //  Tipo de cuenta CR, CHAR(2)
	char szSUCDEB []   , //  Sucursal cuenta DB, CHAR(4)
	char szSUCCRE[]    , //  Sucursal cuenta CR, CHAR(4)
	char szAMMOUNT[]   , //  Monto , CHAR(17)
	/*************************************************/
	char chMARTIT	   , //  Marca de titular
	char szTIPOPE[]    , //  Tipo de Operacion
	//
	PBYTE	pbDPSMSG     // ptr al mensaje DPS en si mismo
	//
    )   // Reversar transaccion DATANET
{
	/////////////////////////////////////////////////////////////
	// Conversion TDE/TCE hacia ISO8583 hacia EXTRACT-TLF
	// mediante la reutilizacion de funciones del producto BATCH.
	/////////////////////////////////////////////////////////////
	DPS_i_batch	  dpsBatchTRA( this->szIP , this->ushPORT );		
	WORD          wRespCde = 0;
	
	// Precondicion : ptrs validos
	if(!(
		szRECTYPE   &&  // Tipo de registro, TDE = debito TCE = credito, CHAR(3)
		szBANDEB    &&  //  Codigo Banco segun BCRA	ej."067" Bansud, CHAR(3)
		szFECSOL    &&  //  Fecha de compensacion, de dia o prox. Habil -aaaammdd-, CHAR(8)
		szNUMTRA    &&  //  Numero de transferencia,CHAR(7) 
		szNUMABO    &&  //  Abonado	, CHAR(7)
		szNUMCTADEB &&  //  Numero de cuenta, formato Banco, CHAR(17)	
		szBANCRE    &&  //  Codigo de Banco segun BCRA, p/credito. CHAR(3)
		szCTACRE    &&  //  Numero de cuenta -Formato Banco-, CHAR(17)	
		szESTADO    &&  //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
		szNUMREF    &&  //  Numero de transferencia PC	   &&  //  nro.lote (N,3)+nro. operacion (N,4) , CHAR(7) 
		szNUMENV    &&  //  Numero de envio del abonadochar(3)
		szCODRESP   &&  // Codigo Respuesta
		szCODUSUOFI &&  // Codigo Usuario Oficial
		szTIPCTADEB &&  //  Tipo de cuenta DB, CHAR(2)
		szTIPCTACRE &&  //  Tipo de cuenta CR, CHAR(2)
		szSUCDEB    &&  //  Sucursal cuenta DB, CHAR(4)
		szSUCCRE    &&  //  Sucursal cuenta CR, CHAR(4)
		szAMMOUNT   &&  //  Monto , CHAR(17)
		szTIPOPE        // Tipo Operacion
		) )
	{ 
		iRetval = TCP4U_EMPTYBUFFER;
		// Error
		return (iRetval );
	} 	

	// Precondicion : PTR valido
	if( !pbDPSMSG )
	{ 
		iRetval = TCP4U_EMPTYBUFFER;
		// Error
		return (iRetval );
	} 	

	// Precondicion : SESION DE USUARIO+CONTRASEÑA, verificar estado de envio
	if ( !Login() )
	{ 
		iRetval = TCP4U_CONNECTFAILED;
		// Error
		return (iRetval );
	} 	
	//////////////////////////////////////////
	COPYSTRING( szTransfInt , szRECTYPE );	
	ushPostLen = strlen(szTransfInt);
	//////////////////////////////////////////
	ushPostLen = strlen((PSTR)pbDPSMSG);
	COPYSTRING( szBuffer, (PSTR)pbDPSMSG );
	szBuffer[ushPostLen] = 0x00; 
	//////////////////////////////////////////

	// Escribir lo enviado a un archivo de trace	
	msgLogg.Write( (PBYTE)szBuffer , ushPostLen );

	/////////////////////////////////////////////////////////////
	// Conversion TDE/TCE hacia ISO8583 hacia EXTRACT-TLF
	// mediante la reutilizacion de funciones del producto BATCH.
	/////////////////////////////////////////////////////////////
	iRetval = dpsBatchTRA.DPRE_ISO8583_Convert(szBuffer, ushPostLen        ,
					  						   szBuffer, sizeof(szBuffer)-1,
											   _ISO_ACCT_PICTURE_LENGHT_, _DTN_ACCT_PICTURE_LENGHT_, 
											   'R' ,
											   // FIX : S=Reverse-Debit? or R=Reverse-Credit? 
											   !strcmp( szRECTYPE, "TDE" ) 
												? 'S'
												: 'R' );
	// Escribir lo enviado a un archivo de trace	
	msgLogg.Write( (PBYTE)szBuffer , strlen(szBuffer) );
	
	iRetval = dpsBatchTRA.ISO8583_Authorize( szBuffer , &wRespCde);
	
	// Escribir lo recibido a un archivo de trace	
	msgLogg.Write( (PBYTE)szBuffer , strlen(szBuffer));
	////////////////////////////////////////////////////////

#if FALSE
	// FIX , NO SE USA PROTOCOLO TCP-PP (2 BYTES ANTERIORES INDICAN LONGITUD) SINO
	// TCP PLANO, ASI NOMAS, SIN CONTROL DE LONGITUD.
	// Enviar y esperar respuesta via TCP
	ushPostLen = strlen(szBuffer);
	iRetval = TCPSendAndReceive( szIP, ushPORT, szBuffer, &ushPostLen, ushGetLen, 
							FALSE /*TRUE*/, 30 ,
							FALSE /* KEEP SESSION ALIVE */ );

	// Escribir lo enviado a un archivo de trace
	msgLogg.Write( (PBYTE)szBuffer , ushPostLen );
#endif

	// Verificar retorno
    if( TCP4U_SUCCESS == iRetval )
    {		
		// LOGOUT RPC
		Logout();
		// Save specific error
		iRetval = wRespCde;
		// Return specific error
		return (iRetval = wRespCde);

    }
    else
	{
		// LOGOUT RPC
		Logout();
        // Return 'NOT PROCESSED'
        return (iRetval = -1);
	};
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Recuperar el valor del estado del sistema
char RPC_CONNECTOR_DNET::GetOrSetState(char chForceState)
{
    // Para recuperar el estado, pasar un valor NULO
    if(0x00 == chForceState)
        return achState[0];
    // Para establecer un estado, pasar un valor valido pasar 'A' o 'F'
    else if('A' == chForceState || 'F' == chForceState)
        return ( achState[0] = chForceState );
    // Para forzar la apertura del sistema, inicialmente pasar '1' o '0'
    else if('1' == chForceState || '0' == chForceState)
        return ( achState[0] = chForceState );
    else
        // Sino se retorna el estado actual
        return achState[0];
}

//////////////////////////////////////////////////////////////////////
// Refrescar el POSTING DATE del KERNEL
boolean_t RPC_CONNECTOR_DNET::RefreshPostingDate( char *szPOSTING_DATE,
												  char *szUSER        ,
												  char *szPASSWORD    )
{	
   // BOOL FLAG ON ACTION
   iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
				? FALSE
				: TRUE ;

   /* Ok ? / Error? */
   return (DNET_ACTION_NO_ERROR == szACCION[0] || TRUE) 
	   ? is_true
	   : is_false ;
};

//////////////////////////////////////////////////////////////////////
// Enviar mensaje de solicitud del acuerdo-sobregiro a un oficial 
// de cuenta, ante bloqueos que excedan el mismo.
boolean_t RPC_CONNECTOR_DNET::SendAuthMessage(
	        char szFECSOL[] ,           // Fecha de Solicitud
	        char szNUMABO[] ,           // Codigo de Abonado
	        char szNUMCTADEB[],         // Cuenta de Debito
            char szNUMREF[],            // Lote, inferido de Referencia
            char szNUMTRA[],            // Nro. Transferencia
            char szMSG_Acuerdo[],       // Monto del Acuerdo
            char szMSG_NomOficial[],    // Nombre del Oficial
	        char szMSG_Abonado[],       // Cod. Abonado
	        char szMSG_CtaDebito[],     // Cta. de Debito
            char szMSG_NomCompletoOficial[], // Nombre Completo del Oficial
            char szMSG_Importe[],            // Importe de TEF
            char szMSG_NomSolicitante[],     // Solicitante - Descrip. Cta.
            char szMSG_QReclamos[] ,         // Cantidad de reclamos
            char szMSG_MonedaCta[] ,         // Moneda de la Cta
            char szMSG_EMail[] )            // E-Mail
{
   // BOOL FLAG ON ACTION
   iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
				? FALSE
				: TRUE ;

   /* Ok ? / Error? */
   return (DNET_ACTION_NO_ERROR == szACCION[0]) 
	   ? is_true
	   : is_false ;
};


///////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Retrieve Transaccion DATANET desde TEF ONLINE
//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::RetrieveRefreshMovement(
	char szBANCO[] ,   //Codigo Banco segun BCRA	ej."067":Bansud, CHAR(3)
	char szNUMCTA[] ,  //Numero de cuenta, formato Banco, CHAR(17)
	char szFECVAL[],  //Fecha Valor -aaaammdd-, CHAR(8)
	char szSECUENCIAL[],//Numero correlativo, CHAR(10)
	char chDEBCRE[],   //Debito-Credito('D'/'C')	, CHAR(1)
	char szIMPORTE[],  //Importe de la transferencia, 15 enteros y 2 decimales, tipo de dato MONEY.
	char szNUMCOMP[],  //Numero de comprobante, CHAR(10)
	char chDIFMOV[],   //Diferido-Movimiento('D'/'M')	, CHAR(1)
    char szCODOPE[],   //Tipo de operacion -	CHAR(3)
	char szHORAMOV[],  //Hora de movimiento, Hhmm, CHAR(4)
	char szSUCURSAL[],//Sucursal, CHAR(3)
	char szFECPRO[],   //Fecha de proceso en el Banco -Aaaammdd-, CHAR(8)
	char chCONTRA[],   //Contraaseinto('1'/'0')	, CHAR(1)
	char szTIPCUE[],   //tipo de cuenta	- CHAR(2)
	char szDEPOSITO[],  //DEPOSITO, VARCHAR(8)
	char szDESCRIP[],  //Texto libre , VARCHAR(25)
	char chCODPRO[],   //Codigo de Proceso	, CHAR(1)
	char szLastValue[] // Last Value of PK
		) // Retrieve 1er Transaccion DATANET
{
   // BOOL FLAG ON ACTION
   iRetval = (DNET_IS_ACTION_OK(szACCION)) 
				? TRUE
				: FALSE ;

   // END OF DATABASE RECORDS...
   if( FALSE == iRetval )
	   return is_false;
   else
	   return is_true;
}

//////////////////////////////////////////////////////////////////////
// Retrieve Transaccion DATANET desde TEF ONLINE, COBRANZAS
//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::RetrieveRefreshMovement(
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
	char szLastValue[] // Last Value of PK
)
{
   /* BOOL FLAG ON ACTION */
   iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
				? FALSE
				: TRUE ;
	/* not implemented */
	return is_false;
}


//////////////////////////////////////////////////////////////////////
// Update COBRANZAS Movement
//////////////////////////////////////////////////////////////////////
boolean_t RPC_CONNECTOR_DNET::UpdateRefreshMovement(
	char szBOLETA[],  //BOLETA, VARCHAR(9)
	char szCOBRANZA[] //Cobranza , VARCHAR(9)
)
{
   /* ERROR BY DEFAULT */
   szACCION[0] = DNET_ACTION_END_OF_FILE;
   /* BOOL FLAG ON ACTION */
   iRetval = (!DNET_IS_ACTION_OK(szACCION)) 
				? FALSE
				: TRUE ;
	/* not implemented */
	return is_false;
}



////////////////////////////////////////////////////////////////////////
// Envio de LOGIN (ECHO-LOGIN), o espera de LOGIN segun indicador
boolean_t RPC_CONNECTOR_DNET::Login( void ) 
{   
	// Marca estatica de LOGIN local
	static bool bLogin = false;

	if (!bLogin)
	{
	    // Enviar mensaje al BackOffice ( no implementado )
		SendUser();
		bLogin = true;		
		return is_true;
	}
	else
	{
		return is_true;
	};
};

////////////////////////////////////////////////////////////////////////
// Envio de LOGIN (ECHO-LOGIN), o espera de LOGIN segun indicador
boolean_t RPC_CONNECTOR_DNET::Logout( void ) 
{  
	// Postcondicion : siempre OK, no implementado
	return is_true;	
}


////////////////////////////////////////////////////////////////////////
// Envio de usuario para despertar el socket, no se espera respuesta,
// solo se envia
void RPC_CONNECTOR_DNET::SendUser() 
{   

	// Precondicion
	if (strlen( szIP ) == 0)        // BACKOFFICE's IP ADDRESS BKP
		strcpy(szIP,szIP_bkp);        // BACKOFFICE's IP ADDRESS BKP
	if (strlen( szIP ) == 0)        // BACKOFFICE's IP ADDRESS BKP
		throw(-1);

}

/********************************************************************************/
