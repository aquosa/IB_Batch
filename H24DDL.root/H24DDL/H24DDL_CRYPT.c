/****************************************************************
 * H24 Data Definition Language, Definitions, and Methods
 * v2.0 2000.12.22 Martin D Cernadas. Etapa 2.
 * v3.0 2001.04.27 Martin D Cernadas. Etapa 3. get_remote_host_config()
 ****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "h24ddl.h"
#include "..\md5lib\md5lib.h"
#include "..\deslib\deslib.h"
#include "..\tcpsend\tcpsend.h"

#ifdef _USE_HEXA_IN_DES_
#error Ya definido _USE_HEXA_IN_DES_
#else
#define _USE_HEXA_IN_DES_		0
#endif /* _USE_HEXA_IN_DES_ */

/* Estructuras predefinidas para formateo y comparacion */
extern HXXX_MESSAGES_UN		unOutMsg;
extern HXXX_MESSAGES_UN		unInMsg;
extern HXXX_MESSAGES_UN		unCryptMsg;
/* Estructura predefinida para input-output */
extern HXXX_MESSAGES_HOST_ST	stIOMsg;
extern int						iIOLen;
/* Clave Publica de sesion */
extern char					szSessionKey[16+1]; 
/* Clave Privada de sesion */
extern char					szPrivateSKey[8+1];
/* Filename mask para trace */
extern const char			szTraceFilenameMask[];
/* Filename para configuracion */
extern char					szConfFilename[256];
/* host y port por defecto */
extern char					szHostName[];
extern unsigned short		usHostPort;
extern unsigned short		ushAlgEncr;
extern time_t				tLastHostCheck;
extern short				shVersion;

/* lee la configuracion remota del host */
int set_config_filename( char *szFilename, short shVersion )
{
	char	szDateTime[]      = "yyyymmddhhmissms";
	char	szTraceFilename[256] = {0};	
	FILE	*hFile = NULL;
	time_t	tmData = time(NULL);
	struct tm *plc_Data = localtime( &tmData );

	if(!szFilename)
		return -1;
	else if (strlen(szFilename) > 255 || strlen(szFilename) == 0)
		return -2;
	else 
		{
		/* copiar nuevo nombre */
		strcpy( szConfFilename, szFilename);
		/* verificar si y solo si no se lo ha hecho nunca */
		if(0L != tLastHostCheck)
			tLastHostCheck = 0L;
		}

	/* fecha y hora actuales */
	strftime(szDateTime, sizeof(szDateTime), ISO_DATETIME_MASK,	plc_Data );
	strftime(szTraceFilename, sizeof(szTraceFilename), szTraceFilenameMask,	plc_Data );
	/* abrir logger */
	hFile=fopen(szTraceFilename,"at+");	
	if(!hFile)
		return -2;
	/* mostrar campo a campo el detalle de la configuracion */
	fprintf(hFile,"\r\n[### ARCHIVO DE CONFIGURACION %s ###]\r\n", szConfFilename);
	fclose(hFile);
	hFile = NULL;

	return 0;

}

/* lee la configuracion local del equipo (IP address) */
get_local_host_config(char *szStrName, int uNameSize, unsigned long *lpAddress)
{	
	TCPGetLocalID (szStrName, uNameSize, lpAddress);
}

/* lee la configuracion remota del host */
get_remote_host_config( char **pszHost, short *pshPort, short *pshAlgEncr )
{
	if (pszHost && pshPort)
	{
		if(pshPort)
			(*pshPort)    = usHostPort;
		if(pszHost)
			(*pszHost)    = szHostName;
		if(pshAlgEncr)
			(*pshAlgEncr) = ushAlgEncr;
		return 0;
	}
	else
		return (-1);
}

/* verificacion de parametros de configuracion del host */
check_remote_host_config( void )
{				
	FILE	*hFile                  = NULL;
	unsigned int	shIPHost        = 1000;
	char			szIPAddress[256]="0.0.0.0";
	char			szAlgEncryp[256]="   ";
	
	/* verificar si y solo si no se lo ha hecho nunca */
	if(0L != tLastHostCheck)
		return 0;
	else
		/* guardar ultima fecha de chequeo */
		tLastHostCheck = time( NULL );

	/* abrir archivo */
	hFile=fopen(szConfFilename,"rt");
	if(!hFile)
		return -1;
	
	/* scannear */
	fscanf(hFile, "%s", szIPAddress );
	fscanf(hFile, "%i", &shIPHost );
	fscanf(hFile, "%s", szAlgEncryp );
	if( shIPHost > 1024)
		usHostPort = shIPHost;
	if( strlen(szIPAddress) > 1 && strlen(szIPAddress) < 256)
		strcpy(szHostName, szIPAddress);
	if( strlen(szAlgEncryp) > 1 ) {
		if (strcmp(szAlgEncryp,"DES")==0 || strcmp(szAlgEncryp,"des")==0)
			ushAlgEncr = ALG_DES;
		else if (strcmp(szAlgEncryp,"RC4")==0 || strcmp(szAlgEncryp,"rc4")==0)
			ushAlgEncr = ALG_RC4;
		else if (strcmp(szAlgEncryp,"   ")==0)
			ushAlgEncr = ALG_NULL;
		else
			ushAlgEncr = ALG_NULL;
	}
	else
		ushAlgEncr = ALG_NULL;

	/* cerrar archivo */
	fclose(hFile);
	return 0;
}

/*	alternar bytes en un short, para LITTLE-ENDIAN hacia BIG-ENDIAN machines */
/*  utilizado optativamente para envios via TCP y subprotocolo PP */
static void	swapbyte(short *sh)
{
	char	*cp = (char *)sh,
		    t   = 0;	

	t         = *(cp);
	*(cp)     = *(cp + 1);
	*(cp + 1) = t;
};

/* enviar y recivir un mensaje en forma generica */
int send_receive(unsigned short *pshLen, unsigned uTimeOut)
{
	int				iRetVal = 0;
	

	/* verificar config. de host remoto */
	check_remote_host_config();


	/* backup de mensaje para comparacion */
	unInMsg = unOutMsg;

	
/* TANDEM utiliza subprotocolo PP para TCP/IP */
#define _PP_PROTOCOL_
#ifdef _PP_PROTOCOL_
	/* armar mensaje de salida */
	stIOMsg.len   = (*pshLen); /* inicializar longitud total del mensaje a enviar */
	stIOMsg.data  = unOutMsg;  /* copiar mensaje de salida */	

	/* envio y recepcion de msg , con o sin protocolo PP */
	iRetVal = TCPSendAndReceive(
					szHostName, 
					usHostPort,
					(char *)&stIOMsg.data,
					pshLen ,
					sizeof(stIOMsg.data),
					TRUE ,
					uTimeOut );	/* 60 seconds as default */
#else
	/* armar mensaje de salida */
	stIOMsg.len   = (*pshLen); /* inicializar longitud total del mensaje a enviar */
	(*pshLen)    += 2;         /* sumar 2 bytes del campo 'len' */
	stIOMsg.data  = unOutMsg;  /* copiar mensaje de salida */
	swapbyte( &stIOMsg.len) ;  /* alternan los bytes de la longitud de 16 bits */

	/* envio y recepcion de msg , con o sin protocolo PP */
	iRetVal = TCPSendAndReceive(
					szHostName, 
					usHostPort,
					(char *)&stIOMsg,
					pshLen ,
					sizeof(stIOMsg.data),
					TRUE ,
					uTimeOut );	/* 60 seconds as default */
#endif /* _PP_PROTOCOL_ */

	/* copiar mensaje entrante */
	unInMsg = stIOMsg.data;  
	/* copiar longitud entrante */
	iIOLen = (*pshLen);
	/* nulificar el resto del mensaje no recibido */
	if(iIOLen < sizeof (unInMsg.chBuffer))
		memset( unInMsg.chBuffer + iIOLen, 0x00, sizeof(unInMsg.chBuffer)-iIOLen );

	return iRetVal;
}

/* envia un mensaje de reverso 0420 */
int send_h420(void)
{
	unsigned short shLen = 0;
	int            rc    = 0;
	
	/* enviar req. y esperar reverso en base a lo ultimo recibido */
	unOutMsg = unCryptMsg;	
	COPY_FIELD(unOutMsg.stHdrLog.msg_type, "H420");

	/* loggear */
	log_msg_h200( &unOutMsg );

	shLen = (sizeof(unOutMsg.stH210) - sizeof(struct HXXX_HEADER_NO_CRYPT))*2 + sizeof(struct HXXX_HEADER_CRC);	
	shLen = max( shLen, iIOLen );
	
	rc = send_receive( &shLen , 0 ); /* timeout de 0 segundos  : sin espera de rta. */
	if(rc > 0 && shLen > 0)	
		return 0;
	else
		return (rc);
}

/* envia y recibe un mensaje de requerimiento 0200 */
int send_receive_h200(void)
{
	unsigned short shLen        = 0,
				   shLenCheksum = 0;
	int            rc    = 0;


	/* numero de secuencia */
	set_field_seqnum( H200 );

	/* logging */
	log_msg_h200(  &unOutMsg );

#if ( _CRC_BEFORE_DES_ == _CRC_DES_OPTION_ )
	/* crc */
	shLen = shLenCheksum = sizeof(unOutMsg.stH200);
	shLenCheksum = set_field_crc( H200 , shLenCheksum );
	/* encriptar mensaje */
	if(ALG_DES == ushAlgEncr)
		shLen = encrypt_msg(&unOutMsg, szSessionKey, &unCryptMsg, _USE_HEXA_IN_DES_ );
	else
		unCryptMsg = unOutMsg;
	unOutMsg = unCryptMsg;	
#else
	/* encriptar mensaje */
	if(ALG_DES == ushAlgEncr)
		shLen = encrypt_msg(&unOutMsg  , szSessionKey, &unCryptMsg   );	
	else
		unCryptMsg = unOutMsg;

	/* para debug */
	/*decrypt_msg(&unInMsg   , szSessionKey, &unCryptMsg   );*/
	unOutMsg = unCryptMsg;	
	/* crc */	
	/*shLen = sizeof(unOutMsg.stH200);*/
	shLen = shLenChecksum = (sizeof(unOutMsg.stH200) - 
			sizeof(struct HXXX_HEADER_NO_CRYPT))*2 +
			sizeof(struct HXXX_HEADER_NO_CRYPT) ;
	if((shLen%8)!=0)
		shLen += 8 - shLen%8;
	shLenCheksum = set_field_crc( H200 , shLen);
#endif /* _CRC_DES_OPTION_ */	

	if(sizeof(struct HXXX_HEADER_NO_CRYPT) > sizeof(struct HXXX_HEADER_CRC))
		shLen += (unsigned short)(sizeof(struct HXXX_HEADER_NO_CRYPT) - 
						sizeof(struct HXXX_HEADER_CRC));
	else
		shLen += (unsigned short)(sizeof(struct HXXX_HEADER_CRC) - 
						sizeof(struct HXXX_HEADER_NO_CRYPT));
	set_msg_len_h200( shLenCheksum );
	shLen += sizeof(struct HXXX_HEADER_CRC);

	/* enviar req. y esperar rta. */
	rc = send_receive( &shLen , 30 ); /* timeout de 30 segundos */	
	/* por ok y con longitud valida , recuperar cod.rta */
	/* para Base-24, un cod.rta. aprobado esta entre 000 y 050 */
	if(rc > 0 && shLen > 0)	
	{
		/* des-encriptar mensaje */
		/* copiar mensaje entrante */
		unCryptMsg = unInMsg;  
		/* desencriptar */
		if(ALG_DES == ushAlgEncr)
			decrypt_msg(&unInMsg, szSessionKey, &unCryptMsg, shLen , _USE_HEXA_IN_DES_ );
		else
			unInMsg = unCryptMsg;

		if( isdigit(unInMsg.stH210.resp_code[0]) && 
			isdigit(unInMsg.stH210.resp_code[1]) &&
			isdigit(unInMsg.stH210.resp_code[2]) ) 
		/**************************************************************************/
		{
			rc = antoi( unInMsg.stH210.resp_code, sizeof unInMsg.stH210.resp_code );
			if(rc>=0 && rc<= 10)
			{
			if( isdigit(unInMsg.stH210.unReq.stPrestacion.stItem_presta.resp_code_item[0]) && 
				isdigit(unInMsg.stH210.unReq.stPrestacion.stItem_presta.resp_code_item[1]) &&
				isdigit(unInMsg.stH210.unReq.stPrestacion.stItem_presta.resp_code_item[2]) ) 
				rc = antoi( unInMsg.stH210.unReq.stPrestacion.stItem_presta.resp_code_item, 
					sizeof unInMsg.stH210.unReq.stPrestacion.stItem_presta.resp_code_item );
			};
		}
		/**************************************************************************/
		else
			rc = (-10) ;
		/* logging del mensaje */
		log_msg_h210( &unInMsg );
		return rc;
	}
	else
		return (rc);
}

/* envia y recibe un mensaje de requerimiento 0205 */
int send_receive_h205(void)
{
	unsigned short shLen        = 0,
				   shLenCheksum = 0;
	int            rc    = 0;


	/* numero de secuencia */
	set_field_seqnum( H205 );

	/* logging */
	log_msg_h205(  &unOutMsg );

#if ( _CRC_BEFORE_DES_ == _CRC_DES_OPTION_ )
	/* crc */
	shLen = shLenCheksum = sizeof(unOutMsg.stH205) - sizeof( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte );
	shLenCheksum = set_field_crc( H205 , shLenCheksum );
	/* encriptar mensaje */
	if(ALG_DES == ushAlgEncr)
		shLen = encrypt_msg(&unOutMsg, szSessionKey, &unCryptMsg, _USE_HEXA_IN_DES_ );
	else
		unCryptMsg = unOutMsg;
	unOutMsg = unCryptMsg;	
#else
	/* encriptar mensaje */
	if(ALG_DES == ushAlgEncr)
		shLen = encrypt_msg(&unOutMsg  , szSessionKey, &unCryptMsg   );	
	else
		unCryptMsg = unOutMsg;

	/* para debug */
	/*decrypt_msg(&unInMsg   , szSessionKey, &unCryptMsg   );*/
	unOutMsg = unCryptMsg;	
	/* crc */	
	/*shLen = sizeof(unOutMsg.stH205);*/
	shLen = shLenChecksum = ( (sizeof(unOutMsg.stH205) - 
		    sizeof( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte ) ) -
			sizeof(struct HXXX_HEADER_NO_CRYPT))*2 +
			sizeof(struct HXXX_HEADER_NO_CRYPT) ;
	if((shLen%8)!=0)
		shLen += 8 - shLen%8;
	shLenCheksum = set_field_crc( H205 , shLen);
#endif /* _CRC_DES_OPTION_ */	

	if(sizeof(struct HXXX_HEADER_NO_CRYPT) > sizeof(struct HXXX_HEADER_CRC))
		shLen += (unsigned short)(sizeof(struct HXXX_HEADER_NO_CRYPT) - 
						sizeof(struct HXXX_HEADER_CRC));
	else
		shLen += (unsigned short)(sizeof(struct HXXX_HEADER_CRC) - 
						sizeof(struct HXXX_HEADER_NO_CRYPT));
	set_msg_len_h205( shLenCheksum );
	shLen += sizeof(struct HXXX_HEADER_CRC);

	/* enviar req. y esperar rta. */
	rc = send_receive( &shLen , 30 ); /* timeout de 30 segundos */	
	/* por ok y con longitud valida , recuperar cod.rta */
	/* para Base-24, un cod.rta. aprobado esta entre 000 y 050 */
	if(rc > 0 && shLen > 0)	
	{
		/* des-encriptar mensaje */
		/* copiar mensaje entrante */
		unCryptMsg = unInMsg;  
		/* desencriptar */
		if(ALG_DES == ushAlgEncr)
			decrypt_msg(&unInMsg, szSessionKey, &unCryptMsg, shLen , _USE_HEXA_IN_DES_ );
		else
			unInMsg = unCryptMsg;

		if( isdigit(unInMsg.stH215.resp_code[0]) && 
			isdigit(unInMsg.stH215.resp_code[1]) &&
			isdigit(unInMsg.stH215.resp_code[2]) ) 
		/**********************************************************************************/
		{
			rc = antoi( unInMsg.stH215.resp_code, sizeof unInMsg.stH215.resp_code );		
		}
		/**********************************************************************************/
		else
			rc = (-10) ;
		/* logging del mensaje */
		log_msg_h215( &unInMsg );
		return rc;
	}
	else
		return (rc);
}

/* envia y recibe un mensaje de login 0600 */
int send_receive_h600(void)
{
	unsigned short	shLen        = sizeof(unOutMsg.stH600),
					shLenCheksum = sizeof(unOutMsg.stH600);	
	int             rc    = 0;

	/* numero de secuencia */
	set_field_seqnum( H600 );
	/* loggear */
	log_msg_h600( &unOutMsg );

	/* crc */
	shLenCheksum = set_field_crc( H600 , shLen);
	/* enviar req. y esperar rta. */
	rc = send_receive( &shLen , 30 ); /* timeout de 30 segundos */	
	/* por ok y con longitud valida , recuperar cod.rta */
	/* para Base-24, un cod.rta. aprobado esta entre 000 y 050 */
	if(rc > 0 && shLen > 0)	
		{
		if( isdigit(stIOMsg.data.stH610.resp_code[0]) && 
			isdigit(stIOMsg.data.stH610.resp_code[1]) &&
			isdigit(stIOMsg.data.stH610.resp_code[2]) )
			rc = antoi( stIOMsg.data.stH610.resp_code, sizeof stIOMsg.data.stH610.resp_code );
		else
			rc = (-10) ;
		if(0==rc)
			{
			/* La clave de sesion viene cifrada con DES y clave privada harcodeada */
			memset( szSessionKey, 0x00, sizeof szSessionKey );
			DES_decrypt_hex( szPrivateSKey , szSessionKey, stIOMsg.data.stH610.clave_sesion );
			/* loggear */
			log_msg_h610( &unInMsg );
			}
		else
			/* loggear */
			log_msg_h610( &unInMsg );

		return rc;
		}
	else
		return (rc);
}

/* obtener mensaje de entrada -para debug- */
get_input_msg(char *szMsg)
{		
	strcpy(szMsg, (char *)&stIOMsg.data );
}

/* obtener mensaje de salida -para debug- */
get_output_msg(char *szMsg)
{		
	strcpy(szMsg, (char *)&unOutMsg );
}

/* envia un mensaje generico */
int send_generic(char *szHost, unsigned short usPort, char *szBuffer , short bPP_Protocol )
{
	unsigned short shLen=strlen(szBuffer);
	return TCPSendAndReceive( szHost, usPort, szBuffer, &shLen, 0, bPP_Protocol, 0 );
}

/* envia un mensaje de requerimiento 0200 */
int send_h200(void)
{
	unsigned short	shLen        = 0,
					shLenCheksum = 0;
	int             rc    = 0;


	/* numero de secuencia */
	set_field_seqnum( H200 );

	/* logging */
	log_msg_h200(  &unOutMsg );

#if ( _CRC_BEFORE_DES_ == _CRC_DES_OPTION_ )
	/* crc */
	shLen        = sizeof(unOutMsg.stH200);
	shLenCheksum = set_field_crc( H200 , shLen);
	/* encriptar mensaje */
	if(ALG_DES == ushAlgEncr)
		shLen = encrypt_msg(&unOutMsg, szSessionKey, &unCryptMsg, _USE_HEXA_IN_DES_ );
	else
		unCryptMsg = unOutMsg;

	unOutMsg = unCryptMsg;	
#else
	/* encriptar mensaje */
	if(ALG_DES == ushAlgEncr)
		shLen = encrypt_msg(&unOutMsg  , szSessionKey, &unCryptMsg   );	
	else
		unCryptMsg = unOutMsg;

	/* para debug */
	/*decrypt_msg(&unInMsg   , szSessionKey, &unCryptMsg   );*/
	unOutMsg = unCryptMsg;	
	/* crc */	
	/*shLen = sizeof(unOutMsg.stH200);*/
	shLen = (sizeof(unOutMsg.stH200) - sizeof(struct HXXX_HEADER_NO_CRYPT))*2 +
			sizeof(struct HXXX_HEADER_NO_CRYPT) ;
	if((shLen%8)!=0)
		shLen += 8 - shLen%8;
	shLenCheksum = set_field_crc( H200 , shLen);
#endif /* _CRC_DES_OPTION_ */	

	if(sizeof(struct HXXX_HEADER_NO_CRYPT) > sizeof(struct HXXX_HEADER_CRC))
		shLen += sizeof(struct HXXX_HEADER_NO_CRYPT) - sizeof(struct HXXX_HEADER_CRC);
	else
		shLen += sizeof(struct HXXX_HEADER_NO_CRYPT) - sizeof(struct HXXX_HEADER_CRC);
	set_msg_len_h200( shLen );
	shLen += sizeof(struct HXXX_HEADER_CRC);

	/* enviar req. y esperar rta. */
	rc = send_receive( &shLen , 0 ); /* sin timeout, sin espera */	
	return (rc);
}
