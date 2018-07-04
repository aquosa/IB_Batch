/////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A.
// Transaction Resolution Header
//
//  En las siguientes definiciones se implementa la logica de dependencia en la
//  resolucion de las transacciones, que queda expresada en el siguiente grafo 
//  de clases, que se lee de arriba hacia abajo:
//
//                           TrxBaseResolution                    1) Esta es la base
//                                  |
//                                  |
//               +------------+-----+------+-------------+
//               |            |            |             |
//               |            |            |             |
//           TrxResPAS   TrxResULTMOV   TrxResAFJP   TrxResSOAT   2) Esta la especializacion
//               |            |            |             |
//               |            |            |             |
//               +------------+-----+------+-------------+
//                                  |
//                                  |
//                             TrxResolution                      3) Y este el controlador
//
//   Segun la implementacion, los metodos especializados puede ser "dummy", o sea un 
// simple retorno en falso sin procesamiento, sin generar el codigo ejecutable 
// actualmente,  con el solo motivo de mantener el esqueleto de codigo fuente 
// original y facilitar el posible enlace a futuro de las subclases especializadas 
// en una sola libreria o modulo.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.07.23      mdc     SOAT, en base a PAS/AFJP/ULTMOV. Logon.
// (Inicial)    1998.07.27      mdc     Logoff.
// (Inicial)    1998.07.28      mdc     VerifyDuplicatedLogon
// (Inicial)    1998.08.05      mdc     FormatLogon-LogoffResponse
// (Inicial)    1998.08.06      mdc     VerifyPreviousLogon
// (Inicial)    1998.08.07      mdc     Response-Code-Text-List,DisconnectDataBase
// (Inicial)    1998.08.13      mdc     Clase DBSOAT sobre RODBC para _SYSTEM_SOAT_
// (Inicial)    1998.08.18      mdc     Se agregan permisos a el LOGON
// (Inicial)    1998.09.02      mdc     CheckTableVersions con buffer generico
// (Inicial)    1998.09.16      mdc     Encrypt-DecryptBlock
// (Inicial)    1998.10.15      mdc     ProcessSystemCmd
// (Inicial)    1998.10.30      mdc     IDs de sentencias ODBC de SQL por accion (Insert,Update,...)
// (Inicial)    1998.12.01      mdc     Rango de codigos de rta. erroneos pasa a {200..999}
// (Inicial)    1998.12.07      mdc     Codigo de Rta. expresado en base 36 (x/exceso de rango)
// (Inicial)    1998.12.21      mdc     ProcessLogon incluye a LogonDuplicado
// (Inicial)    1999.01.06      mdc     <isoapp.h> : Path globalizado
// (Inicial)    1999.03.31      mdc     TrxResSOAT::ProcessLogonPassword(SECRSRVD7_ISO *, PBYTE ), 
//                                      TrxResSOAT::VerifyPIN(...,PBYTE,BOOL=FALSE), y
//                                      TrxResSOAT::UpdateUserAuditData(INSTDATA_ST_P,PSTR,TERMUSERDATA_ST_P); 
//                                        CPINVERIFY_PIN_EXPIRED y CPINVERIFY_PIN_FIRST_TIME.
// (Inicial)    1999.04.05      mdc     TrxResSOAT::UpdateLogonActivity(...);
// (Inicial)    1999.04.07      mdc     Codigos de Error de PinVerify() correspondidos a Codigos de Error de interface.
//                                        VerifyPIN(...,PasswordTrack_st *,BOOL=FALSE);
// (Alfa)       1999.06.11      mdc     CSOAT_VERSION_CHECKVER=425
// (Beta)       2001.12.31      mdc     ATM y virtual BOOL EXPORT ConnectDataBase(PSTR szServer, PSTR szDBase, PSTR szUser);
//                                      Base24 ATM Release 4.0
// (Beta)       2002.01.31      mdc     Tipificacion de tabla interna de asociacion TRANCODE y Std.Procedure COBIS. odbcSentenceNum.
// (Beta)       2002.03.15      mdc     SHORT Format_ISO_Ammount( const char *szAmmount, short shDecPos, char *szNew, short shAlignWidth);
// (Beta)       2002.04.17      mdc     BOOL AssignCOBISCurrencyFromAccType(enumCOBIS *pcMonedaCOBIS, char chDesdeHacia='D');
//                                        BOOL EXPORT ProcessTransaction(PBYTE,INT,PBYTE=NULL,PINT=NULL,enumHostStatus = _HOST_READY_);
//                                      #define _HOST_STATUS_TYPEDEF_
// (Beta)       2002.08.16      mdc     short Transformar_TranCde_BANELCO_REDLINK( void );
// (Beta)       2003.03.12      mdc     INTERBANKING-DATANET, ademas de definir TrxBaseSQL, c/_SQL_ODBC_SENTENCES_
// (Beta)       2003.05.09      mdc     TrxResSIOP() para Sistema SIOP. FormatTrxDateTime_WMask(....)
// (Release-1)  2003.06.17      mdc     Verify_Duplicated_Trx(msgEXTRACT *extractBuff), Verify_Reversal_Exceptions(msgEXTRACT *extractBuff);
//                                      Convert_Extract_Ammount( char *szExtAmmount , const short cEXTR_AMT_LEN);
// (Release-2)  2004.05.27      MDC     WORD  EXPORT ProcessIndividualTransfer_Online(WORD wCause,boolean_t bIsRvsl);  // Transferencia CBU de individuo OFFLINE
// (Release-3)  2004.11.25      mdc     TrxResNEAT() para Prepago Argentina (NEAT LATIN AMERICA), Base24 Release 6.3 POS
// (Release-3)  2005.01.09      mdc     TrxResNEAT::FormatField48_ADD_DATA_PRIVATE( char *szUSER_DATA );
// (Release-3)  2005.01.09      mdc     TrxResNEAT::FormatField38_RETRVL_REF_NUM( char *szREFNUM );
// (Release-3)  2005.01.18      mdc     FormatField90_ORIG_INFO( void ); char *GetField63_PRODUCT_LABEL( void );
// (Release-3)  2005.07.18      mdc     TrxResPOS se define en base a la experiencia con NEAT, y se usa para BANELCO, B24 release 6.0
// (Release-4)  2005.08.25      mdc     TranslateTranCode_ISO_B24(...Producto : '1'=ATM,'2'=POS)
// (Release-4)  2005.09.15      mdc     FormatField5_SETL_AMMT( char *chWITH_ADV_AVAIL )
// (Release-4)  2005.09.20      mdc     void FormatField_TOKENS(char *chWITH_ADV_AVAIL....)
// (Release-5a) 2006.02.28      mdc     TrxResEXTRACT tambien hereda a TrxResPOS.
// (Release-5b) 2006.02.28      mdc     Get/SetISOMessage(PBYTE,int);   
// (Release-5c) 2006.03.07      mdc     #if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )
// (Release-5d) 2006.05.15      mdc     VISUAL .NET EXCEPTION : ~TrxResEXTRACT();
// (Release-6)  2006.09.18      mdc     TrxResTDFUEGO
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _TRXRES_H_
#define _TRXRES_H_

// Header Type definitions
#include <qusrinc/typedefs.h>    
// Header ISO-8583 Application Message
#include <qusrinc/isoapp.h>


// Headers propios
// Aliases de palabras clave C/C++
#include <qusrinc/alias.h>
// Redefiniciones ISO8583
#include <qusrinc/isoredef.h>
// Lista de mensajes de texto asociados a codigos de rta.
#include <qusrinc/resptxt.h>
#include <qusrinc/respcde.h>

///////////////////////////////////////////////////////////////////////////////////////
// Headers de SQL-Query-Commands y Constantes de Tablas
// Antes de invocarse a este Header, debe estar definido el Sistema a Compilar,
// _SYSTEM_PAS_, _SYSTEM_ULTMOV_, _SYSTEM_AFJP_, _SYSTEM_MEP_, etc.,
// sino asume _SYSTEM_BASE_ :
///////////////////////////////////////////////////////////////////////////////////////
#ifdef _SQL_ODBC_SENTENCES_
#include <qusrinc/sqlqcmds.h>
// Header ODBC 
#include <qusrinc/rodbc.h>
// Definicion de tipo RODBC
typedef rodbc RODBC;
// Constantes de sentencias SQL utilizadas en el modulo
#define _SQL_FIRST_SENTENCE_      (0)
#define _SQL_READ_SENTENCE_       (0) // Read query
#define _SQL_INSERT_SENTENCE_     (1) // Insert query
#define _SQL_UPDATE_SENTENCE_     (2) // Update query
#define _SQL_DELETE_SENTENCE_     (3) // Delete query
#define _SQL_READNEXT_SENTENCE_   (4) // Readnext query
#define _SQL_READNEXT_L_SENTENCE_ (5) // Readnext in inner loop query
#define _SQL_COMMAND_SENTENCE_    (6) // Command query
#define _SQL_LAST_SENTENCE_       (6)
#endif // _SQL_ODBC_SENTENCES_
///////////////////////////////////////////////////////////////////////////////////////

// Maxima longitud de String Buffer de Resultados de Queries
#define QUERY_RESULT_SIZE  (1024)
#define _XML_BUFFER_SIZE_		   (1024*20)
///////////////////////////////////////////////////////////////
// Header DPC SYSTEM ATM-SIAF-COBIS
#include <qusrinc/dpcsys.h>
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Header RPC-SYBASE_COBIS
#ifdef _RPCDB_CONNECTION_
#include <qusrinc/rpcdb.h>
#include <qusrinc/cobisdb.h>
#endif // _RPCDB_CONNECTION_
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
// Headers SIAF-AS400 y SIOP-AS400
// Conexion TCP generica
#include <qusrinc/ctotcp4u.h>
// Logger generico
#include <qusrinc/GenLog.h>
// Mensaje struct EXTRACT - SIOP
#include <qusrinc/extrctst.h>
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Clase Base Conexion SQL ODBC -si correspondiere-
///////////////////////////////////////////////////////////////////////////////////////
class TrxBaseSQL
    {
    public :
        
        EXPORT TrxBaseSQL() ;

    protected:

        // Instancias de Clases protegidas de la instancia manager
#ifdef _SQL_ODBC_SENTENCES_
        RODBC        odbcDB;                    // ODBC Instance        
#endif // _SQL_ODBC_SENTENCES_
        char         szQuery[QUERY_RESULT_SIZE];// Query Command to be Executed        
        WORD         wQueryLen;                 // Query Length
        WORD         wResultLen;                // Result Length
        INT             iSentence;              // ODBC Sentence Number to be Executed

    protected:

        // Conexion a base de datos x default
        virtual boolean_t EXPORT ConnectDataBase(PSTR szDBase,PSTR szUser);        
        virtual boolean_t EXPORT ConnectDataBase(PSTR szServer, PSTR szDBase, 
                                PSTR szUser, PSTR szPassw);
        virtual boolean_t EXPORT ReconnectDataBase(PSTR szServer, PSTR szDBase);        
        virtual boolean_t EXPORT DisconnectDataBase(SHORT shNoneRollComm=0); // Desconexion a base de datos x default                
        LPCSTR  EXPORT           GetLastSQLCommand( void );
        LPCSTR  EXPORT           GetInternalStatus( PDWORD pdwInternal );


        // Ejecutar sentencia SQL - Virtual para redefinir en clases superiores
        virtual boolean_t  EXPORT  ExecuteSQLQuery(char *szSQLCommand, int *piSQLSentence,int *piReturn);
        // Ejecutar sentencia SQL - Virtual para redefinir en clases superiores
        virtual boolean_t  EXPORT  ExecuteSQLQuery(char *szSQLCommand, int *piSQLSentence,
                                char *szParam1, size_t iLen1,
                                int *piReturn);
        // Ejecutar sentencia SQL - Virtual para redefinir en clases superiores
        virtual boolean_t  EXPORT  ExecuteSQLQuery(char *szSQLCommand, int *piSQLSentence,
                                 char *szParam1, size_t iLen1,
                                 char *szParam2, size_t iLen2,
                                 int *piReturn);
        // Ejecutar sentencia SQL - Virtual para redefinir en clases superiores
        virtual boolean_t  EXPORT  ExecuteSQLQuery(char *szSQLCommand, int *piSQLSentence,
                                 char *szParam1, size_t iLen1,
                                 char *szParam2, size_t iLen2,
                                 char *szParam3, size_t iLen3,
                                 int *piReturn);

        
        //////////////////////////////////////////////////////////////////////
        // Enumeracion de Sentencias ODBC utilizadas segun transaccion ISO
        typedef enum {
             _ODBC_BalanceInquiry       , // Consulta de saldos
             _ODBC_Transfer             , // Transferencia
             _ODBC_TransferCBU          , // Transferencia x CBU (minorista)
             _ODBC_Withdrawal           , // Extraccion
             _ODBC_DebitNote            , // N.Debito
             _ODBC_CreditNote           , // N.Credito
             _ODBC_Deposit              , // Deposito
             _ODBC_FastCash             , // FastCash o compra
             _ODBC_AccountVerification  , // Verificacion cuenta destino
             _ODBC_Payment              , // Pagos
             _ODBC_Buying               , // Compras y relacionados
             _ODBC_AutServicePayment    , // Pagos Automatico de Servs.
             _ODBC_OtherMsgs            , // Otros
             _ODBC_Logg_Trx               // Logg de transaccion
            } odbcSentenceNum;
        //////////////////////////////////////////////////////////////////////

    };

////////////////////////////////////////////////////////////////////////////////////////////
// Clase Resolucion Base de Transaccion 
// Contiene el mensaje de aplicacion APPMSG, en cambio no contiene la conexion a la b.datos :
// si se desea contener la conexion, heredar "TrxBaseSQL" en una subclase especializada.
// Asimismo sirve de clase virtual para especializar las resoluciones (PAS,AFJP...)
#ifdef _SYSTEM_ATM_
class TrxBaseResolution
#endif // _SYSTEM_ATM_
    {
    public:

        // Metodos publicos de la Instancia
        // Constructor y destructor
        EXPORT TrxBaseResolution(void);        // Constructor x default
        EXPORT TrxBaseResolution(PSTR,PSTR);   // Constructor opcional DBASE : user+pass        
        EXPORT ~TrxBaseResolution(void);       // Destructor
		
		////////////////////////////////////////////////////////////////////////////
		// 2006.03.01 mdc
		int EXPORT SetISOMessage(PBYTE,WORD);   // ISO8583 msg
		int EXPORT GetISOMessage(PBYTE,PWORD);   // ISO8583 msg
		////////////////////////////////////////////////////////////////////////////

        // Para setear en modo ON y OFF las transacciones entrantes :
        virtual BOOL EXPORT SetOnOffLine(boolean_t boolOnlineMode);     // Modo offline-online en la trx?

        // Para loggear errores internos, se disponibilizan estos metodos publicas:
        virtual WORD  EXPORT TransactionStatus( void );
        virtual short EXPORT BackEndAuthStatus(void);
        virtual BOOL  EXPORT IsValidSAFTransaction(void);

    protected:      

        virtual WORD  EXPORT ProcessInit(void);  // Proceso base, Recuperacion campos ISO
        // Formatear monto de trx . ISO con o sin decimales, y tipo de cambio
        virtual SHORT EXPORT Format_ISO_Ammount( const char *szAmmount, short shDecPos, 
                                char *szNew, short shAlignWidth,
                                boolean_t bApplyChangeRate = is_false,
                                char *szExchageRate = NULL );
        // Traducir el codigo de transaccion de ISO hacia Base24 de LINK
        char *TranslateTranCode_ISO_B24(char szTranCodeISO[] ,
										// Producto : '1'=ATM,'2'=POS
										char chPRODUCT );
        // Formateo especial de fechar y hora de trx
        BOOL FormatTrxDateTime(char *szYYYYMMDD, char *szHHMMSS,
                                char *szYYYYMMDD_Trx, char *szHHMMSS_Trx, 
                                char *szYYYYMMDD_Cap);
        // Verifica si es un reverso por respuesta tardia
        WORD VerifyReversalForLateResponse(boolean_t bIsReversal);
        // Extraer el TRACK-2 desde el campo TRACK-2 ISO, hasta el FIELD SEPARATOR
        PCHAR ExtractCardnumFromTrack2( const TRACK2_ISO *pstTrack2ISO );        
        //////////////////////////////////////////////////////////////////////
        // Formateo especial de fechar y hora de trx, c/mascara y ano Y2K
        BOOL FormatTrxDateTime_WMask(char *szYYYYMMDD, char *szHHMMSS,    
                                    char *szYYYYMMDD_Trx, char *szHHMMSS_Trx,
                                    char *szYYYYMMDD_Cap,
                                    boolean_t bUseSeparators  = is_true,
                                    boolean_t bUse4DigitsYear = is_true,
                                    boolean_t bFormatIsDDMMYY = is_true,
                                    boolean_t bAlwaysTimeIsDateAndTime = is_true,
                                    short     nTimestampLength  = 0,
                                    short     nHourLength       = 0);
        //////////////////////////////////////////////////////////////////////



        // Mensaje de Aplicacion ISO-8583/POS/...
        APPMSG         isoMsg;
        // Campos ISO extractados del mensaje, listos para usar
        APPFIELDS     isoFields;
        // Mensaje de Aplicacion ISO-8583/POS/ BACKUP ORIGINAL
        APPMSG         isoMsgBkp;

        // Variables protegidas de la Instancia
        boolean_t     boolOnlineMode;            // ONLINE ? u OFFLINE ?
        //////////////////////////////////////////////////////////////////
        boolean_t       bProcResult;            // Process Result
        WORD           wProcRespCde;            // Process Response Code (ISO o no)        
        enumFormatType efFormatMsg;             // Formato de mensaje ISO o no
        /////////////////////////////////////////////////////////////////////////
        short           shAuthRespCde;          // Auth. Response Code (internal)
        /////////////////////////////////////////////////////////////////////////

    };//end-TrxBaseResolution

///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transaccion ATM Base24 Release 4.0
// Implementada sobre clase base virtual
class TrxResATM_Base24_R4: virtual public TrxBaseResolution
    {
    public:

        // Constructor
        EXPORT TrxResATM_Base24_R4(void); 
        // Procesamiento de transacciones SOAT
        boolean_t EXPORT ProcessTransaction(void);
        // Estado de la ultima ejecucion RPC/ODBC
        boolean_t EXPORT TransactionExecutionStatus();

    protected:      
        
        // Procesamiento de transacciones especificas
        virtual WORD  EXPORT  ProcessBalanceInquiry(WORD wCause,boolean_t bIsRvsl);     // Consulta de saldos
        virtual WORD  EXPORT  ProcessTransfer(WORD wCause,boolean_t bIsRvsl);           // Transferencia Ctas. relacionadas
        virtual WORD  EXPORT  ProcessTransferInterbank(WORD wCause,boolean_t bIsRvsl);  // Transferencia Interbancaria
        virtual WORD  EXPORT  ProcessIndividualTransfer(WORD wCause,boolean_t bIsRvsl); // Transferencia CBU de individuo
        virtual WORD  EXPORT  ProcessWithdrawal(WORD wCause,boolean_t bIsRvsl);         // Extraccion
        virtual WORD  EXPORT  ProcessDeposit(WORD wCause,boolean_t bIsRvsl);            // Deposito
        virtual WORD  EXPORT  ProcessFastCash(WORD wCause,boolean_t bIsRvsl);           // FastCash o compra
        virtual WORD  EXPORT  ProcessAccountVerification(WORD wCause,boolean_t bIsRvsl);// Verificacion cuenta destino
        virtual WORD  EXPORT  ProcessPayment(WORD wCause,boolean_t bIsRvsl);            // Pagos
        virtual WORD  EXPORT  ProcessBuying(WORD wCause,boolean_t bIsRvsl);             // Compras y relacionados
        virtual WORD  EXPORT  ProcessAutServicePayment(WORD wCause,boolean_t bIsRvsl);  // Pagos Automatico de Servs.
        virtual WORD  EXPORT  ProcessOtherMsgs(WORD wCause,boolean_t bIsRvsl);          // Otros mensajes
        virtual WORD  EXPORT  ProcessCreditNote(WORD wCause,boolean_t bIsRvsl);         // Nota de credito
        virtual WORD  EXPORT  ProcessDebitNote(WORD wCause,boolean_t bIsRvsl);          // Nota de debito
        virtual WORD  EXPORT  ProcessReversal(WORD wCause);                             // Reverso generico
        virtual WORD  EXPORT  ProcessGeneric(void);                                     // Procesamiento generico

        // Estado de la ultima ejecucion de transaccion c/b. de datos
        boolean_t        bExecStatus_ok;

        // Funciones de formateo virtuales, ver clases derivadas segun caso.
        // Formatear campo ISO #54 de rta., stB24_0210_Def_Data
        virtual void EXPORT FormatField54_B24_0210_Def_Data( 
                        char *chWITH_ADV_AVAIL,char *chINT_OWE_AUSTRAL ,
                        char *chCASH_AVAIL,char *chMIN_PAYMENT,
                        char *chPAYMENT_DAT,char *chINTEREST_RATE,
                        char *chOWE_DOLAR,char *chMIN_PAYMENT_DOLAR,
                        char *chPURCHASE_DOLAR,char *chCASH_FEE );
        // Formatear campo ISO #55, PRIRSRVD1_ISO
        virtual void EXPORT FormatField55_PRIRSRVD1_ISO( 
                        char *chTrack2,char *chCA,
                        char *chFiid_1,char *chTyp_1,
                        char *chAcct_num_1,char *chFiid_2,
                        char *chTyp_2,char *chAcct_num_2    );
        // Formatear campo ISO #44, RESP_DATA
        virtual void EXPORT FormatField44_RESP_DATA( 
			char *chWITH_ADV_AVAIL, 
			char *chCASH_AVAIL 	  ,
			char *chCREDIT_AMMOUNT );
        // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
        virtual void EXPORT FormatField123_SECNDRY_RSRVD4_PRVT( void );
        // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
        virtual void EXPORT FormatField122_SECNDRY_RSRVD3_PRVT( void );
        // Formatear campo ISO #127, SECNDRY_RSRVD8_PRVT
        virtual void EXPORT FormatField127_SECNDRY_RSRVD8_PRVT( void );
        // Formatear campo ISO #125, SECNDRY_RSRVD6_PRVT
        virtual void EXPORT FormatField125_SECNDRY_RSRVD6_PRVT( char chProc_Acct_Ind );
        virtual void EXPORT FormatField126_SECNDRY_RSRVD7_PRVT( char *szUSER_DATA );
        // Formatear campo ISO #15 y #61 que A POR ERRORES DEL PREHOMO DE LINK, a veces no vienen en el 0200
        virtual void EXPORT FormatFields15_61(void);
        // Formatear campo ISO #60
        virtual void EXPORT FormatField60(void);
        // Formatear campo ISO #124, SECNDRY_RSRVD5_PRVT
        virtual void EXPORT FormatField124_SECNDRY_RSRVD5_PRVT( void );

	    
        // Lista privada de codigos de respuesta y textos asociados
         RESPCDELIST    rtRespCdeCnv;
	

    };//end-TrxResATM_Base24_R4

///////////////////////////////////////////////////////////////////////////////////////

class TrxResATM : virtual public TrxResATM_Base24_R4 { } ;

///////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transacciones ATM y POS contra sistemas Base24 R6.0
// que basicamente agrega el manejo de tokens en los campos 63 y 126. 
// Implementada sobre clase base virtual
class TrxResATM_POS_B24_R6: virtual public TrxResATM_Base24_R4
    {

    public:
        // Constructor
        EXPORT TrxResATM_POS_B24_R6();
        EXPORT ~TrxResATM_POS_B24_R6();

    protected:

		WORD EXPORT ProcessInit(void);  // Proceso base, Recuperacion campos ISO Rel.6
        // Funciones de formateo q´redefinen a la clase padre.
        // Formatear campo ISO #54 de rta., stB24_0210_Def_Data
        void EXPORT FormatField54_B24_0210_Def_Data( 
                        char *chWITH_ADV_AVAIL,char *chINT_OWE_AUSTRAL ,
                        char *chCASH_AVAIL,char *chMIN_PAYMENT,
                        char *chPAYMENT_DAT,char *chINTEREST_RATE,
                        char *chOWE_DOLAR,char *chMIN_PAYMENT_DOLAR,
                        char *chPURCHASE_DOLAR,char *chCASH_FEE );
        // Formatear campo ISO #55, PRIRSRVD1_ISO
        void EXPORT FormatField55_PRIRSRVD1_ISO( 
                        char *chTrack2,char *chCA,
                        char *chFiid_1,char *chTyp_1,
                        char *chAcct_num_1,char *chFiid_2,
                        char *chTyp_2,char *chAcct_num_2    );
        // Formatear campo ISO #44, RESP_DATA
        void EXPORT FormatField44_RESP_DATA( char *chWITH_ADV_AVAIL, 
											 char *chCASH_AVAIL ,
											 char *chCREDIT_AMMOUNT );
        // Formatear campo ISO #123, SECNDRY_RSRVD4_PRVT, "DEPOSIT AMMOUNT"
        void EXPORT FormatField123_SECNDRY_RSRVD4_PRVT( void );
        // Formatear campo ISO #122, SECNDRY_RSRVD3_PRVT
        void EXPORT FormatField122_SECNDRY_RSRVD3_PRVT( void );
        // Formatear campo ISO #127, SECNDRY_RSRVD8_PRVT
        void EXPORT FormatField127_SECNDRY_RSRVD8_PRVT( void );
        // Formatear campo ISO #125, SECNDRY_RSRVD6_PRVT
        void EXPORT FormatField125_SECNDRY_RSRVD6_PRVT( char chProc_Acct_Ind );
        // Formatear campo ISO #126, SECNDRY_RSRVD7_PRVT
        void EXPORT FormatField126_SECNDRY_RSRVD7_PRVT( char *chWITH_ADV_AVAIL, 
													    char *chCASH_AVAIL    ,
														char *chCREDIT_AMMOUNT );
        // Formatear campo ISO #15 y #61 que A POR ERRORES DEL PREHOMO DE LINK, a veces no vienen en el 0200
        void EXPORT FormatFields15_61(void);
        // Formatear campo ISO #60
        void EXPORT FormatField60(void);
        // Formatear campo ISO #124, SECNDRY_RSRVD5_PRVT
        void EXPORT FormatField124_SECNDRY_RSRVD5_PRVT( void );

        // Funciones de formateo fijas.
        // Funciones de formateo fijas.        
		void FormatField63_PRIMARY_RSRVD4_ISO( char *szUSER_DATA1,char *szUSER_DATA2 );
		// Formatear campo ISO #63 de rta., PRIMARY_RSRVD4_ISO para ATM/POS BANELCO
		void FormatField63_PRIMARY_RSRVD4_ISO(char *chWITH_ADV_AVAIL , 
											  char *chCASH_AVAIL     ,
											  char *chCREDIT_AMMOUNT );
		void FormatField48_ADD_DATA_PRIVATE( char *szUSER_DATA, BOOL bUsePOS );
		void FormatField38_RETRVL_REF_NUM( char *szREFNUM );
		void FormatField90_ORIG_INFO( void );		
		void FormatField5_SETL_AMMT( char *chWITH_ADV_AVAIL );

		// Funcion generica de formateo de tokens en 2 campos posibles : 63 o 126
		void FormatField_TOKENS(char *chWITH_ADV_AVAIL , 
							    char *chCASH_AVAIL     ,
							    char *chCREDIT_AMMOUNT ,
								const short nField );


        /////////////////////////////////////////////////////////////////////////////////
		short GetField63_TOKEN ( char *szTokenID, void *pstTokenTag, size_t nTokenSize );
		short GetField126_TOKEN( char *szTokenID, void *pstTokenTag, size_t nTokenSize );
		char *GetField63_PRODUCT_LABEL( char *pszPRODUCT_LABEL );
        /////////////////////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////////////////////////////
        SHORT EXPORT Format_ISO_Ammount( const char *szAmmount, short shDecPos, 
                                char *szNew, short shAlignWidth,
                                boolean_t bApplyChangeRate = is_false,
                                char *szExchageRate = NULL );
		/////////////////////////////////////////////////////////////////////////////////

};

///////////////////////////////////////////////////////////////////////////////////////

#if ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE4_ )
class TrxResATM_POS : virtual public TrxResATM_Base24_R4 { } ;
#elif ( _BASE24_CURRENT_VERSION_ == _BASE24_RELEASE6_ )
class TrxResATM_POS : virtual public TrxResATM_POS_B24_R6 { } ;
#else
class TrxResATM_POS : virtual public TrxResATM_Base24_R4 { } ;
#endif

///////////////////////////////////////////////////////////////////////////////////////


#ifdef  _SYSTEM_ATM_SIAF_

///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transacciones ATM contra sistemas SIAF
// Implementada sobre clase base virtual
class TrxResSIAF: virtual public TrxResATM
    {

    public:
        // Constructor
        EXPORT TrxResSIAF();
        EXPORT TrxResSIAF(char szIPBackEnd[], unsigned short usPortBackEnd);
        EXPORT ~TrxResSIAF();

    protected:
        // Procesamiento de transacciones especificas
        WORD  EXPORT ProcessBalanceInquiry(WORD wCause,boolean_t bIsRvsl);      // Consulta de saldos
        WORD  EXPORT ProcessTransfer(WORD wCause,boolean_t bIsRvsl);            // Transferencia Ctas. relacionadas
        WORD  EXPORT ProcessTransferInterbank(WORD wCause,boolean_t bIsRvsl);   // Transferencia Interbancaria
        WORD  EXPORT ProcessIndividualTransfer(WORD wCause,boolean_t bIsRvsl);  // Transferencia CBU de individuo
        WORD  EXPORT ProcessWithdrawal(WORD wCause,boolean_t bIsRvsl);          // Extraccion
        WORD  EXPORT ProcessDeposit(WORD wCause,boolean_t bIsRvsl);             // Deposito
        WORD  EXPORT ProcessFastCash(WORD wCause,boolean_t bIsRvsl);            // FastCash o compra
        WORD  EXPORT ProcessAccountVerification(WORD wCause,boolean_t bIsRvsl); // Verificacion cuenta destino
        WORD  EXPORT ProcessPayment(WORD wCause,boolean_t bIsRvsl);             // Pagos
        WORD  EXPORT ProcessBuying(WORD wCause,boolean_t bIsRvsl);              // Compras y relacionados
        WORD  EXPORT ProcessAutServicePayment(WORD wCause,boolean_t bIsRvsl);   // Pagos Automatico de Servs.
        WORD  EXPORT ProcessCreditNote(WORD wCause,boolean_t bIsRvsl);          // Nota de credito
        WORD  EXPORT ProcessDebitNote(WORD wCause,boolean_t bIsRvsl);           // Nota de debito
        WORD  EXPORT ProcessReversal(WORD wCause);                              // Reverso generico
        WORD  EXPORT ProcessGeneric(void);                // Procesamiento generico

    protected :

        // Logg de estructura SIAF
        GenericLog  SIAFLog;
        // Conexion con SIAF
        CToTcp4u    SIAFCon;    

        // Formatear, enviar y recibir
        WORD SendAndWaitRespMsg(char *szIPBackEnd = NULL, unsigned short usPortBackEnd = 0);
        // Mapear codigo de rta BANE hacia LINK
        short Mapear_RespCde_BANELCO_REDLINK(char *szBANE, char *szLINK, short nLen);
        // Transformar codigo de transaccion BANELCO hacia RED LINK    
        short Transformar_TranCde_BANELCO_REDLINK(    msgSIAF *siafBuff , 
                                        boolean_t *pbFromPosnet,
                                        const boolean_t bROUTE_SIAF_POS );
        // Transformar tipos de cuentas BANELCO hacia RED LINK
        short Transformar_AcctTypes_BANELCO_REDLINK( msgSIAF *siafBuff );
        // Verificar moneda de transaccion y cotizacion
        boolean_t Verificar_Cotizacion_y_Moneda( msgSIAF *siafBuff , 
            boolean_t *pbCrossXfer , char *chChangeOper );
        // Es valido el monto de reversa parcial ? Copiarlo....
        boolean_t Completar_PartialRvslAmmount_BANELCO_LINK( msgSIAF *siafBuff );
        // Completar montos cotizados - contrapartida
        boolean_t Completar_CrossXfer_Ammounts( msgSIAF *siafBuff, 
            boolean_t bApplyChange, boolean_t bCrossXfer, char chChangeOper );
};
///////////////////////////////////////////////////////////////////////////////////////

#endif // _SYSTEM_ATM_SIAF_


///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transacciones ATM contra sistemas COBIS
// Implementada sobre clase base virtual
class TrxResCOBIS: virtual public TrxResATM_POS
    {

    public:
        // Constructores
        EXPORT TrxResCOBIS();
        EXPORT TrxResCOBIS(PVOID prpcDB);

    protected:
        // Procesamiento de transacciones especificas
        WORD  EXPORT ProcessBalanceInquiry(WORD wCause,boolean_t bIsRvsl);      // Consulta de saldos
        WORD  EXPORT ProcessTransfer(WORD wCause,boolean_t bIsRvsl);            // Transferencia Ctas. relacionadas
        WORD  EXPORT ProcessTransferInterbank(WORD wCause,boolean_t bIsRvsl);   // Transferencia Interbancaria
        WORD  EXPORT ProcessIndividualTransfer(WORD wCause,boolean_t bIsRvsl);  // Transferencia CBU de individuo
        WORD  EXPORT ProcessWithdrawal(WORD wCause,boolean_t bIsRvsl);          // Extraccion
        WORD  EXPORT ProcessDeposit(WORD wCause,boolean_t bIsRvsl);             // Deposito
        WORD  EXPORT ProcessFastCash(WORD wCause,boolean_t bIsRvsl);            // FastCash o compra
        WORD  EXPORT ProcessAccountVerification(WORD wCause,boolean_t bIsRvsl); // Verificacion cuenta destino
        WORD  EXPORT ProcessPayment(WORD wCause,boolean_t bIsRvsl);             // Pagos
        WORD  EXPORT ProcessBuying(WORD wCause,boolean_t bIsRvsl);              // Compras y relacionados
        WORD  EXPORT ProcessAutServicePayment(WORD wCause,boolean_t bIsRvsl);   // Pagos Automatico de Servs.
        WORD  EXPORT ProcessCreditNote(WORD wCause,boolean_t bIsRvsl);          // Nota de credito
        WORD  EXPORT ProcessDebitNote(WORD wCause,boolean_t bIsRvsl);           // Nota de debito
        WORD  EXPORT ProcessReversal(WORD wCause);                              // Reverso generico
        BOOL  EXPORT SetOnOffLine(boolean_t boolOnlineMode);     // Modo offline-online en la trx?
        
       
        //////////////////////////////////////////////////////////////////////
        // Enumeracion de Tipo de Conexion, ODBC o RPC, a base de datos
        typedef enum {
             _ODBC_DB_       , // Via ODBC
             _RPC_DB_        , // Via RPC
            } dbConnectType;
        //////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////
        // Enumeracion de constantes COBIS utilizadas
        typedef enum {
             cCobisDebitoCA      = 264,          // Debito Caja Ahorro
             cCobisDebitoCC      = 50,           // Debito Cuenta Corriente
             cCobisCreditoCA     = 253,          // Credito C.A.
             cCobisCreditoCC     = 48,           // Credito C.C.
             cCobisMonedaPesos   = 80,           // Moneda es PESOS
             cCobisMonedaDolares = 2,            // Moneda es DOLARES
             cCobisMonedaBonos   = 99,           // Pseudomoneda es BONOS
            cCobisProductoCA     = 4,            // Producto es C.A.
            cCobisProductoCC     = 3,            // Producto es C.C.
             cCobisDebito_Gral   = 998,          // Referenciales, debito en general
             cCobisCredito_Gral  = 999,          // Referenciales, credito en general
        } enumCOBIS;
        //////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////
        // Tabla de Mapeos de Codigos de Transacicones hacia Std. Procedures
        typedef struct stTranCodeSP_tag
        {
                char    chEnableYN;          // Procesa SN                
                char    *pszStoredProc;      // SP
                short    shOnCause1;         // Causal asociado para debito, ONLINE
                short    shOnCause2;         // Causal asociado para credito, ONLINE
                short    shOffCause1;        // Causal asociado para debito, OFFLINE
                short    shOffCause2;        // Causal asociado para credito, OFFLINE
                short    shCapOnCause1;      // Causal asociado para debito, NEXT CAPTURE DATE, ONLINE
                short    shCapOnCause2;      // Causal asociado para credito, NEXT CAPTURE DATE, ONLINE
                short    shCapOffCause1;     // Causal asociado para debito, NEXT CAPTURE DATE,  OFFLINE
                short    shCapOffCause2;     // Causal asociado para credito, NEXT CAPTURE DATE, OFFLINE
        }    stTranCodeSP;
        //////////////////////////////////////////////////////////////////////
    
        // Instancia de conexion via RPC a SYBASE_COBIS
#ifdef _RPCDB_CONNECTION_
        cobisdb            *prpcDB;
#else
        PVOID            prpcDB;
#endif // _RPCDB_CONNECTION_
        // Tipo de conexion a base de datos
        dbConnectType    dbctType;

    private :


        // Transferencia Interbancaria (de a una cuenta por vez) Tambien nota de cred/deb.
        WORD ProcessDebitCredit_Note(char chCuentaDesdeHacia,enumCOBIS cDebitoCredito,
                                    CONST stTranCodeSP *ptblTranCodeSP,boolean_t bIsReversal, 
                                    boolean_t bIsInterbankXfer=is_false);
        // Traducir el codigo de rta. de COBIS a ISO-Base24 de LINK
        WORD TranslateRespCode_COBIS_ISO_B24(long lRespCde);
        // Asignar el producto COBIS en funcion de la cuenta origen
        BOOL AssignCOBISProductFromAccType(enumCOBIS *pcProductoCOBIS, char chFromTo='D');
        // Asignar la moneda COBIS en funcion de la cuenta origen
        BOOL AssignCOBISCurrencyFromAccType(enumCOBIS *pcMonedaCOBIS, char chDesdeHacia='D');
        // Asignar el causal COBIS en funcion de la moneda de la cuenta origen y de la trx.
        BOOL AssignCOBISCauseFromCurrency(char chOnlineMode, char *szCause, 
                                enumCOBIS cMonedaCOBIS,
                                enumCOBIS cOperacionCOBIS,
                                enumCOBIS cProductoCOBIS,    
                                CONST stTranCodeSP    tblTranCodeSP_ARG_ARG, 
                                CONST stTranCodeSP    tblTranCodeSP_USD_ARG, 
                                CONST stTranCodeSP    tblTranCodeSP_ARG_USD, 
                                CONST stTranCodeSP    tblTranCodeSP_USD_USD,
                                bool bMultipleCurrency = is_false,
                                enumCOBIS cCurrencyCOBIS_OtherAcct = cCobisMonedaPesos);
        /////////////////////////////////////////////////////////////////////////
        WORD  EXPORT ProcessIndividualTransfer_Online(WORD wCause,boolean_t bIsRvsl);  // Transferencia CBU de individuo
        WORD  EXPORT ProcessIndividualTransfer_Offline(WORD wCause,boolean_t bIsRvsl);  // Transferencia CBU de individuo


    };
///////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transacciones ATM contra sistemas tipo EXTRACT
// Implementada sobre clase base virtual ATM
class TrxResEXTRACT: virtual public TrxResATM_POS
    {

    public:
        // Constructor
        EXPORT TrxResEXTRACT();
        EXPORT TrxResEXTRACT(char szIPBackEnd[], unsigned short usPortBackEnd);        

    protected:
        // Procesamiento de transacciones especificas
        virtual WORD  EXPORT ProcessBalanceInquiry(WORD wCause,boolean_t bIsRvsl);        // Consulta de saldos
        virtual WORD  EXPORT ProcessTransfer(WORD wCause,boolean_t bIsRvsl);            // Transferencia Ctas. relacionadas
        virtual WORD  EXPORT ProcessTransferInterbank(WORD wCause,boolean_t bIsRvsl);    // Transferencia Interbancaria
        virtual WORD  EXPORT ProcessIndividualTransfer(WORD wCause,boolean_t bIsRvsl);    // Transferencia CBU de individuo
        virtual WORD  EXPORT ProcessWithdrawal(WORD wCause,boolean_t bIsRvsl);            // Extraccion
        virtual WORD  EXPORT ProcessDeposit(WORD wCause,boolean_t bIsRvsl);                // Deposito
        virtual WORD  EXPORT ProcessFastCash(WORD wCause,boolean_t bIsRvsl);            // FastCash o compra
        virtual WORD  EXPORT ProcessAccountVerification(WORD wCause,boolean_t bIsRvsl);    // Verificacion cuenta destino
        virtual WORD  EXPORT ProcessPayment(WORD wCause,boolean_t bIsRvsl);                // Pagos
        virtual WORD  EXPORT ProcessBuying(WORD wCause,boolean_t bIsRvsl);                // Compras y relacionados
        virtual WORD  EXPORT ProcessAutServicePayment(WORD wCause,boolean_t bIsRvsl);    // Pagos Automatico de Servs.
        virtual WORD  EXPORT ProcessCreditNote(WORD wCause,boolean_t bIsRvsl);            // Nota de credito
        virtual WORD  EXPORT ProcessDebitNote(WORD wCause,boolean_t bIsRvsl);            // Nota de debito
        virtual WORD  EXPORT ProcessReversal(WORD wCause);                                // Reverso generico
        virtual WORD  EXPORT ProcessGeneric(void);                                        // Procesamiento generico

    protected :

        // Logg de estructura EXTRACT
        GenericLog    glLogg;    

        // Mapear codigo de rta EXTRACT-LINK hacia ISO8583-LINK
        short Mapear_RespCde_EXTRACT_REDLINK(char *szSIOP, char *szLINK, short nLen);
        // Mapeo de codigo de transaccion Red Link ISO hacia EXTRACT Red Link
        short Transformar_TranCde_EXTRACT_REDLINK(    msgEXTRACT *siafBuff );
        // Transformar tipos de cuentas EXTRACT hacia ISO8583
        short Transformar_AcctTypes_EXTRACT_REDLINK( msgEXTRACT *siafBuff );
        // Verificar moneda de transaccion y cotizacion
        boolean_t Verificar_Cotizacion_y_Moneda( msgEXTRACT *siafBuff , 
            boolean_t *pbCrossXfer , char *chChangeOper );
        // Es valido el monto de reversa parcial ? Copiarlo....
        boolean_t Completar_PartialRvslAmmount_EXTRACT_LINK( msgEXTRACT *siafBuff );
        // Completar montos cotizados - contrapartida
        boolean_t Completar_CrossXfer_Ammounts( msgEXTRACT *siafBuff, 
            boolean_t bApplyChange, boolean_t bCrossXfer, char chChangeOper );
        ///////////////////////////////////////////////////////////////////////////////////
        // Completar campos de la estructura EXTRACT desde el mensaje ISO 
        // Este es un procedimiento por default, puede variar en superclases que lo hereden
        virtual WORD EXPORT Completar_MSG_EXTRACT(msgEXTRACT *extractBuff);
        ///////////////////////////////////////////////////////////////////////////////////
        virtual boolean_t EXPORT Verify_Duplicated_Trx(msgEXTRACT *extractBuff);
        virtual boolean_t EXPORT Verify_Reversal_Exceptions(msgEXTRACT *extractBuff);
        virtual boolean_t EXPORT Verify_Advice_Exceptions(msgEXTRACT *extractBuff);
        virtual boolean_t EXPORT Verify_Rejected_SAF( WORD wPrevRespCode );
        ///////////////////////////////////////////////////////////////////////////////////
        // Conversion de signo del monto del EXTRACT (positivo y negativo)
        void Convert_Extract_Ammount( char *szExtAmmount , const short cEXTR_AMT_LEN);
        ///////////////////////////////////////////////////////////////////////////////////

};
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transacciones ATM contra sistemas SIOP
// Implementada sobre clase base virtual
class TrxResSIOP: virtual public TrxResEXTRACT
    {

    public:
        // Constructor
        EXPORT TrxResSIOP();
        EXPORT TrxResSIOP(char szIPBackEnd[], unsigned short usPortBackEnd);
        EXPORT ~TrxResSIOP();

    protected:
        
        // Procesamiento de transacciones especificas y genericas
        WORD  EXPORT ProcessGeneric(void);    // Procesamiento generico
        short Mapear_RespCde_SIOP_REDLINK(char *szSIOP, char *szLINK, short nLen);
        WORD  SendAndWaitRespMsg(char *szIPBackEnd = NULL, unsigned short usPortBackEnd = 0);

    protected :

        // Conexion con SIOP
        CToTcp4u    tcpConn;    
};
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transacciones ATM contra sistemas FINANSUR
// Implementada sobre clase base virtual
class TrxResFINAN: virtual public TrxResEXTRACT
    {

    public:
        // Constructor
        EXPORT TrxResFINAN();        
        EXPORT ~TrxResFINAN();

    protected:
        
        // Procesamiento de transacciones especificas y genericas
        WORD  EXPORT ProcessGeneric(void);    // Procesamiento generico
        


};
///////////////////////////////////////////////////////////////////////////////////////

// Header STRING STD
#include <string>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transacciones ATM contra sistemas FINANSUR
// Implementada sobre clase base virtual
class TrxResTDFUEGO: virtual public TrxResEXTRACT
    {

    public:
        // Constructor x omision
        EXPORT TrxResTDFUEGO();        
        // Constructor opcional
        EXPORT TrxResTDFUEGO( string strIpAddress , unsigned short ushIpPort );        
        // Destructor
        EXPORT ~TrxResTDFUEGO();

    public :
        string strRpcSrvAddress ; // Servidor RPC          
        USHORT ushRpcSrvPort   ;  // Port o Aplicacion RPC 


    protected:
        
        // Procesamiento de transacciones especificas y genericas
        WORD  EXPORT ProcessGeneric(void);    // Procesamiento generico
        


};
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transacciones ATM contra sistemas SIOP
// Implementada sobre clase base virtual
class TrxResTSHOP: virtual public TrxResEXTRACT, virtual public TrxBaseSQL
    {

    public:
        // Constructor
        EXPORT TrxResTSHOP();
        EXPORT TrxResTSHOP(char *szDBase, char *szDBaseUser, char *szPassw);
        EXPORT ~TrxResTSHOP();

    protected:
        
        // Procesamiento de transacciones especificas y genericas
        WORD  EXPORT ProcessGeneric(void);    // Procesamiento generico        
		short Mapear_RespCde_TSHOPP_REDLINK(char *szTSHOP, char *szLINK, short nLen);
        WORD  SendAndWaitRespMsg();
        ///////////////////////////////////////////////////////////////////////
        boolean_t EXPORT Verify_Duplicated_Trx(msgEXTRACT *extractBuff);
        boolean_t EXPORT Verify_Reversal_Exceptions(msgEXTRACT *extractBuff);
        boolean_t EXPORT Verify_Advice_Exceptions(msgEXTRACT *extractBuff);
        ///////////////////////////////////////////////////////////////////////
        

};
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Clase Especializada Resolucion de Transacciones POS contra sistemas NEAT
// Implementada sobre clase base virtual
class TrxResNEAT: virtual public TrxResATM
    {

    public:
        // Constructor
        EXPORT TrxResNEAT();
        EXPORT ~TrxResNEAT();

    protected:
        
        // Procesamiento de transacciones especificas y genericas
        WORD  EXPORT ProcessGeneric(void);    // Procesamiento generico
        WORD  SendAndWaitRespMsg();

    protected :

		// Transacciones individualizadas, que luego, se procesan genericamente
		WORD EXPORT  ProcessBalanceInquiry(WORD wCause,boolean_t bIsRvsl);    // Consulta de saldos
		WORD EXPORT  ProcessTransfer(WORD wCause,boolean_t bIsRvsl);    // Transferencia Ctas. relacionadas
		WORD EXPORT  ProcessTransferInterbank(WORD wCause,boolean_t bIsRvsl);    // Transferencia Interbancaria
		WORD EXPORT  ProcessIndividualTransfer(WORD wCause,boolean_t bIsRvsl);// Transferencia CBU de individuo
		WORD EXPORT  ProcessWithdrawal(WORD wCause,boolean_t bIsRvsl);        // Extraccion
		WORD EXPORT  ProcessDeposit(WORD wCause,boolean_t bIsRvsl);            // Deposito
		WORD EXPORT  ProcessFastCash(WORD wCause,boolean_t bIsRvsl);            // FastCash o compra
		WORD EXPORT  ProcessAccountVerification(WORD wCause,boolean_t bIsRvsl);    // Verificacion cuenta destino
		WORD EXPORT  ProcessPayment(WORD wCause,boolean_t bIsRvsl);            // Pagos
		WORD EXPORT  ProcessBuying(WORD wCause,boolean_t bIsRvsl);            // Compras y relacionados
		WORD EXPORT  ProcessAutServicePayment(WORD wCause,boolean_t bIsRvsl);    // Pagos Automatico de Servs.
		WORD EXPORT  ProcessCreditNote(WORD wCause,boolean_t bIsRvsl);        // Nota de credito
		WORD EXPORT  ProcessDebitNote(WORD wCause,boolean_t bIsRvsl);            // Nota de debito
		WORD EXPORT  ProcessReversal(WORD wCause);            // Reverso generico        
		void FormatField63_PRIMARY_RSRVD4_ISO( char *szUSER_DATA1,char *szUSER_DATA2 );
		void FormatField48_ADD_DATA_PRIVATE( char *szUSER_DATA );
		void FormatField38_RETRVL_REF_NUM( char *szREFNUM );
		void FormatField90_ORIG_INFO( void );        

		void GetField63_TOKEN ( char *szTokenID, void *pstTokenTag, size_t nTokenSize );		

		char *GetField63_PRODUCT_LABEL( char *pszPRODUCTO );

};

/* THE CLASS ITSELF */
class XML_CONNECTOR
{
protected:

    /* CONNECTED FLAG */
    boolean_t bConnected;   // connected ? true-false
    boolean_t bProxyEnable; // proxy enabled ? true-false
    SOCKET    mySock;       // TCP SOCKET
    char      szXMLBuffer[ _XML_BUFFER_SIZE_ ];        // Internal XML buffer 
    char	  szSOAPEnvelopeBuffer[_XML_BUFFER_SIZE_]; // Internal SOAP-Envelope buffer
    char      *ptrLastTag; // internal ptr. to last parsed TAG

public :

    /* DEFAULT CONSTRUCTOR */
    XML_CONNECTOR() 
    { 
        bConnected   = is_false; 
        bProxyEnable = is_false;
        ptrLastTag   = NULL;
        szXMLBuffer[0] = szSOAPEnvelopeBuffer[0] = 0x00;
        mySock         = -1; 
    } ;

    EXPORT XML_CONNECTOR( char *file );

    /* ALTERNATIVE CONSTRUCTOR */
    EXPORT XML_CONNECTOR( char *ptrTXT , size_t nLenght ) ;
    /* DESTRUCTOR */
    EXPORT ~XML_CONNECTOR( ) ;

public :

    /* IMPORT XML MESSAGE TO INTERNAL BUFFER */
    EXPORT int Import( char *ptrTXT , size_t nLenght );
    /* EXPORT XML MESSAGE TO EXTERNAL BUFFER */
    EXPORT int Export( char *ptrTXT , size_t nLenght );
    /* GET TAG VALUE FROM INTERNAL MESSAGE */
    EXPORT char *GetTagValue(  char *ptrTAG, char *ptrVALUE, size_t nLength  );

    /* GET NEXT TAG VALUE FROM INTERNAL MESSAGE */
    EXPORT char *GetNextTagValue(  char *ptrTAG, char *ptrVALUE, size_t nLength, 
                                       short *pshError, BOOL bFromBeginning  );
    /* PUT TAG VALUE INTO INTERNAL MESSAGE */
    EXPORT char *InsertTagValue( char *ptrTAG, char *ptrVALUE, size_t nLenght );
    /* GET TAG COUNT */
    EXPORT size_t GetTagCount( char *ptrTAG );
    
    /* FORMAT SOAP ENVELOPE  */
/*    
	EXPORT int FormatEnvelope( e_xml_format eXML_FORMAT,
                               const char *pszEnvelope_Mask,
                               ... );

*/
    /* FORMAT HTTP+POST HEADER */
    EXPORT int FormatHeader_HTTP_POST( char *pszWS_HOST,
                                       char *pszWS_URL ,
                                       char *pszACTION );
    /* FORMAT SMTP HEADER */
    EXPORT int FormatHeader_SMTP(  char *pszWS_HOST,
                                   char *pszWS_URL ,
                                   char *pszACTION );


protected :
    
    /* SEND AND RECEIVE VIA TCP/IP */
    int TCPSendAndReceive(char *szHost, unsigned short usPort, 
                      char *szBuffer, unsigned short *piLen,
                      unsigned short shRcvLen, short bPP_Protocol,
                      unsigned uTimeOut);
    /* GET LOCAL TCP-IP ADDRESS */
    int TCPGetLocalID (char *szStrName, int uNameSize, unsigned long *lpAddress);	

public :
    

#ifdef _INCLUDE_SAMPLES_METHODS_
    /* TEST-SAMPLES */
    EXPORT int UNIFON_TRANSACTION_SAMPLE ();
    EXPORT int MUNDOMOVIL_TRANSACTION_SAMPLE ();
    /* MORE TEST-SAMPLES... */
    EXPORT int BANCOSUQUIA_FECHA_PROCESO_SAMPLE ( char *pszWS_URL, char *szMsg, int *pnLen );
    EXPORT int BANCOSUQUIA_CONS_UMOV_SAMPLE ( char *pszWS_URL, char *szMsg, int *pnLen,char *szLastValue );
#endif // _INCLUDE_SAMPLES_METHODS_

//AFP: Función que transforma comilla simple en dos comillas simples. Para evitar problemas a la hora de insertar en base de datos.	
public:
    EXPORT_ATTR static std::string tratarComillas(std::string);
};


////////////////////////////////////////////////////////////////////////////////////////////////
// Proyecto DATANET-INTERBANKING
////////////////////////////////////////////////////////////////////////////////////////////////

//
// DATANET-XML-DataBaseConnectivity Class para DATANET
// First Implementation : USE OF ODBC for certain methods instead of XML-WebSrv
//
class XML_CONNECTOR_DNET : virtual public XML_CONNECTOR,
                           virtual public RODBC
{
public :

    // DEFAULT CONSTRUCTOR
    EXPORT XML_CONNECTOR_DNET() ;
    // ALTERNATE CONSTRUCTOR
    EXPORT XML_CONNECTOR_DNET(  const char *szWS_URL, const unsigned usPort, 
                                BOOL bUseTrace, const char *szUser, const char *szPass, 
                                const char *strServ1Caj, const char *strServ1Cte, 
                                const char *strServ2Caj, const char *strServ2Cte, 
                                const char *strServ3Caj, const char *strServ3Cte, 
                                const char *strServ4Caj, const char *strServ4Cte, 
                                const char *strServ5Caj, const char *strServ5Cte, 
                                const char *strServ6Caj, const char *strServ6Cte,
                                const char *strServ7Caj, const char *strServ7Cte,
                                const char *strServ8Caj, const char *strServ8Cte,
                                const char *strRevServ1Caj, const char *strRevServ1Cte, 
                                const char *strRevServ3Caj, const char *strRevServ3Cte,
                                const char *strRevServ5Caj, const char *strRevServ5Cte,
                                const char *strRevServ7Caj, const char *strRevServ7Cte);
    // DESTRUCTOR
    EXPORT ~XML_CONNECTOR_DNET() ;

    // INLINE METHODS : se anula la opcion INLINE (2010.01.28,mdc) funciona mal en RELEASE
    // RESULT & ACTION
    EXPORT PCSTR GetMethodRESULT()			;// RESULTADO BACKOFFICE ***OBSOLETO*** 
    EXPORT PCSTR GetMethodACTION()			; // ACCION A TOMAR S/BACK OFFICE 
    EXPORT PCSTR GetMethodMAPPED_RESULT()	; // ERROR MAPEADO PARA DATANET 
    EXPORT PCSTR GetMethodINTERNAL_RESULT()	; // ERROR INTERNO BACKOFFICE   
    EXPORT void SetMethodRESULT(char *szValue) ;
    EXPORT void SetMethodACTION(char *szValue) ;
    
    // STATUS AND POSTING DATE
    EXPORT PCSTR GetState()					;	// RECUPERAR EL ESTADO INTERNO
    EXPORT PCSTR GetPostingDate()		    ;	// RECUPERAR LA FECHA DE NEGOCIO
    EXPORT long  GetStatus()				;	// RECUPERAR EL RETORNO INTERNO
    EXPORT void  ResetStatus()			    ;	// RESETEAR EL CODIGO DE RETORNO
    
    // BACKOFFICE ACTION PER PRIMITIVE
    EXPORT PCSTR GetACTION_ExistingAccount() ;  // ACCION DE METODO CUENTA EXIST.
    EXPORT PCSTR GetACTION_ExistingBank()    ;	// ACCION DE METODO BANCO EXIST.
    EXPORT PCSTR GetACTION_AccountStatus()   ;  // ACCION DE METODO ESTADO CTA.
    EXPORT PCSTR GetACTION_AccountAssociate() ; // ACCION DE METODO ABONADO
    EXPORT PCSTR GetACTION_Authorize()       ;  // ACCION DEL METODO AUTORIZACION
    EXPORT PCSTR GetACTION_PostingDate()      ; // ACCION DE METODO FECHA NEGOCIO
    EXPORT PCSTR GetACTION_Refresh()          ; // ACCION DE METODO DE REFRESH UMOV

    
    // CONNECT TO DATABASE IF NECESARY
    EXPORT void ConnectDataBase( char *szDBase, char *szDBaseUser, char *szPassw );

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

    //////////////////////////////////////////////////////////////////////////////////////////////////
 //   // Autorizar la transaccion (debitar o acreditar / rechazar)
    //// AFP: Agrego parámetros HORA un parámetro que indica si es forzado y un bool que indica reverso
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
    char szCODUSUOFI[] ,  // Codigo Usuario Oficial (16)
    char szTIPCTADEB [], //  Tipo de cuenta DB, CHAR(2)
    char szTIPCTACRE[] , //  Tipo de cuenta CR, CHAR(2)
    char szSUCDEB []   , //  Sucursal cuenta DB, CHAR(4)
    char szSUCCRE[]    , //  Sucursal cuenta CR, CHAR(4)
    char szAMMOUNT[]   , //  Monto , CHAR(17)
    /*************************************************/
    char chMARTIT	   , //  Marca de mismo titular
    char szTIPOPE[]    ,
    bool esCompensatorio,
    char szHSENDB[] = " ",
    char szHSENCR[] = " ",
    char authorizetransaction = ' ',
    bool esReverso = false
    /*************************************************/
    );

    EXPORT_ATTR boolean_t RetrieveTransactionDemoradas(
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
    ); 

	//EAE ->
	// Retrieve 1er Transaccion DATANET
    // Recuperar la 1er. transaccion 
    EXPORT_ATTR boolean_t RetrieveRefresh( 
	char szCodban [],		//Número de banco según codificación B.C.R.A. char(3)
    char szNumcta [],		// Número de cuenta. Formato interno del banco char(17)
	char szFecmov [],		//Fecha de imputación del movimiento. El formato es AAMMDD char(6)
    char szFecval [],		// char(6)
	char szDebcre [],		//Código de Débito/Crédito ‘D’ o ‘C’. char(1) 
	char szImport [],		//Importe del  movimiento. char(17)
	char szNumcor [],		//Número correlativo técnico char(3)
	char szNrocom [],		// char(12)
	char szCodope [],		//Código de la operación char(3)
	char szNumext [],		// char(4)
	char szFecext [],		// char(6)
	char szFecpro [],		//Fecha de proceso. Debe ser igual a la fecha de día, de lo contrario se rechazará el movimiento. El formato es AA/MM/DD char(6)
	char szSucori [],		//Sucursal (localidad) de origen del movimiento char(5)
	char szDeposi [],		//Código de depositante char(8)
	char szMarcom [],		// char(1)
	char szDescri [],		//Descripción del movimiento. char(25)
	char szDifmov [],		//Tipo de movimiento ‘D’ = diferido,  ‘M’ = del día o anterior (según fecha imputación). char(1)
	char szHormov [],		//Hora del movimiento. char(6)
	char szContra [],		//Marca de contra asiento. ‘1’ Contra asiento char(1)
	char szTipcue [],		//Tipo de cuenta. char(2)
	char szOpebco [],		//Código de la operación según codificación Banco char(5)
	char szEnviado [] );	//Estado del registro char(1)


    EXPORT_ATTR boolean_t UpdateRefresh( 
	char szCodban [],		//Número de banco según codificación B.C.R.A. char(3)
    char szNumcta [],		// Número de cuenta. Formato interno del banco char(17)
	char szFecmov [],		//Fecha de imputación del movimiento. El formato es AAMMDD char(6)
    char szFecval [],		// char(6)
	char szDebcre [],		//Código de Débito/Crédito ‘D’ o ‘C’. char(1) 
	char szImport [],		//Importe del  movimiento. char(17)
	char szNumcor [],		//Número correlativo técnico char(3)
	char szNrocom [],		// char(12)
	char szCodope [],		//Código de la operación char(3)
	char szNumext [],		// char(4)
	char szFecext [],		// char(6)
	char szFecpro [],		//Fecha de proceso. Debe ser igual a la fecha de día, de lo contrario se rechazará el movimiento. El formato es AA/MM/DD char(6)
	char szSucori [],		//Sucursal (localidad) de origen del movimiento char(5)
	char szDeposi [],		//Código de depositante char(8)
	char szMarcom [],		// char(1)
	char szDescri [],		//Descripción del movimiento. char(25)
	char szDifmov [],		//Tipo de movimiento ‘D’ = diferido,  ‘M’ = del día o anterior (según fecha imputación). char(1)
	char szHormov [],		//Hora del movimiento. char(6)
	char szContra [],		//Marca de contra asiento. ‘1’ Contra asiento char(1)
	char szTipcue [],		//Tipo de cuenta. char(2)
	char szOpebco [],		//Código de la operación según codificación Banco char(5)
	char szEnviado [] );	//Estado del registro char(1)

	//EAE <-

	// Retrieve 1er Transaccion DATANET
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
    /****************/
    char szACCION[] );  // Accion indicada en la tabla (no la respuesta del metodo)
    /****************/

    EXPORT_ATTR boolean_t UpdateTransactionCounters(
        LPCSTR szNUMTRA	,
        PSHORT pshCounter);

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

    EXPORT_ATTR boolean_t UpdateTransactionReverse(
    char szNUMTRA [], //  Numero de transferencia,CHAR(7) 
    char szESTADO [], //  Estado de la transferencia	-Ver tabla IV- CHAR(2)
    char szPENDENVIO[],// Pend. Envio?
    char szRECHDB[],   // motivo de rechazo debito
    char szRECHCR[],   // motivo de rechazo credito
    char szESTADOTRAT[],  // Estado tratamiento
    char szCODMAC[],
    char szOPECR1[],
    char szOPECR2[]
    );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //afp: agrego los parámetros de hora débito/crédito y estado tratamiento, para saber si es forzado, necesarios para bantotal
     EXPORT_ATTR WORD ReverseTransaction(
    char szTIPOMOV[], //  "D" / "C" 
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
    char szCODUSUOFI[] ,  // Codigo Usuario Oficial (16)
    /*************************************************/
    /*************************************************/
    char szTIPCTADEB [], //  Tipo de cuenta DB, CHAR(2)
    char szTIPCTACRE[] , //  Tipo de cuenta CR, CHAR(2)
    char szSUCDEB []   , //  Sucursal cuenta DB, CHAR(4)
    char szSUCCRE[]    , //  Sucursal cuenta CR, CHAR(4)
    char szAMMOUNT[]   , //  Monto , CHAR(17)
    /*************************************************/
    char chMARTIT	   , //  Marca de titular
    char szTIPOPE[]	   ,
    char szHSENDB[] = "",
    char szHSENCR[] = "",
    char szESTADOTRAT = ' '
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
            char szMSG_MonedaCta[],        // Moneda de la cta
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
    
    /*int SendAndReceiveHttp(const char *szUrl, 
                                     const char *szSoapAction, 
                                     char *szXMLBuffer,
                                     BOOL bUseWindowsDomain );*/
	int SendAndReceiveHttp(const char *szUrl, 
                                     const char *szSoapAction, 
                                     char *szXMLBuffer);

    //////////////////////////////////////////////////////////////////////////////////
    EXPORT_ATTR boolean_t UpdateRefreshMovement(char szBOLETA[],  char szCOBRANZA[] );
    //////////////////////////////////////////////////////////////////////////////////

	/*EXPORT void setUsuarioClaveHttpAuthentication(std::string usuario, std::string clave);*/

protected:
	std::string stringAutorizacionBasica;
	const std::string _usuariohttpauth;
	const std::string _clavehttpauth;


private :

    /* INTERNAL STATUS */
    char    achState[1+1] ;
    /* INTERNAL POSTING DATE */
    char    achPostingDate_YYYYMMDD[8+1];
    /* INTERNAL SAVED TIME */
    time_t	tTimestampPostingDate;
    /* INTERNAL TRACE FILE */
    FILE    *file_hdl ;
    /* HTTP/POST parameters */
    //afp: le agrego más espacio al string
    char           pszWS_URL[128+1];               // WebService's URL
    //char           pszWS_URL[32+1];               // WebService's URL
    char strServ1Caj[6+1], strServ1Cte[6+1], strServ2Caj[6+1], strServ2Cte[6+1], 
        strServ3Caj[6+1], strServ3Cte[6+1], strServ4Caj[6+1], strServ4Cte[6+1], 
        strServ5Caj[6+1], strServ5Cte[6+1], strServ6Caj[6+1], strServ6Cte[6+1],
        strServ7Caj[6+1], strServ7Cte[6+1], strServ8Caj[6+1], strServ8Cte[6+1],
        strRevServ1Caj[6+1], strRevServ1Cte[6+1], strRevServ3Caj[6+1],
        strRevServ3Cte[6+1], strRevServ5Caj[6+1], strRevServ5Cte[6+1],
        strRevServ7Caj[6+1], strRevServ7Cte[6+1];

    //afp: agrego más espacio al string
    char cpUser[64+1];
    //char cpUser[20+1],
    char cpPass[128+1];
    unsigned short uWS_PORT  ;                // TCP PORT (commonly used '80')
    char           *pszSOAP_targetobjectURI ; // URI
    unsigned short ushPostLen, 
                   ushGetLen ;      
    /* INTERNAL TCP return value */
    int            iRetval  ;

    char *GetTagValue( char *buffer, char *ptrTAG, char *ptrVALUE, size_t nLength  );

    char *GetNextTagValue( char *szXMLBuffer, char *ptrTAG, char *ptrVALUE, size_t nLength, 
                                       short *pshError, BOOL bFromBeginning  );

    void GetDataFromAccount(const char *szAccount, char *szBranch, char *szNumber, char *szTypeOpe);

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
    /* MOTIVO RECHAZO Y CODIGO DE RESULTADO INTERNO 'JEORNIMO' */
    char szMOTIVO_RECHAZO[8+1];
    char szCOD_RESPUESTA[8+1];
    char szCOD_USUARIO_OFI[16+1];
    /* USER AND PASSWORD IF SQL/ODBC/ADO/RPC CONNECTION IS NEEDED */
    char szSQL_USER[255+1];
    char szSQL_PASS[255+1];

public:
    EXPORT_ATTR bool AnalizarTransaccion(const std::string &sRecType, const std::string &sTipoOperacion, const bool bReversa, const std::string &sBanco1, const std::string &sBanco2, const std::string &sProductoCBU, const bool esTitular = false, const char szEstadoTrat = NULL);
};

///////////////////////////////////////////////////////////////////////////////////////


// ERROR CRITICO EN GENERAL
#define _BACKEND_CRITICAL_ERROR_        (1000)
// ERROR BASE DE DATOS
#define _BACKEND_DATABASE_ERROR_        (1000)

//////////////////////////////////////////////////////////////////////////////////
// Clase Resolucion de Transaccion 
// Hereda las especializaciones PAS,AFJP,ULTMOV,SOAT,etc. y es la clase de partida
// para discriminar los tipos de resoluciones necesarios: SIAF, COBIS u otros.
//////////////////////////////////////////////////////////////////////////////////
#if defined( _SYSTEM_ATM_SIAF_ ) 
class TrxResolution : public TrxResSIAF
    {
    public:
        
        // Constructor default
        TrxResolution() { };
        // Constructor opcional, invocado por COBIS, que no hace nada
        TrxResolution(PVOID prpcDB) { };
        // Constructor opcional, invocado por SIAF
        TrxResolution(char szIPBackEnd[], unsigned short usPortBackEnd) 
            : TrxResSIAF( szIPBackEnd, usPortBackEnd ) { };
        // Metodos publicos de la Instancia
        // Procesamiento de transacciones
        BOOL EXPORT ProcessTransaction(PBYTE,INT,
                                        PBYTE pcbRspBuffer = NULL,
                                        PINT piRspBuffLen = NULL,
                                        enumHostStatus hostStatus = _HOST_READY_,
                                        enumFormatType efExtMsgFormat = CNTL_FORMAT_ISO8583,
                                        enumHostStatus *pbackendStatus = NULL );
        // Verificacion de TrxAdvice/SAFAdvice (0220)
        BOOL EXPORT IsTrxAdvice(void);
        // Verificacion de TrxAdviceResp/SAFAdviceResp (0230)
        BOOL EXPORT IsTrxAdviceResponse(void);

    };//end-TrxResolution
#elif defined( _SYSTEM_ATM_COBIS_ )
class TrxResolution : public TrxResCOBIS
    {
    public:
        
        // Constructor default
        TrxResolution() { };
        // Constructor opcional, invocado por SIAF, no hace nada
        TrxResolution(char szIPBackEnd[], unsigned short usPortBackEnd) { };
        // Constructor opcional, COBIS
        TrxResolution(PVOID prpcDB) : TrxResCOBIS( (cobisdb *)prpcDB ) { };
        // Metodos publicos de la Instancia
        // Procesamiento de transacciones
        BOOL EXPORT ProcessTransaction(PBYTE,INT,PBYTE=NULL,PINT=NULL,
                                        enumHostStatus = _HOST_READY_,
                                        enumFormatType = CNTL_FORMAT_ISO8583 ,
                                        enumHostStatus *pbackendStatus = NULL );
        // Verificacion de TrxAdvice/SAFAdvice (0220)
        BOOL EXPORT IsTrxAdvice(void);
        // Verificacion de TrxAdviceResp/SAFAdviceResp (0230)
        BOOL EXPORT IsTrxAdviceResponse(void);

    };//end-TrxResolution
#elif defined( _SYSTEM_ATM_SIOP_ ) 
class TrxResolution : public TrxResSIOP
    {
    public:
        
        // Constructor default
        TrxResolution() { };
        // Constructor opcional, invocado por COBIS, que no hace nada
        TrxResolution(PVOID prpcDB) { };
        // Constructor opcional, invocado por SIAF
        TrxResolution(char szIPBackEnd[], unsigned short usPortBackEnd) 
            : TrxResSIOP( szIPBackEnd, usPortBackEnd ) { };
        // Metodos publicos de la Instancia
        // Procesamiento de transacciones
        BOOL EXPORT ProcessTransaction(PBYTE,INT,
                                        PBYTE pcbRspBuffer = NULL,
                                        PINT piRspBuffLen = NULL,
                                        enumHostStatus hostStatus = _HOST_READY_,
                                        enumFormatType efExtMsgFormat = CNTL_FORMAT_ISO8583,
                                        enumHostStatus *pbackendStatus = NULL );
        // Verificacion de TrxAdvice/SAFAdvice (0220)
        BOOL EXPORT IsTrxAdvice(void);
        // Verificacion de TrxAdviceResp/SAFAdviceResp (0230)
        BOOL EXPORT IsTrxAdviceResponse(void);

    };//end-TrxResolution
#elif defined( _SYSTEM_ATM_TSHOP_ ) 
class TrxResolution : public TrxResTSHOP
    {
    public:
        
        // Constructor default
        TrxResolution() { };
        // Constructor opcional, invocado por SIAF, no hace nada
        TrxResolution(char szIPBackEnd[], unsigned short usPortBackEnd) { };
        // Constructor opcional, invocado por COBIS, que no hace nada
        TrxResolution(PVOID prpcDB) { };
        // Constructor opcional, invocado por TSHOPPING
        TrxResolution(char *szDBase, char *szDBaseUser, char *szPassw) 
            : TrxResTSHOP( szDBase, szDBaseUser, szPassw) { };
        // Metodos publicos de la Instancia
        // Procesamiento de transacciones
        BOOL EXPORT ProcessTransaction(PBYTE,INT,
                                        PBYTE pcbRspBuffer = NULL,
                                        PINT  piRspBuffLen = NULL,
                                        enumHostStatus hostStatus = _HOST_READY_,
                                        enumFormatType efExtMsgFormat = CNTL_FORMAT_ISO8583,
                                        enumHostStatus *pbackendStatus = NULL );
        // Verificacion de TrxAdvice/SAFAdvice (0220)
        BOOL EXPORT IsTrxAdvice(void);
        // Verificacion de TrxAdviceResp/SAFAdviceResp (0230)
        BOOL EXPORT IsTrxAdviceResponse(void);

    };//end-TrxResolution
#elif defined( _SYSTEM_POS_NEAT_ ) 
class TrxResolution : public TrxResNEAT
    {
    public:
        // Constructor default
        TrxResolution() { };
        // Constructor opcional, COBIS, no hace nada
        TrxResolution(PVOID prpcDB) { };
        // Metodos publicos de la Instancia
        // Procesamiento de transacciones
        BOOL EXPORT ProcessTransaction(PBYTE,INT,PBYTE=NULL,PINT=NULL,
                                        enumHostStatus = _HOST_READY_,
                                        enumFormatType = CNTL_FORMAT_ISO8583,
										enumHostStatus *pbackendStatus = NULL);
        // Verificacion de TrxAdvice/SAFAdvice (0220)
        BOOL EXPORT IsTrxAdvice(void);
        // Verificacion de TrxAdviceResp/SAFAdviceResp (0230)
        BOOL EXPORT IsTrxAdviceResponse(void);
		

    };//end-TrxResolution
#elif defined( _SYSTEM_ATM_FINAN_ ) 
class TrxResolution : public TrxResFINAN
    {
    public:
        
        // Constructor default
        TrxResolution() { };              
        // Constructor opcional, invocado por SIAF, no hace nada
        TrxResolution(char szIPBackEnd[], unsigned short usPortBackEnd) { };
        // Constructor opcional, invocado por COBIS, que no hace nada
        TrxResolution(PVOID prpcDB) { };
        // Metodos publicos de la Instancia
        // Procesamiento de transacciones
        BOOL EXPORT ProcessTransaction(PBYTE,INT,
                                        PBYTE pcbRspBuffer = NULL,
                                        PINT piRspBuffLen = NULL,
                                        enumHostStatus hostStatus = _HOST_READY_,
                                        enumFormatType efExtMsgFormat = CNTL_FORMAT_ISO8583,
                                        enumHostStatus *pbackendStatus = NULL );
        // Verificacion de TrxAdvice/SAFAdvice (0220)
        BOOL EXPORT IsTrxAdvice(void);
        // Verificacion de TrxAdviceResp/SAFAdviceResp (0230)
        BOOL EXPORT IsTrxAdviceResponse(void);

    };//end-TrxResolution

#elif defined( _SYSTEM_ATM_TDFUEGO_ ) 
class TrxResolution : public TrxResTDFUEGO
    {
    public:
        
        // Constructor default
        TrxResolution() { };              
        // Constructor opcional, invocado por SIAF, no hace nada
        TrxResolution(char szIPBackEnd[], unsigned short usPortBackEnd) { };
        // Constructor opcional, invocado por COBIS, que no hace nada
        TrxResolution(PVOID prpcDB) { };
        // Metodos publicos de la Instancia
        // Procesamiento de transacciones
        BOOL EXPORT ProcessTransaction(PBYTE,INT,
                                        PBYTE pcbRspBuffer = NULL,
                                        PINT piRspBuffLen = NULL,
                                        enumHostStatus hostStatus = _HOST_READY_,
                                        enumFormatType efExtMsgFormat = CNTL_FORMAT_ISO8583,
                                        enumHostStatus *pbackendStatus = NULL );
        // Verificacion de TrxAdvice/SAFAdvice (0220)
        BOOL EXPORT IsTrxAdvice(void);
        // Verificacion de TrxAdviceResp/SAFAdviceResp (0230)
        BOOL EXPORT IsTrxAdviceResponse(void);

    };//end-TrxResolution

#else 
#error "Sistema Base, atencion, verificar que es EXACTAMENTE lo que se quiere implementar"
class TrxResolution : public TrxResATM
    {
    public:
        
        // Constructor default
        TrxResolution() { };
        // Constructor opcional, COBIS, no hace nada
        TrxResolution(PVOID prpcDB) { };
        // Metodos publicos de la Instancia
        // Procesamiento de transacciones
        BOOL EXPORT ProcessTransaction(PBYTE,INT,PBYTE=NULL,PINT=NULL,
                                        enumHostStatus = _HOST_READY_,
                                        enumFormatType = CNTL_FORMAT_ISO8583,
										enumHostStatus *pbackendStatus = NULL);
        // Verificacion de TrxAdvice/SAFAdvice (0220)
        BOOL EXPORT IsTrxAdvice(void);
        // Verificacion de TrxAdviceResp/SAFAdviceResp (0230)
        BOOL EXPORT IsTrxAdviceResponse(void);

    };//end-TrxResolution
#endif // ( _SYSTEM_ATM_SIAF_, _SYSTEM_ATM_COBIS_ , _SYSTEM_ATM_SIOP_)


#endif // _TRXRES_H_
