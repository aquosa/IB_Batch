/**************************************************************************************
 * H24 Data Definition Language, Definitions, and Methods
 * v2.0 2000.12.22 Martin D Cernadas. Etapa 2.
 * v3.0 2001.04.24 Martin D Cernadas. Etapa 3. Nueva mensajeria Multiples Prestaciones.
 * v3.1 2001.05.09 Martin D Cernadas. Etapa 3. Nuevos campos.
 **************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "h24ddl.h"
#include "..\md5lib\md5lib.h"
#include "..\deslib\deslib.h"
#include "..\tcpsend\tcpsend.h"

/* File name para trace */
extern const char			szTraceFilenameMask[];


/* loggear mensaje H600 formateado */
log_msg_h600( HXXX_MESSAGES_UN *pMsg)
{			
	char	szDateTime[]      = "yyyymmddhhmissms";
	char	szTraceFilename[256] = {0};	
	FILE	*hFile = NULL;
	time_t	tmData = time(NULL);
	struct tm *plc_Data = localtime( &tmData );

	/* precondicion */
	if(!plc_Data || 0L == tmData || !pMsg)
		return -1;
	/* fecha y hora actuales */
	strftime(szDateTime, sizeof(szDateTime), ISO_DATETIME_MASK,	plc_Data );
	strftime(szTraceFilename, sizeof(szTraceFilename), szTraceFilenameMask,	plc_Data );
	/* abrir logger */
	hFile=fopen(szTraceFilename,"at+");	
	if(!hFile)
		return -2;

	/* mostrar campo a campo el detalle del mensaje H600 */
	fprintf(hFile,"\r\n[###%s###]=",szDateTime);
	fprintf(hFile,"|%.*s", sizeof pMsg->stHdrLog.canal_id, pMsg->stHdrLog.canal_id );
	fprintf(hFile,"|%.*s", sizeof pMsg->stHdrLog.version_sw, pMsg->stHdrLog.version_sw);
	fprintf(hFile,"|%.*s", sizeof pMsg->stHdrLog.msg_type, pMsg->stHdrLog.msg_type);
	fprintf(hFile,"|%.*s", sizeof pMsg->stHdrLog.tran_code, pMsg->stHdrLog.tran_code);
	fprintf(hFile,"|%.*s", sizeof pMsg->stHdrLog.long_msg, pMsg->stHdrLog.long_msg );
	fprintf(hFile,"|%.*s", sizeof pMsg->stHdrLog.encr_type, pMsg->stHdrLog.encr_type);	
	fprintf(hFile,"|%.*s", sizeof pMsg->stHdrLog.crc, pMsg->stHdrLog.crc);	
	fprintf(hFile,"|%.*s", sizeof pMsg->stHdrLog.term_id, pMsg->stHdrLog.term_id);
	fprintf(hFile,"|%.*s", sizeof pMsg->stHdrLog.user_id, pMsg->stHdrLog.user_id);		
	fprintf(hFile,"|%.*s", sizeof pMsg->stHdrLog.sec_autorizacion, pMsg->stHdrLog.sec_autorizacion);
	fprintf(hFile,"|%.*s", sizeof pMsg->stHdrLog.sec_transmision, pMsg->stHdrLog.sec_transmision);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.password,pMsg->stH600.password);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.fecha_ver_tablas_loc,pMsg->stH600.fecha_ver_tablas_loc);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.fecha_ver_datos_prest,pMsg->stH600.fecha_ver_datos_prest);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.fecha_ver_diag_prest,pMsg->stH600.fecha_ver_diag_prest);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.calle,pMsg->stH600.calle);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.numero_calle,pMsg->stH600.numero_calle);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.planta,pMsg->stH600.planta);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.piso,pMsg->stH600.piso);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.depto,pMsg->stH600.depto);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.ubicacion_edificio,pMsg->stH600.ubicacion_edificio);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.telefono,pMsg->stH600.telefono);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.fax,pMsg->stH600.fax);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.cod_postal,pMsg->stH600.cod_postal);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.localidad,pMsg->stH600.localidad);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.provincia,pMsg->stH600.provincia);
	fprintf(hFile,"|%.*s", sizeof pMsg->stH600.pais,pMsg->stH600.pais);
	fprintf(hFile,"|%.*s|\r\n", sizeof pMsg->stH600.email,pMsg->stH600.email);	
	fprintf(hFile,"\r\n");
	/* cerrar logger*/

	fclose(hFile);
	hFile = NULL;

	return 0;

}

/* loggear rta. del H600 formateado */
log_msg_h610( HXXX_MESSAGES_UN *pMsg)
{			
	char	szDateTime[]      = "yyyymmddhhmissms";
	char	szTraceFilename[256] = {0};	
	FILE	*hFile = NULL;
	time_t	tmData = time(NULL);
	struct tm *plc_Data = localtime( &tmData );

	/* precondicion */
	if(!plc_Data || 0L == tmData || !pMsg)
		return -1;
	/* fecha y hora actuales */
	strftime(szDateTime, sizeof(szDateTime), ISO_DATETIME_MASK,	plc_Data );
	strftime(szTraceFilename, sizeof(szTraceFilename), szTraceFilenameMask,	plc_Data );
	/* abrir logger */
	hFile=fopen(szTraceFilename,"at+");	
	if(!hFile)
		return -2;

	/* mostrar campo a campo el detalle del mensaje H610 */
	fprintf(hFile,"\r\n[###%s###]=",szDateTime);
	fprintf(hFile,"|canal=%.*s", sizeof pMsg->stHdrLog.canal_id, pMsg->stHdrLog.canal_id );
	fprintf(hFile,"|version=%.*s", sizeof pMsg->stHdrLog.version_sw, pMsg->stHdrLog.version_sw);
	fprintf(hFile,"|msg_type=%.*s", sizeof pMsg->stHdrLog.msg_type, pMsg->stHdrLog.msg_type);	
	fprintf(hFile,"|tran_code=%.*s", sizeof pMsg->stHdrLog.tran_code, pMsg->stHdrLog.tran_code);
	fprintf(hFile,"|long=%.*s", sizeof pMsg->stHdrLog.long_msg, pMsg->stHdrLog.long_msg );
	fprintf(hFile,"|encr=%.*s", sizeof pMsg->stHdrLog.encr_type, pMsg->stHdrLog.encr_type);	
	fprintf(hFile,"|crc=%.*s", sizeof pMsg->stHdrLog.crc, pMsg->stHdrLog.crc);	
	fprintf(hFile,"|term_id=%.*s", sizeof pMsg->stHdrLog.term_id, pMsg->stHdrLog.term_id);
	fprintf(hFile,"|user_id=%.*s", sizeof pMsg->stHdrLog.user_id, pMsg->stHdrLog.user_id);		
	fprintf(hFile,"|sec_auth=%.*s", sizeof pMsg->stHdrLog.sec_autorizacion, pMsg->stHdrLog.sec_autorizacion);
	fprintf(hFile,"|sec_trx=%.*s", sizeof pMsg->stHdrLog.sec_transmision, pMsg->stHdrLog.sec_transmision);
	fprintf(hFile,"|resp_time=%.*s", sizeof pMsg->stH610.resp_timestamp,pMsg->stH610.resp_timestamp);
	fprintf(hFile,"|resp_code=%.*s", sizeof pMsg->stH610.resp_code,pMsg->stH610.resp_code);
	fprintf(hFile,"|desc=%.*s", sizeof pMsg->stH610.descr_resp_code,pMsg->stH610.descr_resp_code);
	fprintf(hFile,"|accion=%c"  , pMsg->stH610.accion);
	fprintf(hFile,"|tipo_usuario=%c"  , pMsg->stH610.tipo_usuario);
	fprintf(hFile,"|clave_sesion=%.*s", sizeof pMsg->stH610.clave_sesion,pMsg->stH610.clave_sesion);
	fprintf(hFile,"|fecha_ult=%.*s", sizeof pMsg->stH610.fecha_ult_dep,pMsg->stH610.fecha_ult_dep);
	fprintf(hFile,"|sincro_sn=%c|\r\n", pMsg->stH610.sincronizar_sn);
	fprintf(hFile,"\r\n");
	/* cerrar logger*/
	fclose(hFile);			  
	hFile = NULL;

	return 0;

}

/* loggear mensaje H200 formateado */
log_msg_h200( HXXX_MESSAGES_UN *pMsg)
{			
	char	szDateTime[]      = "yyyymmddhhmissms";
	char	szTraceFilename[256] = {0};	
	FILE	*hFile = NULL;
	time_t	tmData = time(NULL);
	struct tm *plc_Data = localtime( &tmData );

	/* precondicion */
	if(!plc_Data || 0L == tmData || !pMsg)
		return -1;

	/* fecha y hora del momento */
	strftime(szDateTime, sizeof(szDateTime), ISO_DATETIME_MASK,	plc_Data );
	strftime(szTraceFilename, sizeof(szTraceFilename), szTraceFilenameMask,	plc_Data );

	/* abrir el logger */
	hFile=fopen(szTraceFilename,"at+");	
	if(!hFile)
		return -2;

	/* mostrar campo a campo el detalle del mensaje 0200 */
	fprintf(hFile,"\r\n[###%s###]=",szDateTime);
	fprintf(hFile,"|canal=%.*s", sizeof pMsg->stHeader.canal_id, pMsg->stHeader.canal_id );
	fprintf(hFile,"|version=%.*s", sizeof pMsg->stHeader.version_sw, pMsg->stHeader.version_sw);
	fprintf(hFile,"|msg_type=%.*s", sizeof pMsg->stHeader.msg_type, pMsg->stHeader.msg_type);
	fprintf(hFile,"|long=%.*s", sizeof pMsg->stHeader.long_msg, pMsg->stHeader.long_msg );
	fprintf(hFile,"|encr=%.*s", sizeof pMsg->stHeader.encr_type, pMsg->stHdrLog.encr_type);	
	fprintf(hFile,"|crc=%.*s", sizeof pMsg->stHeader.crc, pMsg->stHeader.crc);
	fprintf(hFile,"|tran_code=%.*s", sizeof pMsg->stHeader.tran_code, pMsg->stHeader.tran_code);
	fprintf(hFile,"|term_id=%.*s", sizeof pMsg->stHeader.term_id, pMsg->stHeader.term_id);
	fprintf(hFile,"|user_id=%.*s", sizeof pMsg->stHeader.user_id, pMsg->stHeader.user_id);		
	fprintf(hFile,"|sec_auth=%.*s", sizeof pMsg->stHeader.sec_autorizacion, pMsg->stHeader.sec_autorizacion);
	fprintf(hFile,"|sec_trx=%.*s", sizeof pMsg->stHeader.sec_transmision, pMsg->stHeader.sec_transmision);
	fprintf(hFile,"|password=%.*s", sizeof pMsg->stH200.password,pMsg->stH200.password); 		
	fprintf(hFile,"|fecha_hora=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.fecha_hora_prestacion, 
		pMsg->stH200.unReq.stPrestacion.fecha_hora_prestacion);
	fprintf(hFile,"|cargada_on=%c", pMsg->stH200.unReq.stPrestacion.cargada_on_sn);
	fprintf(hFile,"|gerenciador=%.*s", sizeof pMsg->stH200.id_gerenciador,pMsg->stH200.id_gerenciador);
	fprintf(hFile,"|prepaga=%.*s", sizeof pMsg->stH200.prepaga_os,pMsg->stH200.prepaga_os);
	fprintf(hFile,"|bandeja=%c", pMsg->stH200.bandeja_auditores);  
	fprintf(hFile,"|nro_auth=%.*s", sizeof pMsg->stH200.nro_autorizacion,pMsg->stH200.nro_autorizacion);
	fprintf(hFile,"|sec_trx=%.*s", sizeof pMsg->stH200.sec_transaccion,pMsg->stH200.sec_transaccion);
	fprintf(hFile,"|nro_lote=%.*s", sizeof pMsg->stH200.nro_lote_canal,pMsg->stH200.nro_lote_canal);
	fprintf(hFile,"|resp=time=%.*s", sizeof pMsg->stH200.resp_timestamp,pMsg->stH200.resp_timestamp);
	fprintf(hFile,"|reversal=%.*s", sizeof pMsg->stH200.reversal_code,pMsg->stH200.reversal_code);
	//fprintf(hFile,"|%c", pMsg->stH200.accion);  
	/* mostrar el header de prestacion */
	fprintf(hFile,"|fecha_hora=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.fecha_hora_prestacion,
								pMsg->stH200.unReq.stPrestacion.fecha_hora_prestacion);
	fprintf(hFile,"|cargada_on=%c", pMsg->stH200.unReq.stPrestacion.cargada_on_sn);
	fprintf(hFile,"|credencial=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.credencial_afiliado,
						pMsg->stH200.unReq.stPrestacion.credencial_afiliado);
	fprintf(hFile,"|captura=%c", pMsg->stH200.unReq.stPrestacion.tipo_captura);
	fprintf(hFile,"|vto=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.vto_credencial,
						pMsg->stH200.unReq.stPrestacion.vto_credencial); 
	fprintf(hFile,"|plan=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.plan,pMsg->stH200.unReq.stPrestacion.plan); 
	fprintf(hFile,"|plan_manual=%c", pMsg->stH200.unReq.stPrestacion.plan_manual_sn);
	fprintf(hFile,"|apellido=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.apellido_afiliado,
						pMsg->stH200.unReq.stPrestacion.apellido_afiliado); 
	fprintf(hFile,"|nombre=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.nombre_afiliado,
						pMsg->stH200.unReq.stPrestacion.nombre_afiliado); 
	fprintf(hFile,"|manual_sn=%c", pMsg->stH200.unReq.stPrestacion.afiliado_manual_sn);
	fprintf(hFile,"|copago=%c", pMsg->stH200.unReq.stPrestacion.cobra_copago_sn);
	fprintf(hFile,"|establec=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.id_establecimiento,
						pMsg->stH200.unReq.stPrestacion.id_establecimiento);
	fprintf(hFile,"|area=%c", pMsg->stH200.unReq.stPrestacion.cod_area_asistencial);
	fprintf(hFile,"|tipo_matricula=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.tipo_matricula_solic,
						pMsg->stH200.unReq.stPrestacion.tipo_matricula_solic);
	fprintf(hFile,"|nro_matricul=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.nro_matricula_solic,
						pMsg->stH200.unReq.stPrestacion.nro_matricula_solic);
	fprintf(hFile,"|nombre=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.nombre_prof_solic,
						pMsg->stH200.unReq.stPrestacion.nombre_prof_solic);
	fprintf(hFile,"|tipo_matricula=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.tipo_matricula_act,
						pMsg->stH200.unReq.stPrestacion.tipo_matricula_act);
	fprintf(hFile,"|nro_matricula=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.nro_matricula_act,
						pMsg->stH200.unReq.stPrestacion.nro_matricula_act);
	fprintf(hFile,"|id_prof=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.id_profesional_act,
						pMsg->stH200.unReq.stPrestacion.id_profesional_act);
	fprintf(hFile,"|nro_inter_pro=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.nro_interno_prof_act,
						pMsg->stH200.unReq.stPrestacion.nro_interno_prof_act);
	fprintf(hFile,"|nombre_prof=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.nombre_prof_act,
						pMsg->stH200.unReq.stPrestacion.nombre_prof_act);
	fprintf(hFile,"|id_equipo=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.id_equipo_act,
						pMsg->stH200.unReq.stPrestacion.id_equipo_act);
	fprintf(hFile,"|especialidad=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.especialidad_act,
						pMsg->stH200.unReq.stPrestacion.especialidad_act);
	fprintf(hFile,"|id_catalogo=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.id_catalogo_diag,
						pMsg->stH200.unReq.stPrestacion.id_catalogo_diag);
	fprintf(hFile,"|diagnostico=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.codigo_diag,
						pMsg->stH200.unReq.stPrestacion.codigo_diag);
	fprintf(hFile,"|horario_prog=%c", pMsg->stH200.unReq.stPrestacion.horario_programado_sn);
	fprintf(hFile,"|nro_lote=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.nro_lote_prestador,
						pMsg->stH200.unReq.stPrestacion.nro_lote_prestador);
	fprintf(hFile,"|cantidad=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.cantidad_prestaciones,
						pMsg->stH200.unReq.stPrestacion.cantidad_prestaciones);
	fprintf(hFile,"|id_tipo_prestacion=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.id_tipo_prestacion,
						pMsg->stH200.unReq.stPrestacion.id_tipo_prestacion);
	fprintf(hFile,"|nro_lote_canc=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.nro_lote_canc,
						pMsg->stH200.unReq.stPrestacion.nro_lote_canc);
	/* si hay al menos 1 prestacion informada, mostrar el primer detalle */
	if(strncmp(pMsg->stH200.unReq.stPrestacion.cantidad_prestaciones,"01",2)>=0)
	{
		fprintf(hFile,"|item_catalogo=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.stItem_presta.id_catalogo_proc_item,
						pMsg->stH200.unReq.stPrestacion.stItem_presta.id_catalogo_proc_item);
		fprintf(hFile,"|item_capitulo=%c", pMsg->stH200.unReq.stPrestacion.stItem_presta.capitulo_item);
		fprintf(hFile,"|item_proced=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.stItem_presta.codigo_proc_item,
						pMsg->stH200.unReq.stPrestacion.stItem_presta.codigo_proc_item);
		fprintf(hFile,"|item_cantidad=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.stItem_presta.cantidad_item,
						pMsg->stH200.unReq.stPrestacion.stItem_presta.cantidad_item);
		fprintf(hFile,"|item_resp_code=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.stItem_presta.resp_code_item,
						pMsg->stH200.unReq.stPrestacion.stItem_presta.resp_code_item);
		fprintf(hFile,"|item_control=%.*s", sizeof pMsg->stH200.unReq.stPrestacion.stItem_presta.control_auth_item,
						pMsg->stH200.unReq.stPrestacion.stItem_presta.control_auth_item);
		fprintf(hFile,"|item_bandeja=%c", pMsg->stH200.unReq.stPrestacion.stItem_presta.bandeja_aud_item);
		fprintf(hFile,"|item_cancelada=%c", pMsg->stH200.unReq.stPrestacion.stItem_presta.cancelada_sn);
	}
	/* fin de registro */
	fprintf(hFile,"|\r\n\r\n");
	/* cerrar archivo */
	fclose(hFile);
	hFile = NULL;

	return 0;
}

/* loggear mensaje H205 formateado */
log_msg_h205( HXXX_MESSAGES_UN *pMsg)
{			
	char	szDateTime[]      = "yyyymmddhhmissms";
	char	szTraceFilename[256] = {0};	
	FILE	*hFile = NULL;
	time_t	tmData = time(NULL);
	struct tm *plc_Data = localtime( &tmData );

	/* precondicion */
	if(!plc_Data || 0L == tmData || !pMsg)
		return -1;

	/* fecha y hora del momento */
	strftime(szDateTime, sizeof(szDateTime), ISO_DATETIME_MASK,	plc_Data );
	strftime(szTraceFilename, sizeof(szTraceFilename), szTraceFilenameMask,	plc_Data );

	/* abrir el logger */
	hFile=fopen(szTraceFilename,"at+");	
	if(!hFile)
		return -2;

	/* mostrar campo a campo el detalle del mensaje 0200 */
	fprintf(hFile,"\r\n[###%s###]=",szDateTime);
	fprintf(hFile,"|canal=%.*s", sizeof pMsg->stHeader.canal_id, pMsg->stHeader.canal_id );
	fprintf(hFile,"|version=%.*s", sizeof pMsg->stHeader.version_sw, pMsg->stHeader.version_sw);
	fprintf(hFile,"|msg_type=%.*s", sizeof pMsg->stHeader.msg_type, pMsg->stHeader.msg_type);
	fprintf(hFile,"|long=%.*s", sizeof pMsg->stHeader.long_msg, pMsg->stHeader.long_msg );
	fprintf(hFile,"|encr=%.*s", sizeof pMsg->stHeader.encr_type, pMsg->stHdrLog.encr_type);	
	fprintf(hFile,"|crc=%.*s", sizeof pMsg->stHeader.crc, pMsg->stHeader.crc);
	fprintf(hFile,"|tran_code=%.*s", sizeof pMsg->stHeader.tran_code, pMsg->stHeader.tran_code);
	fprintf(hFile,"|term_id=%.*s", sizeof pMsg->stHeader.term_id, pMsg->stHeader.term_id);
	fprintf(hFile,"|user_id=%.*s", sizeof pMsg->stHeader.user_id, pMsg->stHeader.user_id);		
	fprintf(hFile,"|sec_auth=%.*s", sizeof pMsg->stHeader.sec_autorizacion, pMsg->stHeader.sec_autorizacion);
	fprintf(hFile,"|sec_trx=%.*s", sizeof pMsg->stHeader.sec_transmision, pMsg->stHeader.sec_transmision);
	fprintf(hFile,"|password=%.*s", sizeof pMsg->stH205.password,pMsg->stH200.password); 		
	fprintf(hFile,"|gerenciador=%.*s", sizeof pMsg->stH205.id_gerenciador,pMsg->stH205.id_gerenciador);
	fprintf(hFile,"|prepaga=%.*s", sizeof pMsg->stH205.prepaga_os,pMsg->stH205.prepaga_os);	
	fprintf(hFile,"|nro_auth=%.*s", sizeof pMsg->stH205.nro_autorizacion,pMsg->stH205.nro_autorizacion);
	fprintf(hFile,"|resp=time=%.*s", sizeof pMsg->stH205.resp_timestamp,pMsg->stH205.resp_timestamp);	

	/* fin de registro */
	fprintf(hFile,"|\r\n\r\n");
	/* cerrar archivo */
	fclose(hFile);
	hFile = NULL;

	return 0;
}

/* loggear mensaje de rta. del H200, o sea, el H210 */
log_msg_h215( HXXX_MESSAGES_UN *pMsg)
{		
	/* precondicion */
	if(pMsg)
		/* como comparten la misma estrcutura, simplemente se redirecciona al H205 */
		log_msg_h205( pMsg );
}

/* loggear mensaje de rta. del H200, o sea, el H210 */
log_msg_h210( HXXX_MESSAGES_UN *pMsg)
{		
	/* precondicion */
	if(pMsg)
		/* como comparten la misma estrcutura, simplemente se redirecciona al H200 */
		log_msg_h200( pMsg );
}
