///////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A.
// RPC-DataBaseConnectivity Class
//
//   Clase conexion a base datos via RPC. Implementacion para Ms-SQL-Server, Sybase y COBIS.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2002.03.04      mdc     Base
// (Inicial)    2002.04.29      mdc     virtual EXPORT_ATTR boolean_t GetLoginStatus(void);
//
//
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RPCDB_H
#define RPCDB_H

// Aliases
#include <qusrinc/alias.h>
// Wide Types and basic types
#include <qusrinc/typedefs.h>


#ifndef SZMAXNAME_SIZE_DEFINED
#define SZMAXNAME_SIZE_DEFINED
// Constantes predefinidas
const int	SZMAXBUFFER_SIZE=1024,	// Tamano maximo del buffer de mensajes
			SZMAXPATH_SIZE=256,		// Tamano maximo del path o nombre generico de ubicacion de datos			
			SZMAXNAME_SIZE=64;		// Tamano maximo del nombre generico			
#endif // SZMAXNAME_SIZE_DEFINED

// RPC-DataBaseConnectivity Class - scheleton
class rpcdb  
{


protected:

	// Conexion SQL por sesion
	long lSQLConn;
	// Retorno de funciones
	long lRetStatus;
	// Atributos de conexion RPC
	CHAR	szServer[SZMAXNAME_SIZE], 
			szUser[SZMAXNAME_SIZE], 
			szPassw[SZMAXNAME_SIZE];			
	// Indicador de ejecucion RPC, ok o fallida
	long		lRPC_status;			
	// Indicador de LOGON efectuado y OK
	boolean_t	bLoginOk;

public:
	

	// Public Constructor-Destructor Methods
	EXPORT_ATTR rpcdb( );	// Default Constructor 
	EXPORT_ATTR ~rpcdb( ); // Default Destructor 

	// Login
	virtual EXPORT_ATTR boolean_t Login(); 
	// Logout
	virtual EXPORT_ATTR boolean_t Logout();
	// Consulta de saldos
	virtual EXPORT_ATTR boolean_t CheckingsAccountInquiry(char *szAccount, 
								 char *szCard    ,
								 char *szReceipt , char *szFIID    ,
								 char *szATM     , char *szTrxLINK ,
								 char *szTrxCobis, char *szCurrency,
								 char *szDate    , char *szTrxDate ,
								 char *szTime    , char *szTrxTime ,
								 char *szOnline  , char *szProduct ,
								 char *szCapDate , char *szNetwork ,
								 char *szAvail   , char *szCountable,
								 char *szAcumEfec, char *szAcumExtra);
	virtual EXPORT_ATTR boolean_t SavingsAccountInquiry(char *szAccount, 
								 char *szCard    ,
								 char *szReceipt , char *szFIID    ,
								 char *szATM     , char *szTrxLINK ,
								 char *szTrxCobis, char *szCurrency,
								 char *szDate    , char *szTrxDate ,
								 char *szTime    , char *szTrxTime ,
								 char *szOnline  , char *szProduct ,
								 char *szCapDate , char *szNetwork ,
								 char *szAvail   , char *szCountable,
								 char *szAcumEfec, char *szAcumExtra);
	// Transferencia de cuenta
	virtual EXPORT_ATTR boolean_t AccountTransfer(char *szAccount1, char *szAccount2 , 
							 char *szAmmount  , 
							 char *szCause1   , char *szCause2   ,
							 char *szCard     ,
							 char *szReceipt  , char *szFIID     ,
							 char *szATM      , char *szTrxLINK  ,
							 char *szTrxCobis1, char *szTrxCobis2,
							 char *szCurrency1, char *szCurrency2,
							 char *szDate     , char *szTrxDate  , 
							 char *szCapDate  ,
							 char *szTime     , char *szTrxTime  , 
							 char *szOnLine   , 
							 char *szProduct1 , char *szProduct2 ,
							 char *szNetwork  ,
							 char *szAvail    ,char *szExchangeRate,
							 char *szTrxCurrency,
							 char *szAcumEfec, char *szAcumExtra ,
							 char *szContable    );
	// Extraccion de cuenta
	virtual EXPORT_ATTR boolean_t AccountWidrawal(char *szAccount , char *szAmmount , 
								 char *szCause	 , char *szCard    ,
								 char *szReceipt , char *szFIID    ,
								 char *szATM     , char *szTrxLINK ,
								 char *szTrxCobis, char *szCurrency,
								 char *szDate    , char *szTrxDate ,
								 char *szTime    , char *szTrxTime ,
								 char *szOnline  , char *szProduct ,
								 char *szCapDate , char *szNetwork ,
								 char *szCBU	 ,
								 char *szAvail   , char *szCountable,
								 char *szAcumEfec, char *szAcumExtra,
								 char *szTrxCurrency, char *szExchangeRate );

	// Hay error?
	virtual EXPORT_ATTR boolean_t Error(void);	
	// Retorno de codigo de status
	virtual EXPORT_ATTR long GetStatus(void);
	// Retorno de codigo de status de LOGIN
	virtual EXPORT_ATTR boolean_t GetLoginStatus(void);
	// Retorno de ultimo query SQL
	virtual EXPORT_ATTR const char	*GetLastSQLCommand(char *);


};

// Tipificaciones de clases RPCDB y puntero a RPCDB
typedef class rpcdb	   RPCDB;
typedef class rpcdb	 * pRPCDB;


#endif // RPCDB_H