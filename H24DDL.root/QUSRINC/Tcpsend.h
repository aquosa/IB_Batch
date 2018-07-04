/*************************************************************************************************
 * IT24 Sistemas - Libreria generica de WRITE y READ bajo TCP protocol.
 * v1.0 2000.10.20 Martin D Cernadas. Etapa 1.
 *************************************************************************************************/


#ifdef __cplusplus
extern "C"  {
#endif // __cplusplus
int TCPSendAndReceive(char *szHost, unsigned short usPort, 
					  char *szBuffer, unsigned short *piLen,
					  unsigned short shRcvLen,  short bPP_Protocol, unsigned uTimeOut);
int TCPGetLocalID (char *szStrName, int uNameSize, unsigned long *lpAddress);
#ifdef __cplusplus
}
#endif // __cplusplus
