///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// Process ATM Transaction Method
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2002.06.01      gsi     Base
// (Alfa)       2002.05.09      mdc     Excluir estructuras de datos propias de SIAF
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CTO_TCP4U_   
#define _CTO_TCP4U_   

// Librerias de TCP/IP
#include <qusrinc/tcp4u.h>
// Alias (para EXPORT y otros)
#include <qusrinc/alias.h>

class CToTcp4u {

	private:

		char szVer[64];
		int iRc;
		unsigned short  usLiPort, 
						usPort;
		SOCKET			mySock, 
						mySockListen, 
						myActiveSock;
		SOCKET      LSock;
		char        szHost[64], 
					szBuffR[8192],
 					szBuff[8192];					
		int			iTimeout;
	
	public:

		EXPORT CToTcp4u() ;
		void  EXPORT setHost(char *szXHost);
		PCHAR EXPORT getHost();
		void  EXPORT setIrc(int xiRc);
		int   EXPORT getIrc();
		void  EXPORT setUsLiPort(unsigned short xusLiPort );
		unsigned short EXPORT getUsLiPort();
		void           EXPORT setUsPort(unsigned short xusPort );
		unsigned short EXPORT getUsPort();
		int  EXPORT getMyActiveSock();
		void EXPORT setMyActiveSock(int xmyActiveSock);
		void EXPORT getInfoLibrary();
		void EXPORT actives();
		void EXPORT listen();
		void EXPORT accept() ;
		void EXPORT connect() ; 
		void EXPORT disconnect() ; 
		int  EXPORT recv(char *str, int len);
		int  EXPORT send(char *, int len);
	
};



#endif // CTO_TCP4U

