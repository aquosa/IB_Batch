//                                                              
// IT24 Sistemas S.A.
// Transaction Resolution Class
// SQL-Query-Commands
//
//		Diversas constantes de Tablas y Comandos SQL, segun que Sistema se este
// compilando: _SYSTEM_PAS_, _SYSTEM_ULTMOV_, _SYSTEM_AFJP_, _SYSTEM_MEP_, etc.
//		Los SQL_QUERY_CMD de GET que terminan en "_", implican un retorno de 
// una unica columna que concatena todas las columnas originales de 
// la tabla respectiva, con "+".
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.07.23      mdc     En base a SQLCMDS.H de PAS/AFJP/ULTMOV, y como
//                                      extension a futuro sobre el header original.
// (Inicial)    1998.07.27      mdc     SOAT Logoff/Logon/Pines/Usuario/Institucion
// (Inicial)    1998.08.04      mdc     SOAT ADDLOGON
// (Inicial)    1998.08.06      mdc     SOAT GETLOGON
// (Inicial)    1998.08.07      mdc     SOAT DELETELOGON
// (Alfa)       1998.09.15      mdc     ODBC_SYS_ADMIN_USER="sa"
// (Beta)       2001.12.31      mdc     ATM
// (Beta)       2002.01.31      mdc     Queries SQL de COBIS para consulta y extraccion
// (Beta)       2002.02.18      mdc     SP de COBIS definitivos( 492,488 y demas )
// (Beta)       2003.11.10      mdc     #define _SQL_INSERT_REQR_SNTC_ y otros para T.SHOPPING
//

#ifndef _SQLQCMDS_H_
#define _SQLQCMDS_H_


// Sistemas definidos a compilar 
// (_SYSTEM_BASE_ define todos los sistemas compatibles entre si)
#if   defined( _SYSTEM_PAS_    )
#elif defined( _SYSTEM_ULTMOV_ ) 
#elif defined( _SYSTEM_AFJP_   )
#elif defined( _SYSTEM_MEP_    )
#elif defined( _SYSTEM_SOAT_   )
#elif defined( _SYSTEM_ATM_    )
#elif defined( _SYSTEM_DATANET_ )
#elif defined( _SYSTEM_BASE_   )
#define _SYSTEM_PAS_    
#define _SYSTEM_ULTMOV_  
#define _SYSTEM_AFJP_   
#define _SYSTEM_MEP_    
#define _SYSTEM_SOAT_ 
#define _SYSTEM_ATM_ 
#define _SYSTEM_DATANET_ 
#else
#error Sin sistema de aplicacion definido para compilar. (_SYSTEM_BASE_)
#endif // _SYSTEM_DEFINED_CHECKING_

// BASE
// Constantes de Acceso a Base de Datos via ODBC
#define ODBC_DEFAULT_SERVER                 "ATM"
#define ODBC_DEFAULT_DATABASE               "ATM"
#define ODBC_DEFAULT_USER                   "tas"
#define ODBC_DEFAULT_USER_PASSWORD          ""
#define ODBC_SYS_ADMIN_USER                 "sa"

// PAGO AUTOMATICO DE SERVICIOS (PAS)
#if defined( _SYSTEM_PAS_ )

// Constantes de Acceso a Base de Datos via ODBC
#define ODBC_PAS_DATABASE                   "PAS"
#define ODBC_PAS_USER                       "TAS"
#define ODBC_PAS_USER_PASSWORD               ""

// Constantes de Campos en Tablas
#define TBL_ENTES_TIPO_PROC_ESTANDARD       "ST"
#define TBL_ENTES_TIPO_PROC_SIN_USUARIOS    "SU"
#define TBL_ENTES_TIPO_PROC_SIN_FACTURACION "SF"
#define TBL_ENTES_TIPO_PROC_SIN_FACT_NI_USU "SN"
#define TBL_ENTES_TIPO_PROC_DEFAULT         TBL_ENTES_TIPO_PROC_ESTANDARD
#define TBL_USUARIOS_MARCA_ADHERIDO_SI		'S'
#define TBL_USUARIOS_MARCA_ADHERIDO_NO		'N'

// Constantes de Comandos de Invocacion a Stored Procedures
#define SQL_QUERY_CMD_SETUSUADHERIDO		"SETUSUADHERIDO%3.3s '%-*.*s'"
#define SQL_QUERY_CMD_GETINST 				"EXEC GETENTE '%3.3s'"
#define SQL_QUERY_CMD_GETINST_				"EXEC GETENTE_ '%3.3s'"
#define SQL_QUERY_CMD_GETUSU 				"EXEC GETUSU%3.3s '%-*.*s'"
#define SQL_QUERY_CMD_GETUSU_				"EXEC GETUSU%3.3s_ '%-*.*s'"
#define SQL_QUERY_CMD_GETHIS 				"EXEC GETHIS%3.3s '%-*.*s'"
#define SQL_QUERY_CMD_GETHIS_				"EXEC GETHIS%3.3s_ '%-*.*s'"
#define SQL_QUERY_CMD_INSERTHIS				"INSERTHIS%3.3s '%-*.*s','%5.5s','%7.7s','%5.5s','%8.8s','%3.3s','%14.14s'"
#define SQL_QUERY_CMD_DELETEHIS				"DELETEHIS%3.3s '%-*.*s','%s5.5'"
#define SQL_QUERY_CMD_GETACTIVEINF			"EXEC GETINF%3.3s"
#define SQL_QUERY_CMD_GETACTIVEINF_			"EXEC GETINF%3.3s_"
#define SQL_QUERY_CMD_GETFACT 				"EXEC GETFAC%3.3s_%5.5s '%-*.*s'"
#define SQL_QUERY_CMD_GETFACT_				"EXEC GETFAC%3.3s_%5.5s_ '%-*.*s'"

#endif // _SYSTEM_PAS_

// ULTIMOS MOVIMIENTOS (ULTMOV)
#if defined( _SYSTEM_ULTMOV_ )

// Constantes de Acceso a Base de Datos via ODBC
#define ODBC_ULTMOV_DATABASE                "ULTMOV"
#define ODBC_ULTMOV_USER                    "TAS"
#define ODBC_ULTMOV_USER_PASSWORD           ""

// Constantes de Comandos de Invocacion a Stored Procedures
#define SQL_QUERY_CMD_UGETINST_				"EXEC GETBANCO '%4.4s'"
#define SQL_QUERY_CMD_GETUMOV_				"EXEC GET%8.8s%2.2s '%-*.*s'"

#endif // _SYSTEM_ULTMOV_

// ADMINISTRADORAS DE FONDOS DE JUBILACION Y PENSION (AFJP)
#if defined( _SYSTEM_AFJP_ )

// Constantes de Acceso a Base de Datos via ODBC
#define ODBC_AFJP_DATABASE                  "AFJP"
#define ODBC_AFJP_USER                      "TAS"
#define ODBC_AFJP_USER_PASSWORD             ""

// Constantes de Comandos de Invocacion a Stored Procedures
#define SQL_QUERY_CMD_AGETINST_				"EXEC GETAFJP '%4.4s'"
#define SQL_QUERY_CMD_AGETINSTPRE_			"EXEC GETAFJPPREFIX '%3.3s'"
#define SQL_QUERY_CMD_GETAPORTES_           "EXEC GETAPO%4.4s '%-*.*s'"
#define SQL_QUERY_CMD_UPDPIN				"UPDPIN%4.4s '%-*.*s', '%4.4s'"
#define SQL_QUERY_CMD_ADDBADPINRETRY		"ADDBADPINRETRY%4.4s '%-*.*s'"
#define SQL_QUERY_CMD_RESETPINRETRY			"RESETPINRETRY%4.4s '%-*.*s'"
#define SQL_QUERY_CMD_ADDUSERTRX            "UPDAPOTRX%4.4s '%-*.*s'"


#endif // _SYSTEM_AFJP_

// MEDIO ELECTRONICO DE PAGO (MEP)
#if defined( _SYSTEM_MEP_ )

// Constantes de Acceso a Base de Datos via ODBC
#define ODBC_MEP_DATABASE                   "MEP"
#define ODBC_MEP_USER                       "TAS"
#define ODBC_MEP_USER_PASSWORD              ""

#endif // _SYSTEM_MEP_

// SISTEMA ON-LINE DE ADMINISTRACION DE TARJETAS (SOAT)
#if defined( _SYSTEM_SOAT_ )

// Constantes de Acceso a Base de Datos via ODBC
#define ODBC_SOAT_DATABASE                  "SOAT"
#define ODBC_SOAT_USER                      "TAS"
#define ODBC_SOAT_USER_PASSWORD             ""

// Query commands del SOAT
#include "sqlqsoat.h"

#endif // _SYSTEM_SOAT_

// AUTOMATIC TELLER MACHINE (ATM)
#if defined( _SYSTEM_ATM_ )

// Constantes de Acceso a Base de Datos via ODBC
#define ODBC_ATM_DATABASE                "ATM"
#define ODBC_ATM_USER                    "TAS"
#define ODBC_ATM_USER_PASSWORD           ""

// Constantes de Acceso a Base de Datos via ODBC
// Sybase-HOST (SYBHOST) v 12 a traves de SQL-Server Driver
#define ODBC_ATM_COBIS_SERVER		           "SYBHOST"		// Servidor Base de Datos
#define ODBC_ATM_COBIS_DATABASE		           "cobis"			// Base por omision o primer conexion
#define ODBC_ATM_COBIS_DATABASE_CA	           "cob_remesas"	// Podria dividirse en base distintas
#define ODBC_ATM_COBIS_DATABASE_CC	           "cob_remesas"	// Podria dividirse en base distintas
#define ODBC_ATM_COBIS_USER                    "operador"		// Usuario y contrasena
#define ODBC_ATM_COBIS_USER_PASSWORD           "operador"

#endif // _SYSTEM_ATM_COBIS_

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transaccion COBIS 492-consulta caja de ahorro
// Es la unica que retorna el saldo contable y el saldo para girar
#define _SQL_CONSULTA_CA_LOCAL_	"DECLARE @girar money, @contable money, @return int " \
								"EXEC @return = cob_remesas..sp_li_ahcalcula_saldo 'N',NULL,NULL,"\
								"492,'%*.*s','%*.*s',NULL,@girar OUTPUT,@contable OUTPUT  " \
								"SELECT @girar,@return" 
#define _SQL_CONSULTA_CA_LOCAL_SP_		(0)
#define _SQL_CONSULTA_CA_DOLARES_SP_	(0)
// Transaccion COBIS 488-consulta cuenta corriente
// Es la unica que retorna el saldo contable y el saldo para girar
#define _SQL_CONSULTA_CC_LOCAL_	"DECLARE @girar money, @contable money, @return int  " \
								"EXEC @return = cob_remesas..sp_li_calcula_saldo 'N',NULL,NULL,"\
								"488,'%*.*s','%*.*s',NULL,@girar OUTPUT,@contable OUTPUT " \
								"SELECT @girar,@return" 
#define _SQL_CONSULTA_CC_LOCAL_SP_		(1)
#define _SQL_CONSULTA_CC_DOLARES_SP_	(1)
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transaccion COBIS 498-extraccion-pago caja de ahorro y cuenta corriente
#define _SQL_EXTRACCION_CA_LOCAL_	"DECLARE @girar money, @contable money, @return int,@sesion int  " \
								"SELECT @contable=CONVERT(money,'%*.*s'),@sesion=CONVERT(INT,RAND()*100000000) "\
								"EXEC @return = cob_remesas..sp_li_ndc_ext 'link',NULL,@sesion,'link','consola',"\
								"498,'N',NULL,'%*.*s',@contable,'%i','%*.*s',%*.*s,'%*.*s',%*.*s,'%*.*s',%i,%i,'%*.*s','%*.*s','%*.*s','%*.*s','%c',%i,'%*.*s',@girar OUTPUT " \
								"SELECT @girar,@return"
#define _SQL_EXTRACCION_CA_LOCAL_SP_	(0)
#define _SQL_EXTRACCION_CA_DOLARES_SP_	(0)
#define _SQL_EXTRACCION_CC_LOCAL_		_SQL_EXTRACCION_CA_LOCAL_
#define _SQL_EXTRACCION_CC_LOCAL_SP_	(1)
#define _SQL_EXTRACCION_CC_DOLARES_SP_	(1)
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transaccion COBIS 487-deposito en ahorro y cuenta corriente
#define _SQL_DEPOSITO_CA_LOCAL_	"DECLARE @return int,@sesion int  "	\
			"SELECT @sesion=CONVERT(INT,RAND()*100000000) "			\
			"EXEC @return = cob_remesas..sp_li_deposito 'link',null,@sesion,'link','consola','N',null,487,'%*.*s',%i,'%c',%i "\
			"SELECT @return "
#define _SQL_DEPOSITO_CA_LOCAL_SP_		(0)
#define _SQL_DEPOSITO_CA_DOLARES_SP_	(0)
#define _SQL_DEPOSITO_CC_LOCAL_			_SQL_DEPOSITO_CA_LOCAL_
#define _SQL_DEPOSITO_CC_LOCAL_SP_		(1)
#define _SQL_DEPOSITO_CC_DOLARES_SP_	(1)
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transaccion COBIS 499-transferencia desde-hacia caja de ahorro y cuenta corriente
#define _SQL_TRANSFERENCIA_CA_CA_LOCAL_	 "DECLARE @girar money, @contable money, @return int, @sesion int  " \
								"SELECT @contable=CONVERT(money,'%*.*s'), @sesion=CONVERT(INT,RAND()*100000000) "\
								"EXEC @return = cob_remesas..sp_li_ndc_trans 'link',NULL,@sesion,'link','consola',"\
								"499,'N',NULL,'%*.*s',@contable,'%i',%*.*s,%*.*s,'%*.*s','%*.*s',%i,%i,'%*.*s','%*.*s','%*.*s','%*.*s','%*.*s','%c',%i,@girar OUTPUT " \
								"SELECT @girar,@return"
#define _SQL_TRANSFERENCIA_CA_CA_LOCAL_SP_		(0)
#define _SQL_TRANSFERENCIA_CA_CA_DOLARES_SP_	(0)
#define _SQL_TRANSFERENCIA_CC_CC_LOCAL_			_SQL_TRANSFERENCIA_CA_CA_LOCAL_
#define _SQL_TRANSFERENCIA_CC_CC_LOCAL_SP_		(1)
#define _SQL_TRANSFERENCIA_CC_CC_DOLARES_SP_	(1)
#define _SQL_TRANSFERENCIA_CA_CC_LOCAL_			_SQL_TRANSFERENCIA_CA_CA_LOCAL_
#define _SQL_TRANSFERENCIA_CA_CC_LOCAL_SP_		(2)
#define _SQL_TRANSFERENCIA_CA_CC_DOLARES_SP_	(2)
#define _SQL_TRANSFERENCIA_CC_CA_LOCAL_			_SQL_TRANSFERENCIA_CA_CA_LOCAL_
#define _SQL_TRANSFERENCIA_CC_CA_LOCAL_SP_		(3)
#define _SQL_TRANSFERENCIA_CC_CA_DOLARES_SP_	(3)
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transaccion COBIS transferencia-CBU
#define _SQL_TRANSFERENCIA_CBU_CA_LOCAL_	\
								"DECLARE @girar money,@return int " \
								"EXEC @return = cob_remesas..sp_ah_consulta_estado_cuenta 'N',NULL,NULL,%*.*s,%*.*s,@girar OUTPUT" \
								"SELECT @girar,@return " 
#define _SQL_TRANSFERENCIA_CBU_CA_LOCAL_SP_		(0)
#define _SQL_TRANSFERENCIA_CBU_CA_DOLARES_SP_	(0)
#define _SQL_TRANSFERENCIA_CBU_CC_LOCAL_	\
								"DECLARE @girar money, @return int " \
								"EXEC @return = cob_remesas..sp_consulta_estado_cuenta 'N',NULL,NULL,%*.*s,%*.*s,@girar OUTPUT" \
								"SELECT @girar,@return" 
#define _SQL_TRANSFERENCIA_CBU_CC_LOCAL_SP_		(1)
#define _SQL_TRANSFERENCIA_CBU_CC_DOLARES_SP_	(1) 
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transaccion COBIS verificacion de cuenta de ahorro y cuenta corriente
#define _SQL_VERIFICACION_CA_LOCAL_	\
								"DECLARE @girar money,@return int " \
								"EXEC @return = cob_remesas..sp_ah_consulta_estado_cuenta 'N',NULL,NULL,%*.*s,%*.*s,@girar OUTPUT" \
								"SELECT @girar,@return " 
#define _SQL_VERIFICACION_CA_LOCAL_SP_		(0)
#define _SQL_VERIFICACION_CA_DOLARES_SP_	(0)
#define _SQL_VERIFICACION_CC_LOCAL_			_SQL_VERIFICACION_CA_LOCAL_
#define _SQL_VERIFICACION_CC_LOCAL_SP_		(1)
#define _SQL_VERIFICACION_CC_DOLARES_SP_	(1) 
////////////////////////////////////////////////////////////////////////////////////////////////////////

// AUTOMATIC TELLER MACHINE (ATM) T.SHOPPING
#if defined( _SYSTEM_ATM_TSHOP_ )

#define ODBC_HOST24_DATABASE               "host24"
#define ODBC_HOST24_USER                   "host24"
#define ODBC_HOST24_USER_PASSWORD          "host24"

// Insercion en tabla de requerimiento EXTRACT_ONLINE
#define _SQL_INSERT_REQR_SNTC_ \
	    "INSERT INTO EXTRACT_ONLINE"\
	    "(PREFIX1,PREFIX2,DAT_TIM,REC_TYP,"\
	    "AUTH_PPD,TERM_LN,TERM_FIID,TERM_ID,"\
	    "CARD_LN,CARD_FIID,CARD_PAN,CARD_MBR_NUM,"\
	    "BRCH_ID,REGN_ID,USER_FLD1X,TYP_CDE,TYP,"\
	    "RTE_STAT,ORIGINATOR,RESPONDER,"\
	    "ENTRY_TIM,EXIT_TIM,RE_ENTRY_TIM,"\
	    "TRAN_DAT,TRAN_TIM,POST_DAT,"\
	    "ACQ_ICHG_SETL_DAT,ISS_ICHG_SETL_DAT,"\
	    "SEQ_NUM,TERM_TYP,TIM_OFST,ACQ_INST_ID_NUM,RCV_INST_ID_NUM,"\
	    "TRAN_CDE,FROM_ACCT,TIPO_DEP,TO_ACCT, MULT_ACCT,"\
	    "AMT_1,	AMT_2,	AMT_3,	FILLER1,DEP_TYP, RESP_CDE,"\
        "TERM_NAME_LOC, TERM_OWNER_NAME,TERM_CITY,TERM_ST_X,TERM_CNTRY_X,"\
        "OSEQ_NUM,OTRAN_DAT,OTRAN_TIM,B24_POST_DAY,ORIG_CRNCY_CDE,"\
	    "TIPO_CLEARING,FILLER2,TIP_EXCHA_COMP,ARBITRAJE,TIP_EXCHA_VEND,"\
	    "FILLER3,RVSL_RSN, PIN_OFST,SHRG_GRP,FILLER4, FILLER5) "\
	    "VALUES"\
	    "('%6.6s','%2.2s','%19.19s','%2.2s',"\
	    "'%4.4s','%4.4s','%4.4s','%16.16s',"\
	    "'%4.4s','%4.4s','%28.28s','%3.3s',"\
	    "'%4.4s','%4.4s','%2.2s','%2.2s','%4.4s',"\
	    "'%2.2s','%c','%c',"\
	    "'%19.19s','%19.19s','%19.19s',"\
	    "'%6.6s','%8.8s','%6.6s',"\
	    "'%6.6s','%6.6s',"\
	    "'%12.12s','%2.2s','%5.5s','%11.11s','%11.11s',"\
	    "'%6.6s','%28.28s','%c','%28.28s','%c',"\
	    "'%19.19s','%19.19s','%19.19s','%10.10s','%c','%3.3s',"\
        "'%25.25s','%22.22s','%13.13s','%3.3s','%2.2s',"\
	    "'%12.12s','%4.4s','%8.8s','%4.4s','%3.3s',"\
	    "'%2.2s','%4.4s','%8.8s','%8.8s','%8.8s',"\
	    "'%11.11s','%2.2s','%16.16s','%c','%71.71s','%-272.272s')"

// Seleccion de Nro.Serie en tabla de requerimiento EXTRACT_ONLINE
#define _SQL_SELECT_SERIAL_SNTC_ "SELECT NRO_SERIE FROM EXTRACT_ONLINE ORDER BY NRO_SERIE DESC"
// Seleccion de respuesta en tabla de requerimiento EXTRACT_ONLINE_RTA
#define _SQL_SELECT_RESPONSE_SNTC_  "SELECT PREFIX1,AMT_3,SEQ_NUM,RESP_CDE,FILLER5 "\
									"FROM EXTRACT_ONLINE_RTA "\
									"WHERE NRO_SERIE=%i "

// end-AUTOMATIC TELLER MACHINE (ATM) T.SHOPPING
#endif // _SYSTEM_ATM_TSHOP_ 

#endif // _SQLQCMDS_H_

