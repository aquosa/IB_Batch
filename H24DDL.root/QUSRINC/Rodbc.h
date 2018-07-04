///////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// IT24 SISTEMAS S.A.
// Proyecto Pago Automatico de Servicios
// R-OpenDataBaseConnectivity Class
//
//   Clase conexion a base datos via ODBC. Implementacion para Ms-SQL-Server.
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1997.10.01      fk      Base
// (Beta)       1998.08.03      mdc     Reemplazo de atributos estaticos (dEnv,hEnv)
//                                      y SetDatasourceName, SetUsername, SetPassword
// (Beta)       1998.08.10      mdc     BindColumn con retorno de bytes transferidos
// (Beta)       1998.09.22      mdc     BeginTransaction y EndTransaction
// (Beta)       1998.10.08      mdc     GetErrorMessage extendida
// (Beta)       1998.11.10      mdc     SetServerOption extendido. Fetch extendido. AllocateSentence extendido.
// (Beta)       1998.11.11      mdc     SQLSetPos en SQLExecDirect. 
// (Beta)       1998.11.13      mdc     SQLGetData.
// (Beta)       1998.11.16      mdc     MAX_STMT=20
// (Beta)       1999.03.10      mdc     void GetNextResultSet(int *);
// (Beta)       1999.05.28      mdc     Eliminacion de metodo Column con parametro "pcbValue"
//                                      MAX_STMTS=15,MAX_COLUMNS=40,MAX_ROWS=100.
//										GetAllData(void *, SWORD, SDWORD, SDWORD *, int *);
// (Beta)       1999.12.05		mdc		MAX_STMTS=30
// (Beta)       2002.01.31		mdc		#define _ODBC_MAXREADNEXT_ROWS_	(100)
// (Beta)       2002.02.19		mdc		szOperation = API that produced the error, or SQL COMMAND 
// (Beta)       2002.02.21		mdc		Defaults modificados en Query(char *, int *, bool=true, bool=true, int=0,int=0);		
//                                      EXPORT const char	*GetLastSQLCommand(char *);
// (Delta)      2005.07.27		mdc		IsConnected()
// (Delta)      2005.11.10		mdc		MAX_COLUMNS=96
// (Delta)      2006.11.22		mdc		FreeSentence( int, bool, bool) sin liberar columnas.
// (Delta)      2008.07.10		mdc		MAX_STMTS=50
//
//
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RODBC_H
#define RODBC_H


#if defined(_WINDOWS) || defined(_WIN32)
// MS-Windows
//#include <windef.h>
#endif // _WINDOWS

// Aliases
#include <qusrinc/alias.h>
#include <qusrinc/typedefs.h>

// SQL Header
#include <sql.h>


// Constantes predefinidas
#ifndef SZMAXNAME_SIZE_DEFINED
#define SZMAXNAME_SIZE_DEFINED
const short	SZMAXBUFFER_SIZE = 1024,	// Tamano maximo del buffer de mensajes
			SZMAXPATH_SIZE   = 256,		// Tamano maximo del path o nombre generico de ubicacion de datos			
			SZMAXNAME_SIZE   = 64;		// Tamano maximo del nombre generico			
#endif // SZMAXNAME_SIZE_DEFINED

const short	MAX_STMTS   = 50,			// Maximun number of Statements			
			MAX_COLUMNS = 96,			// Maximun number of Columns			
			MAX_ROWS    = 100;			// Maximun number of Rows in advance


// Constantes de sentencias SQL utilizadas en ODBC
#define _ODBC_FIRST_SNTC_		(0) 
#define _ODBC_READ_SNTC_		(0) // sentencia Read
#define _ODBC_INSERT_SNTC_		(1) // sentencia Insert
#define _ODBC_DELETE_SNTC_		(2) // sentencia Delete
#define _ODBC_UPDATE_SNTC_		(3) // sentencia Update
#define _ODBC_STATUS_SNTC_		(4) // sentencia Status
#define _ODBC_READNEXT_SNTC_	(5) // 1er sentencia ReadNext
#define _ODBC_EXECUTE_SNTC_		(6) // Execute
#define _ODBC_LAST_SNTC_		(7) 

#define _ODBC_MAXREADNEXT_ROWS_	(100)

// Constantes de comando SQL, con el comando y los argumentos
#define _ODBC_QUERY_MAX_LENGTH_			(1024)
#define _ODBC_CMD_MAX_LENGTH_			(48) 
#define _ODBC_STRING_MAX_LENGTH_		(255)

// R-Statement Class Definition
class rstmt  
{
public:
	
	// Public attributes
	bool	InUse;					// Statement handle in use flag
	HSTMT	hStmt;					// Statement Handle	(One per instance in SQL if no server cursors used)
	UWORD	NumParam;				// Number of binded parameters 
	UWORD	NumCol;					// Number of binded columns	
	SDWORD  acbBytes[MAX_COLUMNS];	// Bytes returned in each column binded
	
	// In-Line Constructor
	rstmt(void)
	{
		// Set attributes to NULL
		InUse    = false;
		hStmt    = NULL;
		NumParam = 0;	
		NumCol   = 0;	
	};

	// In-Line Destructor
	~rstmt(void)
	{
		// Check allocation
		if(InUse)
		{
			// Free statement if in use
			if(InUse)
				SQLFreeStmt(hStmt, SQL_DROP);
			// Reset attributes
			InUse    = false;
			hStmt    = NULL;
			NumParam = 0;	
			NumCol   = 0;	
		}
	};

};


// R-OpenDataBaseConnectivity Class
class rodbc  
{

private:

	// private members	
	HENV	    hEnv;			 // Static Environment Handle (only one per application)
	HDBC		hdbc;			 // Connection Handle (One per instance)
	rstmt		astmt[MAX_STMTS];// Allocated Statements Array
	bool	    dEnv;			 // Boolean for environment allocation
	bool		dConn;			 // Connection flag, When false Destructor can de-allocate environment handle		
	int			nStmt;			 // Number of sentences	

protected:

	// Protected Data Members
	char	szDataSourceName[SZMAXPATH_SIZE];	// Data source name
	char	szUserName[SZMAXNAME_SIZE];			// User login name
	char	szPassword[SZMAXNAME_SIZE];			// User password
	char	szOperation[SZMAXBUFFER_SIZE];		// API that produced the error, or SQL COMMAND if ExecDirect
	char	szMessage[SZMAXBUFFER_SIZE*2];		// Message Description (large enough to hold SQL COMMAND)
	bool	bError;								// Error Flag : TRUE or FALSE
	bool	bTrxInProg;							// Transaction in progress : TRUE or FALSE	
	int     dCommitOption;						// Autocommit Option

	// SQL dependent attributes 
	char	szSqlState[SZMAXNAME_SIZE];			// Error State
	char	szErrorMsg[SZMAXBUFFER_SIZE];		// Error Msg
	SWORD	cbErrorMsgLen;						// Error Msg bytes count
	SDWORD	fNativeError;						// SQL native error info (database engine or mainframe)
	SDWORD	dwStatus;							// SQL Status info
	RETCODE	iRetcode;							// Return code 

	// Establecer mensaje formateado de error
	void	SetErrorMsg(HENV, HDBC, HSTMT, char *szOperation, char *szTheQuery = NULL);

public:
	

	// Public Constructor-Destructor Methods
	EXPORT rodbc(void);	// Default Constructor (Allocates environment)
	EXPORT rodbc(char *,char *,char *); // Optional Constructor
	EXPORT ~rodbc(void); // Default Destructor (De-allocates environments)	

	// Public Base Methods
	EXPORT void AllocateSentence(int *,bool=true,bool=false,int=0,int=0);
	EXPORT void AutoCommit(bool);	
	EXPORT void Column(void *, SWORD, SDWORD, PINT, bool=false);	
	EXPORT void Column(void *, SWORD, SDWORD, SDWORD *, PINT, bool=false);	
	EXPORT void Commit(void);
	EXPORT void Connect(char *szDataSource, char *szCatalog = NULL);
	EXPORT void Disconnect(void);
	EXPORT bool Error(void);	
	EXPORT void Execute(int *);
	EXPORT void Fetch(int *);	
	EXPORT void Fetch(int *, int);		

	EXPORT void FreeSentence(int, bool=false);	
	EXPORT void FreeSentence(int, bool, bool );	/* FIX MDC 20061122 */
	EXPORT INT  IsSentenceInUse(int );			/* FIX MDC 20061122 */

	EXPORT void GetData(int, void *, SWORD, SDWORD, int *, bool=false);
	EXPORT void GetAllData(void *, SWORD, SDWORD, int *, int *);
	EXPORT void GetNextResultSet(int *);
	EXPORT void Prepare(char *, int *, bool=false);
	////////////////////////////////////////////////////////////////////////
	EXPORT void Query(char *szTheQuery   , int *pSntnc , 
						bool bServerBased=false,
						bool bDynamicKeySet=false, 
						int iMaxRows=_ODBC_MAXREADNEXT_ROWS_, 
						int iRowBinding=0);
	////////////////////////////////////////////////////////////////////////
	EXPORT void Param(SWORD, SWORD, SWORD, UDWORD, SWORD, void *, 
	                  SDWORD, SDWORD *,int *,bool=false);				
	EXPORT void ResetLastError(void);	
	EXPORT void RollBack(void);	

	// Transaction oriented methods
	EXPORT bool BeginTransaction(void);
	EXPORT bool EndTransaction(bool,bool=false);
	EXPORT bool TrxInProgress(void);

	// Public Set methods
	EXPORT void SetDataSourceName(char *);
	EXPORT void	SetUserName(char *);
	EXPORT void	SetPassword(char *);
	EXPORT void	SetServerOption(int,bool=true,int=0,int=0);	
	// Public Get methods
	EXPORT const char	*GetErrorMessage(char *);
	EXPORT RETCODE GetReturnCode(void);
	EXPORT SDWORD  GetStatus(void);
	///////////////////////////////////////////////
	EXPORT const char	*GetLastSQLCommand(char *);
	///////////////////////////////////////////////

	// Public Virtual Methods 
	EXPORT virtual void Read(void);
	EXPORT virtual void Delete(void);
	EXPORT virtual void Update(void);
	EXPORT virtual void Insert(void);

    // Esta conectado ?
    bool IsConnected() { return dConn; } // Connection flag


};

// Tipificaciones de clases RODBC y puntero a RODBC
typedef class rodbc	   RODBC;
typedef class rodbc	 * pRODBC;


#endif  // RODBC_H
///////////////////////////////////////////////////////////////////////////////////////////////


