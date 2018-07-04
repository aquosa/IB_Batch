/////////////////////////////////////////////////////////////////////////////////////////////////
// IT24 Sistemas S.A.
// RPC-CONNECTOR (ODBC+RPC) for DATANET system (BANCO T.FUEGO y otros  - IB)
//
// Libreria de Manejo de Transacciones RPC
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2007.08.15      mdc     Base tomando a BISEL como ejemplo
// (Inicial)    2007.09.25      mdc     PBYTE	pbDPSMSG  : ptr al mensaje DPS en si mismo
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _RPC_LIB_H_
#define _RPC_LIB_H_

// Macros de copia STRING, STRUCT, FIELD, etc.
#include <qusrinc/copymacr.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <qusrinc/typedefs.h>
#include <qusrinc/alias.h>
#include <qusrinc/ctotcp4u.h>
#include <qusrinc/rodbc.h>
#include <qusrinc/stdiol.h>
#include <qusrinc/msgtrk.h>
// Verificar que sistema se esta compilando (DNET, BANE, LINK...)
#include <qusrinc/dpcsys.h>

#ifndef _e_dnet_action_tag_
#define _e_dnet_action_tag_
/* DNET ACTION VALUES  */
typedef enum e_dnet_action_tag
{
	DNET_ACTION_NULL				  = ' ',	
	DNET_ACTION_NO_ERROR			  = '0',
	DNET_ACTION_NO_ERROR_IN_AUTH	  = '1',
	DNET_ACTION_END_OF_FILE			  = '1',	
} e_dnet_action;
#endif

#ifndef DNET_IS_ACTION_OK
#define DNET_IS_ACTION_OK(x) (DNET_ACTION_NO_ERROR        == x[0])
#define DNET_IS_ACTION_ERROR(x) (DNET_ACTION_END_OF_FILE  == x[0])
#endif


////////////////////////////////////////////////////////////////////////////////////////////////
// Proyecto DATANET-INTERBANKING
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Clase Interfase para RPC-UNYSIS con comunicacion via TCP/IP, sin PP (2 bytes)
////////////////////////////////////////////////////////////////////////////////////////////////
class RPC
{
	public :
	    // Constructor x omision
		EXPORT_ATTR RPC();	
		// Destructor
        EXPORT_ATTR ~RPC();	
	  // Login/Logout 
        virtual EXPORT_ATTR boolean_t Login( void );
        virtual EXPORT_ATTR boolean_t Logout( void );
		// Captura de Fecha de Negocio (fecha del sistema en este caso)
        virtual EXPORT_ATTR boolean_t RefreshDate( 
                                             ULONG *pYYYYMMDD  ,
											 char *szUSER	   = NULL ,
											 char *szPASSWORD  = NULL  );
		// Captura de Hora de Negocio (hora del sistema en este caso)
        virtual EXPORT_ATTR boolean_t RefreshPostingTime(   
                                            ULONG *pHHMMSS     ,
								            char *szUSER	   = NULL,
								            char *szPASSWORD   = NULL );


protected :

	char		szTransfInt[2048+1];		// Mensaje de transaccion	

protected :

	// Envio y recepcion por protocolo tcp/ip, subprotocolo point-2-point
    int TCPSendAndReceive(char *szHost, unsigned short usPort, 
		    			  char *szBuffer, unsigned short *piLen,
			    		  unsigned short shRcvLen, short bPP_Protocol,
				    	  unsigned uTimeOut ,
						  /* SESSION LIMIT? LIMIT=4 en RPC */
						  const BOOL bTerminateSession = FALSE );
	// Recepcion y envio por protocolo tcp/ip, subprotocolo point-2-point
	int TCPReceiveAndSend(char *szHost           , unsigned short usPort, 
    		    		  char *szBuffer         , unsigned short *piLen,
	    		    	  unsigned short shRcvLen, short bPP_Protocol,
		    		      unsigned uTimeOut      );

    // Auxiliares : Indicador de conexion activa y socket activo
	boolean_t		bConnected;   // connected ? true-false	
	SOCKET			mySock;       // TCP SOCKET
};

////////////////////////////////////////////////////////////////////////////////////////////////
//
// DATANET-RPC-DataBaseConnectivity Class para DATANET
//
////////////////////////////////////////////////////////////////////////////////////////////////
class RPC_CONNECTOR_DNET :  virtual public RPC, 
                            virtual public RODBC                                     
{
public :

	// DEFAULT CONSTRUCTOR
	EXPORT_ATTR RPC_CONNECTOR_DNET() ;
	// ALTERNATE CONSTRUCTOR
	EXPORT_ATTR RPC_CONNECTOR_DNET(	const char *szIP, 
									const unsigned  usPort, 
									BOOL bUseTrace        ,
									LPCSTR cszDBUser      ,
									LPCSTR cszDBPassword  ) ;
	// *** alternate derived class constructors *** 
	// ::RPC ( szIP, usPort, ... ) ,
	// ::RODBC ( cszDBUser , cszDBPassword ) ;
	// *** alternate derived class constructors *** 
	
	// DESTRUCTOR
	EXPORT_ATTR ~RPC_CONNECTOR_DNET() ;

	// INLINE METHODS
	// RESULT & ACTION
	EXPORT_ATTR const char *GetMethodRESULT()		{ return szRESULT; }
    EXPORT_ATTR const char *GetMethodACTION()		{ return szACCION; }	
	EXPORT_ATTR void SetMethodRESULT(char *szValue) { strcpy(szRESULT,szValue); }
    EXPORT_ATTR void SetMethodACTION(char *szValue) { strcpy(szACCION,szValue); }
	// STATUS AND POSTING DATE
	EXPORT_ATTR const char *GetState()				{ return achState; }
    EXPORT_ATTR const char *GetPostingDate()		{ return achPostingDate_YYYYMMDD; }	
    EXPORT_ATTR long  GetStatus()					{ return iRetval; }	

	EXPORT_ATTR void  ResetStatus()					{ iRetval = 0; }

	// ACTION PER SPECIFIC METHOD
	EXPORT_ATTR const char *GetACTION_ExistingAccount()  { return szACCION_ExistingAccount; }
	EXPORT_ATTR const char *GetACTION_ExistingBank()     { return szACCION_ExistingBank; }
	EXPORT_ATTR const char *GetACTION_AccountStatus()    { return szACCION_AccountStatus; }
	EXPORT_ATTR const char *GetACTION_AccountAssociate() { return szACCION_AccountAssociate; }
	EXPORT_ATTR const char *GetACTION_Authorize()        { return szACCION_Authorize; }
	EXPORT_ATTR const char *GetACTION_PostingDate()      { return szACCION_PostingDate; }
	EXPORT_ATTR const char *GetACTION_Refresh()          { return szACCION_Refresh; }

	
	// CONNECT TO DATABASE IF NECESARY
	EXPORT_ATTR void ConnectDataBase( char *szDBase, char *szDBaseUser, char *szPassw );

	
public :

    // Validacion de MAC-1-2-3
	EXPORT_ATTR boolean_t ValidateMAC(char *szTXT, char *szAffiliateCode, char *szBankCode, 
		char *szMACtype, char *szMACoutput ); // Validar la MAC1,2,3
	// Verificar cuenta existente
	EXPORT_ATTR boolean_t ValidateExistingAccount(char *szNroCuenta,short nTipoCuenta);   
	// Verificar estado de la cuenta, incluido TIPO DE CUENTA
	EXPORT_ATTR boolean_t ValidateAccountStatus( char *szNroCuenta, char *szEstado ,
        short nTipoCuenta );     
	 // Verificar abonado de la cuenta
	EXPORT_ATTR boolean_t ValidateAccountAssociate(char *szCodAbonado, char *Clave ); 
    // Recuperar claves de encripcion, en blanco sin cifrar
    EXPORT_ATTR boolean_t GetEncriptionKeys(char *szCodAbonado, char *szCodBanco, 
                                           char *uchAsocKEY  , char *uchBankKEY );
	////////////////////////////////////////////////////////////////////////////////////////////////
	EXPORT_ATTR boolean_t ValidateExistingBank(char *szBankNo, char *Clave )  ;
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Loggear la transaccion en TEF ONLINE
	EXPORT_ATTR boolean_t LoggTransaction( 
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
	char chCONSOL[],    //  "S" / "N" consoildacion (1 lote)
	char chMARTIT[], //  "S" / "N" misma cuenta debito que credito ?
	char chPRIVEZ[], //  "S" / "N" pago previo entre cuentas
	char chDIFCRE[], //  "S" / "N" diferida por credito
	char chRIEABO[], //  "S" / "N" riesgo abonado
	char chRIEBCO[], //  "S" / "N" riesgo banco
    char szTRATAM[]  //  Estado tratamiento
    );
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Loggear la transaccion en TEF ONLINE, con estados anteriores inclusive
	EXPORT_ATTR boolean_t LoggTransaction( 
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
	char chCONSOL[],    //  "S" / "N" consoildacion (1 lote)
	char chMARTIT[], //  "S" / "N" misma cuenta debito que credito ?
	char chPRIVEZ[], //  "S" / "N" pago previo entre cuentas
	char chDIFCRE[], //  "S" / "N" diferida por credito
	char chRIEABO[], //  "S" / "N" riesgo abonado
	char chRIEBCO[], //  "S" / "N" riesgo banco
    char szTRATAM[],  //  Estado tratamiento
    char szESTADOSANT[], // Estados anteriores
	/******************************************************/
	char szCUITDEB[],    // CUIT debito
	char szCUITCRE[],    // CUIT credito
	char szFILLER2[]     // Relleno 2
	/******************************************************/
    );

	EXPORT_ATTR boolean_t UpdateTransactionCounters(
		LPCSTR szNUMTRA	,
		PSHORT pshCounter
	);   

	////////////////////////////////////////////////////////////////////////////////////////////////
    // Autorizar la transaccion (debitar o acreditar / rechazar)
	EXPORT_ATTR WORD AuthorizeTransaction( 
    char szRECTYPE[], // Tipo de registro, TDE = debito TCE = credito, CHAR(3)
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
	char szCODRESP[]   , // Codigo Respuesta (16)
	char szCODUSUOFI[] , // Codigo Usuario Oficial (16)
	/*************************************************/
	char szTIPCTADEB [], //  Tipo de cuenta, CHAR(2)
	char szTIPCTACRE[] , //  Tipo de cuenta, CHAR(2)
	char szSUCDEB []   , //  Sucursal cuenta DB, CHAR(4)
	char szSUCCRE[]    , //  Sucursal cuenta CR, CHAR(4)
	char szAMMOUNT[]   , //  Monto , CHAR(17)
	char chMARTIT      , //  Marca Titular
	char szTIPOPE[]	   , //  Tipo Operacion
	///////////////////////
	PBYTE	pbDPSMSG     // ptr al mensaje DPS en si mismo
	///////////////////////
	/*************************************************/
	);
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Recuperar la 1er. transaccion 
	EXPORT_ATTR boolean_t RetrieveTransaction( 
	char szFRNAME [],// Nombre nodo emisor, ej. "DNET", char(4)
	char szTONAME [], //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
	char szRECTYPE [],// Tipo de registro, TDE = debito TCE = credito, CHAR(3)
    char chACTIVITY[],  //  Actividad, CHAR(1)="N"
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
	char chCONSOL[],    //  "S" / "N" consoildacion (1 lote)
	char chMARTIT[], //  "S" / "N" misma cuenta debito que credito ?
	char chPRIVEZ[], //  "S" / "N" pago previo entre cuentas
	char chDIFCRE[], //  "S" / "N" diferida por credito
	char chRIEABO[], //  "S" / "N" riesgo abonado
	char chRIEBCO[], //  "S" / "N" riesgo banco	
    char chEstadoTratamiento[], // " ", "0", ...etc...
	char szCANTREINT[]
	); 
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Recuperar la 1er. transaccion 
	EXPORT_ATTR boolean_t RetrieveTransactionDemoradas( 
	char szFRNAME [],// Nombre nodo emisor, ej. "DNET", char(4)
	char szTONAME [], //  Nombre nodo receptor, ej."BSUD"  CHAR(4)
	char szRECTYPE [],// Tipo de registro, TDE = debito TCE = credito, CHAR(3)
    char chACTIVITY[],  //  Actividad, CHAR(1)="N"
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
	char chCONSOL[],    //  "S" / "N" consoildacion (1 lote)
	char chMARTIT[], //  "S" / "N" misma cuenta debito que credito ?
	char chPRIVEZ[], //  "S" / "N" pago previo entre cuentas
	char chDIFCRE[], //  "S" / "N" diferida por credito
	char chRIEABO[], //  "S" / "N" riesgo abonado
	char chRIEBCO[], //  "S" / "N" riesgo banco	
    char chEstadoTratamiento[], // " ", "0", ...etc...
	char szCANTREINT[]
	); 	
	////////////////////////////////////////////////////////////////////////////////////////////////
    EXPORT_ATTR boolean_t UpdateTransaction(
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
    char szPENDENVIO[], // pendiente de envio?S/N
    char szRECHDB[], // motivo de rechazo debito
    char szRECHCR[],// motivo de rechazo credito
    char szESTADOTRAT[], // estado tratamiento
	/************************************************/
	PCHAR szCODRESP   = NULL,  // Codigo Respuesta (16)
	PCHAR szCODUSUOFI = NULL   // Codigo Usuario Oficial (16)	     	
	/************************************************/
	);
	/************************************************/
	////////////////////////////////////////////////////////////////////////////////////////////////
    EXPORT_ATTR WORD ReverseTransaction(
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
		char chMARTIT	   ,
		char szTIPOPE[]    ,
		///////////////////////
		PBYTE	pbDPSMSG     // ptr al mensaje DPS en si mismo
		///////////////////////
		/*************************************************/
    );   // Reversar transaccion DATANET
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Refrescar POSTING DATE Datanet
    EXPORT_ATTR boolean_t RefreshPostingDate(char *szPOSTING_DATE = NULL ,
											 char *szUSER		  = NULL ,
											 char *szPASSWORD     = NULL );
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Enviar mensaje de autorizacion de acuerdo-sobregiro
    EXPORT_ATTR boolean_t SendAuthMessage(
	        char szFECSOL[] ,           // Fecha de Solicitud
	        char szNUMABO[] ,           // Codigo de Abonado
	        char szNUMCTADEB[],         // Cuenta de Debito
            char szNUMREF[],            // Lote, inferido de Referencia
            char szNUMTRA[],            // Nro. Transferencia
            char szMSG_Acuerdo[],       // Monto del Acuerdo
            char szMSG_NomOficial[],    // Nombre del Oficial
	        char szMSG_Abonado[],       // Cod. Abonado
	        char szMSG_CtaDebito[],     // Cta. de Debito
            char szMSG_NomCompletoOficial[],// Nombre Completo del Oficial
            char szMSG_Importe[],           // Importe de TEF
            char szMSG_NomSolicitante[],    // Solicitante
            char szMSG_QReclamos[],         // Cant. reclamos enviados
            char szMSG_MonedaCta[],         // Moneda de la cta
            char szMSG_EMail[] );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    EXPORT_ATTR char GetOrSetState(char achForceState = 0x00);
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Recuperar 1er. registro de refresh movimientos online
	EXPORT_ATTR boolean_t RetrieveRefreshMovement( 
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
	char szSURCURSAL[],//Hora de movimiento, Hhmm, CHAR(4)
	char szFECPRO[],   //Fecha de proceso en el Banco -Aaaammdd-, CHAR(8)
	char chCONTRA[],   //Contraaseinto('1'/'0')	, CHAR(1)
	char szTIPCUE[],   //tipo de cuenta	- CHAR(2)
	char szDEPOSITO[],  //DEPOSITO, VARCHAR(8)
	char szDESCRIP[],  //Texto libre Opcional, VARCHAR(25)
	char chCODPRO[] ,   //7Debito-Credito('D'/'C')	, CHAR(1)
	char szLastValue[] ); // Last Value of PK
	////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// Retrieve Transaccion DATANET desde TEF ONLINE, COBRANZAS
	//////////////////////////////////////////////////////////////////////
	EXPORT_ATTR boolean_t RetrieveRefreshMovement(
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
	char szLastValue[] ); // Last Value of PK
	
	//////////////////////////////////////////////////////////////////////////////////
	EXPORT_ATTR boolean_t UpdateRefreshMovement(char szBOLETA[],  char szCOBRANZA[] );
	//////////////////////////////////////////////////////////////////////////////////
    EXPORT_ATTR boolean_t Login( void );
    EXPORT_ATTR boolean_t Logout( void );
	EXPORT_ATTR boolean_t SendDX130( const char *cpAccion);
	EXPORT_ATTR void SendUser();
    //////////////////////////////////////////////////////////////////////////////////


private :

	/* INTERNAL STATUS */
	char    achState[1+1] ;
	/* INTERNAL POSTING DATE */
	char    achPostingDate_YYYYMMDD[8+1];
	/* INTERNAL SAVED TIME */
	time_t	tTimestampPostingDate;
	/* INTERNAL TRACE FILE */
    MessageTracker msgLogg ;
    /* HTTP/POST parameters */
    char           szBuffer[2048];	
    char           szIP[64]      ,    // BackOffice's IP ADDRESS
	               szIP_bkp[64]  ;    // BackOffice's IP ADDRESS 2007.1.19,mdc.
    unsigned short ushPORT;       // TCP PORT 
    unsigned short ushPostLen, 
				   ushGetLen ;      
	/* INTERNAL TCP return value */
    int				iRetval  ;

protected:

	/* INTERNAL PARAMETERS , ALWAYS RETURNED FROM XML TRANSACTIONS */
	char szRESULT[255+1];
    char szACCION[5+1];
	/* ACTION PER METHOD */
	char szACCION_ExistingAccount[5+1];
	char szACCION_ExistingBank[5+1];
	char szACCION_AccountStatus[5+1];
	char szACCION_AccountAssociate[5+1];
	char szACCION_Authorize[5+1];
	char szACCION_PostingDate[5+1];
	char szACCION_Refresh[5+1];
	/* USER AND PASSWORD IF SQL/ODBC/ADO/RPC CONNECTION IS NEEDED */
	char szSQL_USER[255+1];
	char szSQL_PASS[255+1];


};

#ifndef ODBC_DATANET_DATABASE
#define ODBC_DATANET_DATABASE               "datanet"
#define ODBC_DATANET_USER                   "datanet"
#define ODBC_DATANET_USER_PASSWORD          "datanet"
#define _SQL_INSERT_REQR_SNTC_              ""
#define _SQL_SELECT_SERIAL_SNTC_            ""
#define _SQL_SELECT_RESPONSE_SNTC_          ""
#endif

#endif // RPC


