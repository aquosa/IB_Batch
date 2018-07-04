///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 ASAP SA / COA SA
// CTO clase conectora generica a socket IP
//
// Tarea        Fecha        Autor   Observaciones
// (Inicial)    2002.06.15   gsi     Base
// (Inicial)    2006.10.10   mdc     rpcsrv: SIN NOMBRE DETERMINADO: "szServiceName = NULL"
// (Inicial)    2007.09.25   mdc     _MAX_TIMEOUT_	(10) ; _MIN_TIMEOUT_	(5)
// (Alfa)       2008.09.01   mdc     _MAX_TIMEOUT_	(30) ; _MIN_TIMEOUT_	(10)
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <memory.h>          
#include <qusrinc/typedefs.h>
#include <qusrinc/tcp4u.h>
#include <qusrinc/CToTcp4u.h>


#define _MAX_TIMEOUT_	(30)
#define _MIN_TIMEOUT_	(10)

///////////////////////////////////
///Constructor
CToTcp4u::CToTcp4u() 
{	
	memset ( szVer, 0, sizeof(szVer)-1 );
	iRc          = 0;
	usLiPort     = 0; 
	usPort       = 9000;
	mySock       = 0; 
	mySockListen = 0; 
	myActiveSock = 0;
	LSock  = 0;
	strcpy( szHost, "127.0.0.1" ); 
	memset( szBuffR, 0, sizeof(szBuff)-1);
	memset( szBuff, 0, sizeof(szBuff)-1);
	iTimeout = _MAX_TIMEOUT_;
};

///////////////////////////////////
///Get info on this library 

void CToTcp4u::getInfoLibrary() {

	
	Tcp4uVer (szVer, sizeof szVer);   
	/* init library */
	iRc = Tcp4uInit();
	CToTcp4u::setIrc( Tcp4uInit() ) ;

}

/////////////////////////////////////////////////
// connet to server and wait its reply
// 

void CToTcp4u::connect() {

#ifdef _SYSTEM_SIAF_
   char szServiceName[]="SIAF";
#elif _SYSTEM_SIOP_
   char szServiceName[]="SIOP";
#elif _RPC_DLL_CONNECTION_
   char szServiceName[]="rpcsrv";
#elif _XML_CONNECTION_
   char szServiceName[]="xmlsrv";
#else
   // SIN NOMBRE DETERMINADO
   char *szServiceName = NULL; 
#endif 
   if( CToTcp4u::getUsLiPort() == 0 && CToTcp4u::getUsPort() != 0 ) {
	 int tmpiRc = TcpConnect (&mySock, szHost, szServiceName, &usPort );
	 CToTcp4u::setIrc(tmpiRc);
	 if( getIrc() == TCP4U_SUCCESS )
		actives();
   }
}

/////////////////////////////////////////////////
// disconnect from back end
// 

void CToTcp4u::disconnect() {

  if( mySock != 0) {
     iRc = TcpClose ( &mySock );
	 mySock = 0;
  }
  if( mySockListen != 0) {
     iRc = TcpClose ( &mySockListen );
	 mySockListen = 0;
  }
  if( myActiveSock != 0)
     myActiveSock = 0;
}

//////////////////////////////////////////////////
// listen on socket 
void CToTcp4u::listen() {

   if(CToTcp4u::getUsLiPort() != 0)   {
      unsigned short usTemp = CToTcp4u::getUsLiPort() ;
	  	  int tmpiRc = TcpGetListenSocket (& LSock, "PRUEBA", & usTemp, 1); /* 1 pending connection */
	  CToTcp4u::setIrc(tmpiRc);
   } 
}

//////////////////////////////////////////////////
// listen on socket 
void CToTcp4u::accept() {

	  int tmpiRc = TcpAccept( &mySockListen, LSock, 0 ); /* N secs. for timeout, 0 = for ever*/
      CToTcp4u::setIrc(tmpiRc);
	  
}

/////////////////////////////
//
void CToTcp4u::actives() {

	if(0 == CToTcp4u::getMyActiveSock() )
		CToTcp4u::setMyActiveSock( mySock );
	if(0 == CToTcp4u::getMyActiveSock() )
		CToTcp4u::setMyActiveSock( mySockListen );
	if(0 == CToTcp4u::getMyActiveSock() )
		CToTcp4u::setIrc(TCP4U_ERROR) ;		
}

/////////////////////////////////////////////////
// Realiza un send por medio de la libreria tcp4u 
//
int CToTcp4u::send(char * str, int len) {
	
	////////////////////////////////////////////////
	memset( szBuff, 0x00 ,sizeof(szBuff) );
	memcpy( szBuff, str, min(len,sizeof(szBuff)) );
	szBuff[min(len,sizeof(szBuff))]='\0';
	////////////////////////////////////////////////
	int iRcTmp=TcpSend( myActiveSock, szBuff, len, iTimeout,HFILE_ERROR);
	CToTcp4u::setIrc(iRcTmp);
	if ( CToTcp4u::getIrc() <=0 ) {
		// Segun errores recibidos
		if( TCP4U_SOCKETCLOSED   == iRc  ||
			TCP4U_CONNECTFAILED  == iRc  ||
			TCP4U_ERROR          == iRc  ||
			TCP4U_HOSTUNKNOWN    == iRc  ||
			TCP4U_NOMORESOCKET   == iRc  ||
			TCP4U_NOMORERESOURCE == iRc  ||
			TCP4U_BINDERROR      == iRc  ||
			TCP4U_CANCELLED      == iRc  ||
			TCP4U_INSMEMORY      == iRc  ||
			TCP4U_BADPORT        == iRc  )
			// Cerrar
			TcpClose( &myActiveSock );
		return ( CToTcp4u::getIrc() );
	
	}
	return ( CToTcp4u::getIrc() );
}


/////////////////////////////////////////////////
// Realiza un recv por medio de la libreria tcp4u 
//
int CToTcp4u::recv(char *str, int len) {

	int iRcTmp = TCP4U_SUCCESS;
		
	// Blanquear buffer
	memset( szBuffR, 0x00 , sizeof szBuffR);
	// Esperar Rta. de requerimiento ISO
	iRcTmp = TcpRecv( myActiveSock, szBuffR, sizeof szBuffR, iTimeout, HFILE_ERROR);
	CToTcp4u::setIrc(iRcTmp);
	if ( CToTcp4u::getIrc() <=0 ) {
		// Segun errores recibidos
		if( TCP4U_SOCKETCLOSED   == iRc  ||
			TCP4U_CONNECTFAILED  == iRc  ||
			TCP4U_ERROR          == iRc  ||
			TCP4U_HOSTUNKNOWN    == iRc  ||
			TCP4U_NOMORESOCKET   == iRc  ||
			TCP4U_NOMORERESOURCE == iRc  ||
			TCP4U_BINDERROR      == iRc  ||
			TCP4U_CANCELLED      == iRc  ||
			TCP4U_INSMEMORY      == iRc  ||
			TCP4U_BADPORT        == iRc  )
			// Cerrar
			TcpClose( &myActiveSock );
		return ( CToTcp4u::getIrc() );
		// return iRc  
	}
	else
	{
		/////////////////////////////////////////
		if(NULL != str && len > 0)
		{
			memset( str, 0x00, len );
			memmove(str,szBuffR,min(len,iRcTmp));
			/* fix */
			CToTcp4u::setIrc( TCP4U_SUCCESS );
			return ( CToTcp4u::getIrc() );
			/* fix */
		}
		/////////////////////////////////////////
		return ( CToTcp4u::getIrc() );
	};	
	
}

//////////////////////////////////////////////////////////////////
// Metodos SET y GET de atributos internos de clase
void CToTcp4u::setHost(char *szXHost){
	if(szXHost)	strcpy(szHost,szXHost);
};
PCHAR CToTcp4u::getHost(){
	return szHost ;
};
void CToTcp4u::setIrc(int xiRc){
	iRc = xiRc;
};
int CToTcp4u::getIrc(){
	return iRc ;
};
void CToTcp4u::setUsLiPort(unsigned short xusLiPort ){
	usLiPort = xusLiPort  ;
};
unsigned short CToTcp4u::getUsLiPort(){
	return usLiPort ;
};
void CToTcp4u::setUsPort(unsigned short xusPort ){
	usPort = xusPort  ;
};
unsigned short CToTcp4u::getUsPort(){
	return usPort ;
};
int CToTcp4u::getMyActiveSock(){
	return myActiveSock;
};
void CToTcp4u::setMyActiveSock(int xmyActiveSock){
	myActiveSock = xmyActiveSock;
};
//////////////////////////////////////////////////////////////////