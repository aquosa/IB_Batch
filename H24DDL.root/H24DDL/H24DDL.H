/*************************************************************************************************
 * H24 Data Definition Language
 * 2000.11.06, mdc. Etapa 1.
 * 2000.12.11, mdc. Etapa 2. Cambio de longitudes en 2 campos.
 * 2001.02.07, mdc. Prefijo variable. Stress.
 * 2001.04.20, mdc. Etapa 3. Nueva mensajeria Multiples Prestaciones.
 * 2001.04.25, mdc. Etapa 3. Ultimos Movimientos y Cuentas Corrientes.
 * 2001.04.27, mdc. Etapa 3. get_local_host_config
 * 2001.05.09, mdc. Etapa 3. Mas campos en mensajes, y parametros en funciones.
 * 2001.06.04, mdc. trans_autorizadas en el H610, y nro_lote_canc en H200.
 * 2001.06.23, mdc. INIT_FIELD_STRUCT / enum msg_type { H200, H600, H205 } ;
 *************************************************************************************************/

#ifndef _H24DDL_H_
#define _H24DDL_H_

#ifndef TRUE
#define FALSE (0)
#define TRUE  (!FALSE)
#endif

/* opciones de compilacion con o sin DES o CRC */
#ifndef _CRC_BEFORE_DES_
#define _CRC_BEFORE_DES_	(1)
#define _CRC_AFTER_DES_		(2)
#define _CRC_WITHOUT_DES_	(3)
#define _CRC_DES_OPTION_	_CRC_BEFORE_DES_
#endif

/* tipos de algoritmos de encripcion usados */
#define ALG_NULL (0)
#define ALG_DES	 (1)
#define ALG_RC4  (2)
#define ALG_IDEA (3)


/* macro de chequeo de nombre de campo en registro */
#ifndef IS_FIELD
#define IS_FIELD(param,field)				(strcmp(param, field)==0)
#define MIN_FIELD_SIZE(field)				min(sizeof(field),strlen(field))
#define MIN_FIELDS_SIZE_COPY(field,source)	min(sizeof(field),strlen(source))
#define INIT_FIELD(field,value)				memset(field,value,sizeof field)
#define COPY_FIELD(field,source)			memcpy(field,source,MIN_FIELDS_SIZE_COPY(field,source))
#define MOVE_FIELD(field,source)			memcpy(field,source,sizeof source )
#define ISO_DATETIME_MASK					"%Y%m%d%H%M%S"
#define ISO_DATETIME_MS_MASK				"%Y%m%d%H%M%S00"
#define ISO_DATE_MASK						"%Y%m%d"
#define COPY_FIELD_STRUCT(destiny,source)	memcpy( &destiny, source, MIN_FIELDS_SIZE_COPY(destiny,source))
#define MOVE_FIELD_STRUCT(destiny,source)	memcpy( destiny, &source, sizeof source )
#define INIT_FIELD_STRUCT(field,value)		memset( &field,value,sizeof field)
#endif /* IS_FIELD */


/*************************************************************************************************
 * header de mensaje general
 *************************************************************************************************/
typedef struct HXXX_HEADER {	
	char	canal_id[3],		 // web,ip,ivr
			version_sw[5],       // version
			msg_type[4],         // h600, h200, hxxx,...
			////////////////////////////////////////////////////////////////////////////
			tran_code[6],        // 603000=aut.,653000=preaut.,713000=valid.,703000=afi.valid.
			                     // 803000=cancel.,723000=cons.umov.,903000=passw.change
			////////////////////////////////////////////////////////////////////////////
			long_msg[5],         // longitud mensaje
			////////////////////////////////////////////////////////////////////////////
			encr_type[3],        // Encripcion DES, RC4 o en blanco
			////////////////////////////////////////////////////////////////////////////			
			crc[32],             // checksum con MD5, no con CRC
			term_id[32],         // terminal
			user_id[10],         // Compuesto de 6 digs.=prestador,3=usuario,1=dig.ver
			sec_transmision[5],  // secuencia transmision
			sec_autorizacion[5]; // secuencia autorizacion
} HXXX_HEADER_ST;
/*************************************************************************************************/


/*************************************************************************************************
 * header de mensaje hasta CRC
 *************************************************************************************************/
typedef struct HXXX_HEADER_CRC {	
	char	canal_id[3],         // web,ip,ivr
			version_sw[5],       // version
			msg_type[4],         // h600, h200, hxxx,...
			////////////////////////////////////////////////////////////////////////////
			tran_code[6],        // 603000=aut.,653000=preaut.,713000=valid.,703000=afi.valid.
			                     // 803000=cancel.,723000=cons.umov.,903000=passw.change
			////////////////////////////////////////////////////////////////////////////
			long_msg[5],         // longitud mensaje
			////////////////////////////////////////////////////////////////////////////
			encr_type[3],        // Encripcion DES, RC4 o en blanco
			////////////////////////////////////////////////////////////////////////////			
			crc[32];             // checksum con MD5, no con CRC
} HXXX_HEADER_CRC_ST;
/*************************************************************************************************/


/*************************************************************************************************
 * header de mensaje general, en blanco, sin encriptar
 *************************************************************************************************/
typedef struct HXXX_HEADER_NO_CRYPT {	
	char	canal_id[3],		 // web,ip,ivr
			version_sw[5],       // version
			msg_type[4],         // h600, h200, hxxx,...
			////////////////////////////////////////////////////////////////////////////
			tran_code[6],        // 603000=aut.,653000=preaut.,713000=valid.,703000=afi.valid.
			                     // 803000=cancel.,723000=cons.umov.,903000=passw.change
			////////////////////////////////////////////////////////////////////////////
			long_msg[5],         // longitud mensaje
			////////////////////////////////////////////////////////////////////////////
			encr_type[3],        // Encripcion DES, RC4 o en blanco
			////////////////////////////////////////////////////////////////////////////			
			crc[32],             // checksum con MD5, no con CRC
			term_id[32],         // terminal
			user_id[10];         // 6=prestador,3=usuario,1=dig.ver
} HXXX_HEADER_NO_CRYPT_ST;
/*************************************************************************************************/


/*************************************************************************************************
 * header de login
 *************************************************************************************************/
typedef struct HXXX_HEADER_LOGIN {	
	char	canal_id[3],         // web,ip,ivr
			version_sw[5],       // version
			msg_type[4],         // h600, h200, hxxx,...
			////////////////////////////////////////////////////////////////////////////
			tran_code[6],        // 000001=login, 000002=logout
			////////////////////////////////////////////////////////////////////////////
			long_msg[5],         // longitud mensaje
			////////////////////////////////////////////////////////////////////////////
			encr_type[3],        // Encripcion DES, RC4 o en blanco
			////////////////////////////////////////////////////////////////////////////
			crc[32],             // checksum con MD5, no con CRC			
			term_id[32],         // terminal
			user_id[10],         // 6=prestador,3=usuario,1=dig.ver
			sec_transmision[5],	 // secuencia transmision
			sec_autorizacion[5]; // secuencia autorizacion
} HXXX_HEADER_LOGIN_ST;
/*************************************************************************************************/


/*************************************************************************************************
 * mensaje de login
 *************************************************************************************************/
typedef struct H600_LOGIN {
	HXXX_HEADER_LOGIN_ST	stHeader;
	char	password[32],                // N digitos hasheados con MD5
			fecha_ver_tablas_loc[12],    // yyyymmddhhmm
			fecha_ver_datos_prest[12],   // yyyymmddhhmm
			fecha_ver_diag_prest[12],    // yyyymmddhhmm
			calle[30],
			numero_calle[5],
			planta[3],
			piso[2],
			depto[3],					 // 3 digitos en ETAPA-3. 2001.05.11,mdc.
			ubicacion_edificio[40],
			telefono[15],
			fax[15],
			cod_postal[8],
			localidad[40],
			provincia[40],
			pais[40],
			email[40];			
} H600_LOGON_ST;
/*************************************************************************************************/


/*************************************************************************************************
 * respuesta de login
 *************************************************************************************************/
typedef struct H610_LOGIN {
	HXXX_HEADER_LOGIN_ST	stHeader;
	char	password[32];             // N digitos hasheados con MD5
	char	resp_timestamp[14],       // timestamp de rta.
			resp_code[3],             // codigo de rta.
			descr_resp_code[99],	  // descripcion del error
			accion,                   // 1=ok,2=carga,3=error
			tipo_usuario,             //N=normal, A=administrador
			clave_sesion[16],         // clave de sesion
			fecha_ult_dep[8],         // fecha de ultima depuracion
			sincronizar_sn,           // 0=No,1=parcial,2=total,3=pendiente
			trans_autorizadas[20];    // Bytemap de transacciones autorizadas
} H610_LOGON_ST;
/*************************************************************************************************/


/*************************************************************************************************
 * mensaje de requerimiento de trx
 *************************************************************************************************/
typedef struct H200_REQUEST {
	HXXX_HEADER_ST	stHeader;
	char	password[32],             // N digitos hasheados con MD5
		    // Begin-2001.04.20, mdc. Multiples Prestaciones
		    id_gerenciador[6],        // Gerenciador
			prepaga_os[6],            // Prepaga Ob.Social
			resp_code[3],             // Cod.Rta.
			bandeja_auditores,        // Bandeja de Auditores?
			nro_autorizacion[10],     // Control Auth - Nro Auth
			sec_transaccion[5],       // Secuencia transaccion			
			nro_lote_canal[15],       // numero de lote
			resp_timestamp[14],       // fecha-hora rta.
			reversal_code[3];         // codigo de reverso
			//char accion;                   // accion
			union u_request_tag {
				/*******************************************************
				 * Requerimiento Multiple
				 *******************************************************/
				struct t_presta_tag {
				char	fecha_hora_prestacion[14],
						cargada_on_sn,           // cargada S/N
						credencial_afiliado[25], // credencial afiliado
						tipo_captura,            // 1=manual,2=banda,3=smart card
						vto_credencial[6],       // vto. credencial
						plan[40],                // plan
						plan_manual_sn,          // 
						apellido_afiliado[25],   // apellido afiliado
						nombre_afiliado[25],     // nombre afiliado
						afiliado_manual_sn,      // 
						cobra_copago_sn,         // copago s-n?			
						id_establecimiento[10],
						cod_area_asistencial,
						/////////////////////////////////////////////
						tipo_matricula_solic[2], // prof.solicitante
						nro_matricula_solic[12],
						nombre_prof_solic[52],
						/////////////////////////////////////////////
						tipo_matricula_act[2], // prof.actuante
						nro_matricula_act[12],
						id_profesional_act[6],
						nro_interno_prof_act[16],
						nombre_prof_act[52],
						prof_act_manual_sn,    
						id_equipo_act[6],
						especialidad_act[6],
						nro_int_prestador[10],
						/////////////////////////////////////////////
						id_catalogo_diag[4],
						codigo_diag[10],
						horario_programado_sn,
						/////////////////////////////////////////////
						nro_lote_prestador[15],
						id_canal_orig[3],           // IP,WEB,IVR,ALL
						user_id_orig[10],
						tran_code_orig[6],
						fecha_hora_desde_orig[14],
						cantidad_prestaciones[2],
						/////////////////////////////////////////////
						filler[200],			    // Filler
				        sena_preautorizacion[10];
				char	id_tipo_prestacion[2],		// Tipo de prestacion
						nro_lote_canc[15];			// Nro. lote cancelacion
						/////////////////////////////////////////////
						struct t_item__presta_tag {
							char	id_catalogo_proc_item[4],
									capitulo_item, // O=odontologicos, P=Odont. No nomenclado, N:1..9999=Medico
									codigo_proc_item[15],
									cantidad_item[7],
									codigo_pieza_item[2],
									caras_item[5],				// 5 repeticiones de 1 a 5, "S" o "N"
									resp_code_item[3],			// response code
									motivo_audit[3],			// motivo auditoria
									control_auth_item[10],
									control_cancel_item[10],
									bandeja_aud_item,			// S-N
									cancelada_sn,				// S-N
									cod_moneda_copago[3],
									importe_copago[14],
									importe_pagar[14],
									importe_pagar_iva[14],
									importe_a_facturar[14],
									item_procesado,				// S-N
									nro_item[2];							
						} stItem_presta;
				} stPrestacion;
				/*******************************************************
				 * Cambio de Contraseña
				 *******************************************************/
				struct t_cambio_passw_tag {
					char	nueva_password[32];      // nueva password hasheada MD5
					char	vieja_password[32];      // vieja password hasheada MD5
				} stCambio_passw;				
				/*******************************************************
				 * Validacion Afiliado
				 *******************************************************/
				struct t_val_afiliado_tag {
					char	fecha_hora_prestacion[14],
							cargada_on_sn,           // cargada S/N
							credencial_afiliado[25], // credencial afiliado
							tipo_captura,            // 1=manual,2=banda,3=smart card
							vto_credencial[6],       // vto. credencial
							plan[40],                // plan
							plan_manual_sn,          // 
							apellido_afiliado[25],   // apellido afiliado
							nombre_afiliado[25],     // nombre afiliado
							afiliado_manual_sn,      // 
							filler[200];			 // filler
				} stVal_Afiliado;
			} unReq;
			char	end_of_text;
			// End-2001.04.23, mdc. Multiples Prestaciones
						
} H200_REQUEST_ST;
/*************************************************************************************************/

/*************************************************************************************************
 * respuesta de requerimiento
 *************************************************************************************************/
typedef H200_REQUEST_ST H210_RESPONSE_ST  ;


/*************************************************************************************************
 * mensaje de consulta de ult.movs.
 *************************************************************************************************/
typedef struct H205_REQUEST {
	HXXX_HEADER_ST	stHeader;
	char	password[32],             // N digitos hasheados con MD5		    
		    id_gerenciador[6],        // Gerenciador
			prepaga_os[6],            // Prepaga Ob.Social
			resp_code[3],             // Cod.Rta.
			//bandeja_auditores,      // Bandeja de Auditores?
			nro_autorizacion[10],     // Control Auth - Nro Auth			
			resp_timestamp[14],       // fecha-hora rta.
			cantidad_items[2];        // Q items			
			/* cabeceras y-o detalle de movimiento especifico */
			union u_ult_mov_cab_tag {
				/* detalle de movimiento especifico */
				struct t_ult_mov_tag {
				char	id_canal_cons[3],
						user_id_cons[10],
						fecha_hora_desde[14],
						fecha_hora_hasta[14],
						respuesta,				// respuesta {0,1,2,3} no un codigo de 3
						credencial_filtro[25],
						numero_lote_cons[15];
				char	numero_lote_filtro[15],
						control_auth_filtro[10];
						/* detalle de movimiento */
						struct t_detalle_umov_tag {
							char	nro_lote_canal[15],
									control_auth_orig[10],
									resp_code_orig[3],
									fecha_hora_prest[14],
									cargada_on_sn,           
									credencial_afiliado[25], 
									tipo_captura,            
									vto_credencial[6],       
									plan[40],                
									plan_manual_sn,          
									apellido_afiliado[25],   
									nombre_afiliado[25],     
									afiliado_manual_sn,      
									cobra_copago_sn,         
									id_establecimiento[10],
									cod_area_asistencial,									
									tipo_matricula_solic[2], 
									nro_matricula_solic[12],
									nombre_prof_solic[52],									
									tipo_matricula_act[2], 
									nro_matricula_act[12],
									id_profesional_act[6],
									nro_interno_prof_act[16],
									nombre_prof_act[52],
									prof_act_manual_sn,    
									id_equipo_act[6],
									especialidad_act[6],
									nro_int_prestador[10],
									id_catalogo_diag[4],
									codigo_diag[10],
									horario_programado_sn,									
									nro_lote_prestador[15],
									id_canal_orig[3],     
									user_id_orig[10],
									tran_code_orig[6],
									fecha_hora_desde_orig[14],
									cantidad_prestaciones[2],
									filler[200],
									sena_preautorizacion[10],
									tipo_prestacion[2],
									nro_lote_canc[14],
									tran_code_canc[6],
									id_catalogo_proc_item[4],
									capitulo_item, 
									codigo_proc_item[15],
									cantidad_item[7],
									codigo_pieza_item[2],
									caras_item[5],				
									resp_code_item[3],	
									motivo_audit[3],
									control_auth_item[10],
									control_cancel_item[10],
									bandeja_aud_item,			
									cancelada_sn,				
									cod_moneda_copago[3],
									importe_copago[14],
									importe_pagar[14],
									importe_pagar_iva[14],
									importe_a_facturar[14],
									nro_item[2];									
						} stDetalle_umov_tag;
				} stUltMov_tag;
				/* cabeceras de transacciones */
				struct t_cabeceras_tag {
					char	nro_lote_canal[15];
					struct	t_detalle_cab_tag {
						char	id_catalogo_proc_item[4],
								capitulo_item, 
								codigo_proc_item[15],
								cantidad_item[7],
								codigo_pieza_item[2],
								caras_item[5],				
								resp_code_item[3],			
								motivo_audit[3],
								control_auth_item[10],
								control_cancel_item[10],
								bandeja_aud_item,			
								cancelada_sn,				
								cod_moneda_copago[3],
								importe_copago[14],
								importe_pagar[14],
								importe_pagar_iva[14],
								importe_a_facturar[14],
								nro_item[2];
						} stDetalleCab[20]; /* maximo de 20 repeticiones */
				} stCabeceras;
				/* consultas de cuentas corrientes */
				struct t_ctacte_tag {							// offset_base_one + field_length
					char	tipo_comprobante,					// 1+1
						    nro_comprobante[16],				// 2+16
							nro_orden_pago[10];					// 18+10
					struct  t_item_ctacte_tag {					// offset_base_one + field_length
						char	fecha_emision[8],				// 28+8
								fecha_recepcion_gerenciador[8],	// 36+8
								id_estados_comprobantes,		// 44+1
								fecha_emision_orden_pago[8],	// 45+8
								recibo_fiscal_sn,				// 53+1
								id_estado_pagos,				// 54+1
								total_facturado[14],			// 55+14
								cant_items_facturados[5],		// 69+5
								total_liquidado[14],			// 74+14
								cant_items_liquidados[5],		// 88+5
								total_deb_aud_med[14],			// 93+14
								cant_items_deb_aud_med[5],		// 107+5
								total_deb_mot_varios[14],		// 112+14
								cant_items_tot_varios[5],		// 126+5
								total_ret_impositiva[14];		// 131+14
					} stDetCtaCte;
				} stCtaCte; 
				/* consultas de ult.mov. cuentas corrientes */
				struct t_ultmov_ctacte_tag {
					char	fecha_recepcion_desde[8],
							fecha_recepcion_hasta[8],
						    f_id_estado_comprobantes,
							f_id_estado_pagos,
							f_debitos_sn,
							f_tipo_comprobante,
							f_numero_comprobante[16],
							items[5];
					struct  t_umov_det_ctacte_tag {				// offset_base_one + field_length
						char	tipo_comprobante,				// 1+1
								nro_comprobante[16],			// 2+16
								fecha_emision[8],				// 18+8
								fecha_recepcion_gerenciador[8],	// 26+8
								id_estados_comprobantes,		// 34+1
								nro_orden_pago[10],				// 35+10
								fecha_emision_orden_pago[8],	// 45+8
								recibo_fiscal_sn,				// 53+1
								id_estado_pagos,				// 54+1
								total_facturado[14],			// 55+14
								cant_items_facturados[5],		// 69+5
								total_liquidado[14],			// 74+14
								cant_items_liquidados[5],		// 88+5
								total_aud_med[14],				// 93+14
								cant_items_aud_med[5],			// 107+5
								total_varios[14],				// 112+14
								cant_items_varios[5],			// 126+5
								total_ret_impositivas[14];		// 131+14
					} stDetUmovCtaCte[10]; /* 10 movimientos maximo */
				} stUltmovCtaCte; 
			} unUltMovCab;
			char end_of_text;
} H205_REQUEST_ST;

/*************************************************************************************************
 * respuesta de requerimiento consulta
 *************************************************************************************************/
typedef H205_REQUEST_ST H215_RESPONSE_ST  ;


/*************************************************************************************************
 * union general de tipos de mensajes, hasta 8192 (8kb) bytes
 *************************************************************************************************/
typedef union HXXX_MESSAGES {
	HXXX_HEADER_LOGIN_ST	stHdrLog;
	HXXX_HEADER_ST			stHeader;
	HXXX_HEADER_NO_CRYPT_ST	stHeader_NoCrypt;
	H600_LOGON_ST			stH600;
	H610_LOGON_ST			stH610;
	H200_REQUEST_ST			stH200;
	H210_RESPONSE_ST		stH210;
	H205_REQUEST_ST			stH205;
	H215_RESPONSE_ST		stH215;
	char					chBuffer[15360]; /* 15KB */
} HXXX_MESSAGES_UN;
/*************************************************************************************************


/*************************************************************************************************
 * mensaje de envio TCP/IP con longitud indicativa prededente , de 16 bits
 *************************************************************************************************/
typedef struct HXXX_MESSAGES_host {
	unsigned short		len; /* longitud del mensaje , para subsistema de comunicaciones */
	HXXX_MESSAGES_UN	data;/* mensaje de aplicacion en si mismo */
} HXXX_MESSAGES_HOST_ST;
/*************************************************************************************************/


typedef enum msg_type { H200, H600, H205 } msg_type_enum;



/*************************************************************************************************
 * prototipos 
 *************************************************************************************************/
format_header_login(int iTerminal, int iMsgLen, char *szMsgType, char *szTranCode, char *szUserID);
format_header(int iTerminal, int iMsgLen, char *szMsgType, char *szTranCode, char *szUserID);
format_h600(int iTerminal, char *szTranCode, char *szUserID);
format_body_h600();
format_h200(int iTerminal, char *szTranCode, char *szUserID);
format_body_h200();
set_field_h200(char *szField, char *szValue);
get_field_h210(char *szField, char *szValue);
format_h205(int iTerminal, char *szTranCode, char *szUserID);
set_field_h205(char *szField, char *szValue);
get_field_h215(char *szField, char *szValue);
format_h600(int iTerminal, char *szTranCode, char *szUserID);
set_field_h600(char *szField, char *szValue);
get_field_h610(char *szField, char *szValue);
set_field_seqnum(msg_type_enum mtType);
int set_field_crc(msg_type_enum mtType, int iEncLen);
set_msg_len_h200( int iMsgLen );
set_msg_len_h205( int iMsgLen );
int send_receive_h200(void);
int send_receive_h205(void);
int send_receive_h600(void);
int send_h420(void);
int send_h200(void);
int get_remote_host_config( char **pszHost, short *pshPort, short *pshAlgEncr );
check_remote_host_config( void );
log_msg_h600( HXXX_MESSAGES_UN *pMsg);
log_msg_h610( HXXX_MESSAGES_UN *pMsg);
log_msg_h200( HXXX_MESSAGES_UN *pMsg);
log_msg_h210( HXXX_MESSAGES_UN *pMsg);
log_msg_h205( HXXX_MESSAGES_UN *pMsg);
log_msg_h215( HXXX_MESSAGES_UN *pMsg);
int encrypt_msg(HXXX_MESSAGES_UN *pClear, char *szKey, HXXX_MESSAGES_UN *pCiphered, short tfHexa);
int decrypt_msg(HXXX_MESSAGES_UN *pClear, char *szKey, HXXX_MESSAGES_UN *pCiphered, short shLen, short tfHexa);
int antoi(char *pchAsciiNum, int iLen);
int set_config_filename( char *szFilename, short shVersion );
set_field_prefix( char *szPrefix, msg_type_enum mtType);
int send_generic(char *szHost, unsigned short usPort, char *szBuffer , short bPP_Protocol );
get_local_host_config(char *szStrName, int uNameSize, unsigned long *lpAddress);
/*************************************************************************************************/

#endif // #ifndef _H24DDL_H_