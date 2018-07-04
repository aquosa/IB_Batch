/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S..
// (Inicial)    2001.06.14        mdc        Inicial
// (Inicial)    2001.07.31        mdc
// (Inicial)    2001.12.24        mdc        CheckForMsgs ampliada
// (Inicial)    2002.01.08        mdc        StartProcessing incluye a TrxResolution
// (Inicial)    2002.05.07        mdc        ::SetStatus()
// (ALFA)       2002.08.21        mdc        Atributos nuevos bNETWORK_MGMT_ENABLE,
//                                           bISO8583_REPLY_SAF, shOFFSET_OUTPUT_MESSAGE, y bTCPIP_PP_PROTOCOL
// (ALFA)       2002.11.21        mdc        EXPORT short DPCGETLASTTRANSACTIONSTATUS(void) ....
// (Beta)       2002.11.22        mdc        EXPORT enumHostStatus WINAPI DPCGETSTATUSBACKEND() ;
// (Beta)       2002.12.11        mdc        SAF concatenado enviado a SAF LOCAL
// (Beta)       2003.05.14        mdc        Sistema SIOP
// (Release-1)  2003.06.11        mdc        Se agregan atributos de USUARIO y CONTRASENA hacia el backend. TSHOPPING.
// (Release-1)  2003.08.01        mdc        Correccion de parametro PSHORT hacia variable PSHORT, antes PINT. shLen <> ilen.
// (Release-1)  2003.08.26        mdc        Correccion de inicializacion SOCKET = -1 .
// (Release-1)  2003.10.15        mdc        Correcciones de TCP4U_TIMEOUT donde decia TCP4U_ERROR en TcpRecv(...)
// (Release-2.1)2004.06.16        mdc        Mientras haya mensajes pendientes enviados, y no haya 
//                                           corte de conexion con el backend (_NO_COMM_EVENTS_)...continuar...
// (Release-2.2)2004.07.08        mdc        No se responde SAF que no haya podido aplicarse.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////


// Headers propios
// Header Type Definitions
#include <qusrinc/typedefs.h>
// Aliases
#include <qusrinc/alias.h>
// Data Processing Center
#include <qusrinc/dpciso.h>

// Header Standard Library
#include <stdlib.h>
// Header Standard I/O Library
#include <stdio.h>
// Header String
#include <string.h>
// Header Hora
#include <time.h>
// Header Local Settings
#include <locale.h>

// Incluir la macro "htons" si no es Intel-386 / MsWindows
#ifndef _WIN32
#ifndef htons
#define htons(qso_htons) (qso_htons) /* host byte order to short */
#endif
#endif
// Usar formato de YYMMDD en nombres de arhivos de LOGG?
#define _USE_TODAY_STAMP_IN_FILENAME_ (is_true)

// Header TrxResolution general y COBIS
#include <qusrinc/trxres.h>
// Header Standard I/O Library
#include <qusrinc/stdiol.h>
// Header TrxResolution SIAF
#include <qusrinc/ctotcp4u.h>



// constant definitions
#if defined(_SYSTEM_ATM_COBIS_)
static const char _DPC_ISO_SYSTEM_DEFINED_[] = "SISTEMA COBIS      ";
#elif defined(_SYSTEM_ATM_SIAF_)
static const char _DPC_ISO_SYSTEM_DEFINED_[] = "SISTEMA SIAF       ";
#elif defined(_SYSTEM_ATM_SIOP_)
static const char _DPC_ISO_SYSTEM_DEFINED_[] = "SISTEMA SIOP       ";
#elif defined(_SYSTEM_ATM_TSHOP_)
static const char _DPC_ISO_SYSTEM_DEFINED_[] = "SISTEMA TSHOPPING  ";
#elif defined(_SYSTEM_ATM_FINAN_)
static const char _DPC_ISO_SYSTEM_DEFINED_[] = "SISTEMA FINANSUR   ";
#elif defined(_SYSTEM_ATM_)
static const char _DPC_ISO_SYSTEM_DEFINED_[] = "SISTEMA BASE       ";
#else
static const char _DPC_ISO_SYSTEM_DEFINED_[] = "*** SIN SISTEMA ***";
#endif // SYSTEM DEFINED


////////////////////////////////////////////////////////////////////////////////////////////////////
// static member variables
#if (STATIC_PROPERTY_SET == TRUE)
APPMSG              DPCISO::isoMsg;
char                DPCISO::szStr[CISOMESSAGESIZE*10];
short               DPCISO::iLen;
char                DPCISO::szStrOut[CISOMESSAGESIZE*10];
short               DPCISO::iLenOut;
char                DPCISO::szStrBackup[CISOMESSAGESIZE*10];
short               DPCISO::iLenBackup;
SOCKET              DPCISO::mySock,
                    DPCISO::mySockListen,
                    DPCISO::myActiveSock;
char                DPCISO::szISOHost[256];
unsigned short      DPCISO::usISOPort,
                    DPCISO::usISOLiPort;
char                DPCISO::szVer[128];
int                 DPCISO::iRc;
bool                DPCISO::bConnect;
bool                DPCISO::bNETWORK_MGMT_ENABLE;   // Habilita manejo de echo-logon ?
bool                DPCISO::bISO8583_REPLY_SAF;     // Habilita responder SAF y REVERSOS ?
short               DPCISO::shOFFSET_OUTPUT_MESSAGE;// Existe un OFFSET al mensaje ISO ?
bool                DPCISO::bTCPIP_PP_PROTOCOL;     // Habilita protocolo PP del TCP ?
HWND                DPCISO::hHandle;
time_t              DPCISO::tLastEchoLogon;
enumFormatType      DPCISO::efFormat;
int                 DPCISO::iTimeout;
enumEvTypeChecking  DPCISO::eventType;
enumHostStatus      DPCISO::hostStatus;
StoreAndForward     DPCISO::safQueue;    // Cola de requerimientos , "SAF"
MessageTracker      DPCISO::trkLogg;    // Logger de mensajes req/rta
MessageTracker      DPCISO::trkError;    // Logger de errores
MessageTracker      DPCISO::trkEchoes;    // Logger de echoes
rpcdb               DPCISO::prpcDB;        // Punto de conexion de la base de datos externa
unsigned long       DPCISO::ulTrxCounter; // Contador interno de mensajes
bool                DPCISO::bReversalToSAF_SystemFailure;
HFILE               DPCISO::hDumpFile ;
bool                DPCISO::bLocalNetMgmt_Enable;
char                DPCISO::szBackEndHost[256];    // Nombre del BACK END (Cobis, SIAF, etc)
unsigned short      DPCISO::usBackEndPort;        // Si es IP, puerta de conexion
enumHostStatus      DPCISO::hostBackEndStatus;  // Estado del BACK END
time_t              DPCISO::tLastSAFProcessed;    // TimeStamp del ultimo SAF procesado
char                DPCISO::szBackEndUser[256];    // Usuario del BACK END
char                DPCISO::szBackEndPassw[256];// Contrasena del BACK END
#endif // STATIC_PROPERTY_SET
////////////////////////////////////////////////////////////////////////////////////////////////////


// global variables
DPCISO     dpcISO;

////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
VOID CALLBACK DPCTIMERPROC(HWND hWindow, UINT uTimer, UINT uInt, DWORD uDWord)
{ dpcISO.TimerProc(hWindow, uTimer, uInt, uDWord); };
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// API FUNCTIONS
EXPORT const char * WINAPI DPCGETSYSTEMDEFINED(void) { return _DPC_ISO_SYSTEM_DEFINED_; };
EXPORT int  WINAPI DPCCONNECT(char *szHost,unsigned short usPort, unsigned short usLiPort,
                              bool bTCPIP_PP_PROTOCOL, int iTimeout, cobisdb *prpcXDB)
{ return dpcISO.Connect (szHost,usPort,usLiPort,bTCPIP_PP_PROTOCOL, iTimeout, prpcXDB); } ;
EXPORT int  WINAPI DPCDISCONNECT()
{ return dpcISO.Disconnect (); } ;
EXPORT int  WINAPI DPCRECONNECT( )
{ return dpcISO.Reconnect (); } ;
EXPORT int  WINAPI DPCCHECKFORMSGS(unsigned short uTimeOut)
{ return dpcISO.CheckForMsgs ( uTimeOut); } ;
EXPORT int  WINAPI DPCSTARTPROCESSING( boolean_t bLogon,boolean_t bAuth, boolean_t bReversalToSAF)
{ return dpcISO.StartProcessing ( bLogon,bAuth, bReversalToSAF ); } ;
EXPORT int  WINAPI DPCSENDLOGON( enumCntlType wLogon )
{ return dpcISO.SendLogon (wLogon); } ;
EXPORT int  WINAPI DPCSENDLOGOFF( )
{ return dpcISO.SendLogoff (); } ;
EXPORT int  WINAPI DPCGETMSG(char *szMsg, unsigned short *piLen )
{ return dpcISO.GetMsg (szMsg, piLen); } ;
EXPORT int  WINAPI DPCSETMSG(char *szMsg, unsigned short iLen )
{ return dpcISO.SetMsg (szMsg, iLen); } ;
EXPORT int  WINAPI DPCSENDREQANDWAITRESP(short xbPP_Protocol,unsigned uTimeOut)
{ return dpcISO.SendReqAndWaitResp(xbPP_Protocol,uTimeOut); } ;
EXPORT void WINAPI DPCSETNOTIFYHANDLE( HWND hHandle)
{ if(hHandle) dpcISO.SetNotifyHandle( hHandle ); }
EXPORT int  WINAPI DPCSENDREQNOWAIT(short xbPP_Protocol,unsigned uTimeOut)
{ return dpcISO.SendReqNoWait(xbPP_Protocol,uTimeOut); } ;
EXPORT int  WINAPI DPCSMTPSENDMAIL( const char *szFrom, const char *szTo, const char *szMessage,
                                    const char *szHost, const char *szMyDomain )
{ return dpcISO.SmtpSendMail (szFrom, szTo, szMessage, szHost, szMyDomain); } ;
EXPORT enumHostStatus WINAPI DPCGETSTATUS()
{ return dpcISO.GetStatus() ; };
EXPORT enumHostStatus WINAPI DPCSETSTATUS( enumHostStatus s )
{ return dpcISO.SetStatus( s ) ; };
EXPORT enumFormatType WINAPI DPCSETFORMAT( enumFormatType f)
{ return dpcISO.SetFormat( f )  ; };
EXPORT unsigned long WINAPI DPCGETTRXCOUNTER( void )
{ return dpcISO.GetTrxCounter() ; };
EXPORT enumFormatType WINAPI DPCGETFORMAT( void )
{ return dpcISO.GetFormat( ); };
EXPORT void WINAPI DPCSETCONFIG( char *szHost,unsigned short usPort, unsigned short usLiPort,
                                 bool bTCPIP_PP_PROTOCOL, int iTimeout,
                                 char *szIPBackEnd, unsigned short usPortBackEnd )
{ dpcISO.SetConfig(szHost,usPort,usLiPort,bTCPIP_PP_PROTOCOL,
                   iTimeout, szIPBackEnd, usPortBackEnd );
};
EXPORT short DPCGETLASTTRANSACTIONSTATUS(void)
{ return dpcISO.GetLastTransactionStatus(); };
EXPORT short DPCSETLASTTRANSACTIONSTATUS(short shNew)
{ return dpcISO.SetLastTransactionStatus(shNew); };
EXPORT enumHostStatus WINAPI DPCGETSTATUSBACKEND()
{ return dpcISO.GetStatusBackEnd() ; };
EXPORT enumHostStatus WINAPI DPCSETSTATUSBACKEND( enumHostStatus s )
{ return dpcISO.SetStatusBackEnd( s ) ; };

////////////////////////////////////////////////////////////////////////////////////////////////////
// ISO8583 API FUNCTIONS
EXPORT BOOL WINAPI DPCINITMESSAGE(void)
{ return dpcISO.isoMsg.InitMessage() ; };
EXPORT BOOL WINAPI DPCDELETEFIELD(WORD field)
{ return dpcISO.isoMsg. DeleteField(field); };
EXPORT BOOL WINAPI DPCEXPORTMSG(PBYTE pMsg,PWORD pwLen)
{ return dpcISO.isoMsg.Export(pMsg,pwLen); };
EXPORT BOOL WINAPI DPCIMPORTMSG(const PBYTE pBuf, WORD iLen)
{ return dpcISO.isoMsg.Import(pBuf,iLen);  };
EXPORT BOOL WINAPI DPCSETMSGTYPEID(PBYTE lpszValue)
{ return dpcISO.isoMsg.SetMsgTypeId(lpszValue, 4);  };
EXPORT BOOL WINAPI DPCGETMSGTYPEID(PBYTE lpszValue)
{ return dpcISO.isoMsg.GetMsgTypeId(lpszValue, 4+1);  };
EXPORT BOOL WINAPI DPCGETFIELD(WORD iField,PWORD pwLen,PBYTE szBuf,WORD wLen)
{ return dpcISO.isoMsg.GetField(iField,pwLen,szBuf,wLen);  };
EXPORT BOOL WINAPI DPCINSERTFIELD(WORD iField,PWORD pwLen,PBYTE szBuf,WORD wLen)
{ return dpcISO.isoMsg.InsertField(iField,pwLen,szBuf,wLen);  };
EXPORT BOOL WINAPI DPCUPDATEFIELD(WORD iField,PWORD pwLen,PBYTE szBuf,WORD wLen)
{ return dpcISO.isoMsg.SetField(iField,pwLen,szBuf,wLen);  };
EXPORT BOOL WINAPI DPCSETMSGTYPEIDRESPONSE( void )
{ return dpcISO.isoMsg.SetMsgTypeIdResponse(); };
EXPORT void WINAPI DPCSETTIMEOUT( int iTime )
{ dpcISO.SetTimeOut( iTime ); };
EXPORT int  WINAPI DPCSENDDRAFT(short xbPP_Protocol,unsigned uTimeOut,
                                char *szExtStr,int iExtLen )
{ return dpcISO.SendDraft(xbPP_Protocol,uTimeOut,szExtStr,iExtLen ); };
EXPORT BOOL WINAPI DPCSETREPLYTOSAF( BOOL bReply )
{ return dpcISO.SetReplyToSAF( ( bReply ) ? true : false ) ; }

//////////////////////////////////////////////////////////////////////////////////

// API FUNCTIONS

// CONSTANTS
// TimeOuts de norma ISO, segun configuracion propietaria
#define _ISO8583_ECHO_TIMEOUT_         (21)
// Timers en MILISEGUNDOS en caso de que se desee una notificacion via
// S.O. y eventos asincronicos mediante temporizadores.
#define _ISO8583_ECHO_TIMEOUT_MSEG_    (_ISO8583_ECHO_TIMEOUT_*1000L)
#define _POLLING_TIMEOUT_MSEG_         (1000L)
// TimeOut general del DPC (ACCEPT - WRITE)
// Se elije un TIMEOUT que es numero primo para no provocar
// una secuencia que se superponga y a su vez sobrecargue los
// ECHO - LOGON del HOST administrador, RED LINK / BANELCO / BANCOMAT / etc.
#define _DPC_COMM_TIMEOUT_                  (23)
// Alerta de procesamiento de SAF continuo, en segundos
#define _ISO8583_SAF_IN_PROGRESS_ALERT_    (300)

// ISO8583 Msg. Header
static const char _ISO_MSG_HEADER_[]         = "ISO";
// ISO8583 Msg. Header, Product ACI B24-ATM-POS
static const char _ISO_MSG_HEADER_ATM_POS_[] = "ISO0";
// ISO8583 Msg. Header, Product ACI EFT
static const char _ISO_MSG_HEADER_EFT_[]     = "ISO1";


///////////////////////////////////////////////////////////////////////////////////////
// constructor
DPCISO::DPCISO( )
{
   bConnect                      = false;
   ///////////////////////////////////////////////////////////////////////////////
   bTCPIP_PP_PROTOCOL            = true; // Habilita protocolo PP del TCP ?
   bNETWORK_MGMT_ENABLE          = true;    // Habilita manejo de echo-logon ?
   bISO8583_REPLY_SAF            = true;    // Habilita responder SAF y REVERSOS ?
   shOFFSET_OUTPUT_MESSAGE       = 0;    // Existe un OFFSET al mensaje ISO ?
   ///////////////////////////////////////////////////////////////////////////////
   tLastSAFProcessed            = 0L;
   bReversalToSAF_SystemFailure = is_false;
   trxStatus                    = CISO_APPROVED_I;
   iLen    = iLenOut = iLenBackup = 0;
   memset( szStr      , 0x00, sizeof szStr );
   memset( szStrOut   , 0x00, sizeof szStrOut );
   memset( szStrBackup, 0x00, sizeof szStrBackup );
   //////////////////////////////////////////////////////////////
   iRc               = TCP4U_SUCCESS;
   hDumpFile         = HFILE_ERROR;    // Archivo DUMP de TCP4U Library
   mySock = mySockListen = myActiveSock = -1;
   hHandle           = NULL;
   tLastEchoLogon    = 0L;
   efFormat          = CNTL_FORMAT_ISO8583;
   iTimeout          = 0;
   hostStatus        = _HOST_READY_;
   hostBackEndStatus = _HOST_READY_;
   ulTrxCounter      = 0;
#if defined( ___INNER_LOOP_CHECKING_ )
   eventType        = _INNER_LOOP_CHECKING_ ;
#elif defined( ___TIMER_EVENT_CHECKING_ )
   eventType        = _TIMER_EVENT_CHECKING_ ;
#else
#error "Sin definir el modo de procesamiento de eventos"
#endif
   // ISO8583 init
   isoMsg.InitMessage();
   // open-create SAF
   safQueue.Open( SAFFILENAME );
   // open-cerate MSG track
   trkLogg.Open( MSGTRKFILENAME   , _USE_TODAY_STAMP_IN_FILENAME_ );
   // open-create ERROR track
   trkError.Open( ERRTRKFILENAME  , _USE_TODAY_STAMP_IN_FILENAME_ );
   // open-create ECHOE track
   trkEchoes.Open( ECHOTRKFILENAME, _USE_TODAY_STAMP_IN_FILENAME_ );

   //////////////////////////////////////////////
   szISOHost[0]     = 0x00;
   usISOPort        = 0;
   usISOLiPort      = 0 ;
   szBackEndHost[0] = 0x00;
   usBackEndPort    = 0;
   //////////////////////////////////////////////
   szBackEndUser[0]  = 0x00;
   szBackEndPassw[0] = 0x00;
   //////////////////////////////////////////////

   /* clean up library */ 
   Tcp4uCleanup();           
   // Verify locale settings  (decimal point)
   VerifyLocaleSettings();

}
///////////////////////////////////////////////////////////////////////////////////////
// constructor
DPCISO::DPCISO( cobisdb *prpcXDB )
{
    // def. constructor
    ::DPCISO();
    // db backup
    prpcDB = prpcXDB;

   // Verify locale settings  (decimal point)
   VerifyLocaleSettings();

}
///////////////////////////////////////////////////////////////////////////////////////
// optional constructor
DPCISO::DPCISO( char *szHost, unsigned short usPort, unsigned short usLiPort, bool xbPP_Protocol, int *pRc)
{

   bConnect                      = false;
   ///////////////////////////////////////////////////////////////////////////////
   bTCPIP_PP_PROTOCOL            = true; // Habilita protocolo PP del TCP ?
   bNETWORK_MGMT_ENABLE          = true;    // Habilita manejo de echo-logon ?
   bISO8583_REPLY_SAF            = true;    // Habilita responder SAF y REVERSOS ?
   shOFFSET_OUTPUT_MESSAGE       = 0;    // Existe un OFFSET al mensaje ISO ?
   ///////////////////////////////////////////////////////////////////////////////
   tLastSAFProcessed            = 0L;
   bReversalToSAF_SystemFailure = is_false;
   trxStatus                    = CISO_APPROVED_I;
   iLen    = iLenOut = iLenBackup  = 0;
   memset( szStr      , 0x00, sizeof szStr );
   memset( szStrOut   , 0x00, sizeof szStrOut );
   memset( szStrBackup, 0x00, sizeof szStrBackup );
   ////////////////////////////////////////////////
   iRc               = TCP4U_SUCCESS;
   hDumpFile         = HFILE_ERROR;    // Archivo DUMP de TCP4U Library
   mySock = mySockListen = myActiveSock = -1;
   hHandle           = NULL;
   tLastEchoLogon    = 0L;
   efFormat          = CNTL_FORMAT_ISO8583;
   iTimeout          = 0;
   hostStatus        = _HOST_READY_;
   hostBackEndStatus = _HOST_READY_;
   ulTrxCounter      = 0;
#if defined( ___INNER_LOOP_CHECKING_ )
   eventType        = _INNER_LOOP_CHECKING_ ;
#elif defined( ___TIMER_EVENT_CHECKING_ )
   eventType        = _TIMER_EVENT_CHECKING_ ;
#else
#error "Sin definir el modo de procesamiento de eventos"
#endif
   // ISO8583 init
   isoMsg.InitMessage();
   // open-create SAF
   safQueue.Open( SAFFILENAME );
   // open-cerate MSG track
   trkLogg.Open( MSGTRKFILENAME    , _USE_TODAY_STAMP_IN_FILENAME_);
   // open-create ERROR track
   trkError.Open( ERRTRKFILENAME   , _USE_TODAY_STAMP_IN_FILENAME_);
   // open-create ECHOE track
   trkEchoes.Open( ECHOTRKFILENAME , _USE_TODAY_STAMP_IN_FILENAME_);

   //////////////////////////////////////////////
   szISOHost[0]     = 0x00;
   usISOPort        = 0;
   usISOLiPort      = 0 ;
   szBackEndHost[0] = 0x00;
   usBackEndPort    = 0;
   //////////////////////////////////////////////
   szBackEndUser[0]  = 0x00;
   szBackEndPassw[0] = 0x00;
   //////////////////////////////////////////////


   /* clean up library */ 
   Tcp4uCleanup();        
   // TCP Connect
   Connect( szHost, usPort, usLiPort, xbPP_Protocol, iTimeout);

   // Verify locale settings  (decimal point)
   VerifyLocaleSettings();

}
///////////////////////////////////////////////////////////////////////////////////////
// destructor
DPCISO::~DPCISO()
{
   /* shuting down.... */
   hostStatus                   = _HOST_STOPPED_;
   hostBackEndStatus            = _HOST_STOPPED_;
   tLastSAFProcessed            = 0L;
   tLastEchoLogon               = 0L;
   bReversalToSAF_SystemFailure = is_false ;
   bNETWORK_MGMT_ENABLE         = false ;
   if(bConnect)
   {
     /* close socket */
     TcpClose (&mySock);
     /**************************************************/
     /* verify pending msgs                            */
     CheckForMsgs( iTimeout );
     /**************************************************/
     /* clean up library */
     Tcp4uCleanup();
     bConnect = false ;
   }
   // Archivo DUMP de TCP4U Library
   hDumpFile = HFILE_ERROR;
   // close SAF
   safQueue.Close( );
   // close MSG tracker
   trkLogg.Close();
   // close ERR tracker
   trkError.Close();
   // close ECHO tracker
   trkEchoes.Close();
   // Sin contador
   ulTrxCounter = 0;
}
///////////////////////////////////////////////////////////////////////////////////////
// verificacion de SAF pendiente
void DPCISO::VerifyPendingSAF( void )
{
    /* numeros de auditoria y referencia para una transaccion ISO */
    unsigned long    dwTrace    = 0L,
                     dwReferral = 0L;

    /* precondicion : estar habilitado el parametro de envio a SAF */
    if(!bReversalToSAF_SystemFailure)
        return ;

    /* verificar SAF : si hay registros... */
    if( safQueue.StoredRecords() > 0 )
    {
        /* ir al primer registro almacenado */
        safQueue.SetFirstStored();
        /* mientras no sea el fin del archivo... , leer */
        while ( safQueue.ReadStored( (unsigned char *)szStr, (short)sizeof szStr, &iLen,
            &dwTrace, &dwReferral) )
        {
            /* importar mensaje ISO */
            isoMsg.Import( (unsigned char *)szStr, iLen );
            /* procesar mensaje ISO con senal de "origen es cola de SAF" */
            NotifyEvent( is_true );
            /* marcar al registro SAF como procesado */
            safQueue.SetForwarded();
        };/* end while */
        /* al final del ciclo, recalcular estadisticas */
        safQueue.InitStatistics();
    }; /* end if */
}
///////////////////////////////////////////////////////////////////////////////////////
// inicio del procesamiento general del data center
int DPCISO::StartProcessing(boolean_t bLogon,boolean_t bAuthorize, boolean_t bRevToSAF)
{
    char            szStrLoc[256]={0x00};
    unsigned short  uTimeOut     = iTimeout;
    time_t          tCurrentTime = time( NULL );
    /*******************************************************/
    /* variable de retorno de la longitud es SHORT 16 bits */
	unsigned short  shLen = 0;
    int             iRcLocal = 0L;
	/*******************************************************/

    /****************************************************************/
    /* El archivo de DUMP de TCP4ULibrary                           */
    hDumpFile = fileno( trkError.GetFileHandle() );
    /****************************************************************/

    /****************************************************************/
    /* Verificar mensajes pendientes, PRIMERO QUE NADA */
    iRc = CheckForMsgs( uTimeOut ); /* if data avail, X sec time out */
    /****************************************************************/

    /***********************************************************************/
    /* Verificar SAF Previo si es que autoriza, LUEGO DEL CHEQUEO ANTERIOR */
    if(bAuthorize)
        VerifyPendingSAF();
    /***********************************************************************/
    /* Verificar si se envian reversos a SAF  */
    if(bRevToSAF)
        bReversalToSAF_SystemFailure = is_true;
    else
        bReversalToSAF_SystemFailure = is_false;
    /***********************************************************************/

    /******************************************************************/
    /* Se envia un LOGON? Luego de haber CHEQUEADO MENSAJES ENTRANTES */
    if(bLogon && bNETWORK_MGMT_ENABLE)
    {
        iLen = FormatLineCntrlLogonReq( CNTL_LOGON_REQUERIMENT );
        if( bTCPIP_PP_PROTOCOL && bNETWORK_MGMT_ENABLE )
            iRc = TcpPPSend (myActiveSock, szStr, iLen, hDumpFile);
        else if( bNETWORK_MGMT_ENABLE )
            iRc = TcpSend (myActiveSock, szStr, iLen, FALSE, hDumpFile);
        else
            iRc = TCP4U_SUCCESS ;
        // debug y tracking de mensaje ECHO
        if( bNETWORK_MGMT_ENABLE )
            trkEchoes.Write( (LPBYTE)szStr, iLen );
    }
    else
    {
        iRc = TCP4U_SUCCESS;
    }
    /******************************************************************/


    /**********************************************/
    /* Mientras haya mensajes pendientes enviados */
    /* y no haya corte de conexion con el backend */
    /**********************************************/
    while ( TCP4U_SUCCESS == iRc 
            &&
            _NO_COMM_EVENTS_ != hostBackEndStatus )
    /**********************************************/
    {
        memset( szStr, 0x00, sizeof szStr );
        if(bTCPIP_PP_PROTOCOL)
            iRc = TcpPPRecv (myActiveSock, szStr, sizeof szStr, uTimeOut, FALSE, hDumpFile);
        else
            iRc = TcpRecv (myActiveSock, szStr, sizeof szStr, uTimeOut, hDumpFile);
         /* verificar longitudes retornadas */
        if(iRc<=0)
        {
            iLen = 0;
            return iRc;
        }
        else if(iRc>0)
        {
            iLen = iRc;
            isoMsg.Import( (unsigned char *)szStr , iLen );
            iRc = TCP4U_SUCCESS;
            /* verificar si lo recibido es un requerimiento o echo */
            if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtRequest() )
            {
                iLen = sizeof szStrLoc;
                isoMsg.Export( (unsigned char *)szStrLoc, (unsigned short *)&iLen );
                FormatLineCntrlLogonResp( szStrLoc , CNTL_LOGON_RESPONSE )     ;
                iLen = strlen ( szStrLoc );
                if( bTCPIP_PP_PROTOCOL  && bNETWORK_MGMT_ENABLE )
                  iRc = TcpPPSend (myActiveSock, szStrLoc, iLen, hDumpFile);
                else if( bNETWORK_MGMT_ENABLE )
                  iRc = TcpSend (myActiveSock, szStrLoc, iLen, FALSE, hDumpFile);
                else
                  iRc = TCP4U_SUCCESS;
                // debug y tracking de mensaje ECHO
                if( bNETWORK_MGMT_ENABLE )
                    trkEchoes.Write( (LPBYTE)szStrLoc, iLen );
            }
            else if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtResponse() )
            {
                // debug y tracking de mensaje ECHO
                iLen = strlen( szStr );
                trkEchoes.Write( (LPBYTE)szStr, iLen );
            }
            else if( iRc == TCP4U_SUCCESS && isoMsg.IsControlCenterCommand() )
            {
            }
            else if (TCP4U_SUCCESS == iRc)
            {
                // debug y tracking de mensaje ISO
                trkLogg.Write( (LPBYTE)szStr, iLen );
                /* si debe autorizarse o no */
                if(bAuthorize)
                    iRcLocal = NotifyEvent();
            };
        }
        else
            iLen = 0;

        /* Verificar ultimos N segundos de inactividad */
        if(time( NULL ) > (tLastEchoLogon+_ISO8583_ECHO_TIMEOUT_))
        {
            tLastEchoLogon = time( NULL );
            DPCSENDLOGON( CNTL_ECHO_REQUERIMENT );
        }

        /* verificar acciones pendientes del sist.oper.*/
        VerifyPendingOSMessages();

        /* Verificar SAF Previo si es que autoriza,        */
        /* y si hay registros , y si la ultima transaccion */
        /* no fue enviada a SAF por rechazo SYSTEM_FAILURE */
        if( safQueue.IsActionRequired()                                  &&
            GetLastTransactionStatus() != CISO_SYSTEM_FAILURE_I          &&
            GetLastTransactionStatus() != CISO_CUTOVER_IN_PROGRESS_I     &&
            GetLastTransactionStatus() != CISO_DESTINY_NOT_AVAILABLE_I   &&
            bAuthorize    )
            /* verificar SAF */
            VerifyPendingSAF();

    }; /*end while*/

    return iRc;
}
///////////////////////////////////////////////////////////////////////////////////////
// Iniciar procesamiento (verificacion inicial de INPUT)
int DPCISO::StartProcessing( char *szHost, unsigned short usPort,
                             unsigned short usLiPort, bool xbPP_Protocol,
                             boolean_t    bLogon)
{
   if (TCP4U_SUCCESS!=Connect( szHost, usPort, usLiPort, xbPP_Protocol, iTimeout))
       return iRc; /* error */
   return StartProcessing( bLogon );
}
///////////////////////////////////////////////////////////////////////////////////////
// Verificacion de mensajes de INPUT
int DPCISO::CheckForMsgs(unsigned short    uTimeOut)
{
    char            szStrLoc[256]={0x00};
    time_t          tCurrentTime = time( NULL );
    int             iRcLocal = 0L;


    iLen = 0;
    if(!bConnect)
    {
        return (iRc = TCP4U_ERROR);
    }

    /* OK */
    iRc = TCP4U_SUCCESS;
    /* Mientras sea OK....y no haya errores del back-end...*/
    /**********************************************/
    while ( TCP4U_SUCCESS == iRc 
            &&
            _NO_COMM_EVENTS_ != hostBackEndStatus )
    /**********************************************/
    {

        /****************************************************************/
        /* segun el socket desde donde se haya recibido                 */
        if( !TcpIsDataAvail( mySock ) )
        {
            if( !TcpIsDataAvail( mySockListen ) )
                return (iRc = TCP4U_TIMEOUT);
            else
                myActiveSock = mySockListen; /* input desde socket escucha */
        }
        else
            myActiveSock = mySock;    /* input desde socket de envios */
        /****************************************************************/

        memset( szStr, 0x00, sizeof szStr );
        if(bTCPIP_PP_PROTOCOL)
            iRc = TcpPPRecv (myActiveSock, szStr, sizeof szStr, uTimeOut, FALSE, hDumpFile); /* sectimeout */
        else
            iRc = TcpRecv (myActiveSock, szStr, sizeof szStr, uTimeOut, hDumpFile); /* sectimeout */
         /* verificar longitudes retornadas */
        if(iRc<=0)
        {
             iLen = 0;
             iRc = TCP4U_TIMEOUT;
        }
        else if(iRc>0)
        {
            iLen = iRc;
            isoMsg.Import( (unsigned char *)szStr , iLen );
            iRc = TCP4U_SUCCESS;
            /* verificar si lo recibido es un requerimiento o echo */
            if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtRequest() )
            {
                iLen = sizeof szStrLoc;
                isoMsg.Export( (unsigned char *)szStrLoc, (unsigned short *)&iLen );
                FormatLineCntrlLogonResp( szStrLoc , CNTL_LOGON_RESPONSE )     ;
                iLen = strlen ( szStrLoc );
                if( bTCPIP_PP_PROTOCOL && bNETWORK_MGMT_ENABLE )
                  iRc = TcpPPSend (myActiveSock, szStrLoc, iLen, hDumpFile);
                else if( bNETWORK_MGMT_ENABLE )
                  iRc = TcpSend (myActiveSock, szStrLoc, iLen, FALSE, hDumpFile);
                else
                  iRc = TCP4U_SUCCESS;
                // debug y tracking de mensaje ECHO
                if( bNETWORK_MGMT_ENABLE )
                    trkEchoes.Write( (LPBYTE)szStrLoc, iLen );
            }
            else if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtResponse() )
            {
                // debug y tracking de mensaje ECHO
                iLen = strlen( szStr );
                trkEchoes.Write( (LPBYTE)szStr, iLen );
            }
            else if( iRc == TCP4U_SUCCESS && isoMsg.IsControlCenterCommand() )
            {
            }
            else if (TCP4U_SUCCESS == iRc)
            {
                // debug y tracking de mensaje ISO
                trkLogg.Write( (LPBYTE)szStr, iLen );
                // Notificar evento con mensaje ISO actual
                iRcLocal = NotifyEvent( );
            }

            /* verificar acciones pendientes del sist.oper.*/
            VerifyPendingOSMessages();

        }
        else
            iLen = 0;

    }; /* while */
    /* RETURN */
    return iRc;
}
///////////////////////////////////////////////////////////////////////////////////////
// Desconectar
int DPCISO::Disconnect()
{

   /* stop ECHO-LOGON timer */
   DeallocateEvent();

   /****************************************************************/
   /* Verificar mensajes pendientes, PRIMERO QUE NADA */
   iRc = CheckForMsgs( iTimeout ); /* if data avail, X sec time out */
   /****************************************************************/

   if(bConnect)
   {
     /* close socket */
     if(-1 != mySock)
        TcpClose (&mySock);
     /* close socket */
     if(-1 != mySockListen)
        TcpClose (&mySockListen);
     /* no more COMMUNICATION events */
     hostStatus = _NO_COMM_EVENTS_;
     /**************************************************/
     /* Verificar mensajes pendientes, LUEGO DEL CLOSE */
     iRc = CheckForMsgs( iTimeout );
     /**************************************************/
     /* clean up library */
     Tcp4uCleanup();
     /* connect flag to FALSE */
     bConnect = false ;
   }
   /* host unavailable */
   hostStatus = _HOST_UNAVAILABLE_;
   /* return OK */
   return (iRc = TCP4U_SUCCESS);
}
///////////////////////////////////////////////////////////////////////////////////////
// Conectarse
int DPCISO::Connect(char *szHost, unsigned short usPort, unsigned short usLiPort,
                    bool xbPP_Protocol, int iXTimeout, cobisdb *prpcXDB)
{
   /* PRECONDITION */
   if(bConnect)
       return (iRc=TCP4U_SUCCESS);

   /***********************************/
   /* enable log                      */
   Tcp4uEnableLog( LOG4U_PROC | LOG4U_ERROR | LOG4U_DUMP );
   /* get info on this library */
   Tcp4uVer (szVer, sizeof szVer);
   /* init library */
   iRc = Tcp4uInit();
   if (TCP4U_SUCCESS!=iRc)
   {
      /* host unavailable */
      hostStatus = _HOST_UNAVAILABLE_;
      return iRc; /* error */
   }
   /***********************************/

   /* copy external timeout */
   iTimeout = iXTimeout ;

   /* connet to server and wait its reply */
   /* listen on socket */
   if(usLiPort == 0)
   {
     iRc = TcpConnect (&mySock, szHost, NULL, &usPort );
     if (TCP4U_SUCCESS!=iRc)
     {
          /* host unavailable */
          hostStatus = _HOST_UNAVAILABLE_;
          /* cleanup library */
          Tcp4uCleanup();
          return iRc; /* error */
     }
     /* connect to server : ok */
     bConnect = true;
     /* host ready */
     hostStatus = _HOST_READY_;

   }


   /* listen on socket */
   if(usLiPort != 0)
   {
      unsigned short usTemp = usLiPort ;
      SOCKET         LSock  = 0;

      iRc = TcpGetListenSocket (& LSock, "dpciso", & usTemp, 1); /* 1 pending connection */
      if (iRc!=TCP4U_SUCCESS)
      {
        /* host unavailable */
        hostStatus = _HOST_UNAVAILABLE_;
        /* cleanup library */
        Tcp4uCleanup();
        return iRc; /* error */
      }
      iRc = TcpAccept( &mySockListen, LSock, iTimeout ); /* N secs. for timeout, 0 = for ever*/
      if (TCP4U_SUCCESS!=iRc)
      {
        /* host unavailable */
        hostStatus = _HOST_UNAVAILABLE_;
        /* cleanup library */
        Tcp4uCleanup();
        return iRc; /* error */
      }
      /* connect from server : ok */
      bConnect = true;
      /* host ready */
      hostStatus = _HOST_READY_;
   }

   /* backup */
   strcpy(szISOHost, szHost);
   usISOPort   = usPort;
   usISOLiPort = usLiPort;

   /* start timer */
   AllocateEvent();

   /* TCP PP protocol ? */
   bTCPIP_PP_PROTOCOL    = xbPP_Protocol;

   /***************************************************************/
    if(-1 == myActiveSock)
        myActiveSock = mySock;
    if(-1 == myActiveSock)
        myActiveSock = mySockListen;
    if(-1 == myActiveSock)
    {
        /* host unavailable */
        hostStatus = _HOST_UNAVAILABLE_;
        return (iRc = TCP4U_ERROR);
    }
    /***************************************************************/

   /* db connectivity point */
   if(prpcXDB != NULL)
     prpcDB = prpcXDB;

   /* return exit code */
   return iRc; /* ok */
}
///////////////////////////////////////////////////////////////////////////////////////
// Reconectarse
int DPCISO::Reconnect( )
{
   if(bConnect)
        return (iRc=TCP4U_SUCCESS);
   else
        return Connect( szISOHost, usISOPort, usISOLiPort, bTCPIP_PP_PROTOCOL, iTimeout) ;
}
///////////////////////////////////////////////////////////////////////////////////////
// Enviar req. esperando Rta.
int DPCISO::SendReqAndWaitResp(short xbPP_Protocol,  unsigned uTimeOut)
{
   char           szStrLoc[256]={0x00};
   /*******************************************************/
   /* variable de retorno de la longitud es SHORT 16 bits */
   unsigned short shLen = 0;
   /*******************************************************/

   /* OK */
   iRc  = TCP4U_SUCCESS;

   /* get ISO8583 msg */
   shLen = iLen = sizeof(szStr);
   isoMsg.Export( (unsigned char *)szStr, &shLen );
   iLen = shLen ;
   if( iLen <= 0)
       return (iRc=TCP4U_ERROR);

   /* PP protocol (2 first bytes contain length of data) */
   if(xbPP_Protocol)
      iRc = TcpPPSend (myActiveSock, szStr, iLen, hDumpFile);
   else
      iRc = TcpSend (myActiveSock, szStr, iLen, FALSE, hDumpFile);

   do {

       /* uTimeOut > 0 : wait for response */
       if (TCP4U_SUCCESS==iRc && uTimeOut > 0)
       {
            /* esperar rta. */
            memset( szStr, 0x00, sizeof szStr );
            if(xbPP_Protocol)
                iRc = TcpPPRecv (myActiveSock, szStr, sizeof szStr, uTimeOut, FALSE, hDumpFile); /* sectimeout */
            else
                iRc = TcpRecv (myActiveSock, szStr, sizeof szStr, uTimeOut, hDumpFile); /* sectimeout */

             /* verificar longitudes retornadas */
            if(iRc<=0)
            {
                iLen = 0;
            }
            else if(iRc>0)
            {
                iLen = iRc;
                isoMsg.Import( (unsigned char *)szStr , iLen );
                iRc = TCP4U_SUCCESS;
                /* verificar si lo recibido es un requerimiento o echo */
                if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtRequest() )
                {
                    iLen = sizeof szStrLoc;
                    isoMsg.Export( (unsigned char *)szStrLoc, (unsigned short *)&iLen );
                    FormatLineCntrlLogonResp( szStrLoc , CNTL_LOGON_RESPONSE )     ;
                    iLen = strlen ( szStrLoc );
                    if( bTCPIP_PP_PROTOCOL && bNETWORK_MGMT_ENABLE )
                        iRc = TcpPPSend (myActiveSock, szStrLoc, iLen, hDumpFile);
                    else if( bNETWORK_MGMT_ENABLE )
                        iRc = TcpSend (myActiveSock, szStrLoc, iLen, FALSE, hDumpFile);
                    else
                        iRc = TCP4U_SUCCESS;
                    // debug y tracking de mensaje ECHO
                    if( bNETWORK_MGMT_ENABLE )
                        trkEchoes.Write( (LPBYTE)szStrLoc, iLen );
                }
                else if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtResponse() )
                {
                    // debug y tracking de mensaje ECHO
                    iLen = strlen( szStr );
                    trkEchoes.Write( (LPBYTE)szStr, iLen );
                }
                else if( iRc == TCP4U_SUCCESS && isoMsg.IsControlCenterCommand() )
                {
                }
                else if (TCP4U_SUCCESS == iRc)
                {
                    // debug y tracking de mensaje ISO
                    trkLogg.Write( (LPBYTE)szStr, iLen );
                    // NO NOTIFICAR EVENTO CUANDO ESTAMOS ENVIANDO UN REQUERIMIENTO,
                    // SINO PODEMOS ENTRAR EN UN LOOP INFINITO
                };
            }
            else
           {
               strcpy(szStr, _ISO_MSG_HEADER_);
               iLen = strlen(szStr);
               /* update ISO8583 msg */
               isoMsg.Import( (unsigned char *)szStr, (unsigned short)iLen );
           };
       }
       else
       {
           strcpy(szStr, _ISO_MSG_HEADER_);
           iLen = strlen(szStr);
           /* update ISO8583 msg */
           isoMsg.Import( (unsigned char *)szStr, (unsigned short)iLen );
       };

   }
   while ( TCP4U_SUCCESS==iRc &&
       (isoMsg.IsNetworkMgmtResponse() ||
        isoMsg.IsNetworkMgmtRequest()  ||
        isoMsg.IsControlCenterCommand() )
        );

   /* return */
   return (iRc) ;
}

///////////////////////////////////////////////////////////////////////////////////////
// Enviar requerimiento sin esperar rta.
int DPCISO::SendReqNoWait(short xbPP_Protocol,  unsigned uTimeOut)
{
   char            szStrLoc[256]={0x00};
   /*******************************************************/
   /* variable de retorno de la longitud es SHORT 16 bits */
   unsigned short  shLen = 0;
   int             iRcLocal = 0L;
   /*******************************************************/

   /* OK */
   iRc  = TCP4U_SUCCESS;

   /* get ISO8583 msg */
   shLen = iLen = sizeof(szStr);
   isoMsg.Export( (unsigned char *)szStr, &shLen );
   iLen = shLen;
   if( iLen <= 0)
       return (iRc=TCP4U_ERROR);

   /* PP protocole (2 first bytes contain length of data) */
   if(xbPP_Protocol)
      iRc = TcpPPSend (myActiveSock, szStr, iLen, hDumpFile);
   else
      iRc = TcpSend (myActiveSock, szStr, iLen, FALSE, hDumpFile);
   /* uTimeOut > 0 : wait for response */
   if (TCP4U_SUCCESS==iRc && uTimeOut > 0)
   {
        iRc = CheckForMsgs( uTimeOut ); /* if data avail, X sec time out */
        /* verificar si lo recibido es un requerimiento o echo */
        if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtRequest() )
        {
            iLen = sizeof szStrLoc;
            isoMsg.Export( (unsigned char *)szStrLoc, (unsigned short *)&iLen );
            FormatLineCntrlLogonResp( szStrLoc , CNTL_LOGON_RESPONSE )     ;
            iLen = strlen ( szStrLoc );
            if( bTCPIP_PP_PROTOCOL && bNETWORK_MGMT_ENABLE )
                iRc = TcpPPSend (myActiveSock, szStrLoc, iLen, hDumpFile);
            else if( bNETWORK_MGMT_ENABLE )
                iRc = TcpSend (myActiveSock, szStrLoc, iLen, FALSE, hDumpFile);
            else
                iRc = TCP4U_SUCCESS;
            // debug y tracking de mensaje ECHO
            if( bNETWORK_MGMT_ENABLE )
                trkEchoes.Write( (LPBYTE)szStrLoc, iLen );
        }
        else if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtResponse() )
        {
            // debug y tracking de mensaje ECHO
            iLen = strlen( szStr );
            trkEchoes.Write( (LPBYTE)szStr, iLen );
        }
        else if( iRc == TCP4U_SUCCESS && isoMsg.IsControlCenterCommand() )
        {
        }
        else if (TCP4U_SUCCESS == iRc)
        {
           // debug y tracking de mensaje ISO
           trkLogg.Write( (LPBYTE)szStr, iLen );
           // Notificar Evento
           iRcLocal = NotifyEvent();
        };

   };
   /* return ok-error */
   return (iRc) ;
}
///////////////////////////////////////////////////////////////////////////////////////
// ID local TCP-IP
int DPCISO::GetLocalID (char *szStrName, int uNameSize, unsigned long *lpAddress)
{
    if(szStrName && uNameSize > 0 && lpAddress)
    {
        iRc = TCP4U_SUCCESS;
        /* init library */
        if(!bConnect)
        {
            iRc = Tcp4uInit();
            if (TCP4U_SUCCESS!=iRc)
                return (iRc); /* error */
        }
        iRc = TcpGetLocalID ( szStrName, uNameSize, lpAddress);
        /* clean up library */
        if(!bConnect)
            Tcp4uCleanup();
        return (iRc); /* error ? ok ? */
    }
    else
        return (iRc=TCP4U_ERROR) ;
}

///////////////////////////////////////////////////////////////////////////////////////
// Formatear ECHO-LOGON
int DPCISO::FormatLineCntrlLogonReq( enumCntlType wCntlCode )
{
    /* Mensajes de Control de Linea  (Echo Test=301, Logon=1 u Logoff=2) */
    char       szDateTime[16];             // String de DateTime
    char       szSeqNum[16];               // String de Sequence Number
    const char szEOT[2] = { 0x3, 0 };      // End-Of-Text (ASCII-3)



    /* Fecha y Hora actuales del Sistema Operativo (ANSI C Unix) */
    time_t tTime         = time( NULL );        // System Time
    tm     *ptmLocalTime = localtime(&tTime);    // Local System Time
    strftime(szDateTime, sizeof szDateTime, "%m%d%H%M%S", ptmLocalTime);


    // Armado del mensaje ISO-8583, campo a campo, como esta predefinido
    // en el Manual del Sistema, en Base24-Host-Interchange, y en
    // Manual de Mensajes ISO-8583. En su defecto remitirse al codigo fuente.

    // ISO-Header-Indicator [0..2=3]
    strcpy(&szStr[0], _ISO_MSG_HEADER_);
    // Header [3..11=9]
    strcpy(&szStr[3], "014000050"); // B24 Host Input (issuer HOST24)
    // Message-Type-Identifier [12..15=4]
    strcpy(&szStr[12], "0800"); // Network Mgm Requirement
    // Primary-Bitmap [16..31=16]
    strcpy(&szStr[16], "8220000000000000"); // B24 In 0800
    // Optional-Secondary-Bitmap [32..47=16]
    strcpy(&szStr[32], "0400000000000000"); // B24 In 0800
    // Transmision-Date-Time [48..57=10]
    strcpy(&szStr[48], szDateTime);
    // PseudoRandom Trace Number
    sprintf(szSeqNum, "%06i", rand() );
    // Trace-Number [58..63=6]
    strcpy(&szStr[58], szSeqNum);
    // Req Network-Management-Information-Code [64..66=3]
    if (wCntlCode == CNTL_ECHO_REQUERIMENT)
        strcpy(&szStr[64], "301"); // Echo
    else if(wCntlCode == CNTL_LOGON_REQUERIMENT)
        strcpy(&szStr[64], "001"); // Logon
    else
        strcpy(&szStr[64], "002"); // Logoff
    // End-Of-Transmission [67=1]
    strcpy(&szStr[67], szEOT);
    // Import ISO8583
    isoMsg.Import( (unsigned char *)szStr , 68 );

    // bytes?
    return iLen=68;

}
///////////////////////////////////////////////////////////////////////////////////////
// Formatear rta. de ECHO-LOGON
int DPCISO::FormatLineCntrlLogonResp( char *szInMsg , enumCntlType wCntlCode )
{
    /* Mensajes de Control de Linea  (Echo Test=301, Logon=1 u Logoff=2) */
    char       szDateTime[16];             // String de DateTime
    char       szSeqNum[16];               // String de Sequence Number
    const char szEOT[2] = { 0x3, 0 };      // End-Of-Text (ASCII-3)
    unsigned short wLen = 0;



    /* Fecha y Hora actuales del Sistema Operativo (ANSI C Unix) */
    time_t tTime         = time( NULL );        // System Time
    tm     *ptmLocalTime = localtime(&tTime);    // Local System Time
    strftime(szDateTime, sizeof szDateTime, "%m%d%H%M%S", ptmLocalTime);


    // Armado del mensaje ISO-8583, campo a campo, como esta predefinido
    // en el Manual del Sistema, en Base24-Host-Interchange, y en
    // Manual de Mensajes ISO-8583. En su defecto remitirse al codigo fuente.

    // Header [3..11=9]
    strcpy(&szStr[3], "014000055"); // B24 Host Input/Output (issuer-acquirer HOST24)
    // Message-Type-Identifier [12..15=4]
    strcpy(&szStr[12], "0810"); // Network Mgm Response
    // Primary-Bitmap [16..31=16]
    strcpy(&szStr[16], "8220000002000000"); // B24 Out 0810
    // Optional-Secondary-Bitmap [32..47=16]
    strcpy(&szStr[32], "0400000000000000"); // B24 Out 0810
    // Transmision-Date-Time [48..57=10]
    if(    (szInMsg) &&
        (strncmp("0101000000", szInMsg+48, 10) <= 0) &&
        (strncmp("1231235959", szInMsg+48, 10) >= 0))
    {
        strncpy(szDateTime, szInMsg+48, 10 );
        strcpy(&szStr[48], szDateTime);
    }
    else
        strcpy(&szStr[48], szDateTime);
    // InputMessage-Trace-Number ?
    if(    (szInMsg) &&
        (strncmp("000000", szInMsg+58, 6) <= 0) &&
        (strncmp("999999", szInMsg+58, 6) >= 0))
        {
        // Trace-Number [58..63=6]
        strncpy(szSeqNum, szInMsg+58, 6);
        szSeqNum[6] = 0x00;
        }
    else
        {
        // Default Trace-Number [58..63=6]
        strcpy(szSeqNum, "000000");
        };
    // Trace-Number [58..63=6]
    strcpy(&szStr[58], szSeqNum);
    // Response-Code [64..65=2]
    strcpy(&szStr[64], "00"); // Approved
    // InputMessage-Network-Mgm-Information-Code?
    if(    (szInMsg) &&
        ((strncmp(szInMsg+64, "001", 3) == 0) ||
         (strncmp(szInMsg+64, "002", 3) == 0) ||
         (strncmp(szInMsg+64, "301", 3) == 0))
      )
        {
        // In Network-Management-Information-Code [64..66=3]
        // Out Network-Management-Information-Code [66..68=3]
        strncpy(&szStr[66], szInMsg+64, 3);
        szStr[69] = 0x00;
        }
    else
        {
        // Default Network-Management-Information-Code [66..68=3]
        if (wCntlCode == CNTL_ECHO_RESPONSE)
            strcpy(&szStr[66], "301"); // Echo
        else if(wCntlCode == CNTL_LOGON_RESPONSE)
            strcpy(&szStr[66], "001"); // Logon
        else
            strcpy(&szStr[66], "002"); // Logoff
        }
    // End-Of-Transmission [69=1]
    strcpy(&szStr[69], szEOT);
    // Import ISO8583
    isoMsg.Import( (unsigned char *)szStr , 70 );
    // In Msg ?
    if(    szInMsg )
    {
        wLen = 70;
        // Export formatted msg
        isoMsg.Export( (unsigned char *)szInMsg, &wLen );
        // bytes?
        return wLen;
    }
    else
        // bytes?
        return iLen=70;

}
///////////////////////////////////////////////////////////////////////////////////////
// Enviar logon
int DPCISO::SendLogon( enumCntlType wLogon )
{
    char            szStrLoc[256]={0x00} ;
    unsigned short  uTimeOut = iTimeout;


    // Precondicion : formato ISO8583
    if(CNTL_FORMAT_NONISO8583 == efFormat)
        return (iRc = TCP4U_ERROR);
    // Precondicion : conectado
    if (!bConnect)
        return (iRc = TCP4U_ERROR);
    // Precondicion : disponible
    if(hostStatus == _HOST_STOPPED_)
        return (iRc = TCP4U_ERROR);

    // Antes de enviar LOGON-LOGOFF, verificar lo PENDIENTE
    iRc = CheckForMsgs( uTimeOut )  ;

    // Despues, y segun lo solicitado, armar el ECHO o LOGON
    if( wLogon == CNTL_LOGOFF_REQUERIMENT )
        iLen = FormatLineCntrlLogonResp( NULL, wLogon);
    else if( wLogon == CNTL_LOGON_REQUERIMENT )
        iLen = FormatLineCntrlLogonReq( wLogon );
    else
        iLen = FormatLineCntrlLogonReq( wLogon );

    // Segun Protocolo, enviar mensaje de ECHO LOGON
    if(bTCPIP_PP_PROTOCOL && bNETWORK_MGMT_ENABLE )
      iRc = TcpPPSend (myActiveSock, szStr, iLen, hDumpFile);
    else if ( bNETWORK_MGMT_ENABLE )
      iRc = TcpSend (myActiveSock, szStr, iLen, FALSE, hDumpFile);
    else
      iRc = TCP4U_SUCCESS;
    // debug y tracking de mensaje ECHO
    if( bNETWORK_MGMT_ENABLE )
        trkEchoes.Write( (LPBYTE)szStr, iLen );


    /* uTimeOut > 0 : wait for response */
    if (TCP4U_SUCCESS==iRc && uTimeOut > 0)
    {
        memset( szStr, 0x00, sizeof szStr );
        if(bTCPIP_PP_PROTOCOL)
            iRc = TcpPPRecv (myActiveSock, szStr, sizeof szStr, uTimeOut, FALSE, hDumpFile); /* sectimeout */
        else
            iRc = TcpRecv (myActiveSock, szStr, sizeof szStr, uTimeOut, hDumpFile); /* sectimeout */
    }
    else
        iRc = 0;
     /* verificar longitudes retornadas */
    if(iRc<=0)
    {
         iLen = 0;
         iRc = TCP4U_TIMEOUT;
    }
    else if(iRc>0)
    {
        iLen = iRc;
        isoMsg.Import( (unsigned char *)szStr , iLen );
        iRc = TCP4U_SUCCESS;
        /* verificar si lo recibido es un requerimiento o echo */
        if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtRequest() )
        {
            iLen = sizeof szStrLoc;
            isoMsg.Export( (unsigned char *)szStrLoc, (unsigned short *)&iLen );
            FormatLineCntrlLogonResp( szStrLoc , CNTL_LOGON_RESPONSE )     ;
            iLen = strlen ( szStrLoc );
            if( bTCPIP_PP_PROTOCOL && bNETWORK_MGMT_ENABLE )
                iRc = TcpPPSend (myActiveSock, szStrLoc, iLen, hDumpFile);
            else if ( bNETWORK_MGMT_ENABLE )
                iRc = TcpSend (myActiveSock, szStrLoc, iLen, FALSE, hDumpFile);
            else
                iRc = TCP4U_SUCCESS;
            // debug y tracking de mensaje ECHO
            if ( bNETWORK_MGMT_ENABLE )
                trkEchoes.Write( (LPBYTE)szStrLoc, iLen );
        }
        else if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtResponse() )
        {
            // debug y tracking de mensaje ECHO
            iLen = strlen( szStr );
            trkEchoes.Write( (LPBYTE)szStr, iLen );
        }
        else if( iRc == TCP4U_SUCCESS && isoMsg.IsControlCenterCommand() )
        {
        }
        else if (TCP4U_SUCCESS == iRc)
        {
            // debug y tracking de mensaje ISO
            trkLogg.Write( (LPBYTE)szStr, iLen );
            // Notoficar evento
            NotifyEvent();
        }

        /* Verificar ultimos N segundos de inactividad */
        if(time( NULL ) > (tLastEchoLogon+_ISO8583_ECHO_TIMEOUT_))
        {
            tLastEchoLogon = time( NULL );
            DPCSENDLOGON( CNTL_LOGON_REQUERIMENT );
        }

        /* verificar acciones pendientes del sist.oper.*/
        VerifyPendingOSMessages();

    }
    else
        iLen = 0;
   /* return */
   return iRc;
}

///////////////////////////////////////////////////////////////////////////////////////
// Enviar logoff
int DPCISO::SendLogoff()
{
    return SendLogon( CNTL_LOGOFF_REQUERIMENT );
}
///////////////////////////////////////////////////////////////////////////////////////
// Recuperar msg
int DPCISO::GetMsg (char *szMsg, unsigned short *piLen )
{
    if( szMsg && piLen )
        return isoMsg.Export( (unsigned char *)szMsg , piLen );
    else
        return TCP4U_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////
// Setear msg
int DPCISO::SetMsg (char *szMsg, unsigned short iLen )
{
    if( szMsg && iLen > 0)
        return isoMsg.Import( (unsigned char *)szMsg , iLen );
    else
        return TCP4U_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////
// callback timer
VOID CALLBACK DPCISO::TimerProc(HWND hWindow, UINT uTimer, UINT uInt, DWORD uDWord)
{
    char szStrLoc[512]={0x00};
    int  iRc=TCP4U_ERROR;
    time_t tCurrentTime = time( NULL );

    /* stop ECHO-LOGON timer */
    DeallocateEvent();

    /* verificar mensajes si es TIMER EVENT CHECKING */
    iLen = 0;
    if( eventType == _TIMER_EVENT_CHECKING_ )
    {
        /*******************************************/
        iRc = CheckForMsgs( 1 ); /* 1 sec time out */
        /*******************************************/

        /* verificar si lo recibido es un requerimiento o echo */
        if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtRequest() )
        {
            iLen = sizeof szStrLoc;
            isoMsg.Export( (unsigned char *)szStrLoc, (unsigned short *)&iLen );
            FormatLineCntrlLogonResp( szStrLoc , CNTL_LOGON_RESPONSE )     ;
            if( bTCPIP_PP_PROTOCOL && bNETWORK_MGMT_ENABLE )
              iRc = TcpPPSend (mySock, szStr, iLen, hDumpFile);
            else if ( bNETWORK_MGMT_ENABLE )
                iRc = TcpSend (myActiveSock, szStrLoc, iLen, FALSE, hDumpFile);
            else
                iRc = TCP4U_SUCCESS;
            // debug y tracking de mensaje ECHO
            if( bNETWORK_MGMT_ENABLE )
                trkEchoes.Write( (LPBYTE)szStr, iLen );
        }
        else if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtResponse() )
        {
            // debug y tracking de mensaje ECHO
            iLen = strlen( szStr );
            trkEchoes.Write( (LPBYTE)szStr, iLen );
        }
        else if( iRc == TCP4U_SUCCESS && isoMsg.IsControlCenterCommand() )
        {
        }
        else if (TCP4U_SUCCESS == iRc)
        {
            NotifyEvent();
        }
        else if (iRc == TCP4U_ERROR)
        {
        }
        else
        {
            Disconnect();
            Reconnect();
        };
    }; /* fin IF */

    /* Verificar ultimos N segundos de inactividad y enviar ECHO TEST-LOGON */
    /* O si vencio el TIMER DE ECHO */
    if(time( NULL ) >= (tLastEchoLogon+_ISO8583_ECHO_TIMEOUT_) || 1 == uInt)
    {
        tLastEchoLogon = time( NULL );
        DPCSENDLOGON( CNTL_LOGON_REQUERIMENT );
    }; /* fin IF */

    /* start ECHO-LOGON timer */
    AllocateEvent();

}

///////////////////////////////////////////////////////////////////////////////////////
// Alojar evento (timer)
long DPCISO::AllocateEvent()
{
    if ( _INNER_LOOP_CHECKING_ == eventType)
    {
#if defined( ___TIMER_EVENT_CHECKING_ )
        UINT uiRet = 0;
        uiRet = SetTimer( (NULL != hHandle)
                            ? hHandle
                            : NULL,
                            1, _ISO8583_ECHO_TIMEOUT_MSEG_, DPCTIMERPROC ); /* TIME ECHO TEST, 15 SEG */
#endif
        /* NO LANZAR TIMER DE POLLING DE MSGS., PORQUE ES INNER-LOOP EL CHEQUEO */
        return 0;
    }
    else
    {
        UINT uiRet = 0;
#if defined( ___TIMER_EVENT_CHECKING_ )
        uiRet = SetTimer( (NULL != hHandle)
                            ? hHandle
                            : NULL,
                            1, _ISO8583_ECHO_TIMEOUT_MSEG_, DPCTIMERPROC ); /* TIME ECHO TEST, 15 SEG */
        uiRet = SetTimer( (NULL != hHandle)
                            ? hHandle
                            : NULL,
                            2, _POLLING_TIMEOUT_MSEG_, DPCTIMERPROC ); /* TIMER POLLING, 1 SEG */
#endif
        return uiRet ;
    }
}
///////////////////////////////////////////////////////////////////////////////////////
// Desalojar evento (timer)
long DPCISO::DeallocateEvent()
{
    if ( _INNER_LOOP_CHECKING_ == eventType)
    {
#if defined( ___TIMER_EVENT_CHECKING_ )
        KillTimer( NULL, 1 ); /* TIMER DE ECHO */
#endif
        /* NO HAY TIMER DE POLLING PORQUE ES INNER-LOOP */
        return 0;
    }
    else
    {
#if defined( ___TIMER_EVENT_CHECKING_ )
        KillTimer( NULL, 1 ); /* TIMER DE ECHO */
        KillTimer( NULL, 2 ); /* TIMER DE POLLING DE MENSAJES */
#endif
        return 0;
    }
    return 0;
}

/* Funcion de busqueda de un STRING en un buffer de memoria binario, generico */
void *memstr( const void *vBuffer, const char szFind[], const size_t nSize )
{
    const char *ptrAddress = (char *)vBuffer;
    char *ptrBuffer  = (char *)vBuffer;
    char *ptrFirst   = NULL;
    size_t  nLenStr  = 0;
    size_t  iCounter = 0;
    /* precondition */
    if( NULL == vBuffer || NULL == szFind || nSize <= 0)
        return NULL;
    while( (ptrFirst = (char *)memchr( ptrBuffer, szFind[0], nSize)) != NULL )
    {
        for(iCounter=0, nLenStr = strlen(szFind);
            iCounter < nLenStr && szFind[iCounter] == ptrFirst[iCounter];
            iCounter++ )
            ;
        /* check for string length */
        if(iCounter >= nLenStr)
            /* return PTR to first char */
            return ptrFirst;
        else
        {
            if((ptrBuffer+1) <= (ptrAddress + nSize))
                ptrBuffer++;
            else
                /* return NO PTR */
                return NULL;
        };
    };
    /* return NO PTR */
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
// Disparar evento de notificacion (mensaje de notificacion)
// Este procedimiento verifica la existencia o no de mensajes concatenados y desarrolla
// la logica para manejarlos individualmente. Esto se da con protocolo TCP/IP y con un
// buffer de recepcion de hasta 20k bytes. Pueden venir fragmentados tambien, y tambien
// se desarrolla la logica para manejar eso, con un buffer de backup.
///////////////////////////////////////////////////////////////////////////////////////
long DPCISO::NotifyEvent( boolean_t bFromSAFQueue )
{
    // Verificar mensajes ISO concatenados, comenzando con ASCII-2,3,4, o 5
    char    *ptrBegin     = szStr,
            *ptrEnd       = szStr,
            *ptrISO       = szStr,
            *ptrISOBackup = NULL; /* backup de mensaje actual */
    short    nLenISO    = 0;
    bool    bAppendISO = false;
    short    nCounter        = 0;
    /* Constantes de limitacion */
    const short cnMaxValidLength = 1024; /* max trx = 1024 bytes */
    const short    cnLoopLimit      = 20480 / (cnMaxValidLength/2); /* max frame = 20k */

    /* Inicio valido del mensaje ISO es "ISO" para ATM y POS */
    ptrBegin = (char *)memstr( szStr, _ISO_MSG_HEADER_ATM_POS_ , iLen );
    if(!ptrBegin)
        return ( iRc = TCP4U_ERROR );
    /* Busco el proximo mensaje concatenado si hubiere ETX o EOT o NUL */
    /* Inicio valido del mensaje ISO es "ISO" para ATM y POS */
    ptrEnd = (char *)memstr( ptrBegin + 1, _ISO_MSG_HEADER_ATM_POS_ , iLen );
    if(ptrEnd)
        bAppendISO = true; /* Hay concatenados */
    else
        bAppendISO = false; /* No hay concatenados */
    /* Se hallo un concatenado ? */
    if(!ptrEnd && false == bAppendISO)
    {
        ptrEnd     = ptrBegin;
        bAppendISO = false; /* No hay concatenados */
    }
    else if(!ptrEnd && true == bAppendISO)
        ptrEnd     = ptrBegin;
    else
        bAppendISO = true; /* Hay concatenados */

    /* Continua un mensaje fragmentado? Unir las partes desde el backup... */
    if( (ptrBegin - szStr) > 1 && iLenBackup > 0)
    {
        memcpy( szStrBackup + iLenBackup, szStr, ptrBegin - szStr );
        iLenBackup += ptrBegin - szStr;
        szStrBackup[iLenBackup] = 0x00;
        /* Procesar mensaje individual ISO para ATM y POS */
        ptrISO  = (char *)memstr( szStrBackup, _ISO_MSG_HEADER_ATM_POS_, iLen );
        if(ptrISO)
        {
            /* Enviar a SAF el mensaje individual concatenado y partido */
            nLenISO = iLenBackup;
            StoreInSAFQueue( szStrBackup, iLenBackup );
            /* backup de mensaje actual */
            ptrISOBackup = ptrISO;
        };
        /* Limpiar backup  */
        iLenBackup = 0;
        memset( szStrBackup, 0x00, sizeof szStrBackup );

    }
    /* Mientras haya mensajes concatenados, o sino, siempre sea el  1er mensaje.  */
    for(    nCounter = 0;
            ptrBegin != NULL && nCounter < cnLoopLimit && iLen > 0;
            nCounter++ )
        {
        /* Mientras haya mensajes concatenados con STX o EOT o NUL...*/
        if( true == bAppendISO )
        {
            /* Busco un inicio valido del msg ISO para ATM y POS */
            ptrISO = ptrBegin;
            /* Busco el proximo mensaje concatenado si hubiere ETX o EOT */
            /* Inicio valido del mensaje ISO es "ISO" para ATM y POS */
            ptrEnd = (char *)memstr( ptrBegin + 1, _ISO_MSG_HEADER_ATM_POS_ , iLen );
            /* si no hay proximo, pero habia un inicio.... */
            if( ptrISO && (NULL == ptrEnd || (NULL != ptrEnd && ptrEnd == ptrISO))
              )
            {
                /* Atencion, mensaje partido, continua en proximo frame TCP */
                nLenISO    = strlen( ptrISO );
                iLenBackup = nLenISO;
                memcpy( szStrBackup, ptrISO, iLenBackup );
                szStrBackup[iLenBackup] = 0x00;
                /* Verificar caracteres finales , si son de CONTROL */
                if( szStrBackup[iLenBackup-1] <= 31 )
                {
                    szStrBackup[iLenBackup-1] = 0x00;
                    iLenBackup--;
                }
                if( szStrBackup[iLenBackup-1] <= 31 )
                {
                    szStrBackup[iLenBackup-1] = 0x00;
                    iLenBackup--;
                }
                /* Salir */
                return iRc;
            }
            else
            {
                /* Longitud ISO reconocido */
                nLenISO = ptrEnd - ptrBegin;
            };
        }
        else
        {
            /* Buscar el inicio valido del mensaje ISO para ATM y POS */
            ptrISO     = (char *)memstr( ptrBegin, _ISO_MSG_HEADER_ATM_POS_, iLen  );
            nLenISO    = iLen;
        };

        /* Procesar mensaje individual valido , no concatenado */
        if(ptrISO && ptrISOBackup != ptrISO && nLenISO <= cnMaxValidLength && !bAppendISO )
        {
            iRc = NotifyIndividualEvent( ptrISO, nLenISO, bFromSAFQueue );
            ptrISOBackup = ptrISO;
            iLenBackup   = nLenISO;
        }
        /* Enviar a SAF el mensaje individual concatenado */
        else if (ptrISO && ptrISOBackup != ptrISO && nLenISO <= cnMaxValidLength && bAppendISO)
        {
            StoreInSAFQueue( ptrISO, nLenISO );
            ptrISOBackup = ptrISO;
            iLenBackup   = nLenISO;
        }
        /* Si el mensaje ya fue procesado, descartar */
        else
            break;

        /* Resto longitud actual de transaccion a longitud original */
        iLen -= nLenISO;

        /* Inicio valido del mensaje ISO es "ISO" para ATM y POS */
        ptrBegin = (char *)memstr( ptrBegin + 1, _ISO_MSG_HEADER_ATM_POS_ , iLen + nLenISO );
        /* Inicio valido del mensaje ISO es "ISO" para ATM y POS */
        if(ptrBegin)
            ptrEnd = (char *)memstr( ptrBegin + 1, _ISO_MSG_HEADER_ATM_POS_ , iLen );
        else
            ptrEnd = NULL;

        /******************************************/
        if( _NO_COMM_EVENTS_ == hostBackEndStatus )
            break;
        /******************************************/


    }; /* end while */
    /* Retorno del Cod. Rta     */
    return iRc;
}
///////////////////////////////////////////////////////////////////////////////////////
// Enviar a SAF el mensaje ISO actual
int DPCISO::StoreInSAFQueue( char *ptrISO, int nLenISO )
{
    DWORD   dwTrace       = 0,
            dwReferral    = 0;
    CHAR    chNumber[12]  = {0x00};
    WORD    shLen         = sizeof(chNumber);

    // precondicion : punteros y lognitud validas !
    if( NULL == ptrISO || nLenISO <= 0)
        // Error
        return (-1);
    // precondicion : estar habilitado el parametro de envio a SAF
    if(!bReversalToSAF_SystemFailure)
        return (-2);

    // Imoprtar mensaje ISO y obtener el TRACE-NUM y REF-NUM
    isoMsg.Import( (unsigned char *)ptrISO, nLenISO );
    shLen      = sizeof(chNumber);
    isoMsg.GetField( 11, &shLen, (PBYTE)chNumber,sizeof(chNumber)); // TRACE-NUM
    dwTrace    = antoi( chNumber, 6 );
    shLen      = sizeof(chNumber);
    isoMsg.GetField( 37, &shLen, (PBYTE)chNumber,sizeof(chNumber)); // REFERRAL-NUM
    dwReferral = antoi( chNumber, 9 );
    safQueue.WriteStored( (LPBYTE)ptrISO, nLenISO, dwTrace, dwReferral );
    // OK
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
// Disparar evento de notificacion (mensaje de notificacion individual)
long DPCISO::NotifyIndividualEvent( char *ptrISO, int nLenISO, boolean_t bFromSAFQueue )
{
    //////////////////////////////////////////
    // Precondicion : Backend habilitado
    if(_NO_COMM_EVENTS_ == hostBackEndStatus )
        return (iRc = TCP4U_CANCELLED);
    //////////////////////////////////////////

    /* segun tipo de notificacion de evento */
    if ( _INNER_LOOP_CHECKING_ == eventType)
    {

        //////////////////////////////////////////////////////////////////////////////
        // Instancia dinamica de resolucion de transacciones.
        // U opcionalmente, instancia estatica creada solo la primera vez.
        //////////////////////////////////////////////////////////////////////////////
        const boolean_t         bcDYNAMIC_LOCAL_INSTANCE = is_true;
        static TrxResolution *pTrxResolver              = NULL;
        //////////////////////////////////////////////////////////////////////////////
        if((CNTL_FORMAT_ISO8583_COBIS         == efFormat   ||
                 CNTL_FORMAT_ISO8583_COBIS_PREHOMO == efFormat ) &&
                NULL == pTrxResolver )
            pTrxResolver    = new TrxResolution ( (PVOID)this->prpcDB );
        /////////////////////////////////////////////////////////////////////////////
        else if((CNTL_FORMAT_ISO8583_TSHOP    == efFormat   ||
            CNTL_FORMAT_ISO8583_TSHOP_PREHOMO == efFormat ) &&
            NULL == pTrxResolver )
            pTrxResolver    = new TrxResolution (  );
        /////////////////////////////////////////////////////////////////////////////
        else if ( NULL == pTrxResolver )
            pTrxResolver    = new TrxResolution ();

        /////////////////////////////////////////////////////////////////////////////
        // Precondicion : instancia creada
        if( NULL == pTrxResolver )
            return ( iRc = TCP4U_INSMEMORY );
        // Atencion, al terminar, eliminar la instancia recientemente creada
        //////////////////////////////////////////////////////////////////////////////


        int        iIsoLength = sizeof(szStrOut);
        BOOL       bProcOK    = FALSE;
        DWORD      dwInternal = 0L;
        //////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////
        // Procesar la transaccion, capturando la rta. en un buffer distinto.
        // Se discrimina por tipo de MENSAJE de INTERFACE, SIAF, COBIS u ISO8583
        iLenOut = sizeof(szStrOut);            // Longitud de salida del buffer
        // Ejecutar y esperar
        bProcOK    = pTrxResolver->ProcessTransaction( (unsigned char *)ptrISO,
                                        nLenISO,
                                        (unsigned char *)szStrOut,
                                        &iIsoLength,
                                        hostStatus ,
                                        efFormat   ,
                                        &hostBackEndStatus );
        iLenOut = iIsoLength;
        //////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////
        // Guardar estado de la transaccion actual que se proceso
        SetLastTransactionStatus ( pTrxResolver->TransactionStatus() );
        // Debe enviarse a SAF ente errores eventuales ?
        //////////////////////////////////////////////////////////////////////
        if( (pTrxResolver->TransactionStatus() == CISO_SYSTEM_FAILURE_I         ||
             pTrxResolver->TransactionStatus() == CISO_CUTOVER_IN_PROGRESS_I    ||
             pTrxResolver->TransactionStatus() == CISO_DESTINY_NOT_AVAILABLE_I  ||
             hostBackEndStatus != _HOST_READY_)                                 &&
            pTrxResolver->IsValidSAFTransaction()                               &&
            bProcOK != FALSE
          )
        {
            // Enviar a la cola de SAF si es que no vino ya desde ahi
            // y si ademas se permite que los rechazos de reversos vayan a SAF-Queue
            if( bFromSAFQueue == is_false && bReversalToSAF_SystemFailure )
            {
                // Guardar en formato adecuado
                StoreInSAFQueue( ptrISO, nLenISO );
            }
            // Loggear error en archivo de errores
            trkError.Write( (LPBYTE)ptrISO, nLenISO, TRUE );
        }
        else if( (pTrxResolver->TransactionStatus() == CISO_SYSTEM_FAILURE_I ||
             pTrxResolver->TransactionStatus() == CISO_SYSTEM_ERROR_I        ||
             hostBackEndStatus != _HOST_READY_)                              &&
            bProcOK != FALSE
          )
        {
            // Loggear error en archivo de errores
            trkError.Write( (LPBYTE)ptrISO, nLenISO, TRUE );
        }
        else if( !bProcOK ) // Si no se proceso y fallo la interpretacion del msg.
        {
            // Loggear transaccion para track del error en archivo especifico
            trkError.Write( (LPBYTE)ptrISO, nLenISO, TRUE );
        };
        //////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////
        // Offset de respuesta en 2 bytes a derecha ? Para PREHOMOLOGADOR RED LINK
        // Aunque a veces este OFFSET es mas bien aleatorio.
        // Ademas de ello, la configuracion de mensajes de respuesta NO DEBE INCLUIR
        // AL CAMPO #62 (o modificar la configuracion del PREHOMOLOGADOR), como
        // tambien el tipo de RESPONDER en "5" cuando el PREHOMOLOGADOR espera "1".
        if(0 != shOFFSET_OUTPUT_MESSAGE)
        {
            // Se castea el buffer a un SHORT para escribir la longitud inicialmente
            short *pShortLen = (short *)szStrOut;
            // Se hace un offset del mensaje de N bytes a derecha
            memmove( szStrOut + shOFFSET_OUTPUT_MESSAGE, szStrOut, iLenOut );
            // Se escribe la longitud del mensaje en los 2 primeros bytes
            (*pShortLen) = htons( iLenOut );
        };
        //////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////
        // Escribir respuesta, si corresponde, ya sea online o saf
        if( pTrxResolver->IsValidSAFTransaction() && bISO8583_REPLY_SAF &&
            hostBackEndStatus == _HOST_UNAVAILABLE_  )
            // No se responde el SAF si dio error del sistema interno del banco
            // unicamente para ERROR=91 "destino no disponible"
            iRc = TCP4U_SUCCESS ;
        //////////////////////////////////////////////////////////////////////////////
        // Escribir respuesta, si corresponde, ya sea online o saf
        else if( pTrxResolver->IsValidSAFTransaction() && !bISO8583_REPLY_SAF )
            // No se responde el SAF en el PREHOMO
            iRc = TCP4U_SUCCESS ;
        else if( bTCPIP_PP_PROTOCOL && is_false == bFromSAFQueue )
            // Si se responde ONLINE, protocolo PP-TCP/IP
            iRc = TcpPPSend (myActiveSock, szStrOut, iLenOut, hDumpFile);
        else if ( is_false == bFromSAFQueue )
            // Si se responde ONLINE, protocolo TCP/IP draft
            iRc = TcpSend (myActiveSock, szStrOut, iLenOut, FALSE, hDumpFile);
        else
            // No se responde si viene del SAF QUEUE local
            iRc = TCP4U_SUCCESS ;
        //////////////////////////////////////////////////////////////////////////////

        // debug y tracking de mensaje ISO
        trkLogg.Write( (LPBYTE)szStrOut, iLenOut );
        // Sumar 1 al contador interno de TRXs
        ulTrxCounter++;

        //////////////////////////////////////////////////////////////////////////////
        // Verificar ultimos N segundos de procesamiento de SAF continuo
        // Si es ONLINE, resetear registro de tiempo de ultimo SAF, a cero
        // Si es SAF-ONLINE, comparar con ultimo registro SAF
        if( !pTrxResolver->IsValidSAFTransaction() &&
            !pTrxResolver->IsValidSAFTransaction())
            tLastSAFProcessed = 0L;
        else
            tLastSAFProcessed = time( NULL );
        // Si es SAF-ONLINE y pasaron mas de N segundos, alertar
        if( tLastSAFProcessed > 0L &&
            time( NULL ) > (tLastSAFProcessed+_ISO8583_SAF_IN_PROGRESS_ALERT_) &&
            pTrxResolver->IsValidSAFTransaction() )
            // Estado "SAF EN PROGRESO"
            hostStatus = _SAF_IN_PROGRESS_;
        //////////////////////////////////////////////////////////////////////////////


        /////////////////////////////////////////////////////////////
        // Postcondicion : eliminar la instancia recientemente creada
        if( NULL != pTrxResolver && bcDYNAMIC_LOCAL_INSTANCE )
        {
            delete pTrxResolver;
            pTrxResolver = NULL;
        };
        /////////////////////////////////////////////////////////////

        // Retorno del Cod. Rta.
        return iRc;

    }
    else
#ifdef _WINDOWS
        /* Enviar notificacion de mensaje de Temporizador */
        return ( NULL != hHandle )
            ? SendMessage( hHandle, WM_COMMNOTIFY, 0, 0)
            : SendMessage( NULL, WM_COMMNOTIFY, 0, 0);
#else
        return 0L;
#endif // _WINDOWS
}

///////////////////////////////////////////////////////////////////////////////////////
// Notificacion del handle (timer handler) si existiera
void DPCISO::SetNotifyHandle( HWND hWinHandle )
{
#ifdef _WINDOWS
    // Un "handle" puede ser cualquier objeto del sistema operativo
    // que haga referencia a un originador de eventos asincronicos,
    // sea una ventana grafica, socket o archivo generico.
    hHandle = hWinHandle;
#endif // _WINDOWS
}
///////////////////////////////////////////////////////////////////////////////////////
// Formato de los mensajes : ISO, no-ISO o recuperar como esta establecido?
enumFormatType DPCISO::SetFormat(enumFormatType ef_XFormat)
{
    if (CNTL_FORMAT_NULL == ef_XFormat)
        return efFormat;
    // Verificar si el formato es para el PREHOMOLOGADOR de RED LINK
    // Solo para este caso, reasignar atributos internos del protocolo,
    // y segun la version del prehomologador, manejara ACK desde el DPC.
    // ACK no habilitado en el PREHOMOLOGADOR, primera version
    if(CNTL_FORMAT_ISO8583_PREHOMO       == ef_XFormat ||
       CNTL_FORMAT_ISO8583_COBIS_PREHOMO == ef_XFormat ||
       CNTL_FORMAT_ISO8583_SIAF_PREHOMO  == ef_XFormat ||
       CNTL_FORMAT_NONISO8583            == ef_XFormat )
    {
        bNETWORK_MGMT_ENABLE    = false; // Habilita manejo de echo-logon ? NO.
        bISO8583_REPLY_SAF      = false; // Habilita responder SAF y REVERSOS ? NO.
        shOFFSET_OUTPUT_MESSAGE = 2;     // Existe un OFFSET al mensaje ISO ? SI, 2.
        bTCPIP_PP_PROTOCOL      = false; // Habilita protocolo PP del TCP ? NO.
    }
    // ACK habilitado en el PREHOMOLOGADOR, segunda version?
    else if(CNTL_FORMAT_ISO8583_SIOP_PREHOMO  == ef_XFormat ||
            CNTL_FORMAT_ISO8583_TSHOP_PREHOMO == ef_XFormat ||
			CNTL_FORMAT_ISO8583_FINAN_PREHOMO == ef_XFormat ||
            CNTL_FORMAT_NONISO8583            == ef_XFormat )
    {
        bNETWORK_MGMT_ENABLE    = false; // Habilita manejo de echo-logon ? NO.
        bISO8583_REPLY_SAF      = true; // Habilita responder SAF y REVERSOS ? NO.
        shOFFSET_OUTPUT_MESSAGE = 2;     // Existe un OFFSET al mensaje ISO ? SI, 2.
        bTCPIP_PP_PROTOCOL      = false; // Habilita protocolo PP del TCP ? NO.
    }
    else
    {
        bNETWORK_MGMT_ENABLE    = true ; // Habilita manejo de echo-logon ? SI.
        bISO8583_REPLY_SAF      = true ; // Habilita responder SAF y REVERSOS ? SI.
        shOFFSET_OUTPUT_MESSAGE = 0;     // Existe un OFFSET al mensaje ISO ? NO.
        bTCPIP_PP_PROTOCOL      = true ; // Habilita protocolo PP del TCP ? SI.
    };
    // Asignar el nuevo formato de mensajeria del HOST
    return (efFormat = ef_XFormat);
}
///////////////////////////////////////////////////////////////////////////////////////
// Enviar mail via SMTP
int DPCISO::SmtpSendMail(const char *szFrom, const char *szTo, const char *szMessage,
                        const char *szHost, const char *szMyDomain )
{
    const char    *pszHostDomain = szHost;
    /* verificar si se informo el dominio */
    if(szMyDomain == NULL || szMyDomain[0] == 0x00)
        iRc = SmtpSendMessage(szFrom, szTo, szMessage, szHost, pszHostDomain);
    else
        iRc = SmtpSendMessage(szFrom, szTo, szMessage, szHost, szMyDomain);
    return iRc ;
}
///////////////////////////////////////////////////////////////////////////////////////
// Recuperar el estado de conexion si estuviera conectado
enumHostStatus DPCISO::GetStatus()
{
    return (bConnect)
        ? hostStatus
        : _HOST_UNAVAILABLE_;
};
// Establecer el tiempo de TIMEOUT en segundos
void DPCISO::SetTimeOut( int iTime )
{
    iTimeout = iTime;
} ;
///////////////////////////////////////////////////////////////////////////////////////
// Establecer el estado de conexion si estuviera conectado
enumHostStatus DPCISO::SetStatus( enumHostStatus newState )
{
    enumHostStatus oldState = hostStatus; // Save current state
    hostStatus = newState;  // Set new state
    return oldState; // Return old state
};


///////////////////////////////////////////////////////////////////////////////////////
// Recuperar el estado de conexion si estuviera conectado , para el BACK END
enumHostStatus DPCISO::GetStatusBackEnd()
{
    return hostBackEndStatus ; // return current state
};
///////////////////////////////////////////////////////////////////////////////////////
// Establecer el estado de conexion si estuviera conectado, para el BACK END
enumHostStatus DPCISO::SetStatusBackEnd( enumHostStatus newState )
{
    enumHostStatus oldState = hostBackEndStatus; // Save current state
    hostBackEndStatus = newState;  // Set new state
    return oldState; // Return old state
};

///////////////////////////////////////////////////////////////////////////////////////
void DPCISO::VerifyPendingOSMessages( void )
{
#ifdef _WINDOWS
    MSG    msg;
    ////////////////////////////////////////////////////////////////////////
    // Check for messages in this looping code
    while(PeekMessage( &msg, hHandle, 0, 0, PM_REMOVE ))
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        Sleep( 0L );
    };
    Sleep( 0L );
    ////////////////////////////////////////////////////////////////////////
#endif // _WINDOWS
}
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
unsigned long DPCISO::GetTrxCounter()
{
    return ulTrxCounter ;
};
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Enviar mensaje plano, no ISO, sin esperar rta.
int DPCISO::SendDraft(short xbPP_Protocol,  unsigned uTimeOut,
                      char *szExtStr, int iExtLen )
{
   char            szStrLoc[256]={0x00};

   /* OK */
   iRc  = TCP4U_SUCCESS;

   /* get draft msg */
   iLen = iExtLen;
   memcpy( szStr, szExtStr, iExtLen );
   if( iLen <= 0)
       return (iRc=TCP4U_ERROR);

   /* PP protocole (2 first bytes contain length of data) */
   if(xbPP_Protocol)
      iRc = TcpPPSend (myActiveSock, szStr, iLen, hDumpFile);
   else
      iRc = TcpSend (myActiveSock, szStr, iLen, FALSE, hDumpFile);
   /* uTimeOut > 0 : wait for response */
   if (TCP4U_SUCCESS==iRc && uTimeOut > 0)
   {
        iRc = CheckForMsgs( uTimeOut ); /* if data avail, X sec time out */
        /* verificar si lo recibido es un requerimiento o echo */
        if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtRequest() )
        {
            iLen = sizeof szStrLoc;
            isoMsg.Export( (unsigned char *)szStrLoc, (unsigned short *)&iLen );
            FormatLineCntrlLogonResp( szStrLoc , CNTL_LOGON_RESPONSE )     ;
            iLen = strlen ( szStrLoc );
            if( bTCPIP_PP_PROTOCOL  )
              iRc = TcpPPSend (myActiveSock, szStrLoc, iLen, hDumpFile);
            else
              iRc = TcpSend (myActiveSock, szStrLoc, iLen, FALSE, hDumpFile);
            // debug y tracking de mensaje ECHO
            trkEchoes.Write( (LPBYTE)szStrLoc, iLen );
        }
        else if( iRc == TCP4U_SUCCESS && isoMsg.IsNetworkMgmtResponse() )
        {
            // debug y tracking de mensaje ECHO
            iLen = strlen( szStr );
            trkEchoes.Write( (LPBYTE)szStr, iLen );
        }
        else if( iRc == TCP4U_SUCCESS && isoMsg.IsControlCenterCommand() )
        {
        }
        else if (TCP4U_SUCCESS == iRc)
        {
           // debug y tracking de mensaje ISO
           trkLogg.Write( (LPBYTE)szStr, iLen );
           // Notificar Evento
           NotifyEvent();
        };

   };
   /* return ok-error */
   return (iRc) ;
}

///////////////////////////////////////////////////////////////////
// Recuperar el estado de la ultima transaccion procesada
///////////////////////////////////////////////////////////////////
short DPCISO::GetLastTransactionStatus()
{
    return trxStatus;
};

///////////////////////////////////////////////////////////////////
// Establecer el estado de la ultima transaccion procesada
///////////////////////////////////////////////////////////////////
short DPCISO::SetLastTransactionStatus( short shNewStatus )
{
    return ( trxStatus = shNewStatus );
};

///////////////////////////////////////////////////////////////////////////////////////
// Formato de los mensajes : ISO, no-ISO o recuperar como esta establecido?
enumFormatType DPCISO::GetFormat(void)
{
    return efFormat    ;
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// Establece configuracion de conexion del DPC ISO, a priori
//
void DPCISO::SetConfig(char *szHost,unsigned short usPort, unsigned short usLiPort,
                       bool xbPP_Protocol, int iXTimeout,
                       char *szIPBackEnd, unsigned short usXPortBackEnd )
{
   /* Precondicion : No todos los parameteos pueden venir informados.... verificarlos*/
   /* Copiar configuracion de conexion */
   if(szHost)
       strcpy(szISOHost, szHost);
   else
       strcpy(szISOHost, "");
   usISOPort   = usPort;
   usISOLiPort = usLiPort;
   /* TCP PP protocol ? */
   bTCPIP_PP_PROTOCOL = xbPP_Protocol;
   iTimeout = iXTimeout ;
   /* Para el back end ... */
   if(szIPBackEnd)
       strcpy(szBackEndHost, szIPBackEnd);
   else
       strcpy(szBackEndHost, "");
   usBackEndPort = usXPortBackEnd;
};

///////////////////////////////////////////////////////////////////

void DPCISO::VerifyLocaleSettings(void)
{
   ///////////////////////////////////////////////////////////////////////////
   // Local Settings USA : decimal_point "." - thousand_point ","
   const char *szLocale = setlocale( LC_ALL, "LC_MONETARY=US;LC_NUMERIC=US;" );
   const struct lconv *pstConv = localeconv();
   if(pstConv && (strcmp(pstConv->decimal_point,".")!=0 ||
        strcmp(pstConv->thousands_sep,",")!=0))
    {
        hostStatus = _HOST_UNAVAILABLE_;
    }
   ///////////////////////////////////////////////////////////////////////////
};

// Establecer el parametro de replica de SAF
bool DPCISO::SetReplyToSAF( bool bReply ) 
{ 
    return (bISO8583_REPLY_SAF = bReply) ;
};

///////////////////////////////////////////////////////////////////
