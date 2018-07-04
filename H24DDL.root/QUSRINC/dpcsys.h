/////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A.
// DPC System Definition
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2002.08.21      mdc     SIAF, COBIS y otros. Banco MacroBansud y otros.
// (Alfa)       2003.01.21      mdc     _SAF_IN_PROGRESS_ EN TYPEDEF DE HOSTSTATUS.
// (Alfa)       2003.01.21      mdc     CNTL_FORMAT_DATANET EN TYPEDEF FORMATTYPE.
// (Alfa)       2003.05.09      mdc     CNTL_FORMAT_ISO8583_SIOP
// (Alfa)       2003.06.10      mdc     CNTL_FORMAT_ISO8583_TSHOP
// (Beta)       2004.11.23      mdc     CNTL_FORMAT_ISO8583_NEAT
// (Delta)      2005.07.27      mdc     CNTL_FORMAT_DATANET_JERONIMO. Banco Suquia.
// (Delta)      2006.03.01      mdc     Banco Finansur, al mismo tiempo ISO8583. _SYSTEM_DATANET_BATCH_.
// (Delta)      2006.09.07      mdc     Banco T.D.Fuego idem _SYSTEM_DATANET_BATCH_.
/////////////////////////////////////////////////////////////////////////////

#ifndef _DPC_SYSTEM_
#define _DPC_SYSTEM_	 

// Se define el Suprasistema Principal ATM... luego 
// se especializa el mismo en COBIS, SIAF u otro
#ifndef _SYSTEM_ATM_
#define _SYSTEM_ATM_
#endif // _SYSTEM_ATM_

// Se define el Suprasistema Principal POS... luego 
// se especializa el mismo en COBIS, SIAF u otro
#ifndef _SYSTEM_POS_
//#define _SYSTEM_POS_
#endif // _SYSTEM_POS_

// Se define el Suprasistema Principal DATANET... luego 
// se especializa el mismo en COBIS, SIAF u otro
#ifndef _SYSTEM_DATANET_
#define _SYSTEM_DATANET_		// DATANET ONLINE
#endif // _SYSTEM_DATANET_

// Modo BATCH de los sistemas ?
#define _USE_SYSTEM_BATCH_MODE_

#ifdef _USE_SYSTEM_BATCH_MODE_
#define _SYSTEM_DATANET_BATCH_	// DATANET BATCH
#define _SYSTEM_ATM_BATCH_	    // ATM BATCH
#endif // _USE_SYSTEM_BATCH_MODE_

#pragma comment(user, "Copyright 2002,2003,2004,2005,2006 IT24 ASAP S.L." )
#pragma comment(user, "Copyright 2002,2003,2004,2005,2006 Martin D Cernadas" )

/////////////////////////////////////////////////////////////////////////////
// Se define luego el SubSistema SIAF, COBIS u otro segun corresponda 
#undef _SYSTEM_ATM_COBIS_
#undef _SYSTEM_ATM_SIAF_
#undef _SYSTEM_ATM_SIOP_
#undef _SYSTEM_DATANET_COBIS_
#undef _SYSTEM_ATM_TSHOP_
#undef _SYSTEM_POS_NEAT_
#undef _SYSTEM_DATANET_JERONIMO_
#undef _SYSTEM_DATANET_FINANSUR_
#undef _SYSTEM_DATANET_TDFUEGO_
#undef _SYSTEM_H24_
#undef _SYSTEM_SOAT_

/////////////////////////////////////////////////////////////////////////////
// Macros de control de compilacion para Sistemas COBIS, SIAF u otros
// SIAF-atm (BANCO BANSUD)
#ifndef _SYSTEM_ATM_SIAF_
//#define _SYSTEM_ATM_SIAF_ 
//#pragma comment(user, "Se ha definido sistema para _SYSTEM_ATM_SIAF_" )
#endif // _SYSTEM_ATM_SIAF_
// COBIS-atm (BANCO MACRO)
/////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEM_ATM_COBIS_ 
//#define _SYSTEM_ATM_COBIS_
//#pragma comment(user, "Se ha definido sistema para _SYSTEM_ATM_COBIS_" )
//#define _RPCDB_CONNECTION_
#endif // _SYSTEM_ATM_COBIS_
/////////////////////////////////////////////////////////////////////////////
// COBIS-datanet (BANCO MACRO)
#ifndef _SYSTEM_DATANET_COBIS_
//#define _SYSTEM_DATANET_COBIS_
//#pragma comment(user, "Se ha definido sistema para _SYSTEM_DATANET_COBIS_" )
//#define _RPCDB_CONNECTION_
#endif // _SYSTEM_DATANET_COBIS_
// SIOP-atm (BANCO MARIVA)
#ifndef _SYSTEM_ATM_SIOP_
//#define _SYSTEM_ATM_SIOP_ 
//#pragma comment(user, "Se ha definido sistema para _SYSTEM_ATM_SIOP_" )
#endif // _SYSTEM_ATM_SIOP_
// TSHOPPING-atm (TARJETA SHOPPING)
#ifndef _SYSTEM_ATM_TSHOP_
//#define _SYSTEM_ATM_TSHOP_ 
//#pragma comment(user, "Se ha definido sistema para _SYSTEM_ATM_TSHOP_" )
//#ifndef _SQL_ODBC_SENTENCES_
//#define _SQL_ODBC_SENTENCES_
//#endif // _SQL_ODBC_SENTENCES_
#endif // _SYSTEM_ATM_TSHOP_
/////////////////////////////////////////////////////////////////////////////
// PREPAGO-pos (NEAT LATIN AMERTICA)
#ifndef _SYSTEM_POS_NEAT_
//#define _SYSTEM_POS_NEAT_
//#pragma comment(user, "Se ha definido sistema para _SYSTEM_POS_NEAT_" )
#endif // _SYSTEM_POS_NEAT_
/////////////////////////////////////////////////////////////////////////////
// JERONIMO-datanet (BANCO SUQUIA)
#ifndef _SYSTEM_DATANET_JERONIMO_
//#define _SYSTEM_DATANET_JERONIMO_
//#pragma comment(user, "Se ha definido sistema para _SYSTEM_DATANET_JERONIMO_" )
//#ifndef _XML_CONNECTION_
//#define _XML_CONNECTION_
//#endif // _XML_CONNECTION_
#endif // _SYSTEM_DATANET_COBIS_

#ifndef _SYSTEM_ATM_FINAN_
//#define _SYSTEM_ATM_FINAN_ 
//#pragma comment(user, "Se ha definido sistema para _SYSTEM_ATM_FINANSUR_" )
//#define _DLL_CONNECTION_
#endif // _SYSTEM_ATM_FINAN_

#ifndef _SYSTEM_DATANET_FINANSUR_
//#define _SYSTEM_DATANET_FINANSUR_
//#pragma comment(user, "Se ha definido sistema para _SYSTEM_DATANET_FINANSUR_" )
//#define _DLL_CONNECTION_
//#ifndef _SQL_ODBC_SENTENCES_
//#define _SQL_ODBC_SENTENCES_
//#define _DPS_DEBUG_PRINT_PARAMS_	(TRUE)
//#endif // _SQL_ODBC_SENTENCES_
#endif // _SYSTEM_DATANET_FINANSUR_

#ifndef _SYSTEM_ATM_TDFUEGO_
#define _SYSTEM_ATM_TDFUEGO_ 
#pragma comment(user, "Se ha definido sistema para _SYSTEM_ATM_TDFUEGO_" )
#define _RPC_DLL_CONNECTION_
#endif // _SYSTEM_ATM_FINAN_

#ifndef _SYSTEM_DATANET_TDFUEGO_
#define _SYSTEM_DATANET_TDFUEGO_  
#pragma comment(user, "Se ha definido sistema para _SYSTEM_DATANET_TDFUEGO_" )
#define _RPC_DLL_CONNECTION_
#ifndef _SQL_ODBC_SENTENCES_
#define _SQL_ODBC_SENTENCES_
#define _DPS_DEBUG_PRINT_PARAMS_	(TRUE)
#endif // _SQL_ODBC_SENTENCES_
#endif // _SYSTEM_DATANET_TDFUEGO_



#ifdef _HOST_STATUS_TYPEDEF_
#error 'Ya esta definido el Sistema DPC'
#else
#define _HOST_STATUS_TYPEDEF_
// Tipo de Estado del HOST
typedef enum {	_HOST_UNAVAILABLE_    ,
				_HOST_READY_          ,
				_CUTOVER_IN_PROGRESS_ ,
				_NO_COMM_EVENTS_      ,
				_COMM_EVENTS_OK_      ,
				_HOST_STOPPED_        ,
				_SAF_IN_PROGRESS_				// 2003.01.21-mdc
			} enumHostStatus;	

// Formato ISO o no-ISO ?
typedef enum {	CNTL_FORMAT_ISO8583		          , // formato ISO c/2 bytes de long
				CNTL_FORMAT_ISO8583_PREHOMO       , // formato ISO Prehomo, s/long
				// Formatos que detallan sistemas en particular
				CNTL_FORMAT_ISO8583_SIAF          ,
				CNTL_FORMAT_ISO8583_SIAF_PREHOMO  ,
				CNTL_FORMAT_ISO8583_COBIS         ,
				CNTL_FORMAT_ISO8583_COBIS_PREHOMO ,
				CNTL_FORMAT_NONISO8583	          , // No es formato ISO
				CNTL_FORMAT_NULL                  , // SIN FORMATO, es draft
				CNTL_FORMAT_DATANET	              , // 2003.01.21-mdc DPS DATANET
				CNTL_FORMAT_ISO8583_SIOP          , // 2003.05.09-mdc
				CNTL_FORMAT_ISO8583_SIOP_PREHOMO  , // 2003.05.09-mdc
				CNTL_FORMAT_ISO8583_TSHOP         , // 2003.06.10-mdc
				CNTL_FORMAT_ISO8583_TSHOP_PREHOMO , // 2003.06.10-mdc
				CNTL_FORMAT_ISO8583_NEAT          , // 2004.11.23-mdc
				CNTL_FORMAT_ISO8583_NEAT_PREHOMO  , // 2004.11.23-mdc
				CNTL_FORMAT_ISO8583_FINAN         ,
				CNTL_FORMAT_ISO8583_FINAN_PREHOMO 
			} enumFormatType;
#endif // _DPC_SYSTEM_

#endif // _DPC_SYSTEM_
