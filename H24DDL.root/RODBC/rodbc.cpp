//////////////////////////////////////////////////////////////////////
//                                                              
// IT24 Sistemas S.A.
// R-OpenDataBaseConnectivity Class
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1997.10.01      fk      Implementacion
// (Alfa)       1998.06.25      mdc     Correccion punteros validos
// (Alfa)       1998.08.03      mdc     Reemplazo de atributos estaticos (dEnv,hEnv)
//                                      y SetDatasourceName, SetUsername, SetPassword
// (Alfa)       1998.09.22      mdc     BeginTransaction y EndTransaction
// (Alfa)       1998.10.08      mdc     GetErrorMessage extendida
// (Alfa)       1998.11.03      mdc     AllocateSentence permitiendo numero de sentencia externa
// (Alfa)       1998.11.11      mdc     SQLGetData
// (Alfa)       1998.11.16      mdc     Chequeos fuertes de nros. de sentencia
// (Alfa)       1998.11.24      mdc     RowCounter de ReadNext corregido en (-1) 
// (Alfa)       1999.02.19      mdc     FreeSentence(int,bool) : astmt[SentNumber].NumCol = 0;
// (Beta)       1999.03.10      fk      GetNextResultSet(...) desde PAS
// (Beta)       1999.06.17      mdc     rodbc::GetAllData() = Correccion de maximo de buffer x columna
// (Beta)       1999.07.08      mdc     SQL_LOGIN_TIMEOUT =  5 seg
// (Beta)       1999.09.15      mdc     AllocateSentence() : precondicion de conexion a DataSource
// (Beta)       1999.12.05      mdc     MAX_STMT=30
// (Beta)       2000.01.12      mdc     Chequeo de sentencia en uso en ::Fetch()
// (Beta)       2000.02.20      mdc     iRetcode = SQLSetConnectOption(hdbc, SQL_ATTR_QUERY_TIMEOUT, 15L);
// (Release-1)  2003.06.11      mdc     #include <qusrinc/alias.h>
// (Release-1)  2003.09.16      mdc     SQL_CONCURRENCY
// (Release-1)  2006.05.11      mdc     VISUAL .NET : Warning de "iRetCode" en "Fetch(int *pSntnc, int iRowPos)".
// (Release-2a) 2007.04.19      mdc     FIX : GetErrorMessage(char *pextBuffer)....
// (Release-2b) 2008.07.03      mdc     FIX : Unbind implica que se desacoplan las variables.... */
// (Release-2c) 2008.07.10      mdc     FIX : poner a CERO la cantidad de sentencias en uso.... me desconecte!
//
//
//////////////////////////////////////////////////////////////////////


#if defined(_WINDOWS) || defined(_WIN32)
// MS-Windows
#include <windows.h>
#endif // _WINDOWS


// Header de nombres alias
#include <qusrinc/alias.h>
// Headers de la clase RODBC
#include <qusrinc/rodbc.h>

// Standard C Headers
#include <sql.h>
#include <sqlext.h>
#include <stdio.h>
#include <string.h>


// Segun sistema operativo, optimizar invocaciones de funciones de I/O estandards
#if defined(_WINDOWS) || defined(_WIN32)
#define copystring   lstrcpy	// Windows's Kernel Function Call
#define appendstring lstrcat    // Windows's Kernel Function Call
#define stringlength lstrlen	// Windows's Kernel Function Call
#else
#define copystring   strcpy     // Standard I/O Function Call
#define appendstring strcat     // Standard I/O Function Call
#define stringlength strlen		// Standard I/O Function Call
#endif // _WINDOWS


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//-------------------
EXPORT rodbc::rodbc(void)
//-------------------
// Default constructor
//-------------------
{		
	// Instance initialization
	dEnv  = false;		// No environment allocated
	dConn = false;		// Connection flag: when 'false', Destructor can 
						// de-allocates environment-handle
	hdbc       = NULL;	// Sin handle	

	bError        = false;	// Error Flag
	bTrxInProg    = false;	// Transaction in Progress Flag
	dCommitOption = 0;		// Autocommit Option

	// SQL specific attributes
	nStmt        = 0;	 // Sql statement counter (when 0 no sentence to de-allocate)
	fNativeError = 0;    // No sql native error info
	dwStatus     = 0;    // No sql status info
	iRetcode     = 0;    // No sql specific info

	// Data source by default is NULL
	SetDataSourceName("");
	SetUserName("");
	SetPassword("");

	// Allocate environment (only one per application)
	hEnv    = NULL;
	iRetcode = SQLAllocEnv(&hEnv);
	// Error Checking
	if(iRetcode == SQL_SUCCESS)
	{
		// Allocation successful
		dEnv	= true;		// Sets environment flag to true
		bError	= false;	// Sets bError flag member to false
		dConn	= false;	// set to 0 Number of connections
	}
	else
	{
		// Allocation bError
		bError = true;	// Sets bError flag member
		dConn  = false;	// set to 0 Number of connections
		// Set error description
		SetErrorMsg(hEnv, SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLAllocEnv()");
	}
}

//---------------------------------------------------------------------
EXPORT rodbc::rodbc(char *szDataSource, char *szUser, char *szSecurity)
//---------------------------------------------------------------------
// Constructor with database connection
//---------------------------------------------------------------------
{
	// Instance initialization
	dEnv  = false;		// No environment allocated
	dConn = false;		// Connection flag: when 'false', Destructor can 
						// de-allocates environment-handle
	hdbc       = NULL;	// Sin handle	

	bError        = false;	// Error Flag
	bTrxInProg    = false;	// Transaction in Progress Flag
	dCommitOption = SQL_AUTOCOMMIT_OFF;		// Autocommit Option

	// SQL specific attributes
	nStmt        = 0;	 // Sql statement counter (when 0 no sentence to de-allocate)
	fNativeError = 0;    // No sql native error info
	dwStatus     = 0;    // No sql status info
	iRetcode     = 0;    // No sql specific info

	// Data source by default is NULL
	SetDataSourceName("");
	SetUserName("");
	SetPassword("");

	// Allocate environment (only one per application)
	hEnv    = NULL;
	iRetcode = SQLAllocEnv(&hEnv);
	// Error Checking
	if(iRetcode == SQL_SUCCESS)
	{
		// Allocation successful
		dEnv	= true;		// Sets environment flag to true
		bError	= false;	// Sets bError flag member to false
		dConn	= false;	// set to 0 Number of connections
	}
	else
	{
		// Allocation bError
		bError = true;	// Sets bError flag member
		dConn  = false;	// set to 0 Number of connections
		// Set error description
		SetErrorMsg(hEnv, SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLAllocEnv()");
	}
	// Specific initializations for Connection
	if (!bError)
	{
		// Assign data source
		SetDataSourceName(szDataSource);
		// Assigns username & password		
		SetUserName(szUser);
		SetPassword(szSecurity);
		// Connect to data source
		Connect(szDataSource);
	}
}

//--------------------
EXPORT rodbc::~rodbc(void)
//--------------------
// Default Destructor
// If Pending sentences for the connection, de-allocate sentence space
// If there is an open connection, close connection
// If there is connection space allocated, de-allocate connection space
// If no pending instances, de-allocates environment
//--------------------
{
	// Check Transaction in progress
	if(bTrxInProg)
		{
		// Rollback if pending transaction
		RollBack();
		bTrxInProg = false;
		}

	// Frees Previous sentences space
	// Closes Previous Connection
	// Frees Connection environment
	Disconnect();	

	// Always free environment space when exiting
	iRetcode = SQLFreeEnv(hEnv);
	// Check error...
	if(iRetcode == SQL_SUCCESS)
		dEnv = false; // No environment for current instance
	else
		SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLFreeEnv()");
}


//------------------// Member Functions //-------------------------------//

//---------------------------------------------------------------------
EXPORT void rodbc::AllocateSentence(int *pSntnc , bool bServerBased, 
									bool ServerKeySet, int iMaxRows, int iRowBinding)
//---------------------------------------------------------------------
// Allocates a new sentence handler
// Returns the sentence number in the array
//---------------------------------------------------------------------
{
	int ni = 0;
	
	// If no Sentence Handler, Allocate one. Search first free handler
	// DataSource connected
	if(nStmt < MAX_STMTS && hdbc != NULL)
	{

		// Check valid pointer and valid value
		if(pSntnc == NULL)
			{
			// Error in statement precondition
			bError = true;
			// Set Error description
			copystring(szOperation,"SQLAllocStmt()");
			copystring(szMessage, 
			   "(00000): [rodbc Class], Null sentence number.\n");
			return ;
			}

		/////////////////////////////////////////////////////////////////////////////////
		// Check for the asked slot to be free
		/////////////////////////////////////////////////////////////////////////////////
		if((*pSntnc < MAX_STMTS && *pSntnc >= 0) &&
		   (astmt[*pSntnc].InUse == false))
			ni = (*pSntnc);
		else
			// Look for a free slot
			for(ni=0; (ni < MAX_STMTS) && (astmt[ni].InUse == true) ; ni++);
		/////////////////////////////////////////////////////////////////////////////////

		// Try to allocate sentence
		iRetcode = SQLAllocStmt(hdbc, &(astmt[ni].hStmt));
		if(iRetcode == SQL_SUCCESS)
		{
			bError = false;
			if((bServerBased) || (iMaxRows > 0) || (iRowBinding > 0))
			{
				// if Sets server cursor option
				SetServerOption(ni, ServerKeySet, iMaxRows, iRowBinding);
			}
			// If ok up to here
			if(!bError)
			{
				// ok, mark flags
				astmt[ni].InUse		= true;
				astmt[ni].NumParam	= 0;
				astmt[ni].NumCol	= 0;
				(*pSntnc)			= ni;
				nStmt++;
				bError = false;
			}
		}
		else
		{
			// Error in statement allocation
			bError = true;		// Sets bError flag member
			// Set error description
			SetErrorMsg(hEnv, hdbc, astmt[ni].hStmt, "SQLAllocStmt()");
		}
	}
	else
	{
		bError = true;
		copystring(szOperation,"SQLAllocStmt()");
		copystring(szMessage, 
			   "(00000): [rodbc Class], Maximum number of statement handlers reached.\n");
	}
}

//---------------------------------------------
EXPORT void rodbc::FreeSentence(int SentNumber, bool bClose)
//---------------------------------------------
// Frees the sentence resource
//---------------------------------------------
{
	if((SentNumber < MAX_STMTS) && (SentNumber >= 0))
	{
		if(astmt[SentNumber].InUse)
		{
			bError = false;
			/////////////////////////////////////////////////////////////////
			// Close or Drop statement?
			/////////////////////////////////////////////////////////////////
			if(bClose)
				{
				iRetcode = SQLFreeStmt(astmt[SentNumber].hStmt, SQL_CLOSE);
				astmt[SentNumber].NumParam = 0;
				astmt[SentNumber].NumCol   = 0;
				}
			else
				{	
				iRetcode = SQLFreeStmt(astmt[SentNumber].hStmt, SQL_DROP);				
				astmt[SentNumber].NumParam = 0;
				astmt[SentNumber].NumCol   = 0;
				astmt[SentNumber].InUse    = false;				
				nStmt--;
				}
			/////////////////////////////////////////////////////////////////
		}
		else
		{
			bError = true;
			copystring(szOperation,"SQLFreeStmt()");
			copystring(szMessage, "(00000): [rodbc Class], Sentence does not exist.\n");
		}
	}
	else
	{
		bError = true;
		copystring(szOperation,"SQLFreeStmt()");
		copystring(szMessage, "(00000): [rodbc Class], Invalid sentence number.\n");				
	}
}

//--------------------------------------------
EXPORT void rodbc::Connect(char *szDataSource=NULL,char *szCatalog)
//--------------------------------------------
// Connects to a data source using default Username and szPassword
//--------------------------------------------
{
	// Frees connection resources
	if(dConn)
	{
		if(hdbc)
		{ 
			iRetcode = SQLDisconnect(hdbc);
			iRetcode = SQLFreeConnect(hdbc);
			hdbc    = NULL;
		}
		dConn = false;
	}	
	iRetcode = SQLAllocConnect(hEnv, &hdbc);
	if ((iRetcode == SQL_SUCCESS))
	{
		// If szDataSourceName specified
		if ((szDataSource != NULL) && 
			(stringlength(szDataSource) != 0))
			copystring(szDataSourceName, szDataSource);
		
		//////////////////////////////////////////////////////////////////
		// Timeout de al menos 5 segundos al conectarse
		iRetcode = SQLSetConnectOption(hdbc, SQL_ATTR_LOGIN_TIMEOUT, 5L); 
		//////////////////////////////////////////////////////////////////
		// Timeout de al menos 15 segundos en la ejecucion de un query
		iRetcode = SQLSetConnectOption(hdbc, SQL_ATTR_QUERY_TIMEOUT, 15L);
		//////////////////////////////////////////////////////////////////
		
		// Posee Catalogo (SQL SERVER y similares) ?
		if(szCatalog != NULL)
			iRetcode = SQLSetConnectOption(hdbc, SQL_ATTR_CURRENT_CATALOG, 0L);

		// Connects to data source
		iRetcode = SQLConnect(hdbc,
							 (unsigned char *) szDataSourceName,
							 SQL_NTS,
							 (unsigned char *) szUserName,
							 SQL_NTS,
							 (unsigned char *) szPassword,
							 SQL_NTS);
		if (iRetcode == SQL_SUCCESS)
		{
			dConn = true;
			bError = false;		// Sets bError flag member
		}
		else if(iRetcode == SQL_SUCCESS_WITH_INFO)
		{
			dConn = true;
			bError = false;		// Sets bError flag member
			SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLConnect()");
		}
		else
		{
			// Error in Connect
			dConn = false;
			bError = true;		// Sets bError flag member			
			// Set error description
			SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLConnect()");
			// Frees connection environment
			SQLFreeConnect(hdbc);
		}
	}
	else
	{
		// Error in Alloc
		dConn = false;
		bError = true;		// Sets bError flag member		
		// Set error description
		SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLAllocConnect()");
	}
}

//-----------------------------
EXPORT void rodbc::Disconnect()
//-----------------------------
// Disconnects from a data source
//-----------------------------
{
	// Frees all Statement resources
	if(nStmt < MAX_STMTS && nStmt > 0)
	{
		for(int ni = 0;ni < (sizeof(astmt)/sizeof(astmt[0])); ni++)
		{
			if(astmt[ni].InUse)			
				FreeSentence(ni);			
		}//end-for
	}//end-if-statement
	// Frees connection resources
	if(dConn)
	{
		//////////////////////////////////////////////////////////////////
		iRetcode = SQL_SUCCESS;
		if(hdbc)
		{ 
			iRetcode = SQLDisconnect(hdbc);
			if(iRetcode)
				SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLDisconnect()");
			iRetcode = SQLFreeConnect(hdbc);				
			if(iRetcode)
				SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLFreeConnect()");
			else
				hdbc = NULL;
		}//end-if-hdbc
		//////////////////////////////////////////////////////////////////
		// No connection
		dConn = false;
		//////////////////////////////////////////////////////////////////
	}//end-if-connection
	// FIX: poner a CERO la cantidad de sentencias en uso.... me desconecte!
	nStmt = 0;
	// FIX
}

//---------------------------------------
void rodbc::Param(SWORD		TipoParam,		// INPUT|OUTPUT|INPUT_OUTPUT
				  SWORD		swDataType,
				  SWORD		SQLType,
				  UDWORD	SQLPrecision,
				  SWORD		SQLScale,
				  void		*Parametro,
				  SDWORD	MaxParametro,
				  SDWORD	*cbValue,
				  int		*pSntnc,	// Sentence number to return
				  bool		bServerBased)

//---------------------------------------
// Binds parameter markers (SQLBindParameter)
//---------------------------------------
{
	// If not connected, Connect
	if(!dConn)
	{
		Connect(szDataSourceName);
	}
	// If no previous Error
	if (!bError)
	{
		// SentNumber is empty, 
		if((nStmt == 0) ||
		   (*pSntnc < 0) ||
		   (*pSntnc > MAX_STMTS) ||
		   (!(astmt[*pSntnc].InUse)))
		{
			// Allocate new sentence Handler
			AllocateSentence(pSntnc, bServerBased);
		}
		else if((*pSntnc < 0) ||
				(*pSntnc > MAX_STMTS))
		{
			bError = true;
			// Set error description
			copystring(szOperation,"Param()");
			copystring(szMessage, "(00000): [rodbc Class], Invalid sentence number.\n");				
			return ;
		}
		if(!bError)
		{
			// Bind parameter marker
			astmt[*pSntnc].NumParam ++;
			iRetcode = SQLBindParameter(astmt[*pSntnc].hStmt,
									   astmt[*pSntnc].NumParam,
									   TipoParam,
									   swDataType,
									   SQLType,
									   SQLPrecision,
									   SQLScale,
									   Parametro,
									   MaxParametro,
									   cbValue);

			if(iRetcode != SQL_SUCCESS)
			{
				astmt[*pSntnc].NumParam--;
				bError = true;		// Sets bError flag member
				// Set error description
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLParameter()");
			}
		}
	}
}
//-----------------------------------------------
EXPORT void rodbc::Column(void		*pbColumna,
						  SWORD		swDataType,
						  SDWORD	sdwDataLen,						  
						  int		*pSntnc,
						  bool		bServerBased)
//-----------------------------------------------
// Binds a Column to the result set
//-----------------------------------------------
{
	// Method remapping, with no Row length value returned
	Column(pbColumna, swDataType, sdwDataLen, NULL, pSntnc, bServerBased);
}

//-----------------------------------------------
EXPORT void rodbc::Column(void		*pbColumna,
						  SWORD		swDataType,
						  SDWORD	sdwDataLen,	
						  SDWORD	*psdwRowLen, 
						  int		*pSntnc,
						  bool		bServerBased)
//-----------------------------------------------
// Binds a Column to the result set
//-----------------------------------------------
{	
 	// If not connected, Connect
	if(!dConn)
	{
		Connect(szDataSourceName);
	}
	// If no previous Error
	if (!bError)
	{
		if((nStmt == 0) ||
		   (*pSntnc < 0) ||
		   (*pSntnc > MAX_STMTS) ||
		   (!(astmt[*pSntnc].InUse)))
		{
			// Allocate new sentence Handler
			AllocateSentence(pSntnc, bServerBased);
		}
		else if((*pSntnc < 0) || (*pSntnc > MAX_STMTS))
		{
			bError = true;
			// Set error description
			copystring(szOperation,"Column()");
			copystring(szMessage, "(00000): [rodbc Class], Invalid sentence number.\n");				
			return ;
		}
		if(!bError)
		{
			// Check column limit
			if((astmt[*pSntnc].NumCol+1) == MAX_COLUMNS)
			{
				// Sets bError flag member
				bError = true;		
				// Set error description
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLBindCol() max limit");
				// Exit method
				return;
			}
			// Add 1 column to statement
			astmt[*pSntnc].NumCol++;
			// Not known field lenght for current column
			// External row length depending on data type? 
			if(psdwRowLen) 
				(*psdwRowLen) = SQL_NO_TOTAL;
 			astmt[*pSntnc].acbBytes[astmt[*pSntnc].NumCol-1] = SQL_NO_TOTAL ;
			// Bind Column to result set
			iRetcode = SQLBindCol(astmt[*pSntnc].hStmt,
								 astmt[*pSntnc].NumCol,
								 swDataType,
								 pbColumna,
								 sdwDataLen,
								 // External row length?
								 (psdwRowLen) 
									? psdwRowLen
									: &astmt[*pSntnc].acbBytes[astmt[*pSntnc].NumCol-1]	);
			if(iRetcode != SQL_SUCCESS)
			{
				astmt[*pSntnc].NumCol--;
				bError = true;		// Sets bError flag member
				// Set error description
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLBindCol() failed");
			}
		}
	}
	else
	{
		// Set error description
		SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLBindCol() precondition failed!");
	};
}

//----------------------------------------------------------------------------
EXPORT void rodbc::Prepare(char *szQuery, int *pSntnc, bool bServerBased)
//----------------------------------------------------------------------------
// Prepares a sentence for execution (Not useful in SQL Server)
//----------------------------------------------------------------------------
{
	// Sentence to be prepared 
	// NOTE: ** NOT RECOMMENDED FOR SQL SERVER AS IT DOES NOT SUPPORTS **
	//       ** THE PREPARE/EXECUTE MODEL                              **
	if(!bError)
	{
		if((nStmt == 0) ||
		   (*pSntnc < 0) ||
		   (*pSntnc > MAX_STMTS) ||
		   (!(astmt[*pSntnc].InUse)))
		{
			// Allocate new sentence Handler
			AllocateSentence(pSntnc, bServerBased);
		}
		else if((*pSntnc < 0) ||
				(*pSntnc > MAX_STMTS))
		{
			bError = true;
			// Set error description
			copystring(szOperation,"Prepare()");
			copystring(szMessage, "(00000): [rodbc Class], Invalid sentence number.\n");				
			return ;
		}
		if(!bError)
		{
			iRetcode = SQLPrepare(astmt[*pSntnc].hStmt,
								 (unsigned char *) szQuery,
								 SQL_NTS);
			if(iRetcode == SQL_SUCCESS)
				bError = false;
			else if(iRetcode == SQL_SUCCESS_WITH_INFO)
			{
				bError = false;
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLPrepare()");
			}
			else
			{
				bError = true;		// set bError flag member
				// Set error description
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLPrepare()");
			}
		}
	}
}

//------------------------------------------
EXPORT void rodbc::Execute(int *pSntnc)
//------------------------------------------
// Ejecuta una sentencia preparada
//------------------------------------------
{
	if(!bError)
	{
		if((nStmt == 0) ||
		   (*pSntnc < 0) ||
		   (*pSntnc > MAX_STMTS) ||
		   (!(astmt[*pSntnc].InUse)))
		{
			bError = true;
			// Set error description
			copystring(szOperation,"Execute()");
			copystring(szMessage, "(00000): [rodbc Class], Invalid sentence number.\n");				
			return ;
		}
		else
		{
			// Executes prepared statement
			iRetcode = SQLExecute(astmt[*pSntnc].hStmt);
			if(iRetcode == SQL_SUCCESS)
				bError = false;
			else if(iRetcode == SQL_SUCCESS_WITH_INFO)
			{				
				bError = false;		// Sets bError flag member				
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLExecute()");
			}
			else
			{
				bError = true;		// set bError flag member
				// Set error description
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLExecute()");
			}
		}
	}
}

//--------------------------------------------------------------------------
EXPORT void rodbc::Query(char *szTheQuery   , int *pSntnc , bool bServerBased,
						 bool bDynamicKeySet, int iMaxRows, int iRowBinding)
//--------------------------------------------------------------------------
// Dispatch a query/command to the DBMS
//--------------------------------------------------------------------------
{
	// Controls pending errors	
	if(!bError)
	{
		if((nStmt == 0) ||
		   (*pSntnc < 0) ||
		   (*pSntnc > MAX_STMTS) ||
		   (!(astmt[*pSntnc].InUse)))
		{
			// Allocate new sentence Handler
			// Maybe DynamicKeySet or ForwardOnly, and up to MaxRows
			AllocateSentence(pSntnc, bServerBased, bDynamicKeySet, iMaxRows, iRowBinding);
		}
		else if((*pSntnc < 0) ||
			   (*pSntnc > MAX_STMTS))
		{
			bError = true;
			////////////////////////////////////////////////////
			// Set error description, with current SQL COMMAND 
			copystring(szOperation, szTheQuery);
			////////////////////////////////////////////////////
			copystring(szMessage, "(00000): [rodbc Class], Invalid sentence number.\n");				
			return ;
		}
		if(!bError)
		{
			// Execute sentence (Prepared|Direct|Stored)
			iRetcode = SQLExecDirect(astmt[*pSntnc].hStmt,
									(unsigned char *)szTheQuery,
									SQL_NTS);
			if(iRetcode == SQL_SUCCESS) 
			{
				bError = false;
				copystring(szOperation, szTheQuery);
			}
			else if(iRetcode == SQL_SUCCESS_WITH_INFO)
			{				
				bError = false;		// Sets bError flag member								
				copystring(szOperation, szTheQuery);
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLExecDirect()", szTheQuery);
			}
			else
			{	
				bError = true;		// Sets bError flag member
				copystring(szOperation, szTheQuery);
				// Set error description
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLExecDirect()", szTheQuery);
			}
		}
	}
}

//----------------------------------------------
EXPORT void rodbc::SetServerOption(int Sentence, bool bKeySet, int iMaxRows, int iRowBinding)
//----------------------------------------------
// Setea la opcion de server
//----------------------------------------------
{
	// Sets cursor option to SERVER CURSOR
	if(!bError)
	{
		if((Sentence < 0) ||
		   (Sentence > MAX_STMTS))
		{
			bError = true;
			// Set error description
			copystring(szOperation,"SetServerOption()");
			copystring(szMessage, "(00000): [rodbc Class], Invalid sentence number.\n");				
			return ;
		}
		/*************************************/
		/* SQL_CONCURRENCY options           */
		/* SQL_CONCUR_READ_ONLY            1 */
		/* SQL_CONCUR_ROWVER               3 */
		/* SQL_CONCUR_VALUES               4 */
		/*************************************/
		iRetcode = SQLSetStmtOption(astmt[Sentence].hStmt, 
					SQL_CONCURRENCY, SQL_CONCUR_READ_ONLY);
		/***************************************************/
		if(iRowBinding > 0)
			iRetcode = SQLSetStmtOption(astmt[Sentence].hStmt, 
						SQL_BIND_TYPE, iRowBinding); 
		if(iMaxRows > 0)
			iRetcode = SQLSetStmtOption(astmt[Sentence].hStmt, 
						SQL_MAX_ROWS, iMaxRows);
		if(bKeySet)
			iRetcode = SQLSetStmtOption(astmt[Sentence].hStmt, 
						SQL_CURSOR_TYPE, SQL_CURSOR_KEYSET_DRIVEN);						
		else
			iRetcode = SQLSetStmtOption(astmt[Sentence].hStmt, 
						SQL_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY);						
		if(iRetcode == SQL_SUCCESS) 
			bError = false;
		else if(iRetcode == SQL_SUCCESS_WITH_INFO)
		{
			bError = false;
			SetErrorMsg(hEnv, hdbc, astmt[Sentence].hStmt, "SQLSetStmtOption()");
		}
		else
		{
			bError = false;		
			// Set error description
			SetErrorMsg(hEnv, hdbc, astmt[Sentence].hStmt, "SQLSetStmtOption()");
		}
	}
}

//----------------------------------------
EXPORT void rodbc::Fetch(int *pSntnc)
//----------------------------------------
// Gets data (SQLFetch/SQLExtendedFetch)
//----------------------------------------
{
	// KindOfFetch (Common|Extended)
	if (!bError)
	{	
		if((*pSntnc < 0) ||
		   (*pSntnc > MAX_STMTS))
		{
			bError = true;
			// Set error description
			copystring(szOperation,"Fetch()");
			copystring(szMessage, "(00000): [rodbc Class], Invalid sentence number.\n");				
			return ;
		}

		//////////////////////////////////////////
		// 2000.01.12, chequeo de sentencia en uso
		if(astmt[*pSntnc].InUse != true)
		{
			bError = true;		// Sets Error flag member
			SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLFetch() : No sentence"); // Set error description
			return ;
		}
		//////////////////////////////////////////

		iRetcode = SQLFetch(astmt[*pSntnc].hStmt);
		if(iRetcode == SQL_SUCCESS_WITH_INFO)
		{				
			bError = false;		// Sets no Error flag
			SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLFetch()");
		}	
		else if(iRetcode != SQL_SUCCESS)
		{
			bError = true;		// Sets bError flag member
			// Set error description
			SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLFetch()");			
		}		
	}
}

//--------------------------------------
EXPORT void rodbc::AutoCommit(bool Mode)
//--------------------------------------
// Sets Autocommit mode ON/OFF
//--------------------------------------
{
	dCommitOption = SQL_AUTOCOMMIT_ON;
	if(!dConn)
	{
		Connect(szDataSourceName);
	}
	if(!bError)
	{
		
		if(Mode)
			dCommitOption = SQL_AUTOCOMMIT_ON;
		else
			dCommitOption = SQL_AUTOCOMMIT_OFF;

		iRetcode = SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT, dCommitOption);
		if(iRetcode == SQL_SUCCESS)
			bError = false;
		else if(iRetcode == SQL_SUCCESS_WITH_INFO)
		{				
			bError = false;		// Sets bError flag member
			SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLSetConnectOption");
		}
		else
		{
			bError = true;       // Sets bError flag member
			SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLSetConnectOption");
		}
	}
}

//-----------------------------
EXPORT void rodbc::Commit(void)
//-----------------------------
// Commits transactions
//-----------------------------
{
	if(dConn)
	{
		// There is a connection, can commit
		iRetcode = SQLTransact(SQL_NULL_HENV, hdbc, SQL_COMMIT);
		if(iRetcode == SQL_SUCCESS)
			bError = false;
		else if(iRetcode == SQL_SUCCESS_WITH_INFO)
		{				
			bError = false;		// Sets bError flag member
			SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLTransact()");
		}
		else
		{
			bError = true;       // Sets bError flag member
			SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLTransact()");
		}
	}
}

//-------------------------------
EXPORT void rodbc::RollBack(void)
//-------------------------------
// Sets Autocommit mode ON/OFF
//-------------------------------
{
	if(dConn)
	{
		// There is a connection, can commit
		iRetcode = SQLTransact(SQL_NULL_HENV, hdbc, SQL_ROLLBACK);
		if(iRetcode == SQL_SUCCESS)
			bError = false;
		else if(iRetcode == SQL_SUCCESS_WITH_INFO)
		{				
			bError = false;		// Sets bError flag member
			SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLTransact()");
		}
		else
		{
			bError = true;      // Sets bError flag member 
			SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLTransact()");
		}
	}
}
//-----------------------
EXPORT bool rodbc::Error(void)
//-----------------------
// Error?
//-----------------------
{ 
	return (bError); 
}

//-----------------------
EXPORT void rodbc::ResetLastError(void)
//---------------------------------
// Error to false & Message to NULL
//---------------------------------
{ 
	bError = false; 
	szMessage[0] = 0x00;
}

//-----------------------
EXPORT bool rodbc::TrxInProgress(void)
//-----------------------
// TrxInProgress?
//-----------------------
{ 
	return (bTrxInProg); 
}

//------------------/ Private member functions /------------------------//

//----------------------------------------
void rodbc::SetErrorMsg(HENV m_henv,
						HDBC m_hdbc,
						HSTMT m_hstmt,
						char *m_Operation,
						char *szTheQuery)
//----------------------------------------
// Private member function, sets public bError data members
//----------------------------------------
{	
	// Get sql specific error info
	SQLError(m_henv, 
			 m_hdbc,
			 m_hstmt,
			 (unsigned char *) szSqlState,
			 &fNativeError,
			 (unsigned char *) szErrorMsg,
			 sizeof(szErrorMsg) - 1,
			 &cbErrorMsgLen);
	// Generates errordescription string
	copystring  (szOperation, m_Operation);
	copystring  (szMessage  , m_Operation);
	appendstring(szMessage  , ", ");
	appendstring(szMessage  , szErrorMsg);
	appendstring(szMessage  , ", ");
	appendstring(szMessage  , szSqlState);
	appendstring(szMessage  , "\n");
	if(szTheQuery)
		appendstring(szMessage  , szTheQuery);
	// Estado como error nativo
	dwStatus = fNativeError;
}

//-----------------------// Virtual Methods //----------------//
EXPORT void rodbc::Insert(void){}

EXPORT void rodbc::Update(void){}

EXPORT void rodbc::Delete(void){}

EXPORT void rodbc::Read(void){}
//----------------------------------------------------------------------//


//----------------------------------------------------------------------
//
// Set data source name
//
//----------------------------------------------------------------------
void  rodbc::SetDataSourceName(char *szXDataSource)
{
	if(szXDataSource != NULL)
		copystring((char *)szDataSourceName, szXDataSource);
}

//----------------------------------------------------------------------
//
// Set user name
//
//----------------------------------------------------------------------
void  rodbc::SetUserName(char *szXUser)
{
	if(szXUser != NULL)
		copystring((char *)szUserName, szXUser);
}

//----------------------------------------------------------------------
//
// Set user password
//
//----------------------------------------------------------------------
void  rodbc::SetPassword(char *szXPassword)
{
	if(szXPassword != NULL)
		copystring((char *)szPassword, szXPassword);
}

//----------------------------------------------------------------------
//
// Get error message
//
//----------------------------------------------------------------------
const char	*rodbc::GetErrorMessage(char *pextBuffer)
{
	// Copia a buffer externo
	if(pextBuffer != NULL)
		strcpy(pextBuffer, szErrorMsg); /* FIX QUE VARIABLE RETORNAR */
	// Puntero constante a Mensaje de Error, retornado tambien
	return (const char *)szErrorMsg; /* FIX QUE VARIABLE RETORNAR */
}

//----------------------------------------------------------------------
//
// Get last SQL Command
//
//----------------------------------------------------------------------
const char	*rodbc::GetLastSQLCommand(char *pextBuffer)
{
	// Copia a buffer externo
	if(pextBuffer != NULL)
		strcpy(pextBuffer, szOperation);
	// Puntero constante a comando SQL u operacion ultima de error
	return (const char *)szOperation;
}

//----------------------------------------------------------------------
//
// Get return code from sql 
//
//----------------------------------------------------------------------
RETCODE  rodbc::GetReturnCode(void)
{
	// Valor del Return-Code, dependiente del SQL
	return (iRetcode);
}


//----------------------------------------------------------------------
//
// Get status-error code from sql 
//
//----------------------------------------------------------------------
SDWORD  rodbc::GetStatus(void)
{
	// Valor del Return-Code, dependiente del SQL
	return (dwStatus);
}

//----------------------------------------------------------------------
//
// Begin Transaction 
//
//----------------------------------------------------------------------
bool rodbc::BeginTransaction(void)
{
	// Precondicion: Transaccion no en progreso
	if( TrxInProgress() )
		return false;

	// Auto-Completar a falso
	AutoCommit(false);	
	// Verdadero a indicador de trx en progreso
	bTrxInProg = true;
	
	// Retorno ok
	return ( true );
}

//----------------------------------------------------------------------
//
// End Transaction 
//
//----------------------------------------------------------------------
bool rodbc::EndTransaction(bool bResultTrx, bool bAutoCommit)
{
	// Precondicion: Transaccion en progreso
	if( !TrxInProgress() )
		return false;

	// Completar la transaccion segun resultados...
	if(bResultTrx)
		Commit();	
	else
		RollBack();

	// Habilitar Auto-Completar?
	if(bAutoCommit)
	{
		// Auto-Completar a verdadero
		AutoCommit(true);	
		// Falso a indicador
		bTrxInProg = false;
	}

	// Retorno dependiente
	return ( Error() );
}

//----------------------------------------------------------------------//

//----------------------------------------
EXPORT void rodbc::Fetch(int *pSntnc, int iRowPos)
//----------------------------------------
// Gets data (SQLFetch/SQLExtendedFetch)
//----------------------------------------
{
	/*********/
	int i = 0;
	/*********/
	// KindOfFetch (Common|Extended)
	if (!bError)
	{	
		if((*pSntnc < 0) ||
		   (*pSntnc > MAX_STMTS))
		{
			bError = true;
			// Set error description
			copystring(szOperation,"Fetch()");
			copystring(szMessage, "(00000): [rodbc Class], Invalid sentence number.\n");				
			return ;
		}
		//////////////////////////////////////////
		// 2000.01.12, chequeo de sentencia en uso
		if(astmt[*pSntnc].InUse != true)
		{
			bError = true;		// Sets Error flag member
			SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLFetch() : No sentence"); // Set error description
			return ;
		}
		//////////////////////////////////////////
		// Read until specified row
		for(i=1,
			iRetcode = SQLFetch(astmt[*pSntnc].hStmt); 
			i < iRowPos && (iRetcode == SQL_SUCCESS || iRetcode == SQL_SUCCESS_WITH_INFO);
			i++ ) 
				iRetcode = SQLFetch(astmt[*pSntnc].hStmt);
		
		if(iRetcode == SQL_SUCCESS_WITH_INFO)
		{				
			bError = false;		// Sets no Error flag
			SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLFetch()+SQLSetPos();error=false;");
		}	
		else if(iRetcode != SQL_SUCCESS)
		{
			bError = true;		// Sets bError flag member
			// Set error description
			SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLFetch()+SQLSetPos()");			
		}		
	}
}

//-----------------------------------------------
EXPORT void rodbc::GetData(int		iColumnNumber,
						   void		*pbColumna,
						   SWORD	swDataType,
						   SDWORD	sdwDataLen,						   
						   int		*pSntnc,
						   bool		bServerBased)
//-----------------------------------------------
// Gets data from a column of the result set
//-----------------------------------------------
{		
 	// If not connected, Connect
	if(!dConn)
	{
		Connect(szDataSourceName);
	}
	// If no previous Error
	if (!bError)
	{
		if((nStmt == 0) ||
		   (*pSntnc < 0) ||
		   (*pSntnc > MAX_STMTS) ||
		   (!(astmt[*pSntnc].InUse)) ||
		   (iColumnNumber <= 0 ) || (iColumnNumber > MAX_COLUMNS))
		{
			bError = true;
			// Set error description
			copystring(szOperation,"GetData()");
			copystring(szMessage, "(00000): [rodbc Class], Invalid sentence or column number.\n");				
			return ;
		}
		if(!bError)
		{
			// Field lenght unknown for current column; depends on data type:
			astmt[*pSntnc].acbBytes[iColumnNumber-1] = SQL_NO_TOTAL ;
			// Get data from column of result set				
			iRetcode = SQLGetData(astmt[*pSntnc].hStmt,
								  iColumnNumber,
								  swDataType,
								  pbColumna,
								  sdwDataLen,
								  &astmt[*pSntnc].acbBytes[iColumnNumber-1]);
			if(iRetcode != SQL_SUCCESS)
			{				
				bError = true;		// Sets bError flag member
				// Set error description
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLGetData()");
			}
		}
	}
}

//---------------------------------------------------
EXPORT void rodbc::GetNextResultSet(int *pSntnc)
//---------------------------------------------------
// Gets the next result set from a statement or stored procedure that returns more 
// than 1 result set
//---------------------------------------------------
{
 	// If not connected, Connect
	if(!dConn)
	{
		Connect(szDataSourceName);
	}
	// If no previous Error
	if (!bError)
	{
		if((nStmt == 0) ||
		   (*pSntnc < 0) ||
		   (*pSntnc > MAX_STMTS) ||
		   (!(astmt[*pSntnc].InUse)))
		{
			// Allocate new sentence Handler
			AllocateSentence(pSntnc);
		}
		if(!bError)
		{
			// Obtains the next result set from a multi-set statement
			iRetcode = SQLMoreResults(astmt[*pSntnc].hStmt);
			if(iRetcode != SQL_SUCCESS)
			{
				bError = true;		// Sets error flag member
				// Gets error description
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLMoreResults()");
			}
		}
	}
}

//---------------------------------------------------
EXPORT void rodbc::GetAllData(void   *pbBuffer,
							  SWORD  swDataType,
							  SDWORD sdwDataLen,
							  int    *iDataXfer,
							  int	 *pSntnc)
//---------------------------------------------------
// Gets all the available data columns
//---------------------------------------------------
{
	// Buffer offset and length
	INT	iOffset = 0, iLength = 0, iCol = 0;

 	// If not connected, Connect
	if(!dConn)
	{
		Connect(szDataSourceName);
	}
	// If no previous Error
	if (!bError)
	{
		if((nStmt == 0) ||
		   (*pSntnc < 0) ||
		   (*pSntnc > MAX_STMTS) ||
		   (!(astmt[*pSntnc].InUse)))
		{
			// Allocate new sentence Handler
			AllocateSentence(pSntnc);
		}
		if(!bError)
		{
			// Get the columns number
			iRetcode = SQLNumResultCols(astmt[*pSntnc].hStmt, (PSHORT)&astmt[*pSntnc].NumCol);
			if(iRetcode != SQL_SUCCESS)
			{
				bError = true;		// Sets error flag member
				// Gets error description
				SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLGetAllData()");
			}
			else
			{
				// Get all the available data
				for(iCol = 0, iLength = 0, iOffset = 0; 
					iCol < astmt[*pSntnc].NumCol 
					&&
					iLength < sdwDataLen; 
					iCol++)
				{
					// Field lenght unknown for current column; depends on data type:
					astmt[*pSntnc].acbBytes[iCol] = SQL_NO_TOTAL ;
					// Get data from column of result set				
					iRetcode = SQLGetData(astmt[*pSntnc].hStmt,
										  iCol+1,
										  swDataType,
										  (PBYTE)pbBuffer+iOffset,
										  sdwDataLen-iLength,
										  &astmt[*pSntnc].acbBytes[iCol]);
					if(iRetcode == SQL_SUCCESS)
					{				
						// Add to buffer offset
						iOffset += astmt[*pSntnc].acbBytes[iCol];
						iLength += astmt[*pSntnc].acbBytes[iCol];
					}
					else if(iRetcode == SQL_SUCCESS_WITH_INFO)
					{					
						bError = false;		// Do not set the error flag member 
						// Set warning description
						SetErrorMsg(hEnv, hdbc, SQL_NULL_HSTMT, "SQLGetAllData() warning");
						// Add to buffer offset
						iOffset += astmt[*pSntnc].acbBytes[iCol];
						iLength += astmt[*pSntnc].acbBytes[iCol];
					}
					else
					{
						bError = true;		// Sets bError flag member
						// Set error description
						SetErrorMsg(hEnv, hdbc, astmt[*pSntnc].hStmt, "SQLGetAllData()");
						// Exit method
						return;
					};
				};//end-for
				// Total buffer length
				(*iDataXfer) = iOffset;
			};//end-if-iRetcode
		};//end-if-bError
	};//end-if-bError
}
//--------------------------------------------------------------------------------
EXPORT void rodbc::FreeSentence(int SentNumber, bool bClose, bool bUnbindColumns )
//--------------------------------------------------------------------------------
// Frees the sentence resource
//---------------------------------------------
{
	if((SentNumber < MAX_STMTS) && (SentNumber >= 0))
	{
		if(astmt[SentNumber].InUse)
		{
			bError = false;
			/////////////////////////////////////////////////////////////////
			// Close or Drop statement?
			/////////////////////////////////////////////////////////////////
			if(bClose)
				{
				iRetcode = SQLFreeStmt(astmt[SentNumber].hStmt, SQL_CLOSE);
				if( bUnbindColumns )
					{
					astmt[SentNumber].NumParam = 0;
					astmt[SentNumber].NumCol   = 0;
					/* FIX : unbind implica que se desacoplan las variables.... */
					astmt[SentNumber].InUse    = false;				
					/* FIX */
					nStmt--;
					};
				}
			else
				{	
				iRetcode = SQLFreeStmt(astmt[SentNumber].hStmt, SQL_DROP);				
				astmt[SentNumber].NumParam = 0;
				astmt[SentNumber].NumCol   = 0;
				astmt[SentNumber].InUse    = false;				
				nStmt--;
				}
			/////////////////////////////////////////////////////////////////
		}
		else
		{
			bError = true;
			copystring(szOperation,"SQLFreeStmt()");
			copystring(szMessage, "(00000): [rodbc Class], Sentence does not exist.\n");
		}
	}
	else
	{
		bError = true;
		copystring(szOperation,"SQLFreeStmt()");
		copystring(szMessage, "(00000): [rodbc Class], Invalid sentence number.\n");				
	}
}

//---------------------------------------------
EXPORT int rodbc::IsSentenceInUse(int SentNumber)
//------------------------------------------------
// Tells if sentence in use, and NUMBER of columns
//------------------------------------------------
{
	if ( astmt[SentNumber].InUse && astmt[SentNumber].NumCol > 0 )
		return astmt[SentNumber].NumCol ;
	else if ( astmt[SentNumber].InUse )
		return astmt[SentNumber].InUse;
	else
		return astmt[SentNumber].InUse;
}

