/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// (Inicial)    2001.06.14        mdc   Inicial
// (Inicial)    2001.12.24        mdc   eventTypeChecking
// (Inicial)    2001.12.31        mdc   VOID CALLBACK DPCTIMERPROC(HWND hWindow, UINT uTimer, UINT uInt, DWORD uDWord);
// (Inicial)    2002.03.05        mdc   RPC-database connectivity (rpcdb *prpcDB)
//                                      EXPORT BOOL WINAPI DPCIMPORTMSG(const PBYTE pBuf, WORD iLen);
// (Alfa)       2002.05.21        mdc   MessageTracker        trkEchoes;    // Logger de echo-logon ISO
// (Alfa)       2002.06.25        mdc   EXPORT unsigned long GetTrxCounter(void);
// (Alfa)       2002.07.15        mdc   long NotifyIndividualEvent( char *ptrISO, int nLenISO, bool bAppendISO );
// (Beta)       2002.07.29        mdc   bool    bReversalToSAF_SystemFailure; -- Envia a SAF reversos rechazados ?
//                                      short    trxStatus;  -- Estado de la ultima transaccion procesada
//                                      EXPORT short GetLastTransactionStatus();
// (Beta)       2002.08.21        mdc   enumFormatType GetFormat(void);
// (Beta)       2002.11.22        mdc   EXPORT enumHostStatus WINAPI DPCGETSTATUSBACKEND() ;
// (Relase-1)   2003.06.11        mdc   Se agregan atributos de USUARIO y CONASEN hacia el backend
// (Relase-1)   2003.07.07        mdc   #include <cobisdb.h> es modificado con #ifdef _SYSTEM_ATM_COBIS_
// (Relase-2)   2004.07.08        mdc   REPLY_TO_SAF parametrizado. SetReplyToSAF( boolean_t bReply );
// (Relase-3)   2006.09.06      mdc     FIX VISUAL .NET "typedef void   cobisdb ;" para AFX vs. WINDOWS
// (2.0.0.1)    2012.07.26      mdc,jaf Multiples PORTS y multiples PROCESOS. ID XXXXX.   
// (2.0.0.2)    2013.04.10      mdc,jaf "int iStation" : Ordinal o Nro de Estacion 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DPCISO_
#define _DPCISO_

// Type definitions
#include <qusrinc/typedefs.h>
// ISO-8583 Application message
#include <qusrinc/isoapp.h>
// TCP/IP 4U v3.30
#include <qusrinc/tcp4u.h>
// SMTP 4U v3.30
#include <qusrinc/smtp4u.h>
// Time functions
#include <time.h>

// Store and Forward module
#include <qusrinc/saf.h>
// Message Tracker  module
#include <qusrinc/msgtrk.h>
// RPC-database connectivity
#include <qusrinc/rpcdb.h>
// Sistema en uso siendo compilado
#include <qusrinc/dpcsys.h>
/////////////////////////////////////////////////
#ifndef _SYSTEM_ATM_COBIS_
// NO COBIS
typedef void    cobisdb ;  
#else
// COBIS
#ifndef RPCDB_H
#include <qusrinc/rpcdb.h>  
typedef rpcdb   cobisdb ;  
#else
typedef rpcdb   cobisdb ;  
#endif // RPCDB_H
#endif // _SYSTEM_ATM_COBIS 
/////////////////////////////////////////////////

// Tipo de verificacion que se desea utilizar para los mensajes entrantes
typedef enum {  _INNER_LOOP_CHECKING_ ,
                _TIMER_EVENT_CHECKING_    } enumEvTypeChecking;    

// Codigos de Mensajes de Control de Linea
typedef enum {  CNTL_LOGON_RESPONSE     ,
                CNTL_LOGON_REQUERIMENT  ,
                CNTL_LOGOFF_RESPONSE    ,
                CNTL_LOGOFF_REQUERIMENT ,
                CNTL_ECHO_RESPONSE      ,
                CNTL_ECHO_REQUERIMENT } enumCntlType;

// Header DPC SYSTEM ATM-SIAF-COBIS
#include <qusrinc/dpcsys.h>
#ifndef _WIN32   
#define WINAPI   
#endif // _WIN32 

//////////////////////////////////////////////////////////////////////////////////
// Clase Mensaje ISO-8583
//////////////////////////////////////////////////////////////////////////////////
class DPCISO 
    {
public:

    EXPORT DPCISO( );
	// (2.0.0.1)    2012.07.26   mdc,jaf Multiples PORTS y ORDINAL
    EXPORT DPCISO( char *szHost, unsigned short usPort, unsigned short usLiPort, 
                   bool bPP_Protocol, int *pRc) ;
    EXPORT DPCISO(cobisdb *prpcDB);
    EXPORT ~DPCISO();

    ///////////////////////////////////////////////////////////    
    // public member methods 
    EXPORT int  Connect(char *szHost, unsigned short usPort, unsigned short usLiPort, 
                        bool bPP_Protocol, int iTimeout, cobisdb *prpcXDB = NULL);
    EXPORT int  Disconnect();
    EXPORT int  Reconnect( ) ;
    EXPORT int  CheckForMsgs(unsigned short uTimeout);
    EXPORT int  StartProcessing( boolean_t bLogon = is_false, 
                boolean_t bAuth=is_true, boolean_t bReversalToSAF=is_false);
    EXPORT int  StartProcessing( char *szHost, unsigned short usPort, 
                                        unsigned short usLiPort, bool bPP_Protocol,                            
                                        boolean_t bLogon = is_false) ;
    ////////////////////////////////////////////////////////////////////////////////////
    EXPORT void SetConfig( char *szHost,unsigned short usPort, unsigned short usLiPort,
                           bool bTCPIP_PP_PROTOCOL, int iTimeout, 
                           char *szIPBackEnd, unsigned short usPortBackEnd );
    ////////////////////////////////////////////////////////////////////////////////////
    EXPORT enumHostStatus GetStatus();
    EXPORT enumHostStatus SetStatus( enumHostStatus stNew ) ;
    ///////////////////////////////////////////////////////////
    EXPORT enumHostStatus GetStatusBackEnd();
    EXPORT enumHostStatus SetStatusBackEnd( enumHostStatus stNew ) ;
    ///////////////////////////////////////////////////////////
    EXPORT short GetLastTransactionStatus();
    EXPORT short SetLastTransactionStatus( short shNewStatus );
    ///////////////////////////////////////////////////////////
    EXPORT void SetTimeOut( int iTime );
    EXPORT int SendLogon( enumCntlType wLogon = CNTL_LOGON_REQUERIMENT );
    EXPORT int SendLogoff( );
    EXPORT int SendReqAndWaitResp(short xbPP_Protocol,unsigned uTimeOut);
    EXPORT int SendReqNoWait(short xbPP_Protocol,unsigned uTimeOut);
    EXPORT int SmtpSendMail(const char *szFrom, const char *szTo, const char *szMessage, 
                                const char *szHost, const char *szMyDomain );
    EXPORT int SendDraft(short xbPP_Protocol,  unsigned uTimeOut, char *szExtStr, int iExtLen);
    ///////////////////////////////////////////////////////////
    EXPORT int GetLocalID (char *szStrName, int uNameSize, unsigned long *lpAddress);
    EXPORT int GetMsg (char *szMsg, unsigned short *piLen );
    EXPORT int SetMsg (char *szMsg, unsigned short iLen );
    EXPORT unsigned long GetTrxCounter(void);
    EXPORT void SetNotifyHandle( HWND hhWinHandle );
    ///////////////////////////////////////////////////////////
    VOID CALLBACK TimerProc(HWND hWindow, UINT uTimer, UINT uInt, DWORD uDWord);
    ///////////////////////////////////////////////////////////
    enumFormatType SetFormat(enumFormatType ef_XFormat);
    enumFormatType GetFormat(void);
    ///////////////////////////////////////////////////////////    

    // Ahora, la replica de los SAF son parametrizadas por consola
    EXPORT bool SetReplyToSAF( bool bReply ) ;

    /* public member attributes */
    APPMSG    isoMsg;

protected:

    /* protected member methods */
    int FormatLineCntrlLogonReq( enumCntlType wCntlCode );
    int FormatLineCntrlLogonResp( char *szInMsg , enumCntlType wCntlCode );
    void VerifyPendingSAF( void );
    void VerifyPendingOSMessages( void );
    
    /* events for msg checking in lopping */
    long AllocateEvent();
    long DeallocateEvent();    
    long NotifyEvent( boolean_t bFromSAFQueue = is_false );    
    long NotifyIndividualEvent( char *ptrISO, int nLenISO, boolean_t bFromSAFQueue );
    int  StoreInSAFQueue( char *ptrISO, int nLenISO );
    /* verify locale settings */
    void VerifyLocaleSettings(void);
    
    /* handle for async. events */
    HWND hHandle;
    /* database connectivity point */
    cobisdb *prpcDB;
    
private:

    // Buffers de entrada-salida de transacciones
    char        szStr[CISOMESSAGESIZE*10];
    size_t      iLen;                
    char        szStrOut[CISOMESSAGESIZE*10];
    size_t      iLenOut;
    char        szStrBackup[CISOMESSAGESIZE*10];
    short       iLenBackup;                
    
    //////////////////////////////////////////////////////////////////////////////
    // Atributos del host ISO 8583
    // Sockets de envio y recepcion de transacciones
    char            szISOHost[256];   // Nombre del HOST o direccion IP/X.25 o cualquier otra
    unsigned short  usISOPort,        // Si es IP, puerta de entrada-salida 
                    usISOLiPort;      // Si es IP, puerta de escucha
    SOCKET          mySock,           // Sockets luego del CONNECT
                    mySockListen,
                    myActiveSock;    
    //////////////////////////////////////////////////////////////////////////////
    // Atributos del DPC (Comportamiento ISO, Version, etc)
    char            szVer[128];         // Version de la libreria de comunicaciones TCP
    HFILE           hDumpFile;          // Archivo DUMP de TCP4U Library
    int             iRc;                // Response code de la libreria de comunicaciones
    bool            bConnect;           // Senal de conectado o desconectado
    bool            bNETWORK_MGMT_ENABLE;    // Habilita manejo de echo-logon ?
    bool            bISO8583_REPLY_SAF;      // Habilita responder SAF y REVERSOS ?
    short           shOFFSET_OUTPUT_MESSAGE; // Existe un OFFSET al mensaje ISO ?
    bool            bTCPIP_PP_PROTOCOL;      // Usa protocolo PP del TCP/IP ?    
    time_t              tLastEchoLogon;      // TimeStamp del ultimo ECHO-LOGON de red
    int                 iTimeout;            // Segundos de timeout de ECHO-LOGON
    enumFormatType      efFormat;   // Formato de mensajes    
    enumEvTypeChecking  eventType;  // Tipo de eventos sincronicos o asincronicos?    
    enumHostStatus      hostStatus; // Estado del host
    short               trxStatus;  // Estado de la ultima transaccion procesada
    //////////////////////////////////////////////////////////////////////////////
    // Cola de mensajes, logger y demas
    StoreAndForward   safQueue;   // Cola de requerimientos , "SAF"
    MessageTracker    trkLogg;    // Logger de mensajes req/rta
    MessageTracker    trkError;   // Logger de errores
    MessageTracker    trkEchoes;  // Logger de echo-logon ISO
	//////////////////////////////////////////////////////////////////////////////
    // (2.0.0.1)   2012.07.26 Multiples PORTS y multiples PROCESOS. ID XXXXX.   
    char szMSGTRKFILENAME[SZTRKFNAMELEN] ;
    char szERRTRKFILENAME[SZTRKFNAMELEN] ;
    char szECHTRKFILENAME[SZTRKFNAMELEN] ;
    //////////////////////////////////////////////////////////////////////////////
    // Contador de transacciones ISO
    unsigned long    ulTrxCounter; // Contador de transacciones ISO    
    // Se mandan reversos a SAF por si vienen desordenados y se responden con "SYSTEM_FAILURE"?
    bool            bReversalToSAF_SystemFailure;
    // Esta habilitada la administracion de red ISO-Echo-Logon local (emite echo-logon)?
    bool            bLocalNetMgmt_Enable;        
    //////////////////////////////////////////////////////////////////////////////
    // Atributos del back end COBIS, SIAF, etc
    char              szBackEndHost[256]; // Nombre del BACK END (Cobis, SIAF, etc)
    unsigned short    usBackEndPort;      // Si es IP, puerta de conexion
    enumHostStatus    hostBackEndStatus;  // Estado del host Back End
    time_t            tLastSAFProcessed;  // TimeStamp del ultimo SAF procesado
    //////////////////////////////////////////////////////////////////////////////
    // Atributos del LOGIN al back end (TSHOP y otros similares ODBC, OLEDB, PIPES...)
    char            szBackEndUser[256],     // Usuario
                    szBackEndPassw[256]; // Contrasena
    //////////////////////////////////////////////////////////////////////////////
    int             iStation;            // Ordinal o Nro de Estacion 
    //////////////////////////////////////////////////////////////////////////////

} ;


// PRIVATE FUNCTIONS
VOID CALLBACK DPCTIMERPROC(HWND hWindow, UINT uTimer, UINT uInt, DWORD uDWord);


// API FUNCTIONS
EXPORT int  WINAPI DPCCONNECT(char *szHost,unsigned short usPort, unsigned short usLiPort,
                              bool bPP_Protocol, int iTimeout,cobisdb *prpcDB , int iOrdinal )  ;
EXPORT int  WINAPI DPCDISCONNECT() ;
EXPORT int  WINAPI DPCSENDREQNOWAIT(short xbPP_Protocol,unsigned uTimeOut);
EXPORT int  WINAPI DPCSENDREQANDWAITRESP(short xbPP_Protocol,unsigned uTimeOut) ;
EXPORT BOOL WINAPI DPCINITMESSAGE(void);
EXPORT BOOL WINAPI DPCGETFIELD(WORD wField,PWORD pwLen,PBYTE szBuf,WORD wLen);    
EXPORT BOOL WINAPI DPCINSERTFIELD(WORD wField,PWORD pwLen,PBYTE szBuf,WORD wLen);    
EXPORT BOOL WINAPI DPCUPDATEFIELD(WORD wField,PWORD pwLen,PBYTE szBuf,WORD wLen);    
EXPORT BOOL WINAPI DPCDELETEFIELD(WORD wField);    
EXPORT BOOL WINAPI DPCIMPORTMSG(const PBYTE pBuf, WORD iLen);
EXPORT BOOL WINAPI DPCEXPORTMSG(PBYTE pMsg,PWORD pwLen) ;
EXPORT enumHostStatus WINAPI DPCGETSTATUS() ;
EXPORT enumHostStatus WINAPI DPCGETSTATUSBACKEND() ;
EXPORT enumHostStatus WINAPI DPCSETSTATUS( enumHostStatus s ) ;
EXPORT enumHostStatus WINAPI DPCSETSTATUSBACKEND( enumHostStatus s ) ;
EXPORT unsigned long WINAPI DPCGETTRXCOUNTER( void ) ;


EXPORT int  WINAPI DPCRECONNECT( ) ;
EXPORT int  WINAPI DPCCHECKFORMSGS(unsigned short uTimeOut)  ;     
EXPORT int  WINAPI DPCSTARTPROCESSING( boolean_t bLogon , boolean_t bAuth, 
                                      boolean_t bReversalToSAF)  ;
EXPORT int  WINAPI DPCSENDLOGON( enumCntlType wLogon )  ;
EXPORT int  WINAPI DPCSENDLOGOFF( )  ;
EXPORT int  WINAPI DPCGETMSG(char *szMsg, unsigned short *piLen ) ;
EXPORT int  WINAPI DPCSETMSG(char *szMsg, unsigned short iLen ) ;
EXPORT int  WINAPI DPCSENDDRAFT(short xbPP_Protocol,  unsigned uTimeOut, char *szExtStr, 
                                int iExtLen );
EXPORT BOOL WINAPI DPCSETMSGTYPEID(PBYTE lpszValue);
EXPORT void WINAPI DPCSETNOTIFYHANDLE( HWND hHandle) ;
EXPORT BOOL WINAPI DPCSETMSGTYPEIDRESPONSE( void ) ;
EXPORT int  WINAPI DPCSMTPSENDMAIL(const char *szFrom, const char *szTo, const char *szMessage, 
                      const char *szHost, const char *szMyDomain ) ;
EXPORT void WINAPI DPCSETTIMEOUT( int iTime );
EXPORT enumFormatType WINAPI DPCSETFORMAT( enumFormatType f) ;
EXPORT enumFormatType WINAPI DPCGETFORMAT( void ) ;

EXPORT const char * WINAPI DPCGETSYSTEMDEFINED(void);
EXPORT void WINAPI DPCSETCONFIG( char *szHost,unsigned short usPort, unsigned short usLiPort,
                                 bool bTCPIP_PP_PROTOCOL, int iTimeout, 
                                 char *szIPBackEnd, unsigned short usPortBackEnd );
EXPORT short DPCGETLASTTRANSACTIONSTATUS(void);
EXPORT short DPCSETLASTTRANSACTIONSTATUS(short);
////////////////////////////////////////////////////
EXPORT BOOL  WINAPI DPCSETREPLYTOSAF(BOOL);
////////////////////////////////////////////////////

// API FUNCTIONS

#endif //  _DPCISO_H_



