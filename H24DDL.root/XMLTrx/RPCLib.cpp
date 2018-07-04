/////////////////////////////////////////////////////////////////////////////////////////////////
// IT24 Sistemas S.A.
// RPC Library  
//
// Libreria de Manejo de Transacciones RPC
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2007.08.15      mdc     Base, tomando como ejemplo lo del BISEL con SFB
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <qusrinc/RPCLib.h>
#include <qusrinc/stdiol.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <locale.h>

#ifndef DNET_AMMT_LENGTH
#define DNET_AMMT_LENGTH (17)
#endif


// MONTO ASCII TODO CEROS :      ....:....1....:.. (LONG. ES 17 BYTES)  
#define SZ_ALL_ZEROES_AMMOUNT	"00000000000000000"
#define _RPC_CHAR_CUT_			0xFF

////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
////////////////////////////////////////////////////////////////////////////
RPC::RPC() 
{ 
    bConnected = is_false;
	mySock     = -1;     // invalid sock no. by default 
}

////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
////////////////////////////////////////////////////////////////////////////
RPC::~RPC() 
{ 
    bConnected = is_false;
	mySock     = -1;     
}

////////////////////////////////////////////////////////////////////////////
// LOGIN X OMISION ES SIEMPRE 'FALSO'
////////////////////////////////////////////////////////////////////////////
boolean_t RPC::Login() 
{ 
    return is_false;
}

////////////////////////////////////////////////////////////////////////////
// LOGOUT X OMISION ES SIEMPRE 'FALSO'
////////////////////////////////////////////////////////////////////////////
boolean_t RPC::Logout( void ) 
{ 
    return is_false;
}

////////////////////////////////////////////////////////////////////////////
// POSTING DATE COMO FECHA Y HORA ACTUALES DEL SISTEMA
////////////////////////////////////////////////////////////////////////////
boolean_t RPC::RefreshDate(ULONG *pYYMMDD,	char *szUSER, char *szPASSWORD)
{
    // PRECONDICION : FECHA Y HORA ACTUALES DEL SISTEMA
    time_t t       = time( NULL );
    struct tm *ptm = localtime( &t );

    if(!pYYMMDD)
        return is_false;

    // YYMMDD binary = ej. 060101
    (*pYYMMDD) = ( ptm->tm_year % 100)*10000 + /* default es año actual */
                 ( ptm->tm_mon + 1)*100      + /* default es mes actual */
                   ptm->tm_mday;               /* default es dia actual */
    //----------------------------------------------------------------------
    // YYYYMMDD binary : ej. 20060101
    //(*pYYMMDD) = ( ptm->tm_year + 1900)*10000 + /* default es año actual */
    //             ( ptm->tm_mon + 1)*100       + /* default es mes actual */
    //               ptm->tm_mday;                /* default es dia actual */
    //----------------------------------------------------------------------

    return is_true;
}

////////////////////////////////////////////////////////////////////////////////
// POSTING TIME COMO HORA ACTUAL DEL SISTEMA
////////////////////////////////////////////////////////////////////////////////
boolean_t RPC::RefreshPostingTime(   
                                ULONG *pHHMMSS ,
								char *szUSER	 ,
								char *szPASSWORD )
{
    // PRECONDICION : FECHA Y HORA ACTUALES DEL SISTEMA
    time_t t       = time( NULL );
    struct tm *ptm = localtime( &t );

    if(!pHHMMSS)
        return is_false;

    // HHMMSS binary
    (*pHHMMSS) = ptm->tm_hour*10000 + 
                 ptm->tm_min*100    + 
                 ptm->tm_sec ;

    return is_true;
}

////////////////////////////////////////////////////////////////////////////
// SEND AND RECEIVE VIA TCP/IP WITH PP-SUB-PROTOCOL
////////////////////////////////////////////////////////////////////////////
int RPC::TCPSendAndReceive(char *szHost, unsigned short usPort, 
    		    			char *szBuffer, unsigned short *piLen,
	    					unsigned short shRcvLen, short bPP_Protocol,
		    		    	unsigned uTimeOut ,
							const BOOL bTerminateSession )
{
	const BOOL     bProxyEnable         = FALSE;    /* flag for indicaing a proxy in between  */
	const BOOL     bDisconnectEveryTime = FALSE;    /* flag for disconnecting per transaction */
	const unsigned uDelayBeforeRcv      = uTimeOut; /* delay between SEND and RECEIVE         */
	int            Rc                   = TCP4U_SUCCESS;
	char           szVer [128];
	unsigned short Total = 0;

	/* Precondition : connected to a given socket */
	if( !bConnected  &&  !bTerminateSession )
	{
		/* get info on this library */
		Tcp4uVer (szVer, sizeof szVer);   
		/* init library */
		Rc = Tcp4uInit();

		if (TCP4U_SUCCESS!=Rc)  
			return (Rc); /* error */ 
		
		/* connet to server and wait its reply */
		Rc = TcpConnect (&mySock, szHost, NULL, &usPort );
	}
	else if( bConnected  &&  !bTerminateSession )
	{
		Rc=TCP4U_SUCCESS; /* already connected */
	}
	else if( !bConnected  &&  bTerminateSession )
	{
		Rc=TCP4U_SUCCESS; /* not connected and end of session */
		mySock = -1;
		return (Rc); /* error */ 
	}
	else if( !bConnected )
	{
		Rc=TCP4U_CONNECTFAILED; /* not connected */
		mySock = -1;
		return (Rc); /* error */ 
	}

	/* Precondition : Connection Ok */
	if (TCP4U_SUCCESS==Rc)
	{
		/* Connected flag */
		bConnected = is_true;
		/* PP protocole (2 first bytes contains length of data) */
		if(bPP_Protocol)
		{
			/* first 2 bytes INCLUDE the length of the SHORT INTEGER */		  
			Total = htons ((unsigned short) (*piLen) + sizeof(short));
			Rc    = TcpSend (mySock, (LPCSTR) & Total, sizeof(short), FALSE, HFILE_ERROR);
			Rc    = TcpSend (mySock, szBuffer        , (*piLen)     , FALSE, HFILE_ERROR);
		}
		else
			Rc = TcpSend (mySock, szBuffer, (*piLen), FALSE, HFILE_ERROR);

		/* uTimeOut > 0 : wait for response */
		while ( TCP4U_SUCCESS==Rc 
			    && 
			    uTimeOut > 0
			    && 
				/* FIX NO ESPERAR RESPUESTA CUANDO SE TERMINA LA SESION(LOGOUT) */
			    !bTerminateSession )   
		{
			if(bPP_Protocol)
			{
				/* first 2 bytes INCLUDE the length of the SHORT INTEGER */
				Total = 0;
				Rc    = TcpRecv (mySock, (LPSTR) & Total, sizeof(short), uTimeOut, HFILE_ERROR);
				Total = ntohs (Total) /* - sizeof(short)*/ ;
				
				if( 0 != Total )
					Rc = TcpRecv (mySock, szBuffer, min(shRcvLen,max(Total,0)), uTimeOut, HFILE_ERROR); /* X sec timeout */
			}
			else		 
				Rc = TcpRecv (mySock, szBuffer, shRcvLen, uTimeOut, HFILE_ERROR); /* X sec timeout */
			
			/* verificar longitudes retornadas */
			if(Rc<0)
			{
				/********************************************************/
				/* FIX DE VERIFICACION DE MODO ESCUCHA "COLGADO" EN RPC */
				if( TCP4U_TIMEOUT == Rc )
				{
					szBuffer[0] = _RPC_CHAR_CUT_ ;
					szBuffer[1] = _RPC_CHAR_CUT_ ;
					(*piLen)    = 1;
					Rc = TcpSend (mySock, szBuffer, (*piLen), FALSE, HFILE_ERROR);
					if( TCP4U_SUCCESS==Rc )
					{
						Rc = TcpRecv (mySock, szBuffer, shRcvLen, uTimeOut, HFILE_ERROR); /* X sec timeout */
					}
					/* SIGUE COLGADO? ABORTAR, SINO CAPTURAR RTA. */
					if(Rc>0)
					{			   
						(*piLen) = min(Rc,shRcvLen);			   
						szBuffer[(*piLen)] = 0x00; /* end-of-text*/
						Rc = TCP4U_SUCCESS;
						break;
					};
				}
				/********************************************************/
				(*piLen) = 0;
				break;
			}
			else if(Rc>0)
			{			   
				(*piLen) = min(Rc,shRcvLen);			   
				szBuffer[(*piLen)] = 0x00; /* end-of-text*/
				Rc = TCP4U_SUCCESS;
				break;
			};
			
			/* Overwrite ResposeCode (Rc) if SUCCESS */
			if(Rc>0)
				Rc = TCP4U_SUCCESS;
		};/*end-if-Rc*/
	};/*end-if-while*/
   
	/* Postcondition : Close only on failure OR when flag OR session end  */
	if( bDisconnectEveryTime
		|| 
		( Rc <= TCP4U_SOCKETCLOSED && mySock >= 0) 
		|| 
		/* FIX TERMINAR SESION CUANDO SE INDICA, SINO NO (LOGOUT) */
		bTerminateSession
		)
	{
		/* close socket */
		TcpClose (&mySock);
		/* clean up library */
		Tcp4uCleanup();
		/* flag to false */
		bConnected = is_false;
	};

	/* OK-ERROR */
	return Rc ;
} ;
///////////////////////////////////////////////////////////////////////////////////////////