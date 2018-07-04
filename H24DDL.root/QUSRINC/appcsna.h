/////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A.
// APPC SNA Ms-Windows's Class Interface
/////////////////////////////////////////////////////////////////////////////

/* (C) COPYRIGHT DATA CONNECTION LIMITED 1993 */

/*****************************************************************************/
/* Change History                                                              */
/*                                                                             */
/*       04/05/1993 NGR Created.                                               */
/* OUT1  02/07/1993 NGR Change output string                                   */
/* OUT2  17/09/2003 MDC C++                                                    */
/* OUT3  01/10/2003 MDC Get_Transaction_Request / Response                     */                                        
/* OUT4  30/10/2003 MDC IO-Enable state, BOOL EXPORT RestartTP();              */                                        
/* OUT5  06/09/2004 MDC BOOL bUpdateRecordFlag=TRUE                            */
/* OUT6  09/08/2005 MDC ODBC-Jeronimo, BANCO SUQUIA                            */
/* OUT7  17/07/2006 MDC enumHostStatus hsStatus  = on u offline                */
/*****************************************************************************/

#ifndef _WAPPCSNA_H_          
#define _WAPPCSNA_H_

/************************************/
#if defined ( _AFXEXT )
#include <afxwin.h>
#elif defined ( _WIN32 )
#include <windows.h>
#else
#error 'Plataforma desconocida'
#endif /* __AFXEXT : AFX Extensions */
/************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/************************/
#if (_MSC_VER >= 800)
#include <qusrinc/winappc.h>
#include <qusrinc/wincsv.h>
#else
#include <winappc.h>
#include <wincsv.h>
#endif
/************************/
#include <memory.h>
#include <string.h>
#include <time.h>
#include <locale.h>
/* alias header */
#include <QUSRINC/alias.h>
#include <qusrinc/typedefs.h>

/* DPC System */
#include <qusrinc/dpcsys.h>     /* OUT7 */
/* SAF QUEUE */
#include <qusrinc/saf.h>        /* OUT7 */

#ifndef  true
#define  true	(1)
#define  false  (!true)
#endif /* true */

// Local definitions
typedef unsigned long RESULT;

// Local MsWindows Messaging Mapper
LONG  PASCAL LocalTPWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) ;

///////////////////////////////////////////////////////////////////////////////////OUT2//
// Clase Base WinAPPCSNA                                                           OUT2//
///////////////////////////////////////////////////////////////////////////////////OUT2//
class WinAPPCSNA
{
protected :

    // APPC full-struct union
    union
    {
       struct appc_hdr              hdr;
       struct tp_started            tpstart;
       struct mc_send_conversation  sndconv;
       struct mc_allocate           allocate;
       struct mc_send_data          snddata;
       struct mc_deallocate         deallocate;
       struct mc_confirm            confirm;
       struct receive_allocate      rcvalloc;
       struct mc_receive_and_wait   rcvwait;
       struct mc_receive_immediate  rcvimmed;
       struct mc_receive_and_post   rcvpost;
       struct mc_confirmed          confirmed;
       struct mc_prepare_to_receive prepare;
       struct tp_ended              tpend;
       struct mc_request_to_send    request;
    } vcb;
    char  *	vcbptr; //ptr APPC-msg union

    /// EBCDIC-ASCII structs
    struct convert	cnvt;    // EBCDIC-ASCII page
    char  *		    cnvtptr; // ptr EBCDIC-ASCII page

    // TP and LU's names, Filenames and Data ptrs....
    char			LocalTPName[64];
    char			LocalLUAlias[8];
    char			RemoteLUAlias[8];
    char			ModeName[8];
    char			RemoteTPName[64];
    char			TPid[8];
    char			FileName[60];
    char            szCfgFile[60];
    char            szRecvLogFile[60];
    char            szSendLogFile[60];
    char    *       DataPtr;
    char    *       DataPtrBkp;
    RESULT  *	    ResultPtr;
    RESULT  *	    ResultBuf;

    // Counters
    unsigned short	SendSize,
                    SendSizeBkp;
    unsigned short	NumConversations;
    unsigned short	NumSends;
    unsigned short	ConfirmEvery;
    unsigned short	SendCount;
    unsigned short	ConfirmCount;
    unsigned short	ConvCount;

    // Flags 
    char			datach;
    unsigned long	Convid;
    int				GetStage;    
    RESULT			ConvStarted;
    BOOL			Deallocated;
    BOOL			GotNumConv;
    BOOL			TPDead;
    BOOL			FirstConv;
    BOOL	        bIsSendTP;
    BOOL			bSendConversation;
    BOOL            bSendRunDetails;    
    long            lReceiveTPMode;     
    time_t          tLastSendTime,                      /* OUT4 */
                    tLastTPEndTime;                     /* OUT4 */
    BOOL            bIOEnable,                          /* OUT4 */
                    bTPStarted ;                        /* OUT4 */

    // MsWindows dependant members
    WAPPCDATA       APPCData;
    HWND			hWndMain;
    HANDLE			async_corr;
    unsigned short	ASYNC_COMPLETE;    
    char            szText[256];    

    // Specific task processing members
    void Send_Run_Details(void);
    void Get_Run_Details(void);
    void Get_Transaction_Request(void);                 /* OUT3 */
    void Send_Transaction_Response(void);               /* OUT3 */
    void IssueNextVerb(void);
    void ProcessReturns(void);
    void NewConversation(void);    
    int  EXPORT ReadString(char  * lpValueName, char  * lpData, int maxlen);
    int  WriteString(char  * lpData, size_t nLen);
    void OutputResults(void);
    virtual int RouteTransactionToAuth(char *lpData, size_t nLen, char *lpDataOut);
    virtual int RetrieveTransactionFromAuth(char * lpData, unsigned short *pnLen,
                                            char * pszEstadoTrat,
                                            char * lpDataBkp, unsigned short *pnLenBkp );
    virtual int UpdateTransactionInAuth(char *DataPtrBkp, unsigned short SendSizeBkp);



private :

    // Specific APPC Verbs builders-primitives
    void Build_TP_ENDED(void);
    void Build_TP_STARTED(void);
    void Build_MC_ALLOCATE(void);
    void Build_MC_DEALLOCATE(void);
    void Build_MC_SEND_DATA(void);
    void Build_MC_SEND_CONVERSATION(void);
    void Build_MC_CONFIRM(void);
    void Build_RECEIVE_ALLOCATE(void);
    void Build_MC_PREPARE_TO_RECEIVE(void);
    void Build_MC_RECEIVE_IMMEDIATE(void);
    void Build_MC_RECEIVE_AND_WAIT(void);
    void Build_MC_RECEIVE_AND_POST(void);
    void Build_MC_CONFIRMED(void);
    void Build_MC_REQUEST_TO_SEND(void);
    void CONV_A_TO_E(char  * string,int length);
    void CONV_E_TO_A(char  * string,int length);
    void PadString(char  * string, int length);
    void GenerateData(void);


public :

    // Constructor
    EXPORT WinAPPCSNA();
    // Detructor
    EXPORT ~WinAPPCSNA();

    // Public Members
    BOOL EXPORT SetIOEnable( BOOL bSFlag = TRUE );
    BOOL EXPORT InitialiseWinMain(HINSTANCE hInstance, WNDPROC pTPWndProc);
    void EXPORT DestroyWinMain(void);
    void EXPORT InitialiseMain(void);    
    void EXPORT ReadConfig(void);
    LONG EXPORT PASCAL TPWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    BOOL EXPORT RestartTP();

    virtual BOOL EXPORT SetAsSendTP( PVOID ) ;
    virtual BOOL EXPORT SetAsReceiveTP( PVOID ) ;

    // Specific Atomic SEND_DATA to TRANSACTION PROGRAM
    BOOL EXPORT AtomicSendDataToTP( char *achDataOut , size_t nLenOut ,
                                    char *achDataIn  , size_t nLenIn,
                                    PVOID pXptr, BOOL bUpdateRecordFlag = TRUE);

};

/////////////////////////////////////////////////////////////////////////////////
// Clase de Resolucion de Trx hacia BACKEND (COBIS,...) desde WinAPPCSNA                                   
/////////////////////////////////////////////////////////////////////////////////
class WinAPPC_BackEnd : public WinAPPCSNA 
{

public :
    
    // Constructor
    EXPORT WinAPPC_BackEnd();

    BOOL EXPORT SetAsSendTP( PVOID ) ;
    BOOL EXPORT SetAsReceiveTP( PVOID ) ;

    // Specific Atomic SEND_DATA to TRANSACTION PROGRAM
    BOOL EXPORT AtomicSendDataToTP( char *achDataOut , size_t nLenOut ,
                                    char *achDataIn  , size_t nLenIn  , 
                                    PVOID pXptr      , BOOL bUpdateRecordFlag);
    // Specific Atomic SEND_DATA to TRANSACTION PROGRAM
    BOOL EXPORT AtomicSendDataToTP( char *achDataOut , size_t nLenOut ,
                                    char *achDataIn  , size_t nLenIn  , 
                                    PVOID pXptr      );

    // Get Counters data
    VOID EXPORT GetCounters( long *plTrxCounter, long *plErrCounter);

    // Set BackEnd Status                                                           /* OUT7 */
    VOID EXPORT SetBackEndStatus( enumHostStatus hsExtStatus );                     /* OUT7 */
    // Get BackEnd Status                                                           /* OUT7 */
    enumHostStatus EXPORT GetBackEndStatus( enumHostStatus *phsExtStatus = NULL );  /* OUT7 */
    // Verify Pending SAF                                                           /* OUT7 */
    void VerifyPendingSAF( void );                                                  /* OUT7 */

protected :
    
    int EXPORT RetrieveTransactionFromAuth(char * lpData, unsigned short *pnLen,
                                           char * pszEstadoTrat ,
                                           char * lpDataBkp, unsigned short *pnLenBkp);
    int EXPORT RouteTransactionToAuth(char * lpData, size_t nLen, char *lpDataOut );
    int EXPORT UpdateTransactionInAuth(char *DataPtrBkp, unsigned short SendSizeBkp);

    PVOID  ptr_backend; // BackEnd : COBIS,Sybase,ODBC,etc. (ptr)
    ////////////////////////////////////////////////////
    // Contadores de Transferencias OK y ERRORES
    long   lTrxCounter,                                 /* OUT6 */
           lErrCounter;                                 /* OUT6 */
    ////////////////////////////////////////////////////
    // Estado del HOST
    enumHostStatus hsStatus  ;                          /* OUT7 */
    BOOL              bFromSAFLoop ;                    /* OUT7 */
    ////////////////////////////////////////////////////


};


#endif /* OUT2 */

/////////////////////////////////////////////////////////////////////////////


