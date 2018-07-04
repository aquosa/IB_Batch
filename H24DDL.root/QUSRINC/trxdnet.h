///////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A. - COA S.A. - MARTIN D. CERNADAS
// Transaction Resolution Header DATANET
// 
// D.P.S. - DISTRIBUTED PROCESSING SYSTEM - Version 5.0
// D.P.S. - DISTRIBUTED PROCESSING SYSTEM - Version 5.1
//
//
//   Segun la implementacion, los metodos especializados puede ser "dummy", o sea un 
// simple retorno en falso sin procesamiento, sin generar el codigo ejecutable 
// actualmente,  con el solo motivo de mantener el esqueleto de codigo fuente 
// original y facilitar el posible enlace a futuro de las subclases especializadas 
// en una sola libreria o modulo.
//
// Tarea        Fecha           Autor   Observaciones
// (Alfa)       2003.03.21      mdc     INTERBANKING-DATANET, Banco Macro-Bansud.
// (Alfa)       2003.04.07      mdc     ::ValidateMac1, 2, 3 y similares
// (Alfa)       2003.09.29      mdc     ::RetrieveTransaction()
// (Alfa)       2003.10.02      mdc     ::Format_ISO_Ammount(...)
// (Alfa)       2003.10.20      mdc     ::LoggTransaction() agrega el parametro de ESTADO TRATAMIENTO
// (Alfa)       2003.10.21      mdc     ::ValidatePostingDate() agrega : (e_settlement_retcode *pesRc = NULL)
// (Beta)       2003.11.21      mdc     Nueva logica de INSERCION y LECTURA posterior
// (Beta)       2004.01.08      mdc     ::VerifySendFlag(e_settlement_retcode esReqDate,....)
// [Delta]      2004.02.19      mdc     ::RetrieveFirstRefreshMovement(...)
// [Delta]      2004.02.23      mdc     ::FormatOutputDataFromParams()
// [Delta]      2004.07.14      mdc     CUTOVER TIME
// [Delta]      2004.08.30      mdc     Refresh de Cobranzas : Parametriza tipo de registro
// [Release-2]  2005.07.27      mdc     Banco del Suquia, nuevas reglas del negocio y otros ejecutables.
// [Release-2]  2005.08.09      mdc     virtual WORD  EXPORT  AuthorizeTransaction( long *plExtRetStatus );
// [Release-2]  2005.10.21      mdc     EXPORT RetrieveDPIFCOMM(DPIFCOMM &refrExtMsg) { refrExtMsg = refrMsg; }
// [Release-2]  2005.11.15      mdc     UpdateACTION_PER_CONTROL(...)
// [Release-2]  2006.01.02      mdc     Banco Ciudad, plataforma ON2/CONTINUUS
// [Release-3]  2006.03.03      mdc     friend class DPS_i_batch;	TrxResBATCH ;
// [Release-3]  2006.05.01      mdc     RetrieveFirstRefreshMovement() PARA BATCH.
// [Release-3]  2006.05.17      mdc     PUBLIC EXPORT: Format_ISO_Ammount(const char *szAmmount, short shDecPos, ...)
// [Release-3]  2006.06.07      mdc     PCSTR TrxResBATCH::SearchInternalCodOpe( PCSTR szExtCodOpe, PCSTR szExtDescrip, PCSTR szExtCRDB...);
// 
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _TRXRESDATANET_H_
#define _TRXRESDATANET_H_

// Headers propios
// Header Type definitions
#include <qusrinc/typedefs.h>    
// Header DPS Application Message
#include <qusrinc/datanet.h>
// Header DPC System
#include <qusrinc/dpcsys.h>
// Aliases de palabras clave C/C++
#include <qusrinc/alias.h>
// TIME
#include <time.h>    
///////////////////////////////////////////////////////////////
// Header RPC-SYBASE_COBIS u ODBC-JERONIMO
#ifdef _RPCDB_CONNECTION_
#include <qusrinc/rpcdb.h>
#include <qusrinc/cobisdb.h>
#endif // RPCDB
#if defined( _SQL_ODBC_SENTENCES_ )
#include <qusrinc/odbcdnet.h>
#endif // ODBC
#if defined( _XML_CONNECTION_ )
#include <qusrinc/xmltrx.h>
#endif // XML
#if defined( _MOSAIC_ON2_CONNECTION_ ) || defined( _SYSTEM_DATANET_ON2_ )
#include <qusrinc/on2cont.h>
#endif // MOSAIC
#if defined( _SYSTEM_DATANET_TDFUEGO_ )
#include <qusrinc/rpclib.h>
#endif // RPC EXTRACT
///////////////////////////////////////////////////////////////

#if defined( _SYSTEM_ATM_ ) && (!defined (_SYSTEM_DATANET_) || !defined( _SYSTEM_DATANET_BATCH_ ) )
#error "Sistema definido es incorrecto para incluir este header"
#endif

///////////////////////////////////////////////////////////////
// DEBUG PRINT PARAMS 
#define _DPS_DEBUG_PRINT_PARAMS_					(TRUE)
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
// Controles definidos para hacer en cada transaccion
#define _DPS_CONTROL_MAC_123_						(TRUE)
#define _DPS_CONTROL_BANK_EXISTENCE_				(TRUE)
#define _DPS_CONTROL_ACCOUNT_EXISTENCE_				(FALSE)
#define _DPS_CONTROL_ACCOUNT_STATUS_				(FALSE) 
#define _DPS_CONTROL_ASSOCIATE_EXISTENCE_			(TRUE)
#define _DPS_CONTROL_POSTING_DATE_					(TRUE)
///////////////////////////////////////////////////////////////

// En ATM tambien hay una clase 'TrxResolution' por eso este
#ifdef _SYSTEM_ATM_
#define _TRX_BASE_RESOLUTION_	TrxBaseResolutionDNET
#else
#define _TRX_BASE_RESOLUTION_	TrxBaseResolution
#endif // _SYSTEM_ATM_

///////////////////////////////////////////////////////////////////////////////////////
// Clase Resolucion Base de Transaccion 
// Asimismo sirve de clase virtual para especializar las resoluciones (PAS,AFJP...)
class _TRX_BASE_RESOLUTION_
    {
    public:

		// Metodos publicos de la Instancia
		// Constructor y destructor
		EXPORT _TRX_BASE_RESOLUTION_(void);      // Constructor x default
		EXPORT ~_TRX_BASE_RESOLUTION_(void);     // Destructor

		// Para setear en modo ON y OFF las transacciones entrantes :
		virtual BOOL EXPORT SetOnOffLine(boolean_t boolOnlineMode);	 // Modo offline-online en la trx?
		// Para loggear errores internos, se disponibilizan estos metodos publicas:
		virtual WORD  EXPORT TransactionStatus( void );
		virtual short EXPORT BackEndAuthStatus(void);

	protected:      

		virtual WORD  EXPORT ProcessInit(void);	// Proceso base, validacion
        // Metodos de formateo de campos especificos
        BOOL  EXPORT FormatTrxDateTime_WMask(char *szYYYYMMDD, char *szHHMMSS,    
                                    char *szYYYYMMDD_Trx, char *szHHMMSS_Trx,
                                    boolean_t bUseSeparators ,
                                    boolean_t bUse4DigitsYear,
                                    boolean_t bFormatIsMMDDYY,
                                    boolean_t bAlwaysTimeIsDateAndTime,
                                    short     nTimestampLength,                                    
                                    short     nHourLength,    
                                    short     nOptionalInputYearDigits = 0);

	public:      

		// Formatea segun PICTURE "ISO" , COMA IMPLICITA EJ. PIC 9(12)V99.
        SHORT EXPORT Format_ISO_Ammount(const char *szAmmount, short shDecPos, 
                                 char *szNew, short shAlignWidth,
                                 boolean_t bApplyChangeRate ,
                                 char *szExchangeRate);
	protected:      


		// Mensaje de Aplicacion DPS entrante y saliente ("DPRE")
		DPS_in_out  dpsMsg;
		// Mensaje de Aplicacion DPS / BACKUP
		DPS_in_out  dpsMsgBkp;        
        // Mensaje de Aplicacion REFRESH ("DPIF")
        DPIFCOMM    refrMsg;

		// Variables protegidas de la Instancia
		boolean_t	   boolOnlineMode;			// ONLINE ? u OFFLINE ?
		//////////////////////////////////////////////////////////////////
		boolean_t	   bProcResult;			    // Process Result
		WORD		   wProcRespCde;			// Process Response Code (ISO o no)		
		enumFormatType efFormatMsg;				// Formato de mensaje ISO o no
		/////////////////////////////////////////////////////////////////////////
		short		   shAuthRespCde;			// Auth. Response Code (internal)
		/////////////////////////////////////////////////////////////////////////

    };//end-TrxBaseResolution

///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transaccion DATANET
// Implementada sobre clase base virtual
class TrxResDATANET: virtual public _TRX_BASE_RESOLUTION_
    {
    public:

		// Constructor
		EXPORT TrxResDATANET(void); 
		// Procesamiento de transacciones DATANET
		boolean_t EXPORT ProcessTransaction(void);
		// Estado de la ultima ejecucion RPC/ODBC
		boolean_t EXPORT TransactionExecutionStatus();
        // Establecer hora de corte
        boolean_t EXPORT SetCutoverTime( struct tm *pstCutoverTime ) ;
        // Es la hora de corte ya ?
        boolean_t EXPORT IsCutoverTimeElapsed( void ) ; // Hora de Corte

	protected:      
		
		// Procesamiento de transacciones especificas
		virtual WORD  EXPORT  ProcessAccountVerification(WORD wCause,boolean_t bIsAuth);// Verificacion cuenta destino
		virtual WORD  EXPORT  ProcessOtherMessages(WORD wCause,boolean_t bIsAuth);		// Otros mensajes
		virtual WORD  EXPORT  ProcessCreditNote(WORD wCause,boolean_t bIsAuth);			// Nota de credito
		virtual WORD  EXPORT  ProcessDebitNote(WORD wCause,boolean_t bIsAuth);			// Nota de debito
		virtual WORD  EXPORT  ProcessCreditDebitNote(WORD wCause,boolean_t bIsAuth);	// Nota de credito y credito asociada
		virtual WORD  EXPORT  ProcessReversal(WORD wCause,boolean_t bIsAuth);   		// Reverso generico
		virtual WORD  EXPORT  AuthorizeTransaction();									// Autorizar 1er transaccion disponible
        virtual WORD  EXPORT  AuthorizeTransaction( long *plExtRetStatus );
		virtual void  EXPORT  ResetStatus();
		virtual WORD  EXPORT  LoggTransaction(char *szESTADOTRAT,			            // Loggear transaccion determinada
                                              char *szPENDENVIO = NULL );
        virtual WORD  EXPORT  RetrieveTransaction();                                    // Recuperar 1er trx disponible
		// Controles de trasacciones en general
        virtual WORD  EXPORT ProcessInit(boolean_t bIsAuth);	// Proceso base, validacion		
		virtual WORD  EXPORT ProcessInit(boolean_t bIsAuth, PBYTE pMAC_Used);	// Proceso base, validacion		
		virtual WORD  EXPORT ValidateMAC(char *szMACTYPE);// Verificacion cuenta y MAC-n
		virtual WORD  EXPORT CalculateMAC(char *szMACTYPE, char*szExtMAC);// Calcular MAC 2 o 3
		virtual WORD  EXPORT ValidateMAC1();              // Verificacion cuenta y MAC1
		virtual WORD  EXPORT ValidateMAC2();              // Verificacion cuenta y MAC2
		virtual WORD  EXPORT ValidateMAC3();              // Verificacion cuenta y MAC3
		virtual WORD  EXPORT ValidateExistingAccount();   // Verificar cuenta existente
		virtual WORD  EXPORT ValidateAccountStatus(char *pszEstado);  // Verificar estado de la cuenta
		virtual WORD  EXPORT ValidateAccountAssociate();  // Verificar abonado de la cuenta
        // Verificacion de fecha de procesamiento versus hoy y fecha de estado
		virtual WORD  EXPORT ValidatePostingDate(e_settlement_retcode *pesRcToday ,
                                                 e_settlement_retcode *pesRcStatus,
                                                 boolean_t bValidate_CR_DB = is_false);  
        // Verificacion de marca de envio en funcion de las fechas solicitud / envio
        virtual WORD  EXPORT VerifySendFlag(e_settlement_retcode esReqDate,
                                            e_settlement_retcode esSendDate,
                                            boolean_t bESTADO_VALOR_AL_COBRO);   

        // Algoritmos base de calculo de MAC-1-2-3
		WORD  EXPORT ValidateMAC(char *szMACTYPE,UCHAR uchAsocKEY[], UCHAR uchBankKEY[]);// Verificacion cuenta y MAC-n
		WORD  EXPORT CalculateMAC(char *szMACTYPE, char*szExtMAC,UCHAR uchAsocKEY[], UCHAR uchBankKEY[]);// Calcular MAC 2 o 3

        /////////////////////////////////////////////////////////////////////
		// Estado de la ultima ejecucion de transaccion c/b. de datos
		boolean_t	bExecStatus_ok;
        // Estado Pendiente de ENVIO segun van ejecutandose los diversos controles
        char        szPENDENVIO[2];
        // Hora de Corte
        struct tm stCutoverTime;
        /////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////
		friend class DPS_i_batch;
		/////////////////////////////////////////////////////////////////////

    };//end-TrxResDATANET


///////////////////////////////////////
#if defined( _SYSTEM_DATANET_COBIS_ )
///////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transaccion DATANET
// Implementada sobre clase base virtual
class TrxResCOBIS: virtual public TrxResDATANET
    {
    public:

		// Constructor
		EXPORT TrxResCOBIS(void); 
		EXPORT TrxResCOBIS(cobisdb *prpcDB);

        // Autorizar primer transaccion disponible, y retornar RESP-CDE
        boolean_t EXPORT AuthorizeFirstTransaction(long *plRetStatus);	
        // Recuperar primer transaccion disponible
        boolean_t EXPORT RetrieveFirstTransaction(char *lpData, 
                                                  unsigned short *pnLen,
                                                  char *szTStatus,
                                                  char * lpDataBkp, 
                                                  unsigned short *pnLenBkp);
        // Actualizar transaccion
        boolean_t EXPORT UpdateTransaction( char *pszTxt , size_t nLen );
        // Recuperar cuerpo de mensaje de notificacion
        LPCSTR EXPORT GetMSG_Mail( char *pszMsg , short *pshQSends, char *pszDestiny);
        ////////////////////////////////////////////////////////////////////
        // Recuperar primer movimiento de refresh disponible
        boolean_t EXPORT RetrieveFirstRefreshMovement(char *lpData, 
                                    unsigned short *pnLen,                                                  
                                    char * lpDataBkp, 
                                    unsigned short *pnLenBkp,
                                    const char cszRECTYPE[] ,
                                    long *plSecCC1 ,    //Secuencial rango desde CC, output
                                    long *plSecCC2 ,    //Secuencial rango hasta CC, output
                                    long *plSecCA1 ,    //Secuencial rango desde CA, output
                                    long *plSecCA2 ) ;  //Secuencial rango hasta CA, output
        ////////////////////////////////////////////////////////////////////

	protected:      
		
		// Validacion de trasacciones
		WORD  EXPORT ProcessInit(boolean_t bIsAuth);	// Proceso base, validacion		
		WORD  EXPORT ValidateMAC(char *szMACTYPE);// Verificacion cuenta y MAC-n
		WORD  EXPORT CalculateMAC(char *szMACTYPE, char*szExtMAC);// Calcular MAC 2 o 3
		WORD  EXPORT ValidateExistingAccount();   // Verificar cuenta existente
		WORD  EXPORT ValidateAccountStatus(char *pszEstado);  // Verificar estado de la cuenta
		WORD  EXPORT ValidateAccountAssociate();  // Verificar abonado de la cuenta
		WORD  EXPORT LoggTransaction(char *szESTADOTRAT, // Loggear Transaccion en TEF ONLINE
                                     char *szPENDENVIO = NULL );
        WORD  EXPORT RetrieveTransaction( char *szTStatus ); // Recuperar 1er Trx
		WORD  EXPORT AuthorizeTransaction(long *plExtRetStatus ); // Autorizar transaccion determinada (debito-credito/rechazo)
        WORD  EXPORT RetrieveRefreshMovement(long *plSecCC1,    //Secuencial rango desde CC, output
                            long *plSecCC2,    //Secuencial rango hasta CC, output
                            long *plSecCA1,    //Secuencial rango desde CA, output
                            long *plSecCA2,   //Secuencial rango hasta CA, output
                            /*******************************************************/
                            const char cszRECTYPE[] ); // Tipo de registro
                            /*******************************************************/

    private:

        WORD  MapCOBISErrors( long lRetStatus ); // Mapeo de errores COBIS
        // Formatear mensaje de entrada desde los parametros  DPRE DPS
        void  FormatInputDataFromParams( 
	            char * szFRNAME ,	char * szTONAME ,
	            char * szRECTYPE,   char * chACTIVITY,
                char * szFILLER1,	char * szBANDEB ,
	            char * szFECSOL ,   char * szNUMTRA ,
	            char * szNUMABO ,   char * szTIPOPE ,
	            char * szIMPORTE,   char * szSUCDEB ,
	            char * szNOMSOL ,   char * szTIPCUEDEB,
	            char * szNCUECMDEB ,char * szNUMCTADEB ,
	            char * szFSENDB ,   char * szHSENDB ,
	            char * szOPEDB1 ,   char * szOPEDB2 ,
	            char * szRECHDB ,   char * szBANCRE ,
	            char * szSUCCRE ,   char * szNOMBEN,
	            char * szTIPCRE ,   char * szCTACRE,
	            char * szFSENCR,    char * szHSENCR,
	            char * szOPECR1,    char * szOPECR2,
	            char * szRECHCR,    char * szOPECON,
	            char * szOPEAU1,    char * szOPEAU2,
	            char * szOPEAU3,    char * szFECAUT,
	            char * szHORAUT,    char * szESTADO,
	            char * szFECEST,    char * szOBSER1,
	            char * szOBSER2,    char * szCODMAC,
	            char * szNUMREF,    char * szNUMENV,
                char * chCONSOL,    char * chMARTIT,
                char * chPRIVEZ,    char * chDIFCRE,
                char * chRIEABO,    char * chRIEBCO );
        // Formatear parametros desde el mensaje de entrada DPRE DPS
        void  FormatParamsFromInputData( 
	            char * szFRNAME ,	char * szTONAME ,
	            char * szRECTYPE,   char * chACTIVITY,
                char * szFILLER1,	char * szBANDEB ,
	            char * szFECSOL ,   char * szNUMTRA ,
	            char * szNUMABO ,   char * szTIPOPE ,
	            char * szIMPORTE,   char * szSUCDEB ,
	            char * szNOMSOL ,   char * szTIPCUEDEB,
	            char * szNCUECMDEB ,char * szNUMCTADEB ,
	            char * szFSENDB ,   char * szHSENDB ,
	            char * szOPEDB1 ,   char * szOPEDB2 ,
	            char * szRECHDB ,   char * szBANCRE ,
	            char * szSUCCRE ,   char * szNOMBEN,
	            char * szTIPCRE ,   char * szCTACRE,
	            char * szFSENCR,    char * szHSENCR,
	            char * szOPECR1,    char * szOPECR2,
	            char * szRECHCR,    char * szOPECON,
	            char * szOPEAU1,    char * szOPEAU2,
	            char * szOPEAU3,    char * szFECAUT,
	            char * szHORAUT,    char * szESTADO,
	            char * szFECEST,    char * szOBSER1,
	            char * szOBSER2,    char * szCODMAC,
	            char * szNUMREF,    char * szNUMENV,
                char * chCONSOL,    char * chMARTIT,
                char * chPRIVEZ,    char * chDIFCRE,
                char * chRIEABO,    char * chRIEBCO );
        // Formatear mensaje de salida desde los parametros (REFRESH ONLINE)
        void  FormatOutputDataFromParams( 
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
	            char szSURCURSAL[],//Sucursal, CHAR(3)
	            char szFECPRO[],   //Fecha de proceso en el Banco -Aaaammdd-, CHAR(8)
	            char chCONTRA[],   //Contraaseinto('1'/'0')	, CHAR(1)
	            char szTIPCUE[],   //tipo de cuenta	- CHAR(2)
	            char szDEPOSITO[],  //DEPOSITO, VARCHAR(8)
	            char szDESCRIP[],  //Texto libre , VARCHAR(25)
	            char chCODPRO[] , //Codigo de Proceso	, CHAR(1)
                const char cszRECTYPE[], // Tipo de Registro
                /*******************************************************/
                char szNUMCTAEDIT[], //Nro. Cta. editado , CHAR(23)
                char szMONEDA[]    , //Codigo de Moneda, CHAR(3)
                char szTIPOMOV[]   , //Tipo de Movimiento , CHAR(1)
                char szCLEARING[]  , //Clearing, CHAR(1)
	            char szNROCHEQUE[] , //Nro. Cheque, CHAR(4)
	            char szCODCLI[]    , //Codigo Cliente, CHAR(10)
	            char szNOMCLI[]    , //Codigo Cliente, CHAR(24)
	            char szCOMPROBANTE[],//Comprobante-Factura-Recibo, CHAR(16)
	            char szBOLETA[]     ,//BOLETA, VARCHAR(9)
                char szVALCOBRO[]   ,//Valor al Cobro, CHAR(1)
	            char szCOBRANZA[]    //Cobranza , VARCHAR(9)
                );
                /*******************************************************/

		// Instancia de conexion via RPC a SYBASE_COBIS
		cobisdb			*prpcDB;
        ////////////////////////////////////////////
        // Cuerpo del mensaje de notificacion
        char            szMSG_Mail[2048];
        // Contador de notificaciones
        short           shMSG_Counter;
        // Destinatario del email de notificacion
        char            szMSG_Destiny[512];
        ////////////////////////////////////////////



    };//end-TrxResCOBIS

typedef class TrxResCOBIS TrxResSpecialized  ;

// VALORES INTERNOS DE ERRORES DEL AUTORIZADOR DEL HOST, DEPENDE DE C/IMPLANTACION
#if defined( _SYSTEM_DATANET_COBIS_ )
#define _COBIS_ERROR_BLOQUEO_EXCEDE_SALDO_PARA_GIRAR_   (201072)
#define _COBIS_ERROR_SISTEMA_CONGESTIONADO_             (40002)
#define _COBIS_PRODUCTO_BANCARIO_DESHABILITADO_         (40003)
#define _COBIS_ERROR_TIEMPO_EXCEDIDO_                   (30002)
// MACRO DE BLOQUEOS VARIOS
#define _IS_COBIS_ERROR_BLOQUEOS_VARIOS_(x)           \
 (258018==x || 258017==x || 258016==x || 251058==x || \
  251025==x || 251007==x || 201085==x || 201008==x )

#endif // _SYSTEM_DATANET_COBIS_
// VALORES INTERNOS DE ERRORES DEL AUTORIZADOR DEL HOST, GENERICOS, POR OMISION
#define _DPS_ERROR_BLOQUEO_EXCEDE_SALDO_PARA_GIRAR_   _COBIS_ERROR_BLOQUEO_EXCEDE_SALDO_PARA_GIRAR_
#define _DPS_ERROR_SISTEMA_CONGESTIONADO_             _COBIS_ERROR_SISTEMA_CONGESTIONADO_
#define _DPS_ERROR_TIEMPO_EXCEDIDO_                   _COBIS_ERROR_TIEMPO_EXCEDIDO_
#define _DPS_COBIS_ERROR_BLOQUEO_DEPOSITO_Y_RETIRO_   _COBIS_ERROR_BLOQUEO_DEPOSITO_Y_RETIRO_       
#define _DPS_COBIS_ERROR_BLOQUEO_RETIRO_              _COBIS_ERROR_BLOQUEO_RETIRO_                  
#define _DPS_COBIS_ERROR_BLOQUEO_DEPOSITO_            _COBIS_ERROR_BLOQUEO_DEPOSITO_                
#define _IS_DPS_ERROR_BLOQUEOS_VARIOS_                _IS_COBIS_ERROR_BLOQUEOS_VARIOS_                
//////////////////////////////////////////////////////////////////////////////

#endif // _SYSTEM_DATANET_COBIS_


///////////////////////////////////////
#if defined( _SYSTEM_DATANET_JERONIMO_ )
///////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transaccion DATANET
// Implementada sobre clase base virtual
class TrxResJERONIMO: virtual public TrxResDATANET
    {
    public:

		// Constructor
		EXPORT TrxResJERONIMO(void); 
        EXPORT TrxResJERONIMO(void *ptr_xml);
		EXPORT TrxResJERONIMO(XML_CONNECTOR_DNET *ptr_xml);

        // Autorizar primer transaccion disponible, y retornar RESP-CDE
        boolean_t EXPORT AuthorizeFirstTransaction(long *plRetStatus);	
        // Recuperar primer transaccion disponible
        boolean_t EXPORT RetrieveFirstTransaction(char *lpData, 
                                                  unsigned short *pnLen,
                                                  char *szTStatus,
                                                  char * lpDataBkp, 
                                                  unsigned short *pnLenBkp);
        // Actualizar transaccion
        boolean_t EXPORT UpdateTransaction( char *pszTxt , size_t nLen );
        // Recuperar cuerpo de mensaje de notificacion
        LPCSTR EXPORT GetMSG_Mail( char *pszMsg , short *pshQSends, char *pszDestiny);
        ////////////////////////////////////////////////////////////////////
        // Recuperar primer movimiento de refresh disponible
        boolean_t EXPORT RetrieveFirstRefreshMovement(char *lpData, 
                                    unsigned short *pnLen,                                                  
                                    char * lpDataBkp, 
                                    unsigned short *pnLenBkp,
                                    const char cszRECTYPE[] ,
                                    char *szSTAMP ) ;  // Stamp PK
        ////////////////////////////////////////////////////////////////////

        // Recuperar Mensaje de Aplicacion REFRESH ("DPIF") hacia programa externo
        EXPORT RetrieveDPIFCOMM(DPIFCOMM *prefrExtMsg) 
			{ if(prefrExtMsg) (*prefrExtMsg) = refrMsg; }
		// Actualizar ultima ACCION segun metodo de control 
		EXPORT void UpdateACTION_PER_CONTROL( char szACCION_IND[] );
		// Recuperar ultimo codigo de resultado
		EXPORT WORD GetRespCde( ) { return wRespCde; }




	protected:      
		
		// Validacion de trasacciones
		WORD  EXPORT ProcessInit(boolean_t bIsAuth);	// Proceso base, validacion		
		WORD  EXPORT ValidateMAC(char *szMACTYPE);// Verificacion cuenta y MAC-n
		WORD  EXPORT CalculateMAC(char *szMACTYPE, char*szExtMAC);// Calcular MAC 2 o 3
		WORD  EXPORT ValidateExistingAccount();   // Verificar cuenta existente
		WORD  EXPORT ValidateAccountStatus(char *pszEstado);  // Verificar estado de la cuenta
		WORD  EXPORT ValidateAccountAssociate();  // Verificar abonado de la cuenta
		WORD  EXPORT LoggTransaction(char *szESTADOTRAT, // Loggear Transaccion en TEF ONLINE
                                     char *szPENDENVIO = NULL );
        WORD  EXPORT RetrieveTransaction( char *szTStatus ); // Recuperar 1er Trx
		WORD  EXPORT AuthorizeTransaction(long *plExtRetStatus ); // Autorizar transaccion determinada (debito-credito/rechazo)
        WORD  EXPORT RetrieveRefreshMovement(char *szSTAMP,             // STAMP - PK
										     const char cszRECTYPE[] ); // Tipo de registro                            

    private:

        WORD  MapJERONIMOErrors( long lRetStatus, WORD wRespCde ); // Mapeo de errores COBIS
        // Formatear mensaje de entrada desde los parametros  DPRE DPS
        void  FormatInputDataFromParams( 
	            char * szFRNAME ,	char * szTONAME ,
	            char * szRECTYPE,   char * chACTIVITY,
                char * szFILLER1,	char * szBANDEB ,
	            char * szFECSOL ,   char * szNUMTRA ,
	            char * szNUMABO ,   char * szTIPOPE ,
	            char * szIMPORTE,   char * szSUCDEB ,
	            char * szNOMSOL ,   char * szTIPCUEDEB,
	            char * szNCUECMDEB ,char * szNUMCTADEB ,
	            char * szFSENDB ,   char * szHSENDB ,
	            char * szOPEDB1 ,   char * szOPEDB2 ,
	            char * szRECHDB ,   char * szBANCRE ,
	            char * szSUCCRE ,   char * szNOMBEN,
	            char * szTIPCRE ,   char * szCTACRE,
	            char * szFSENCR,    char * szHSENCR,
	            char * szOPECR1,    char * szOPECR2,
	            char * szRECHCR,    char * szOPECON,
	            char * szOPEAU1,    char * szOPEAU2,
	            char * szOPEAU3,    char * szFECAUT,
	            char * szHORAUT,    char * szESTADO,
	            char * szFECEST,    char * szOBSER1,
	            char * szOBSER2,    char * szCODMAC,
	            char * szNUMREF,    char * szNUMENV,
                char * chCONSOL,    char * chMARTIT,
                char * chPRIVEZ,    char * chDIFCRE,
                char * chRIEABO,    char * chRIEBCO );
        // Formatear parametros desde el mensaje de entrada DPRE DPS
        void  FormatParamsFromInputData( 
	            char * szFRNAME ,	char * szTONAME ,
	            char * szRECTYPE,   char * chACTIVITY,
                char * szFILLER1,	char * szBANDEB ,
	            char * szFECSOL ,   char * szNUMTRA ,
	            char * szNUMABO ,   char * szTIPOPE ,
	            char * szIMPORTE,   char * szSUCDEB ,
	            char * szNOMSOL ,   char * szTIPCUEDEB,
	            char * szNCUECMDEB ,char * szNUMCTADEB ,
	            char * szFSENDB ,   char * szHSENDB ,
	            char * szOPEDB1 ,   char * szOPEDB2 ,
	            char * szRECHDB ,   char * szBANCRE ,
	            char * szSUCCRE ,   char * szNOMBEN,
	            char * szTIPCRE ,   char * szCTACRE,
	            char * szFSENCR,    char * szHSENCR,
	            char * szOPECR1,    char * szOPECR2,
	            char * szRECHCR,    char * szOPECON,
	            char * szOPEAU1,    char * szOPEAU2,
	            char * szOPEAU3,    char * szFECAUT,
	            char * szHORAUT,    char * szESTADO,
	            char * szFECEST,    char * szOBSER1,
	            char * szOBSER2,    char * szCODMAC,
	            char * szNUMREF,    char * szNUMENV,
                char * chCONSOL,    char * chMARTIT,
                char * chPRIVEZ,    char * chDIFCRE,
                char * chRIEABO,    char * chRIEBCO );

	public : 
        // Formatear mensaje de salida desde los parametros (REFRESH ONLINE)
        EXPORT void  FormatOutputDataFromParams( 
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
	            char szSURCURSAL[],//Sucursal, CHAR(3)
	            char szFECPRO[],   //Fecha de proceso en el Banco -Aaaammdd-, CHAR(8)
	            char chCONTRA[],   //Contraaseinto('1'/'0')	, CHAR(1)
	            char szTIPCUE[],   //tipo de cuenta	- CHAR(2)
	            char szDEPOSITO[],  //DEPOSITO, VARCHAR(8)
	            char szDESCRIP[],  //Texto libre , VARCHAR(25)
	            char chCODPRO[] , //Codigo de Proceso	, CHAR(1)
                const char cszRECTYPE[], // Tipo de Registro
                /*******************************************************/
                char szNUMCTAEDIT[], //Nro. Cta. editado , CHAR(23)
                char szMONEDA[]    , //Codigo de Moneda, CHAR(3)
                char szTIPOMOV[]   , //Tipo de Movimiento , CHAR(1)
                char szCLEARING[]  , //Clearing, CHAR(1)
	            char szNROCHEQUE[] , //Nro. Cheque, CHAR(4)
	            char szCODCLI[]    , //Codigo Cliente, CHAR(10)
	            char szNOMCLI[]    , //Codigo Cliente, CHAR(24)
	            char szCOMPROBANTE[],//Comprobante-Factura-Recibo, CHAR(16)
	            char szBOLETA[]     ,//BOLETA, VARCHAR(9)
                char szVALCOBRO[]   ,//Valor al Cobro, CHAR(1)
	            char szCOBRANZA[]    //Cobranza , VARCHAR(9)
                );
                /*******************************************************/

	private:
		// Instancia de conexion via XML/ODBC/RPC a Jeronimo
		XML_CONNECTOR_DNET			*ptrXML;
        ////////////////////////////////////////////
        // Cuerpo del mensaje de notificacion
        char            szMSG_Mail[2048];
        // Contador de notificaciones
        short           shMSG_Counter;
        // Destinatario del email de notificacion
        char            szMSG_Destiny[512];
		// Ultimo codigo de resultado
		WORD			wRespCde;        
        ////////////////////////////////////////////



    };//end-TrxResJERONIMO

typedef class TrxResJERONIMO TrxResSpecialized  ;

/* DNET-Jeronimo ACTION VALUES  */
#define	_DPS_ACTION_NO_ERROR_IN_AUTH_     (DNET_ACTION_NO_ERROR_IN_AUTH	- DNET_ACTION_NO_ERROR)
#define	_DPS_ACTION_RETRY_BY_OFFICER_     (DNET_ACTION_RETRY_BY_OFFICER - DNET_ACTION_NO_ERROR)
#define	_DPS_ACTION_DENIED_BY_OFFICER_    (DNET_ACTION_DENIED_BY_OFFICER - DNET_ACTION_NO_ERROR)
#define	_DPS_ACTION_OFFICER_REQUIRED_     (DNET_ACTION_OFFICER_REQUIRED	- DNET_ACTION_NO_ERROR)
#define	_DPS_ACTION_APPROVED_BY_OFFICER_  (DNET_ACTION_APPROVED_BY_OFFICER - DNET_ACTION_NO_ERROR)
#define	_DPS_ACTION_AUTH_BATCH_PROCESS_   (DNET_ACTION_AUTH_BATCH_PROCESS - DNET_ACTION_NO_ERROR)
#define	_DPS_ACTIONS_RETRY_BY_OFFICER_(x)  ((DNET_ACTION_RETRY_BY_OFFICER - DNET_ACTION_NO_ERROR) == x || \
											(DNET_ACTION_RETRY_BY_OFFICER_CR - DNET_ACTION_NO_ERROR) == x)
#define	_DPS_ACTIONS_AUTH_BATCH_PROCESS_(x) ((DNET_ACTION_AUTH_BATCH_PROCESS - DNET_ACTION_NO_ERROR) == x || \
											 (DNET_ACTION_AUTH_BATCH_PROCESS_CR - DNET_ACTION_NO_ERROR) == x)
#define	_DPS_ACTIONS_DENIED_BY_OFFICER_(x)  ((DNET_ACTION_DENIED_BY_OFFICER - DNET_ACTION_NO_ERROR) == x || \
											 (DNET_ACTION_DENIED_BY_OFFICER_CR - DNET_ACTION_NO_ERROR) == x)


// VALORES INTERNOS DE ERRORES DEL AUTORIZADOR DEL HOST, GENERICOS, POR OMISION
#define _DPS_ERROR_BLOQUEO_EXCEDE_SALDO_PARA_GIRAR_  (DNET_ACTION_OFFICER_REQUIRED)
#define _IS_DPS_ERROR_BLOQUEO_CUENTA_                (DNET_ACTION_OFFICER_REQUIRED)
//////////////////////////////////////////////////////////////////////////////


#endif // _SYSTEM_DATANET_JERONIMO_

///////////////////////////////////////////////////////////////////////////////////
#if defined( _SYSTEM_DATANET_ON2_ )

///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transaccion DATANET
// Implementada sobre clase base virtual
class TrxResON2 : virtual public TrxResDATANET
    {
    public:

		// Constructor
		EXPORT TrxResON2( void );
        EXPORT TrxResON2( void *ptr_backoffice );
		EXPORT TrxResON2( ON2_CONTINUUS_CONNECTOR_DNET *ptr_backoffice );

        // Autorizar primer transaccion disponible, y retornar RESP-CDE
        boolean_t EXPORT AuthorizeFirstTransaction(long *plRetStatus);	
        // Recuperar primer transaccion disponible
        boolean_t EXPORT RetrieveFirstTransaction(char           *lpData, 
                                                  unsigned short *pnLen,
                                                  char           *szTStatus,
                                                  char           *lpDataBkp, 
                                                  unsigned short *pnLenBkp);
        // Actualizar transaccion
        boolean_t EXPORT UpdateTransaction( char *pszTxt , size_t nLen );
        // Recuperar cuerpo de mensaje de notificacion
        LPCSTR EXPORT GetMSG_Mail( char *pszMsg , short *pshQSends, char *pszDestiny);
        ////////////////////////////////////////////////////////////////////
        // Recuperar primer movimiento de refresh disponible
        boolean_t EXPORT RetrieveFirstRefreshMovement(char *lpData, 
                                            unsigned short *pnLen, 
                                            char           *lpDataBkp, 
                                            unsigned short *pnLenBkp,
                                            const char      cszRECTYPE[] ,
                                            char           *szSTAMP ) ;  // Stamp PK
        ////////////////////////////////////////////////////////////////////

        // Recuperar Mensaje de Aplicacion REFRESH ("DPIF") hacia programa externo
        EXPORT RetrieveDPIFCOMM(DPIFCOMM &refrExtMsg) { refrExtMsg = refrMsg; }



	protected:      
		
		// Validacion de trasacciones
		WORD  EXPORT ProcessInit(boolean_t bIsAuth);	// Proceso base, validacion		
		WORD  EXPORT ValidateMAC(char *szMACTYPE);// Verificacion cuenta y MAC-n
		WORD  EXPORT CalculateMAC(char *szMACTYPE, char*szExtMAC);// Calcular MAC 2 o 3
		WORD  EXPORT ValidateExistingAccount();   // Verificar cuenta existente
		WORD  EXPORT ValidateAccountStatus(char *pszEstado);  // Verificar estado de la cuenta
		WORD  EXPORT ValidateAccountAssociate();  // Verificar abonado de la cuenta
		WORD  EXPORT LoggTransaction(char *szESTADOTRAT, // Loggear Transaccion en TEF ONLINE
                                     char *szPENDENVIO = NULL );
        WORD  EXPORT RetrieveTransaction( char *szTStatus ); // Recuperar 1er Trx
		WORD  EXPORT AuthorizeTransaction(long *plExtRetStatus ); // Autorizar transaccion determinada (debito-credito/rechazo)
        WORD  EXPORT RetrieveRefreshMovement(char *szSTAMP,             // STAMP - PK
										     const char cszRECTYPE[] ); // Tipo de registro                            

    private:

        WORD  MapON2Errors( long lRetStatus ); // Mapeo de errores ON2/CONTINUUS - MOSAIC

        // Formatear mensaje de entrada desde los parametros  DPRE DPS
        void  FormatInputDataFromParams( 
	            char * szFRNAME ,	char * szTONAME ,
	            char * szRECTYPE,   char * chACTIVITY,
                char * szFILLER1,	char * szBANDEB ,
	            char * szFECSOL ,   char * szNUMTRA ,
	            char * szNUMABO ,   char * szTIPOPE ,
	            char * szIMPORTE,   char * szSUCDEB ,
	            char * szNOMSOL ,   char * szTIPCUEDEB,
	            char * szNCUECMDEB ,char * szNUMCTADEB ,
	            char * szFSENDB ,   char * szHSENDB ,
	            char * szOPEDB1 ,   char * szOPEDB2 ,
	            char * szRECHDB ,   char * szBANCRE ,
	            char * szSUCCRE ,   char * szNOMBEN,
	            char * szTIPCRE ,   char * szCTACRE,
	            char * szFSENCR,    char * szHSENCR,
	            char * szOPECR1,    char * szOPECR2,
	            char * szRECHCR,    char * szOPECON,
	            char * szOPEAU1,    char * szOPEAU2,
	            char * szOPEAU3,    char * szFECAUT,
	            char * szHORAUT,    char * szESTADO,
	            char * szFECEST,    char * szOBSER1,
	            char * szOBSER2,    char * szCODMAC,
	            char * szNUMREF,    char * szNUMENV,
                char * chCONSOL,    char * chMARTIT,
                char * chPRIVEZ,    char * chDIFCRE,
                char * chRIEABO,    char * chRIEBCO );
        // Formatear parametros desde el mensaje de entrada DPRE DPS
        void  FormatParamsFromInputData( 
	            char * szFRNAME ,	char * szTONAME ,
	            char * szRECTYPE,   char * chACTIVITY,
                char * szFILLER1,	char * szBANDEB ,
	            char * szFECSOL ,   char * szNUMTRA ,
	            char * szNUMABO ,   char * szTIPOPE ,
	            char * szIMPORTE,   char * szSUCDEB ,
	            char * szNOMSOL ,   char * szTIPCUEDEB,
	            char * szNCUECMDEB ,char * szNUMCTADEB ,
	            char * szFSENDB ,   char * szHSENDB ,
	            char * szOPEDB1 ,   char * szOPEDB2 ,
	            char * szRECHDB ,   char * szBANCRE ,
	            char * szSUCCRE ,   char * szNOMBEN,
	            char * szTIPCRE ,   char * szCTACRE,
	            char * szFSENCR,    char * szHSENCR,
	            char * szOPECR1,    char * szOPECR2,
	            char * szRECHCR,    char * szOPECON,
	            char * szOPEAU1,    char * szOPEAU2,
	            char * szOPEAU3,    char * szFECAUT,
	            char * szHORAUT,    char * szESTADO,
	            char * szFECEST,    char * szOBSER1,
	            char * szOBSER2,    char * szCODMAC,
	            char * szNUMREF,    char * szNUMENV,
                char * chCONSOL,    char * chMARTIT,
                char * chPRIVEZ,    char * chDIFCRE,
                char * chRIEABO,    char * chRIEBCO );

	public : 

        // Formatear mensaje de salida desde los parametros (REFRESH ONLINE)
        EXPORT void  FormatOutputDataFromParams( 
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
	            char szSURCURSAL[],//Sucursal, CHAR(3)
	            char szFECPRO[],   //Fecha de proceso en el Banco -Aaaammdd-, CHAR(8)
	            char chCONTRA[],   //Contraaseinto('1'/'0')	, CHAR(1)
	            char szTIPCUE[],   //tipo de cuenta	- CHAR(2)
	            char szDEPOSITO[],  //DEPOSITO, VARCHAR(8)
	            char szDESCRIP[],  //Texto libre , VARCHAR(25)
	            char chCODPRO[] , //Codigo de Proceso	, CHAR(1)
                const char cszRECTYPE[], // Tipo de Registro
                /*******************************************************/
                char szNUMCTAEDIT[], //Nro. Cta. editado , CHAR(23)
                char szMONEDA[]    , //Codigo de Moneda, CHAR(3)
                char szTIPOMOV[]   , //Tipo de Movimiento , CHAR(1)
                char szCLEARING[]  , //Clearing, CHAR(1)
	            char szNROCHEQUE[] , //Nro. Cheque, CHAR(4)
	            char szCODCLI[]    , //Codigo Cliente, CHAR(10)
	            char szNOMCLI[]    , //Codigo Cliente, CHAR(24)
	            char szCOMPROBANTE[],//Comprobante-Factura-Recibo, CHAR(16)
	            char szBOLETA[]     ,//BOLETA, VARCHAR(9)
                char szVALCOBRO[]   ,//Valor al Cobro, CHAR(1)
	            char szCOBRANZA[]    //Cobranza , VARCHAR(9)
                );
                /*******************************************************/

	private:

		// Instancia de conexion via XML/ODBC/RPC/TCPIP a ON2 CONTINUUS (backoffice)
		ON2_CONTINUUS_CONNECTOR_DNET *ptrBackOffice;
        ////////////////////////////////////////////
        // Cuerpo del mensaje de notificacion
        char            szMSG_Mail[2048];
        // Contador de notificaciones
        short           shMSG_Counter;
        // Destinatario del email de notificacion
        char            szMSG_Destiny[512];
        ////////////////////////////////////////////



    };//end-TrxResON2

typedef class TrxResON2 TrxResSpecialized  ;

// VALORES INTERNOS DE ERRORES DEL AUTORIZADOR DEL HOST, GENERICOS, POR OMISION
#define _DPS_ERROR_BLOQUEO_EXCEDE_SALDO_PARA_GIRAR_   (1001)
#define _IS_DPS_ERROR_BLOQUEOS_VARIOS_                (1001)
//////////////////////////////////////////////////////////////////////////////

#endif
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
#if defined( _SYSTEM_DATANET_BATCH_ )

///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transaccion DATANET
// Implementada sobre clase base virtual
class TrxResBATCH : virtual public TrxResDATANET
    {
    public:

		EXPORT TrxResBATCH( ) { ptrBackOffice = NULL; };
		EXPORT TrxResBATCH(ODBC_CONNECTOR_DNET *ptr_backoffice);
		// Validacion de trasacciones
        WORD  EXPORT ProcessInit( void );	// Proceso base, validacion		
		WORD  EXPORT ProcessInit( PBYTE pbMAC_Used );	// Proceso base, validacion		
		// Recuperar 1er registro de interfase BATCH CTA,PAG,COB,...
		boolean_t EXPORT RetrieveFirstRefreshMovement(char *lpData, unsigned short *pnLen,                                                 
											   char *lpDataBkp, unsigned short *pnLenBkp,
											   const char cszRECTYPE[] ,
											   char *szSTAMP  ) ;  // Stamp Pk
        // Recuperar Mensaje de Aplicacion REFRESH ("DPIF") hacia programa externo
		// Fix .Net 2005 "missing type specifier"
        void EXPORT RetrieveDPIFCOMM(DPIFCOMM *prefrExtMsg) { if(prefrExtMsg) (*prefrExtMsg) = refrMsg; }
        // Recupera un CODOPE similar o parecido segun parametros de INPUT
        PCSTR EXPORT SearchInternalCodOpe( PCSTR szExtCodOpe, PCSTR szExtDescrip, PCSTR szExtCRDB,
                                           char *szCODOPE, char *szDESCRIP );



	protected:

		// Validar MAC tipo 1, 2 o 3
		WORD EXPORT ValidateMAC(char *szMACTYPE) ;                // Verificacion MAC1, 2 o 3
		WORD EXPORT CalculateMAC(char *szMACTYPE, char*szExtMAC); // Verificacion MAC 2 o 3
		WORD EXPORT ValidateAccountAssociate();  // Verificar abonado de la cuenta
		WORD EXPORT ValidateExistingAccount();   // Verificar cuenta existente
		// Loggea la transferencia en la TEF BATCH-ONLINE
		// via ODBC, RPC o la que se implemente en el BACK OFFICE
		WORD EXPORT LoggTransaction(char *szESTADOTRAT ,  //  Estado tratamiento externo
		                            char *szEXTPEDENVIO); //  Pendiente de envio?
		// Recuperacion de registro actual desde B.Datos
		WORD EXPORT RetrieveRefreshMovement( char *szSTAMP,
										     const char cszRECTYPE[] ); // Tipo de Registro a recuperar
		// Formateo de interfases CTA,COB,PAG....
		void FormatOutputDataFromParams( 
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
			char chCODPRO[] , //Codigo de Proceso	, CHAR(1)
			const char cszRECTYPE[],
			char szNUMCTAEDIT[], //Nro. Cta. editado , CHAR(23)
			char szMONEDA[]    , //Codigo de Moneda, CHAR(3)
			char szTIPOMOV[]   , //Tipo de Movimiento , CHAR(1)
			char szCLEARING[]  , //Clearing, CHAR(1)
			char szNROCHEQUE[] , //Nro. Cheque, CHAR(4)
			char szCODCLI[]    , //Codigo Cliente, CHAR(10)
			char szNOMCLI[]    , //Codigo Cliente, CHAR(24)
			char szCOMPROBANTE[],//Comprobante-Factura-Recibo, CHAR(16)
			char szBOLETA[]     ,//BOLETA, VARCHAR(9)
			char szVALCOBRO[]   ,//Valor al Cobro, CHAR(1)
			char szCOBRANZA[]    //Cobranza , VARCHAR(9)
			);


	private:

		// Instancia de conexion via XML/ODBC/RPC/TCPIP a ON2 CONTINUUS (backoffice)
		ODBC_CONNECTOR_DNET *ptrBackOffice;
        ////////////////////////////////////////////

		friend class DPS_i_batch;

    };//end-TrxResBATCH

typedef class TrxResBATCH TrxResSpecialized  ;


#endif
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
#if defined( _SYSTEM_DATANET_TDFUEGO_ )

///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transaccion DATANET
// Implementada sobre clase base virtual
class TrxResBTF : virtual public TrxResBATCH
    {
    public:

		// Constructor
		EXPORT TrxResBTF( void );
        EXPORT TrxResBTF( void *ptr_backoffice );
		/*EXPORT TrxResBTF( RPC_EXTRACT_CONNECTOR_DNET *ptr_backoffice );*/

        // Autorizar primer transaccion disponible, y retornar RESP-CDE
        boolean_t EXPORT AuthorizeFirstTransaction(long *plRetStatus);	
        // Recuperar primer transaccion disponible
        boolean_t EXPORT RetrieveFirstTransaction(char           *lpData, 
                                                  unsigned short *pnLen,
                                                  char           *szTStatus,
                                                  char           *lpDataBkp, 
                                                  unsigned short *pnLenBkp);
        // Actualizar transaccion
        boolean_t EXPORT UpdateTransaction( char *pszTxt , size_t nLen );
        // Recuperar cuerpo de mensaje de notificacion
        LPCSTR EXPORT GetMSG_Mail( char *pszMsg , short *pshQSends, char *pszDestiny);
        ////////////////////////////////////////////////////////////////////
        // Recuperar primer movimiento de refresh disponible
        boolean_t EXPORT RetrieveFirstRefreshMovement(char *lpData, 
                                            unsigned short *pnLen, 
                                            char           *lpDataBkp, 
                                            unsigned short *pnLenBkp,
                                            const char      cszRECTYPE[] ,
                                            char           *szSTAMP ) ;  // Stamp PK
        ////////////////////////////////////////////////////////////////////

        // Recuperar Mensaje de Aplicacion REFRESH ("DPIF") hacia programa externo
		// Fix .Net 2005 "missing type specifier"
        void EXPORT RetrieveDPIFCOMM(DPIFCOMM &refrExtMsg) { refrExtMsg = refrMsg; }

		void EXPORT ResetStatus();

	protected:      
		
		// Validacion de trasacciones
		WORD  EXPORT ProcessInit(boolean_t bIsAuth);	// Proceso base, validacion		
		WORD  EXPORT ValidateMAC(char *szMACTYPE);// Verificacion cuenta y MAC-n
		WORD  EXPORT CalculateMAC(char *szMACTYPE, char*szExtMAC);// Calcular MAC 2 o 3
		WORD  EXPORT ValidateExistingAccount();   // Verificar cuenta existente
		WORD  EXPORT ValidateAccountStatus(char *pszEstado);  // Verificar estado de la cuenta
		WORD  EXPORT ValidateAccountAssociate();  // Verificar abonado de la cuenta
		WORD  EXPORT LoggTransaction(char *szESTADOTRAT, // Loggear Transaccion en TEF ONLINE
                                     char *szPENDENVIO = NULL );
        WORD  EXPORT RetrieveTransaction( char *szTStatus ); // Recuperar 1er Trx
		WORD  EXPORT AuthorizeTransaction(long *plExtRetStatus ); // Autorizar transaccion determinada (debito-credito/rechazo)
        WORD  EXPORT RetrieveRefreshMovement(char *szSTAMP,             // STAMP - PK
										     const char cszRECTYPE[] ); // Tipo de registro                            

    private:

        WORD  MapBTFErrors( long lRetStatus ); // Mapeo de errores BTF

        // Formatear mensaje de entrada desde los parametros  DPRE DPS
        void  FormatInputDataFromParams( 
	            char * szFRNAME ,	char * szTONAME ,
	            char * szRECTYPE,   char * chACTIVITY,
                char * szFILLER1,	char * szBANDEB ,
	            char * szFECSOL ,   char * szNUMTRA ,
	            char * szNUMABO ,   char * szTIPOPE ,
	            char * szIMPORTE,   char * szSUCDEB ,
	            char * szNOMSOL ,   char * szTIPCUEDEB,
	            char * szNCUECMDEB ,char * szNUMCTADEB ,
	            char * szFSENDB ,   char * szHSENDB ,
	            char * szOPEDB1 ,   char * szOPEDB2 ,
	            char * szRECHDB ,   char * szBANCRE ,
	            char * szSUCCRE ,   char * szNOMBEN,
	            char * szTIPCRE ,   char * szCTACRE,
	            char * szFSENCR,    char * szHSENCR,
	            char * szOPECR1,    char * szOPECR2,
	            char * szRECHCR,    char * szOPECON,
	            char * szOPEAU1,    char * szOPEAU2,
	            char * szOPEAU3,    char * szFECAUT,
	            char * szHORAUT,    char * szESTADO,
	            char * szFECEST,    char * szOBSER1,
	            char * szOBSER2,    char * szCODMAC,
	            char * szNUMREF,    char * szNUMENV,
                char * chCONSOL,    char * chMARTIT,
                char * chPRIVEZ,    char * chDIFCRE,
                char * chRIEABO,    char * chRIEBCO );
        // Formatear parametros desde el mensaje de entrada DPRE DPS
        void  FormatParamsFromInputData( 
	            char * szFRNAME ,	char * szTONAME ,
	            char * szRECTYPE,   char * chACTIVITY,
                char * szFILLER1,	char * szBANDEB ,
	            char * szFECSOL ,   char * szNUMTRA ,
	            char * szNUMABO ,   char * szTIPOPE ,
	            char * szIMPORTE,   char * szSUCDEB ,
	            char * szNOMSOL ,   char * szTIPCUEDEB,
	            char * szNCUECMDEB ,char * szNUMCTADEB ,
	            char * szFSENDB ,   char * szHSENDB ,
	            char * szOPEDB1 ,   char * szOPEDB2 ,
	            char * szRECHDB ,   char * szBANCRE ,
	            char * szSUCCRE ,   char * szNOMBEN,
	            char * szTIPCRE ,   char * szCTACRE,
	            char * szFSENCR,    char * szHSENCR,
	            char * szOPECR1,    char * szOPECR2,
	            char * szRECHCR,    char * szOPECON,
	            char * szOPEAU1,    char * szOPEAU2,
	            char * szOPEAU3,    char * szFECAUT,
	            char * szHORAUT,    char * szESTADO,
	            char * szFECEST,    char * szOBSER1,
	            char * szOBSER2,    char * szCODMAC,
	            char * szNUMREF,    char * szNUMENV,
                char * chCONSOL,    char * chMARTIT,
                char * chPRIVEZ,    char * chDIFCRE,
                char * chRIEABO,    char * chRIEBCO );

	public : 

        // Formatear mensaje de salida desde los parametros (REFRESH ONLINE)
        EXPORT void  FormatOutputDataFromParams( 
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
	            char szSURCURSAL[],//Sucursal, CHAR(3)
	            char szFECPRO[],   //Fecha de proceso en el Banco -Aaaammdd-, CHAR(8)
	            char chCONTRA[],   //Contraaseinto('1'/'0')	, CHAR(1)
	            char szTIPCUE[],   //tipo de cuenta	- CHAR(2)
	            char szDEPOSITO[],  //DEPOSITO, VARCHAR(8)
	            char szDESCRIP[],  //Texto libre , VARCHAR(25)
	            char chCODPRO[] , //Codigo de Proceso	, CHAR(1)
                const char cszRECTYPE[], // Tipo de Registro
                /*******************************************************/
                char szNUMCTAEDIT[], //Nro. Cta. editado , CHAR(23)
                char szMONEDA[]    , //Codigo de Moneda, CHAR(3)
                char szTIPOMOV[]   , //Tipo de Movimiento , CHAR(1)
                char szCLEARING[]  , //Clearing, CHAR(1)
	            char szNROCHEQUE[] , //Nro. Cheque, CHAR(4)
	            char szCODCLI[]    , //Codigo Cliente, CHAR(10)
	            char szNOMCLI[]    , //Codigo Cliente, CHAR(24)
	            char szCOMPROBANTE[],//Comprobante-Factura-Recibo, CHAR(16)
	            char szBOLETA[]     ,//BOLETA, VARCHAR(9)
                char szVALCOBRO[]   ,//Valor al Cobro, CHAR(1)
	            char szCOBRANZA[]    //Cobranza , VARCHAR(9)
                );
                /*******************************************************/

	private:

		// Instancia de conexion via XML/ODBC/RPC/TCPIP a BTF (backoffice)
		RPC_CONNECTOR_DNET *ptrBackOffice;
        ////////////////////////////////////////////
        // Cuerpo del mensaje de notificacion
        char            szMSG_Mail[2048];
        // Contador de notificaciones
        short           shMSG_Counter;
        // Destinatario del email de notificacion
        char            szMSG_Destiny[512];
        ////////////////////////////////////////////



    };//end-TrxResBTF

typedef class TrxResBTF TrxResSpecializedOnOff  ;

// VALORES INTERNOS DE ERRORES DEL AUTORIZADOR DEL HOST, GENERICOS, POR OMISION
#define _DPS_ERROR_BLOQUEO_EXCEDE_SALDO_PARA_GIRAR_   (1001)
#define _IS_DPS_ERROR_BLOQUEOS_VARIOS_                (1001)
//////////////////////////////////////////////////////////////////////////////

#endif
///////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Clase Resolucion de Transaccion DATANET general
//////////////////////////////////////////////////////////////////////////////////

// En ATM tambien hay una clase 'TrxResolution' por eso este
#ifdef _SYSTEM_ATM_
#define _TRX_RESOLUTION_	TrxResolutionDNET
#else
#define _TRX_RESOLUTION_	TrxResolution
#endif // _SYSTEM_ATM_


class _TRX_RESOLUTION_ : public TrxResSpecializedOnOff

    {
    public:
		
		// Constructor default
		_TRX_RESOLUTION_() { };
#ifdef _SYSTEM_DATANET_COBIS_
		// Constructor opcional, COBIS, no hace nada
		_TRX_RESOLUTION_(PVOID prpcDB) : TrxResCOBIS( (cobisdb *)prpcDB ) { };
#elif defined( _SYSTEM_DATANET_JERONIMO_ )
		// Constructor opcional, JERONIMO, no hace nada
		_TRX_RESOLUTION_(PVOID ptr_xml) : TrxResJERONIMO( ptr_xml ) { };
#elif defined( _SYSTEM_DATANET_ON2_ )
		// Constructor opcional, ON2-CONTINUUS, no hace nada
		_TRX_RESOLUTION_(PVOID ptr_db) : TrxResON2( ptr_db ) { };
#elif defined( _SYSTEM_DATANET_TDFUEGO_ )
		// Constructor opcional, ON2-CONTINUUS, no hace nada
		_TRX_RESOLUTION_(PVOID ptr_rpc) : TrxResBTF( ptr_rpc ) { };
#endif // _SYSTEM_DATANET_COBIS_
		// Procesamiento de transacciones
		BOOL EXPORT ProcessTransaction( PBYTE  pcbBuffer   , 
										INT    iBuffLen    ,
										PBYTE  pcbRspBuffer,
										PINT   piRspBuffLen,
										enumHostStatus hostStatus,
										enumFormatType efExtMsgFormat,
										enumHostStatus *pbackendStatus);


	};//end-TrxResolution

#endif // _TRXRESDATANET_H_


