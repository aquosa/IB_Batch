/*************************************************************************************************
 * H24 Data Definition Language, Definitions, and Methods
 * v1.0 2000.10.20 Martin D Cernadas. Etapa 1.
 * v2.0 2000.12.22 Martin D Cernadas. Etapa 2.
 * v2.1 2001.02.07 Martin D Cernadas. Prefijo variable. Stress.
 * v3.0 2001.04.20 Martin D Cernadas. Etapa 3. Nueva mensajeria Multiples Prestaciones.
 * v3.1 2001.06.09 Martin D Cernadas. extern unsigned short	ushAlgEncr;
 * v3.1 2001.06.10 Martin D Cernadas. set_field_h205(); resp_code_header vs resp_code;
 * v3.5 2001.08.20 Martin D Cernadas. Etapa intermedia.
 *************************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <h24ddl.h>

#include "..\md5lib\md5lib.h"
#include "..\deslib\deslib.h"
#include "..\tcpsend\tcpsend.h"



/* Estructuras predefinidas para formateo y comparacion */
HXXX_MESSAGES_UN		unOutMsg;
HXXX_MESSAGES_UN		unInMsg;
HXXX_MESSAGES_UN		unCryptMsg;
/* Estructura predefinida para input-output */
HXXX_MESSAGES_HOST_ST	stIOMsg;
int						iIOLen=0;
/* Clave Publica de sesion */
char					szSessionKey[16+1]; 
/* Clave Privada de sesion por default, hasta intercambio efectivo Diffie-Hellman */
char					szPrivateSKey[8+1]="12345678";
/* Filename mask para trace */
const char				szTraceFilenameMask[]="h24ddlv4.%Y%m%d.log";
/* Filename para configuracion */
char					szConfFilename[256]="h24ddl.cnf";
/* IP Host y port por default : luego se inicializan desde un archivo exteno , H24DDL.CNF */
char					szHostName[256]="TANDEM";
unsigned short			usHostPort=9100;
/* Sin algoritmo de encripcion por default : luego se inicializa desde un archivo exteno , H24DDL.CNF */
unsigned short          ushAlgEncr=ALG_NULL;
/* Ultimo time-stamp de verificacion del host, en NULO por default */
time_t					tLastHostCheck=0L;
/* Version 0, inicial, por default */
short					shVersion=0;


/* is numeric string ? -macro- */
#define ISNUMERICSTR(x)	IsNumericString(x,sizeof(x))
#define ZEROAMMOUNT		"0000000000.0000"


/* is numeric string ? */
int IsNumericString( char *szString, int iLen)
{
	int i = 0,
		j = 0;
	/* check for digits */
	for(i=0; i < iLen && szString[i] != 0x00; i ++)
	{
		if(!isdigit(szString[i]))
			return FALSE;
		else
			j++;
	}/* end-for */
	/* return ok */
	return (j==iLen);
}

/* establece el prefijo del mensaje */
set_field_prefix(char *szPrefix, msg_type_enum mtType)
{	
	INIT_FIELD(unOutMsg.stHeader.canal_id, ' ');
	if( mtType == H200 )
	{
		COPY_FIELD(unOutMsg.stHeader.canal_id, szPrefix);	
		if(strcmp(szPrefix,"IP")==0)
			strnset(unOutMsg.stHeader.term_id, ' ', sizeof(unOutMsg.stHdrLog.term_id));
		else if(strcmp(szPrefix,"WEB")==0)
			strnset(unOutMsg.stHeader.term_id, ' ', sizeof(unOutMsg.stHdrLog.term_id));
	}
	else
	{
		COPY_FIELD(unOutMsg.stHdrLog.canal_id, szPrefix);
		if(strcmp(szPrefix,"IP")==0)
			strnset(unOutMsg.stHdrLog.term_id, ' ', sizeof(unOutMsg.stHdrLog.term_id));
		else if(strcmp(szPrefix,"WEB")==0)
			strnset(unOutMsg.stHdrLog.term_id, ' ', sizeof(unOutMsg.stHdrLog.term_id));
	}
}


/* formateo del nro. aleatorio de secuencia */
set_field_seqnum(msg_type_enum mtType)
{		
	long ulSeqNum = 0L;
	char szSeqNum[sizeof(unOutMsg.stHeader.sec_transmision)+1];

	srand( time(NULL) + rand() );
	ulSeqNum = rand();

	sprintf(szSeqNum, "%0*i", sizeof(unOutMsg.stHeader.sec_transmision), ulSeqNum );
	
	if(mtType == H600) 
		strncpy( unOutMsg.stHdrLog.sec_transmision, szSeqNum, sizeof unOutMsg.stHeader.sec_transmision);
	else if(mtType == H200)
		strncpy( unOutMsg.stHeader.sec_transmision, szSeqNum, sizeof unOutMsg.stHeader.sec_transmision);
	else if(mtType == H205)
		strncpy( unOutMsg.stHeader.sec_transmision, szSeqNum, sizeof unOutMsg.stHeader.sec_transmision);
	else
		strncpy( unOutMsg.stHeader.sec_transmision, szSeqNum, sizeof unOutMsg.stHeader.sec_transmision);
}

/* formateo del crc */
int set_field_crc(msg_type_enum mtType, int iEncLen)
{		
	char *pszMsgBegin = NULL;
	char szMD5[32+1]={0}; /* MD5 fingerprint */
	int  iLen=0;
	
	if(mtType == H600) 
	{		
		pszMsgBegin = unOutMsg.stHdrLog.term_id;
#if ( _CRC_BEFORE_DES_ == _CRC_DES_OPTION_ )
		iLen = sizeof(unOutMsg.stH600) - 
			(pszMsgBegin - unOutMsg.stHeader.canal_id);
#else ( _CRC_AFTER_DES_ == _CRC_DES_OPTION_ )
		iLen = iEncLen - (pszMsgBegin - unOutMsg.stHeader.canal_id);
#endif
	}
	else if(mtType == H200)
	{		
		pszMsgBegin = unOutMsg.stHeader.term_id;
#if ( _CRC_BEFORE_DES_ == _CRC_DES_OPTION_ )
		iLen =	sizeof(unOutMsg.stH200)  -
				(pszMsgBegin - unOutMsg.stHeader.canal_id);
#else
		iLen = iEncLen - (pszMsgBegin - unOutMsg.stHeader.canal_id);
#endif
		/* el mensaje cifrado incluye espacios en blanco, en bloques multiplos de 8 bytes */
		if((iLen%8)!=0)
			iLen += 8 - iLen%8;
	}
	else
	{		
		pszMsgBegin = unOutMsg.stHeader.term_id;
		iLen= sizeof(unOutMsg.stH200) - (pszMsgBegin - unOutMsg.stHeader.canal_id);
	}

	INIT_FIELD (unOutMsg.stHeader.crc, '0');	
	MD5String( pszMsgBegin, iLen, szMD5 );
	strupr( szMD5 );
	strncpy( unOutMsg.stHeader.crc, szMD5, sizeof unOutMsg.stHeader.crc);
	return iLen;
	
}

/* formateo del header del msg de login */
format_header_login(int iTerminal, int iMsgLen, char *szMsgType, char *szTranCode, char *szUserID)
{		
	/*************************************************************************************************/
	COPY_FIELD(unOutMsg.stHdrLog.canal_id, "IVR");	
	/*INIT_FIELD (unOutMsg.stHdrLog.version_sw, '0'); */
	/* Version 1.00 de Aplicacion IVR                 */
	sprintf(unOutMsg.stHdrLog.version_sw, "%0*i", sizeof unOutMsg.stHdrLog.version_sw, 100 );
	COPY_FIELD(unOutMsg.stHdrLog.msg_type, szMsgType);
	COPY_FIELD(unOutMsg.stHdrLog.tran_code, szTranCode);	
	sprintf(unOutMsg.stHdrLog.long_msg, "%0*i", sizeof unOutMsg.stHdrLog.long_msg, iMsgLen );	
	if(ALG_DES == ushAlgEncr)
		COPY_FIELD(unOutMsg.stHdrLog.encr_type, "DES");
	else if(ALG_RC4 == ushAlgEncr)
		COPY_FIELD(unOutMsg.stHdrLog.encr_type, "RC4");
	else
		COPY_FIELD(unOutMsg.stHdrLog.encr_type, "   ");
	INIT_FIELD (unOutMsg.stHdrLog.crc, '0');	
	/* El nombre de terminal es "IVR@@@@@@", donde "@@@@@@" identifica el nro. de canal telefonico */
	/* Este nombre de terminal debe existir en el archivo de terminales del device handler de IVR  */
	sprintf(unOutMsg.stHdrLog.term_id, "IVR%0*i", sizeof(unOutMsg.stHdrLog.term_id)-3, iTerminal );
	sprintf(unOutMsg.stHdrLog.user_id, "%-*s", sizeof unOutMsg.stHdrLog.user_id, szUserID );		
	INIT_FIELD (unOutMsg.stHdrLog.sec_autorizacion, '0');
	INIT_FIELD (unOutMsg.stHdrLog.sec_transmision, '0');
	/*************************************************************************************************/
}

/* establecer la longitud del mensaje H200 */
set_msg_len_h200( int iMsgLen )
{
	char szLenght[sizeof(unOutMsg.stHeader.long_msg)+1];
	sprintf(szLenght, "%0*i", sizeof unOutMsg.stHeader.long_msg, iMsgLen );
	strncpy( unOutMsg.stHeader.long_msg, szLenght, sizeof unOutMsg.stHeader.long_msg);
}

/* establecer la longitud del mensaje H200 */  
set_msg_len_h205( int iMsgLen )
{
	char szLenght[sizeof(unOutMsg.stHeader.long_msg)+1];
	sprintf(szLenght, "%0*i", sizeof unOutMsg.stHeader.long_msg, iMsgLen );
	strncpy( unOutMsg.stHeader.long_msg, szLenght, sizeof unOutMsg.stHeader.long_msg);
}

/* formatear inicialmente el mensaje H200 */
format_h200(int iTerminal, char *szTranCode, char *szUserID)
{
	/*************************************************************************************************/
	memset( &unOutMsg.stH200, ' ', sizeof unOutMsg.stH200 );
	/*************************************************************************************************/
	format_header(iTerminal,
					/* longitud del mensaje no incluye el CRC  */
					sizeof(unOutMsg.stH200) -
					(unOutMsg.stH200.stHeader.tran_code - unOutMsg.stH200.stHeader.canal_id), 
					"H200", 
					szTranCode, 
					szUserID);
	format_body_h200();
	/*************************************************************************************************/
}							

/* formateo del cuerpo del msg */
format_body_h205()
{		
	int			inx          = 0;
	time_t		tmDataTo     = time(NULL) ;
	time_t		tmDataFrom   = time(NULL) - ( 30*24*60*60 ); /* 30 dias para atras */
	struct tm	ltDataTo    = *(localtime( &tmDataTo ));	
	struct tm	ltDataFrom  = *(localtime( &tmDataFrom ));
	char		*psz8Block   = NULL;
		
	/*************************************************************************************************/
	INIT_FIELD (unOutMsg.stH205.password,'0'); 		
	INIT_FIELD (unOutMsg.stH205.id_gerenciador, '0');
	INIT_FIELD (unOutMsg.stH205.prepaga_os, '0');
	INIT_FIELD (unOutMsg.stH205.resp_code, '0');
	INIT_FIELD (unOutMsg.stH205.nro_autorizacion, '0');	
	INIT_FIELD (unOutMsg.stH205.resp_timestamp, '0');
	INIT_FIELD (unOutMsg.stH205.cantidad_items, '0');

	INIT_FIELD (unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.fecha_recepcion_desde, '0');
	strftime(unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.fecha_recepcion_desde, 
			sizeof(unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.fecha_recepcion_desde)+1, 
			ISO_DATETIME_MS_MASK, &ltDataFrom );
	INIT_FIELD (unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.fecha_recepcion_hasta, '0');
	strftime(unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.fecha_recepcion_hasta, 
			sizeof(unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.fecha_recepcion_hasta)+1, 
			ISO_DATETIME_MS_MASK, &ltDataTo );
	unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.f_id_estado_comprobantes = ' '; /* indistinto */
	unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.f_id_estado_pagos = ' '; /* indistinto */
	unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.f_debitos_sn = ' '; /* indistinto */
	unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.f_tipo_comprobante = ' '; /* indistinto */
	INIT_FIELD ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.f_numero_comprobante, '0');
	INIT_FIELD ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.items, '0');
	INIT_FIELD_STRUCT ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[0], '0');
	INIT_FIELD_STRUCT ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[1], '0');
	INIT_FIELD_STRUCT ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[2], '0');
	INIT_FIELD_STRUCT ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[3], '0');
	INIT_FIELD_STRUCT ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[4], '0');
	INIT_FIELD_STRUCT ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[5], '0');
	INIT_FIELD_STRUCT ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[6], '0');
	INIT_FIELD_STRUCT ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[7], '0');
	INIT_FIELD_STRUCT ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[8], '0');
	INIT_FIELD_STRUCT ( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[9], '0');	
	
		
	/* completar con blancos un bloque de 8 bytes -para DES- */
	for(inx=0, psz8Block = &unOutMsg.stH205.end_of_text; inx<8; psz8Block++, inx++ )
			(*psz8Block) = ' ';
}

/* formatear inicialmente el mensaje H205 */
format_h205(int iTerminal, char *szTranCode, char *szUserID)
{
	/*************************************************************************************************/
	memset( &unOutMsg.stH205, ' ', sizeof unOutMsg.stH205 );
	/*************************************************************************************************/
	format_header(iTerminal,
					/* longitud del mensaje no incluye el CRC  */
					sizeof(unOutMsg.stH205) -
					(unOutMsg.stH205.stHeader.tran_code - unOutMsg.stH205.stHeader.canal_id), 
					"H205", 
					szTranCode, 
					szUserID);
	format_body_h205();
	/*************************************************************************************************/
}							

/* formatear inicialmente el mensaje H600 */
format_h600(int iTerminal, char *szTranCode, char *szUserID)
{
	/*************************************************************************************************/
	memset( &unOutMsg.stH600, ' ', sizeof unOutMsg.stH600 );
	/*************************************************************************************************/
	format_header_login(iTerminal,
						/* longitud del mensaje no incluye el CRC  */
						sizeof(unOutMsg.stH600) - 
						(unOutMsg.stH600.stHeader.term_id - unOutMsg.stH600.stHeader.canal_id), 
						"H600", 
						szTranCode, 
						szUserID);
	format_body_h600();
	/*************************************************************************************************/
}

/* formateo del header del msg general */
format_header(int iTerminal, int iMsgLen, char *szMsgType, char *szTranCode, char *szUserID)
{		
	/*************************************************************************************************/
	COPY_FIELD(unOutMsg.stHeader.canal_id, "IVR");
	/* INIT_FIELD (unOutMsg.stHeader.version_sw, '0');*/
	/* Version 1.00 de Aplicacion                     */
	sprintf(unOutMsg.stHeader.version_sw, "%0*i", sizeof unOutMsg.stHeader.version_sw, 100 );
	COPY_FIELD(unOutMsg.stHeader.msg_type, szMsgType);
	sprintf(unOutMsg.stHeader.long_msg, "%0*i", sizeof unOutMsg.stHeader.long_msg, iMsgLen );		
	if(ALG_DES == ushAlgEncr)
		COPY_FIELD(unOutMsg.stHeader.encr_type, "DES");
	else if(ALG_RC4 == ushAlgEncr)
		COPY_FIELD(unOutMsg.stHeader.encr_type, "RC4");
	else
		COPY_FIELD(unOutMsg.stHeader.encr_type, "   ");
	COPY_FIELD(unOutMsg.stHeader.tran_code, szTranCode);	
	INIT_FIELD (unOutMsg.stHeader.crc, '0');	
	sprintf(unOutMsg.stHeader.term_id, "IVR%0*i", sizeof(unOutMsg.stHeader.term_id)-3, iTerminal );
	sprintf(unOutMsg.stHeader.user_id, "%-*s", sizeof unOutMsg.stHeader.user_id, szUserID );		
	INIT_FIELD (unOutMsg.stHeader.sec_autorizacion, '0');
	INIT_FIELD (unOutMsg.stHeader.sec_transmision, '0');
	/*************************************************************************************************/
}

/* formateo del cuerpo del msg */
format_body_h600()
{			
	/* variables locales a la funcion de formateo del cuerpo del mensaje de login */
	int		retval = 0;
	char	szHOST[128] = { "(local)" };
	unsigned long binIPaddress = 0; /* 32 bits = 4 bytes */
	/* redefinicion de a 4 bytes sobre los 32 bits */
	struct ipAddress_tag {
		unsigned char byte[4]; /* 8 bits x 4 = 32 bits */
	} *pipAddress = (struct ipAddress_tag *) &binIPaddress;

	/* Se completan someramente los datos de ubicacion de la terminal IVR que son invariables */
	/* recuperar el IP y nombre del equipo */
	retval = get_local_host_config( szHOST, sizeof szHOST, &binIPaddress );

	/*************************************************************************************************/
	INIT_FIELD (unOutMsg.stH600.password,'0'); 
	INIT_FIELD (unOutMsg.stH600.fecha_ver_tablas_loc, '0');
	INIT_FIELD (unOutMsg.stH600.fecha_ver_datos_prest, '0');
	INIT_FIELD (unOutMsg.stH600.fecha_ver_diag_prest, '0');
	INIT_FIELD (unOutMsg.stH600.calle,' ');
	INIT_FIELD (unOutMsg.stH600.numero_calle,'0');
	INIT_FIELD (unOutMsg.stH600.planta,' ');
	INIT_FIELD (unOutMsg.stH600.piso,'0');
	INIT_FIELD (unOutMsg.stH600.depto,'0');
	INIT_FIELD (unOutMsg.stH600.ubicacion_edificio,' ');
	INIT_FIELD (unOutMsg.stH600.telefono,' ');
	INIT_FIELD (unOutMsg.stH600.fax,' ');
	INIT_FIELD (unOutMsg.stH600.cod_postal,' ');
	INIT_FIELD (unOutMsg.stH600.localidad,' ');
	INIT_FIELD (unOutMsg.stH600.provincia,' ');
	INIT_FIELD (unOutMsg.stH600.pais,' ');
	INIT_FIELD (unOutMsg.stH600.email,' ');
	/* Se agrega el nombre del equipo, a simple nivel informativo */	
	sprintf( unOutMsg.stH600.email, "IP=%u.%u.%u.%u NAME=%s",
		pipAddress->byte[0],
		pipAddress->byte[1],
		pipAddress->byte[2],
		pipAddress->byte[3],
		szHOST );
	/*************************************************************************************************/
}

/* formateo del cuerpo del msg */
format_body_h200()
{		
	int			inx        = 0;
	time_t		tmData     = time(NULL);
	struct tm	*ltData    = localtime( &tmData );
	char		*psz8Block = NULL;
		
	/*************************************************************************************************/
	INIT_FIELD (unOutMsg.stH200.password,'0'); 		
	INIT_FIELD (unOutMsg.stH200.id_gerenciador, '0');
	INIT_FIELD (unOutMsg.stH200.prepaga_os, '0');
	INIT_FIELD (unOutMsg.stH200.resp_code, '0');
	unOutMsg.stH200.bandeja_auditores = 'N';
	INIT_FIELD (unOutMsg.stH200.nro_autorizacion, '0');
	INIT_FIELD (unOutMsg.stH200.sec_transaccion, '0');
	INIT_FIELD (unOutMsg.stH200.nro_lote_canal, '0');
	INIT_FIELD (unOutMsg.stH200.resp_timestamp, '0');
	INIT_FIELD (unOutMsg.stH200.reversal_code, '0');
	/*************************************************************************************************/
	INIT_FIELD (unOutMsg.stH200.warnings, ' ');
	INIT_FIELD (unOutMsg.stH200.cod_verif, ' ');
	INIT_FIELD (unOutMsg.stH200.cant_reprocesos, '0');
	/*************************************************************************************************/			
	
	strftime(unOutMsg.stH200.unReq.stPrestacion.fecha_hora_prestacion, 
			sizeof(unOutMsg.stH200.unReq.stPrestacion.fecha_hora_prestacion)+1, 
			ISO_DATETIME_MS_MASK, ltData );
	unOutMsg.stH200.unReq.stPrestacion.cargada_on_sn = 'S'; // Si-No	
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.credencial_afiliado,'0');
	unOutMsg.stH200.unReq.stPrestacion.tipo_captura = '1'; // 1=manual,2=banda,3=smart card
	COPY_FIELD (unOutMsg.stH200.unReq.stPrestacion.vto_credencial,"204912"); // Default Diciembre 31, 2049
	COPY_FIELD (unOutMsg.stH200.unReq.stPrestacion.vto_credencial,"000000"); // Default 00, 0000
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.plan,' ');
	unOutMsg.stH200.unReq.stPrestacion.plan_manual_sn = 'S'; // Si-No
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.apellido_afiliado,' ');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.nombre_afiliado,' ');
	unOutMsg.stH200.unReq.stPrestacion.afiliado_manual_sn = 'N'; // Si-No
	unOutMsg.stH200.unReq.stPrestacion.cobra_copago_sn = 'N'; // Si-No
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.id_establecimiento,' ');
	unOutMsg.stH200.unReq.stPrestacion.cod_area_asistencial = '0';
	/*************************************************************************************************/
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.tipo_matricula_solic,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.nro_matricula_solic,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.nombre_prof_solic,' ');
	/*************************************************************************************************/
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.tipo_matricula_act,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.nro_matricula_act,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.id_profesional_act,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.nro_interno_prof_act,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.nombre_prof_act,' ');
	unOutMsg.stH200.unReq.stPrestacion.prof_act_manual_sn = 'S';
	/*************************************************************************************************/
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.id_equipo_act,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.especialidad_act,' ');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.nro_int_prestador,'0');	
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.id_catalogo_diag,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.codigo_diag,'0');		
	unOutMsg.stH200.unReq.stPrestacion.horario_programado_sn = 'S';          // horario programado S-N
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.nro_lote_prestador,'0');			
	COPY_FIELD (unOutMsg.stH200.unReq.stPrestacion.id_canal_orig, "IVR");    // IP,WEB,IVR,ALL
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.user_id_orig,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.tran_code_orig,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.fecha_hora_desde_orig, '0');
	COPY_FIELD (unOutMsg.stH200.unReq.stPrestacion.cantidad_prestaciones, "01");
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.filler, '-');
	/*************************************************************************************************/
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.sena_preautorizacion,'0');
	COPY_FIELD (unOutMsg.stH200.unReq.stPrestacion.id_tipo_prestacion,"01");
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.nro_lote_canc,'0');
	/*************************************************************************************************/
	COPY_FIELD (unOutMsg.stH200.unReq.stPrestacion.fecha_receta,
				unOutMsg.stH200.unReq.stPrestacion.fecha_hora_prestacion);
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.categ_profesional,'0');
	/*************************************************************************************************/
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.id_catalogo_proc_item, '0');
	unOutMsg.stH200.unReq.stPrestacion.stItem_presta.capitulo_item = '1'; // O=odontologicas, 0..999:Medicas
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.codigo_proc_item,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.cantidad_item,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.codigo_pieza_item,'0');
	COPY_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.caras_item, "NNNNN"); 
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.resp_code_item,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.control_auth_item,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.control_cancel_item,'0');
	unOutMsg.stH200.unReq.stPrestacion.stItem_presta.bandeja_aud_item = 'N';			// S-N
	unOutMsg.stH200.unReq.stPrestacion.stItem_presta.cancelada_sn     = 'N';			// S-N
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.cod_moneda_copago,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.importe_copago,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.importe_pagar,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.importe_pagar_iva,'0');
	INIT_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.importe_a_facturar,'0');
	COPY_FIELD (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.nro_item, "01");
	/*************************************************************************************************/
	
	
	/* completar con blancos un bloque de 8 bytes -para encripcion con DES- */
	for(inx=0, psz8Block = &unOutMsg.stH200.end_of_text; inx<8; psz8Block++, inx++ )
			(*psz8Block) = ' ';
}



/* establece los valores a los campos del registro */
set_field_h600(char *szField, char *szValue)
{
	/*************************************************************************************************/
	if IS_FIELD(szField,"password")
	{
		char szMD5[32+1]={0}; /* MD5 fingerprint */
		MD5String( szValue, strlen(szValue), szMD5 );
		strupr( szMD5 );
		COPY_FIELD (unOutMsg.stH600.password, szMD5);
	}	
	else if IS_FIELD(szField,"canal_id")
		COPY_FIELD  (unOutMsg.stHdrLog.canal_id, szValue);
	else if IS_FIELD(szField,"term_id")
		COPY_FIELD  (unOutMsg.stHdrLog.term_id, szValue);
	else if IS_FIELD(szField,"sec_transmision")
		COPY_FIELD  (unOutMsg.stHdrLog.sec_transmision, szValue);
	else if IS_FIELD(szField,"fecha_ver_tablas_loc")
		COPY_FIELD (unOutMsg.stH600.fecha_ver_tablas_loc, szValue);
	else if IS_FIELD(szField,"fecha_ver_datos_prest")
		COPY_FIELD  (unOutMsg.stH600.fecha_ver_datos_prest, szValue);
	else if IS_FIELD(szField,"fecha_ver_diag_prest")
		COPY_FIELD  (unOutMsg.stH600.fecha_ver_diag_prest, szValue);
	else if IS_FIELD(szField,"calle")
		COPY_FIELD  (unOutMsg.stH600.calle, szValue);
	else if IS_FIELD(szField,"numero_calle")
		COPY_FIELD  (unOutMsg.stH600.numero_calle, szValue);
	else if IS_FIELD(szField,"planta")
		COPY_FIELD  (unOutMsg.stH600.planta,szValue);
	else if IS_FIELD(szField,"piso")
		COPY_FIELD  (unOutMsg.stH600.piso,szValue);
	else if IS_FIELD(szField,"depto")
		COPY_FIELD  (unOutMsg.stH600.depto,szValue);
	else if IS_FIELD(szField,"ubicacion_edificio")
		COPY_FIELD  (unOutMsg.stH600.ubicacion_edificio,szValue);
	else if IS_FIELD(szField,"telefono")
		COPY_FIELD  (unOutMsg.stH600.telefono,szValue);
	else if IS_FIELD(szField,"fax")
		COPY_FIELD  (unOutMsg.stH600.fax,szValue);
	else if IS_FIELD(szField,"cod_postal")
		COPY_FIELD  (unOutMsg.stH600.cod_postal,szValue);
	else if IS_FIELD(szField,"localidad")
		COPY_FIELD  (unOutMsg.stH600.localidad,szValue);
	else if IS_FIELD(szField,"provincia")
		COPY_FIELD  (unOutMsg.stH600.provincia,szValue);
	else if IS_FIELD(szField,"pais")
		COPY_FIELD  (unOutMsg.stH600.pais,szValue);
	else if IS_FIELD(szField,"email")
		COPY_FIELD  (unOutMsg.stH600.email,szValue);
	else
		strcpy(szValue, "ERROR");	
	/*************************************************************************************************/
}

/* recuperar valores de los campos del registro */
get_field_h610(char *szField, char *szValue)
{
	/*************************************************************************************************/
	if (IS_FIELD(szField,"tipo_login") || IS_FIELD(szField,"tran_code"))
		MOVE_FIELD  (szValue, unInMsg.stHdrLog.tran_code);
	else if IS_FIELD(szField,"term_id")
		MOVE_FIELD  (szValue, unInMsg.stHdrLog.term_id);
	else if IS_FIELD(szField,"sec_autorizacion")	
		MOVE_FIELD  (szValue, unInMsg.stHdrLog.sec_autorizacion);
	else if IS_FIELD(szField,"sec_transmision")	 /* verificado ok con EWF */
		MOVE_FIELD  (szValue, unInMsg.stHdrLog.sec_transmision);
	else if IS_FIELD(szField,"resp_timestamp")
		MOVE_FIELD  (szValue, unInMsg.stH610.resp_timestamp);
	else if (IS_FIELD(szField,"resp_code")||IS_FIELD(szField,"resp_code_header"))
		MOVE_FIELD  (szValue, unInMsg.stH610.resp_code);
	else if IS_FIELD(szField,"descr_resp_code")
		MOVE_FIELD  (szValue, unInMsg.stH610.descr_resp_code);
	else if IS_FIELD(szField,"accion") {
		szValue[0] = unInMsg.stH610.accion; 
		szValue[1] = 0x00; }
	else if IS_FIELD(szField,"tipo_usuario") {
		szValue[0] = unInMsg.stH610.tipo_usuario;
		szValue[1] = 0x00; }
	else if IS_FIELD(szField,"clave_sesion") /* verificado ok con EWF */
		MOVE_FIELD  (szValue, unInMsg.stH610.clave_sesion);
	else if IS_FIELD(szField,"clave_sesion_descifrada") 
		MOVE_FIELD  (szValue, szSessionKey);
	else if IS_FIELD(szField,"fecha_ult_dep")
		MOVE_FIELD  (szValue, unInMsg.stH610.fecha_ult_dep);
	else if IS_FIELD(szField,"sincronizar_sn") {
		szValue[0] = unInMsg.stH610.sincronizar_sn; 
		szValue[1] = 0x00; }			 
	else
		return -1;
	return 0;
	/*************************************************************************************************/
}

/* establece los valores a los campos del registro */
set_field_h200(char *szField, char *szValue)
{
	/*************************************************************************************************/
	if IS_FIELD(szField,"password") /* verificado ok con EWF */
	{
		char szMD5[32+1]={0}; /* MD5 fingerprint */
		MD5String( szValue, strlen(szValue), szMD5 );
		strupr( szMD5 );
		COPY_FIELD (unOutMsg.stH200.password, szMD5);
	}
	else if IS_FIELD(szField,"canal_id")
		COPY_FIELD  (unOutMsg.stHeader.canal_id, szValue);
	else if IS_FIELD(szField,"term_id")
		COPY_FIELD  (unOutMsg.stHeader.term_id, szValue);
	else if (IS_FIELD(szField,"id_financiador") || IS_FIELD(szField,"id_gerenciador")) /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.id_gerenciador,szValue);
	else if IS_FIELD(szField,"prepaga_os")
		COPY_FIELD  (unOutMsg.stH200.prepaga_os,szValue);
	else if IS_FIELD(szField,"bandeja_auditores")
		unOutMsg.stH200.bandeja_auditores = szValue[0];
	else if IS_FIELD(szField,"nro_autorizacion") /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.nro_autorizacion,szValue);
	else if IS_FIELD(szField,"sec_transaccion")
		COPY_FIELD  (unOutMsg.stH200.sec_transaccion,szValue);
	else if IS_FIELD(szField,"nro_lote_canal")
		COPY_FIELD  (unOutMsg.stH200.nro_lote_canal,szValue);
	else if IS_FIELD(szField,"resp_timestamp")
		COPY_FIELD  (unOutMsg.stH200.resp_timestamp,szValue);
	else if IS_FIELD(szField,"reversal_code")
		COPY_FIELD  (unOutMsg.stH200.reversal_code,szValue);
	/*************************************************************************************************/
	else if IS_FIELD(szField,"warnings")
		COPY_FIELD  (unOutMsg.stH200.warnings,szValue);
	else if IS_FIELD(szField,"cod_verif")
		COPY_FIELD  (unOutMsg.stH200.cod_verif,szValue);
	else if IS_FIELD(szField,"cant_reprocesos")
		COPY_FIELD  (unOutMsg.stH200.cant_reprocesos,szValue);
	/*************************************************************************************************/
	else if IS_FIELD(szField,"fecha_prestacion")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.fecha_hora_prestacion,szValue);
	else if IS_FIELD(szField,"cargada_on_sn")
		unOutMsg.stH200.unReq.stPrestacion.cargada_on_sn = szValue[0]; 
	else if IS_FIELD(szField,"credencial_afiliado") /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.credencial_afiliado,szValue);
	else if IS_FIELD(szField,"tipo_captura")
		unOutMsg.stH200.unReq.stPrestacion.tipo_captura = szValue[0];
	else if IS_FIELD(szField,"vto_credencial") /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.vto_credencial,szValue);
	else if IS_FIELD(szField,"plan")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.plan,szValue);
	else if IS_FIELD(szField,"plan_manual_sn")
		unOutMsg.stH200.unReq.stPrestacion.plan_manual_sn = szValue[0];
	else if IS_FIELD(szField,"apellido_afiliado")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.apellido_afiliado,szValue);
	else if IS_FIELD(szField,"nombre_afiliado")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.nombre_afiliado,szValue);
	else if IS_FIELD(szField,"afiliado_manual_sn")
		unOutMsg.stH200.unReq.stPrestacion.afiliado_manual_sn = szValue[0];
	else if IS_FIELD(szField,"cobra_copago_sn")
		unOutMsg.stH200.unReq.stPrestacion.cobra_copago_sn = szValue[0];
	else if IS_FIELD(szField,"id_establecimiento")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.id_establecimiento,szValue);
	else if IS_FIELD(szField,"cod_area_asistencial") /* verificado ok con EWF */
		unOutMsg.stH200.unReq.stPrestacion.cod_area_asistencial = szValue[0];
	/*************************************************************************************************/
	else if IS_FIELD(szField,"tipo_matricula_solic")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.tipo_matricula_solic,szValue);
	else if IS_FIELD(szField,"nro_matricula_solic")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.nro_matricula_solic,szValue);
	else if IS_FIELD(szField,"nombre_prof_solic")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.nombre_prof_solic,szValue);
	/*************************************************************************************************/
	else if (IS_FIELD(szField,"tipo_matricula_act")||IS_FIELD(szField,"tipo_matricula")) /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.tipo_matricula_act,szValue);
	else if (IS_FIELD(szField,"nro_matricula_act")||IS_FIELD(szField,"nro_matricula"))   /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.nro_matricula_act,szValue);
	else if IS_FIELD(szField,"nombre_prof_act")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.nombre_prof_act,szValue);
	else if (IS_FIELD(szField,"id_profesional_act")||IS_FIELD(szField,"id_profesional")) /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.id_profesional_act,szValue);
	else if IS_FIELD(szField,"nro_interno_prof_act")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.nro_interno_prof_act,szValue);
	else if IS_FIELD(szField,"nombre_prof_act")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.nombre_prof_act,szValue);
	else if IS_FIELD(szField,"prof_act_manual_sn")
		unOutMsg.stH200.unReq.stPrestacion.prof_act_manual_sn = szValue[0];
	/*************************************************************************************************/
	else if (IS_FIELD(szField,"id_equipo_act")||IS_FIELD(szField,"id_equipo")) /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.id_equipo_act,szValue);
	else if IS_FIELD(szField,"especialidad_act")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.especialidad_act,szValue);	
	else if IS_FIELD(szField,"nro_int_prestador")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.nro_int_prestador,szValue);
	/*************************************************************************************************/
	else if IS_FIELD(szField,"id_catalogo_diag") /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.id_catalogo_diag,szValue);
	else if IS_FIELD(szField,"codigo_diag")      /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.codigo_diag,szValue);
	else if IS_FIELD(szField,"horario_programado_sn")
		unOutMsg.stH200.unReq.stPrestacion.horario_programado_sn = szValue[0];
	/*************************************************************************************************/
	else if IS_FIELD(szField,"nro_lote_prestador")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.nro_lote_prestador,szValue);
	else if IS_FIELD(szField,"id_canal_orig")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.id_canal_orig, "IVR"); /* IP,WEB,IVR,ALL */
	else if IS_FIELD(szField,"user_id_orig")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.user_id_orig,szValue);
	else if IS_FIELD(szField,"fecha_hora_desde_orig")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.fecha_hora_desde_orig,szValue);
	else if IS_FIELD(szField,"cantidad_prestaciones") 
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.cantidad_prestaciones,szValue);
	else if IS_FIELD(szField,"filler")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.filler,szValue);
	/**********************************************************************************/
	else if IS_FIELD(szField,"sena_preautorizacion")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.sena_preautorizacion,szValue);
	else if IS_FIELD(szField,"id_tipo_prestacion")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.id_tipo_prestacion,szValue);
	else if IS_FIELD(szField,"nro_lote_canc")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.nro_lote_canc,szValue);
	/**********************************************************************************/
	else if IS_FIELD(szField,"id_catalogo_proc") /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.id_catalogo_proc_item,szValue);
	else if IS_FIELD(szField,"capitulo")         /* verificado ok con EWF */
		unOutMsg.stH200.unReq.stPrestacion.stItem_presta.capitulo_item = szValue[0];
	else if IS_FIELD(szField,"codigo_proc")      /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.codigo_proc_item,szValue);
	else if IS_FIELD(szField,"cantidad")         /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.cantidad_item,szValue);
	else if IS_FIELD(szField,"codigo_pieza")     /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.codigo_pieza_item,szValue);
	else if IS_FIELD(szField,"cara1")
		unOutMsg.stH200.unReq.stPrestacion.stItem_presta.caras_item[0] = szValue[0]; 
	else if IS_FIELD(szField,"cara2")
		unOutMsg.stH200.unReq.stPrestacion.stItem_presta.caras_item[1] = szValue[1]; 
	else if IS_FIELD(szField,"cara3")
		unOutMsg.stH200.unReq.stPrestacion.stItem_presta.caras_item[2] = szValue[2]; 
	else if IS_FIELD(szField,"cara4")
		unOutMsg.stH200.unReq.stPrestacion.stItem_presta.caras_item[3] = szValue[3]; 
	else if IS_FIELD(szField,"cara5")				   
		unOutMsg.stH200.unReq.stPrestacion.stItem_presta.caras_item[4] = szValue[4]; 
	else if IS_FIELD(szField,"resp_code")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.resp_code_item,szValue);
	else if IS_FIELD(szField,"control_auth")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.control_auth_item,szValue);
	else if IS_FIELD(szField,"control_cancel") /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.control_cancel_item,szValue);
	else if IS_FIELD(szField,"bandeja_aud")
		unOutMsg.stH200.unReq.stPrestacion.stItem_presta.bandeja_aud_item = szValue[0]; 
	else if IS_FIELD(szField,"cancelada_sn")
		unOutMsg.stH200.unReq.stPrestacion.stItem_presta.cancelada_sn = szValue[0]; 
	else if IS_FIELD(szField,"cod_moneda_copago")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.cod_moneda_copago,szValue);
	else if IS_FIELD(szField,"importe_copago")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.importe_copago,szValue);
	else if IS_FIELD(szField,"importe_pagar")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.importe_pagar,szValue);
	else if IS_FIELD(szField,"importe_pagar_iva")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.importe_pagar_iva,szValue);
	else if IS_FIELD(szField,"importe_a_facturar")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.importe_a_facturar,szValue);
	else if IS_FIELD(szField,"nro_item")
		COPY_FIELD  (unOutMsg.stH200.unReq.stPrestacion.stItem_presta.nro_item,szValue);
	else if IS_FIELD(szField,"nueva_password") /* verificado ok con EWF */		
	{
		char szMD5[32+1]={0}; /* MD5 fingerprint */
		MD5String( szValue, strlen(szValue), szMD5 );
		strupr( szMD5 );
		/* nueva password */
		COPY_FIELD  (unOutMsg.stH200.unReq.stCambio_passw.nueva_password,szMD5);
		/* anterior password */
		COPY_FIELD  (unOutMsg.stH200.unReq.stCambio_passw.vieja_password,
					 unOutMsg.stH200.password);
	}
	else if IS_FIELD(szField,"vieja_password") 		
		COPY_FIELD  (unOutMsg.stH200.unReq.stCambio_passw.vieja_password,szValue);
	else
		strcpy(szValue, "ERROR");
	/*************************************************************************************************/
}


/* establece los valores a los campos del registro DE CONSULTA */
set_field_h205(char *szField, char *szValue)
{
	/*************************************************************************************************/
	if IS_FIELD(szField,"password") /* verificado ok con EWF */
	{
		char szMD5[32+1]={0}; /* MD5 fingerprint */
		MD5String( szValue, strlen(szValue), szMD5 );
		strupr( szMD5 );
		COPY_FIELD (unOutMsg.stH205.password, szMD5);
	}
	else if IS_FIELD(szField,"canal_id")
		COPY_FIELD  (unOutMsg.stHeader.canal_id, szValue);
	else if IS_FIELD(szField,"term_id")
		COPY_FIELD  (unOutMsg.stHeader.term_id, szValue);
	else if (IS_FIELD(szField,"id_financiador") || IS_FIELD(szField,"id_gerenciador")) /* verificado ok con EWF */
		COPY_FIELD  (unOutMsg.stH205.id_gerenciador,szValue);
	else if IS_FIELD(szField,"prepaga_os")
		COPY_FIELD  (unOutMsg.stH205.prepaga_os,szValue);
	else if IS_FIELD(szField,"resp_code")
		COPY_FIELD  (unOutMsg.stH205.resp_code,szValue);
	else if IS_FIELD(szField,"nro_autorizacion")
		COPY_FIELD  (unOutMsg.stH205.nro_autorizacion,szValue);
	else if IS_FIELD(szField,"resp_timestamp")
		COPY_FIELD  (unOutMsg.stH205.resp_timestamp,szValue);
	else if IS_FIELD(szField,"cantidad_items")
		COPY_FIELD  (unOutMsg.stH205.cantidad_items,szValue);
	else if IS_FIELD(szField,"cantidad_items")
		COPY_FIELD  (unOutMsg.stH205.cantidad_items,szValue);
	/***************************************************************************************/
	else if IS_FIELD(szField,"tipo_comprobante")
		unOutMsg.stH205.unUltMovCab.stCtaCte.tipo_comprobante = szValue[0];
	else if IS_FIELD(szField,"nro_comprobante")
		COPY_FIELD  (unOutMsg.stH205.unUltMovCab.stCtaCte.nro_comprobante,szValue);
	else if IS_FIELD(szField,"nro_orden_pago")
		COPY_FIELD  (unOutMsg.stH205.unUltMovCab.stCtaCte.nro_orden_pago,szValue);
	else if IS_FIELD(szField,"ctacte_detalle_unico")
		COPY_FIELD_STRUCT( unOutMsg.stH205.unUltMovCab.stCtaCte, szValue);
	/***************************************************************************************/
	else if IS_FIELD(szField,"fecha_recepcion_desde")
		COPY_FIELD  (unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.fecha_recepcion_desde,szValue);
	else if IS_FIELD(szField,"fecha_recepcion_hasta")
		COPY_FIELD  (unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.fecha_recepcion_hasta,szValue);
	else if IS_FIELD(szField,"f_id_estado_comprobantes")
		unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.f_id_estado_comprobantes = szValue[0];
	else if IS_FIELD(szField,"f_id_estado_pagos")
		unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.f_id_estado_pagos = szValue[0];
	else if IS_FIELD(szField,"f_debitos_sn")
		unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.f_debitos_sn = szValue[0];
	else if IS_FIELD(szField,"f_tipo_comprobante")
		unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.f_tipo_comprobante = szValue[0];
	else if IS_FIELD(szField,"f_numero_comprobante")
		COPY_FIELD  (unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.f_numero_comprobante,szValue);
	else if IS_FIELD(szField,"items")
		COPY_FIELD  (unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.items,szValue);
	else if IS_FIELD(szField,"ctacte_detalle_1")
		COPY_FIELD_STRUCT( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[0], szValue);
	else if IS_FIELD(szField,"ctacte_detalle_2")
		COPY_FIELD_STRUCT( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[1], szValue);
	else if IS_FIELD(szField,"ctacte_detalle_3")
		COPY_FIELD_STRUCT( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[2], szValue);
	else if IS_FIELD(szField,"ctacte_detalle_4")
		COPY_FIELD_STRUCT( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[3], szValue);
	else if IS_FIELD(szField,"ctacte_detalle_5")
		COPY_FIELD_STRUCT( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[4], szValue);
	else if IS_FIELD(szField,"ctacte_detalle_6")
		COPY_FIELD_STRUCT( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[5], szValue);
	else if IS_FIELD(szField,"ctacte_detalle_7")
		COPY_FIELD_STRUCT( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[6], szValue);
	else if IS_FIELD(szField,"ctacte_detalle_8")
		COPY_FIELD_STRUCT( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[7], szValue);
	else if IS_FIELD(szField,"ctacte_detalle_9")
		COPY_FIELD_STRUCT( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[8], szValue);
	else if IS_FIELD(szField,"ctacte_detalle_10")
		COPY_FIELD_STRUCT( unOutMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[9], szValue);
	else
		strcpy(szValue, "ERROR");
	/*************************************************************************************************/
}

/* establece los valores a los campos del registro DE CONSULTA */
get_field_h215(char *szField, char *szValue)
{
	/*************************************************************************************************/
	if IS_FIELD(szField,"term_id")
		MOVE_FIELD  (szValue,unInMsg.stHeader.term_id);
	else if IS_FIELD(szField,"tran_code")
		MOVE_FIELD  (szValue,unInMsg.stHeader.tran_code);
	else if IS_FIELD(szField,"user_id")
		MOVE_FIELD  (szValue,unInMsg.stHeader.user_id);
	else if IS_FIELD(szField,"canal_id")
		MOVE_FIELD  (szValue, unInMsg.stHeader.canal_id);
	/*************************************************************************************************/
	else if (IS_FIELD(szField,"id_financiador") || IS_FIELD(szField,"id_gerenciador")) /* verificado ok con EWF */
		MOVE_FIELD  ( szValue, unInMsg.stH205.id_gerenciador);
	else if IS_FIELD(szField,"prepaga_os")
		MOVE_FIELD  ( szValue, unInMsg.stH205.prepaga_os);
	else if IS_FIELD(szField,"resp_code_header")
		MOVE_FIELD  ( szValue,unInMsg.stH205.resp_code);
	else if IS_FIELD(szField,"nro_autorizacion")
		MOVE_FIELD  ( szValue,unInMsg.stH205.nro_autorizacion);
	else if IS_FIELD(szField,"resp_timestamp")
		MOVE_FIELD  ( szValue,unInMsg.stH205.resp_timestamp);
	else if IS_FIELD(szField,"cantidad_items")
		MOVE_FIELD  ( szValue,unInMsg.stH205.cantidad_items);
	else if IS_FIELD(szField,"cantidad_items")
		MOVE_FIELD  ( szValue,unInMsg.stH205.cantidad_items);
	/*************************************************************************/	
	else if IS_FIELD(szField,"tipo_comprobante")
	{
		szValue[0] = unOutMsg.stH205.unUltMovCab.stCtaCte.tipo_comprobante;
		szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"nro_comprobante")
		MOVE_FIELD  (unInMsg.stH215.unUltMovCab.stCtaCte.nro_comprobante,szValue);
	else if IS_FIELD(szField,"nro_orden_pago")
		MOVE_FIELD  (unInMsg.stH215.unUltMovCab.stCtaCte.nro_orden_pago,szValue);
	else if IS_FIELD(szField,"ctacte_detalle_unico")
		MOVE_FIELD_STRUCT(szValue, unInMsg.stH215.unUltMovCab.stCtaCte);
	/*************************************************************************/
	else if IS_FIELD(szField,"fecha_recepcion_desde")
		MOVE_FIELD  ( szValue,unInMsg.stH205.unUltMovCab.stUltmovCtaCte.fecha_recepcion_desde);
	else if IS_FIELD(szField,"fecha_recepcion_hasta")
		MOVE_FIELD  ( szValue,unInMsg.stH205.unUltMovCab.stUltmovCtaCte.fecha_recepcion_hasta);
	else if IS_FIELD(szField,"f_id_estado_comprobantes")
		unInMsg.stH205.unUltMovCab.stUltmovCtaCte.f_id_estado_comprobantes = szValue[0];
	else if IS_FIELD(szField,"f_id_estado_pagos")
		unInMsg.stH205.unUltMovCab.stUltmovCtaCte.f_id_estado_pagos = szValue[0];
	else if IS_FIELD(szField,"f_debitos_sn")
		unInMsg.stH205.unUltMovCab.stUltmovCtaCte.f_debitos_sn = szValue[0];
	else if IS_FIELD(szField,"f_tipo_comprobante")
		unInMsg.stH205.unUltMovCab.stUltmovCtaCte.f_tipo_comprobante = szValue[0];
	else if IS_FIELD(szField,"f_numero_comprobante")
		MOVE_FIELD  ( szValue,unInMsg.stH205.unUltMovCab.stUltmovCtaCte.f_numero_comprobante);
	else if IS_FIELD(szField,"items")
		MOVE_FIELD  ( szValue,unInMsg.stH205.unUltMovCab.stUltmovCtaCte.items);
	else if IS_FIELD(szField,"ctacte_detalle_1")
		MOVE_FIELD_STRUCT( szValue, unInMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[0]);
	else if IS_FIELD(szField,"ctacte_detalle_2")
		MOVE_FIELD_STRUCT( szValue, unInMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[1]);
	else if IS_FIELD(szField,"ctacte_detalle_3")
		MOVE_FIELD_STRUCT( szValue, unInMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[2]);
	else if IS_FIELD(szField,"ctacte_detalle_4")
		MOVE_FIELD_STRUCT( szValue, unInMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[3]);
	else if IS_FIELD(szField,"ctacte_detalle_5")
		MOVE_FIELD_STRUCT( szValue, unInMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[4]);
	else if IS_FIELD(szField,"ctacte_detalle_6")
		MOVE_FIELD_STRUCT( szValue, unInMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[5]);
	else if IS_FIELD(szField,"ctacte_detalle_7")
		MOVE_FIELD_STRUCT( szValue, unInMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[6]);
	else if IS_FIELD(szField,"ctacte_detalle_8")
		MOVE_FIELD_STRUCT( szValue, unInMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[7]);
	else if IS_FIELD(szField,"ctacte_detalle_9")
		MOVE_FIELD_STRUCT( szValue, unInMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[8]);
	else if IS_FIELD(szField,"ctacte_detalle_10")
		MOVE_FIELD_STRUCT( szValue, unInMsg.stH205.unUltMovCab.stUltmovCtaCte.stDetUmovCtaCte[9]);
	else
		strcpy(szValue, "ERROR");
	/*************************************************************************************************/
}

/* recuperar valores de los campos del registro */
get_field_h210(char *szField, char *szValue)
{
	/*************************************************************************************************/
	if IS_FIELD(szField,"term_id")
		MOVE_FIELD  (szValue,unInMsg.stHeader.term_id);
	else if IS_FIELD(szField,"tran_code")
		MOVE_FIELD  (szValue,unInMsg.stHeader.tran_code);
	else if IS_FIELD(szField,"user_id")
		MOVE_FIELD  (szValue,unInMsg.stHeader.user_id);
	else if (IS_FIELD(szField,"id_financiador") || IS_FIELD(szField,"id_gerenciador")) /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.id_gerenciador);
	else if IS_FIELD(szField,"prepaga_os")
		MOVE_FIELD  (szValue,unInMsg.stH210.prepaga_os);
	else if IS_FIELD(szField,"bandeja_auditores")
	{
		szValue[0] = unInMsg.stH210.bandeja_auditores ; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"nro_autorizacion") /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.nro_autorizacion);
	else if IS_FIELD(szField,"sec_transaccion")  /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.sec_transaccion);
	else if IS_FIELD(szField,"nro_lote_canal")
		MOVE_FIELD  (szValue,unInMsg.stH210.nro_lote_canal);
	else if IS_FIELD(szField,"resp_timestamp")
		MOVE_FIELD  (szValue,unInMsg.stH210.resp_timestamp);
	else if IS_FIELD(szField,"reversal_code")
		MOVE_FIELD  (szValue,unInMsg.stH210.reversal_code);
	/*************************************************************************************************/
	else if IS_FIELD(szField,"warnings")
		MOVE_FIELD  (szValue,unOutMsg.stH200.warnings);
	else if IS_FIELD(szField,"cod_verif")
		MOVE_FIELD  (szValue,unOutMsg.stH200.cod_verif);
	else if IS_FIELD(szField,"cant_reprocesos")
		MOVE_FIELD  (szValue,unOutMsg.stH200.cant_reprocesos);
	/*************************************************************************************************/
	else if IS_FIELD(szField,"fecha_prestacion")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.fecha_hora_prestacion);
	else if IS_FIELD(szField,"cargada_on_sn")
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.cargada_on_sn ; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"credencial_afiliado")  /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.credencial_afiliado);
	else if IS_FIELD(szField,"tipo_captura")
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.tipo_captura; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"vto_credencial")	     /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.vto_credencial);
	else if IS_FIELD(szField,"plan")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.plan);
	else if IS_FIELD(szField,"plan_manual_sn")
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.plan_manual_sn; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"apellido_afiliado")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.apellido_afiliado);
	else if IS_FIELD(szField,"nombre_afiliado")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.nombre_afiliado);
	else if IS_FIELD(szField,"afiliado_manual_sn")
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.afiliado_manual_sn; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"cobra_copago_sn")
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.cobra_copago_sn; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"id_establecimiento")   /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.id_establecimiento);
	else if IS_FIELD(szField,"cod_area_asistencial") /* verificado ok con EWF */
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.cod_area_asistencial; szValue[1] = 0x00;
	}
	/*************************************************************************************************/
	else if IS_FIELD(szField,"tipo_matricula_solic")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.tipo_matricula_solic);
	else if IS_FIELD(szField,"nro_matricula_solic")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.nro_matricula_solic);
	else if IS_FIELD(szField,"nombre_prof_solic")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.nombre_prof_solic);
	/*************************************************************************************************/
	else if (IS_FIELD(szField,"tipo_matricula_act")||IS_FIELD(szField,"tipo_matricula")) /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.tipo_matricula_act);
	else if (IS_FIELD(szField,"nro_matricula_act") || IS_FIELD(szField,"nro_matricula")) /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.nro_matricula_act);
	else if IS_FIELD(szField,"nombre_prof_act")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.nombre_prof_act);
	else if (IS_FIELD(szField,"id_profesional_act")||IS_FIELD(szField,"id_profesional")) /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.id_profesional_act);
	else if IS_FIELD(szField,"nro_interno_prof_act")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.nro_interno_prof_act);
	else if IS_FIELD(szField,"nombre_prof_act")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.nombre_prof_act);
	else if IS_FIELD(szField,"prof_act_manual_sn")
		unInMsg.stH210.unReq.stPrestacion.prof_act_manual_sn = szValue[0];
	/*************************************************************************************************/
	else if (IS_FIELD(szField,"id_equipo_act")||IS_FIELD(szField,"id_equipo")) /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.id_equipo_act);
	else if IS_FIELD(szField,"especialidad_act")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.especialidad_act);	
	else if IS_FIELD(szField,"nro_int_prestador")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.nro_int_prestador);
	/*************************************************************************************************/
	else if IS_FIELD(szField,"id_catalogo_diag")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.id_catalogo_diag);
	else if IS_FIELD(szField,"codigo_diag") /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.codigo_diag);
	else if IS_FIELD(szField,"horario_programado_sn")
		unInMsg.stH210.unReq.stPrestacion.horario_programado_sn = szValue[0];
	/*************************************************************************************************/
	else if IS_FIELD(szField,"nro_lote_prestador")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.nro_lote_prestador);
	else if IS_FIELD(szField,"id_canal_orig")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.id_canal_orig); /* IP,WEB,IVR,ALL */
	else if IS_FIELD(szField,"user_id_orig")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.user_id_orig);
	else if IS_FIELD(szField,"fecha_hora_desde_orig")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.fecha_hora_desde_orig);
	else if IS_FIELD(szField,"cantidad_prestaciones")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.cantidad_prestaciones);
	else if IS_FIELD(szField,"filler")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.filler);
	/**********************************************************************************/
	else if IS_FIELD(szField,"sena_preautorizacion")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.sena_preautorizacion);
	else if IS_FIELD(szField,"id_tipo_prestacion")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.id_tipo_prestacion);
	else if IS_FIELD(szField,"nro_lote_canc")    /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.nro_lote_canc);
	/**********************************************************************************/
	else if IS_FIELD(szField,"id_catalogo_proc") /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.id_catalogo_proc_item);
	else if IS_FIELD(szField,"capitulo")         /* verificado ok con EWF */
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.stItem_presta.capitulo_item ; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"codigo_proc")      /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.codigo_proc_item);
	else if IS_FIELD(szField,"cantidad")         /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.cantidad_item);
	else if IS_FIELD(szField,"codigo_pieza")     /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.codigo_pieza_item);
	else if IS_FIELD(szField,"cara1")
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.stItem_presta.caras_item[0]; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"cara2")
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.stItem_presta.caras_item[1]; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"cara3")
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.stItem_presta.caras_item[2]; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"cara4")
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.stItem_presta.caras_item[3]; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"cara5")				   
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.stItem_presta.caras_item[4]; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"resp_code") /* vs. resp_code_header */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.resp_code_item);
	else if IS_FIELD(szField,"control_auth")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.control_auth_item);
	else if IS_FIELD(szField,"control_cancel") /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.control_cancel_item);
	else if IS_FIELD(szField,"bandeja_aud")
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.stItem_presta.bandeja_aud_item; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"cancelada_sn")
	{
		szValue[0] = unInMsg.stH210.unReq.stPrestacion.stItem_presta.cancelada_sn; szValue[1] = 0x00;
	}
	else if IS_FIELD(szField,"cod_moneda_copago")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.cod_moneda_copago);
	else if IS_FIELD(szField,"importe_copago")
	{
		if('N'==unInMsg.stH210.unReq.stPrestacion.cobra_copago_sn ||
		   'n'==unInMsg.stH210.unReq.stPrestacion.cobra_copago_sn )
		   MOVE_FIELD  (szValue,ZEROAMMOUNT);
		else
		{
			if(ISNUMERICSTR(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_copago))
			{
			   memcpy  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_copago, 
				   sizeof(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_copago)-4);
			   szValue[sizeof(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_copago)-4] = '.' ;
			   memcpy  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_copago,  4);
			}/*end-if*/
			else
				MOVE_FIELD  (szValue,ZEROAMMOUNT);
		}/*end-if*/
	}
	else if IS_FIELD(szField,"importe_pagar")		
	{
		if(ISNUMERICSTR(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_pagar))
		{			
		   memcpy  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_pagar, 
			   sizeof(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_pagar)-4);
		   szValue[sizeof(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_pagar)-4] = '.' ;
		   memcpy  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_pagar,  4);
		}
		else
			MOVE_FIELD  (szValue,ZEROAMMOUNT);
	}
	else if IS_FIELD(szField,"importe_pagar_iva")		
	{
		if(ISNUMERICSTR(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_pagar_iva))
		{
		   memcpy  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_pagar_iva, 
			   sizeof(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_pagar_iva)-4);
		   szValue[sizeof(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_pagar_iva)-4] = '.' ;
		   memcpy  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_pagar_iva,  4);
		}
		else
			MOVE_FIELD  (szValue,ZEROAMMOUNT);
	}
	else if IS_FIELD(szField,"importe_a_facturar")
	{
		if(ISNUMERICSTR(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_a_facturar))
		{		
		   memcpy  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_a_facturar, 
			   sizeof(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_a_facturar)-4);
		   szValue[sizeof(unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_a_facturar)-4] = '.' ;
		   memcpy  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.importe_a_facturar,  4);
		}
		else
			MOVE_FIELD  (szValue,ZEROAMMOUNT);
	}
	else if IS_FIELD(szField,"nro_item")
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stPrestacion.stItem_presta.nro_item);
	else if IS_FIELD(szField,"nueva_password") /* verificado ok con EWF */
		MOVE_FIELD  (szValue,unInMsg.stH210.unReq.stCambio_passw.nueva_password);	
	else
		strcpy(szValue, "ERROR");
	/*************************************************************************************************/
}

