#ifdef _USE_FTP_API_
/////////////////////////////////////////////////////////////////////////////////////////
//
// IT24 Sistemas S.A.
// Libreria de Utilidades Generales
// Funcion de Lectura de Parametros de FTP en Base de Datos via ODBC
//
// Este debe ser el diseno de la tabla :
//
// CREATE TABLE FTPSites 
// (	FTPHost VARCHAR(10) ,
//		FIID VARCHAR(4),
// 		XFerHost VARCHAR(32),
// 		XFerUser VARCHAR(32),
// 		XFerPassw VARCHAR(32),
// 		XFerMode VARCHAR(32),
// 		XFerRemotePath VARCHAR(64),
// 		CONSTRAINT PK_FTPSITES_1 PRIMARY KEY ( FTPHost , FIID )
// )
//
// Fecha      Autor   Obs
// 2000.02.03 mdc     Inicial
// 
/////////////////////////////////////////////////////////////////////////////////////////

// Header WIN32
#include <windows.h>

// Headers ANSI C/C++
#include <stdlib.h>
#include <stdio.h>

// Headers del modulo
#include <qusrinc/rodbc.h>
//#include <qusrinc/sqlqcmds.h>
#include <qusrinc/stdiol.h>
// SQL EXTENSIONS
#include <sqlext.h>

///////////////////////////////////////////////////////////////////////
// Macro local de Right Trim para cadenas de caracteres ANSI C
#define TRIM_RIGHT(x) if(strchr(x,' ') != NULL) *strchr(x,' ') = 0x00;
///////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Funcion de lectura de parametros de ftp en base de datos via ODBC
// Retorno : {0=Ok,-1..-6:Error de parametros,-7..-10:Error de ODBC}
/////////////////////////////////////////////////////////////////////
int ftp_read_db_params(
	char *szFTPHost, size_t nHost,			// Identificador de tipo de Host (input)
	char *szFIID, size_t nFIID,				// FIID de Institucion (input opcional)
	char *szXFerHost, size_t nIPHost,		// IP del host (output) 
	char *szXFerUser, size_t nUser,			// Username (output)
	char *szXFerPassw, size_t nPassw,		// Clave de acceso (output)
	char *szXFerMode, size_t nMode,			// Modo binario o ascii (output)
	char *szXFerRemotePath, size_t nPath	// Path remoto (output)
)
{
	// Conexion ODBC y parametros
	rodbc	rodbcInst;	
	int		iSentence = 1;			
	char	szSQLCmd[2048]={0};

	// Precondiciones y defaults
	if(!szFTPHost)	{ szFTPHost = "TANDEM";	nHost = 6; }
	if(!szFIID)		{ szFIID    = "    "  ; nFIID = 4; }
	// Precondiciones y retornos por error
	if(!szXFerHost)			return (-1);
	if(!szXFerUser)			return (-2);
	if(!szXFerPassw)		return (-3);
	if(!szXFerMode)			return (-4);
	if(!szXFerRemotePath)	return (-5);
	if(!(nHost && nFIID && nIPHost && nUser && nPassw && nMode && nPath))
		return (-6);

	// Conexion a base de datos por cada inicializacion del combo box
#ifdef ODBC_SOAT_DATABASE
	rodbcInst.SetDataSourceName( ODBC_SOAT_DATABASE );
	rodbcInst.SetUserName( ODBC_SOAT_USER );
#else
	rodbcInst.SetDataSourceName( "FTP_DB_PARAMS" );
	rodbcInst.SetUserName( "FTP_DB_PARAMS" );
#endif
	rodbcInst.Connect(NULL);	
	if(rodbcInst.Error())
		// Retorno error
		return (-7);

	// Comando SQL de lectura de parametros de Sitio FTP
	// Ordenado de tal forma que si existe una excepcion x FIID, sea esta la primer
	// fila que traiga, sino, la fila general donde el FIID se halla en blanco.
	sprintf(szSQLCmd,
		"SELECT TOP 1 * FROM FTPSITES "
		"WHERE FTPHOST='%s' AND (FIID = '' OR FIID='%s') "
		"ORDER BY FIID DESC ",
		szFTPHost, szFIID );

	// Ejecucion de comando SQL
	rodbcInst.Query( szSQLCmd, &iSentence );
	if(rodbcInst.Error())
		// Retorno error
		return (-8);

	// Binding de columnas	
	rodbcInst.Column(szFTPHost,SQL_C_CHAR,nHost,&iSentence); // FTPHost
	rodbcInst.Column(szFIID,SQL_C_CHAR,nFIID,&iSentence); // FIID
	rodbcInst.Column(szXFerHost,SQL_C_CHAR,nIPHost,&iSentence); // XFerHost
	rodbcInst.Column(szXFerUser,SQL_C_CHAR,nUser,&iSentence); // XFerUser
	rodbcInst.Column(szXFerPassw,SQL_C_CHAR,nPassw,&iSentence); // XFerPassw
	rodbcInst.Column(szXFerMode,SQL_C_CHAR,nMode,&iSentence); // XFerMode
	rodbcInst.Column(szXFerRemotePath,SQL_C_CHAR,nPath,&iSentence); // XFerRemotePath
	if(rodbcInst.Error())
		// Retorno error
		return (-9);

	// Fetch de primer fila de resultados
	rodbcInst.Fetch( &iSentence );
	if(rodbcInst.Error())
		// Retorno error
		return (-10);


	// ASCII TrimRight : Elimina espacios en blanco a derecha
	TRIM_RIGHT(szFTPHost);
	TRIM_RIGHT(szFIID);	
	TRIM_RIGHT(szXFerHost);
	TRIM_RIGHT(szXFerUser);
	TRIM_RIGHT(szXFerPassw);
	TRIM_RIGHT(szXFerMode);
	TRIM_RIGHT(szXFerRemotePath);

	// Ok
	return (0);
};
#endif // _USE_FTP_API_