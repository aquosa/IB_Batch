// DPS32FeriadosDlg.cpp: archivo de implementación

#include "stdafx.h"
#include "DPS32Feriados.h"
#include "DPS32FeriadosDlg.h"
#include "afxdialogex.h"

#include "qusrinc\opciones.h"
#include "FuegoDPS\opciones.cpp"

#include <ctime>
#include <string>
#include <sstream>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cuadro de diálogo de CDPS32FeriadosDlg
CDPS32FeriadosDlg::CDPS32FeriadosDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDPS32FeriadosDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);

	time_t timeToday;
	time(&timeToday);

	tm* now = std::localtime(&timeToday);

	std::stringstream ss;

	ss << now->tm_mday << '/' << now->tm_mon << '/' << now->tm_year;

	std::string formattedTime = ss.str();

	dataTimerPicker_Agregar = formattedTime.c_str();
	dataTimerPicker_Quitar = formattedTime.c_str();

	char strRuta[1024] = {'\0'};
	char datos[512];

	char cInterfases[10+1] = {0x00};
	OPCIONES *xmlOpciones = new OPCIONES("Opciones.xml");
	xmlOpciones->GetTagValue("WSServer", strSERVER, sizeof(strSERVER));
	xmlOpciones->GetTagValue("DBase", dBase, sizeof(dBase));
	xmlOpciones->GetTagValue("WSPort", wsPort, sizeof(wsPort));
	xmlOpciones->GetTagValue("dirEncryptfile", strRuta, sizeof(strRuta));
	xmlOpciones->GetTagValue("Interfaces", cInterfases, sizeof(cInterfases));
	editInterCTACTE_CAJAHO = cInterfases;

	if(strRuta[0] != '\0')
		strcat(strRuta, "/conexion.enc");
	else
		strcpy(strRuta, std::string("conexion.enc").c_str());

	FILE *enc;
	if ((enc = fopen(strRuta, "rb")) == NULL)
	{
		AfxMessageBox("No se encuentra el archivo con los datos de conexión ejecute el DPS32ENC.exe para generarlo por favor");
		return;
	}

	// Lectura de datos
	int count = fread(datos, 1, sizeof datos, enc);
	// Cierre de archivo
	fclose( enc ) ;
	// Desencripcion
	desenc = UnprotectStringValue(datos, count);
	// Ok? Error?
	if (desenc == NULL)
	{
		AfxMessageBox("Hay un problema con los datos de conexión ejecute el DPS32ENC.exe para generarlo por favor");
		return;
	}

	strcpy(wUSER, desenc);
	wUSER[strchr(wUSER, '/') - wUSER] = '\0';
	strcpy(wPASSWORD, strchr(desenc, '/') + 1);
	wPASSWORD[strchr(wPASSWORD, '/') - wPASSWORD] = '\0';

	strUSER = wUSER;
	strPASSWORD = wPASSWORD;
	
	std::size_t pos = strPASSWORD.find(":");
	if( pos != std::string::npos)
		strPASSWORD = strPASSWORD.substr(0, pos);

	strDATABASE = dBase;
	wAPPLICAT = atoi(wsPort);
}


void CDPS32FeriadosDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_Agregar, dataTimerPicker_Agregar);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_Quitar, dataTimerPicker_Quitar);
}


BEGIN_MESSAGE_MAP(CDPS32FeriadosDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Agregar, &CDPS32FeriadosDlg::OnBnClickedButtonAgregar)
	ON_BN_CLICKED(IDC_BUTTON_Quitar, &CDPS32FeriadosDlg::OnBnClickedButtonQuitar)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_Agregar, &CDPS32FeriadosDlg::OnDtnDatetimechangeDatetimepickerAgregar)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_Quitar, &CDPS32FeriadosDlg::OnDtnDatetimechangeDatetimepickerQuitar)
END_MESSAGE_MAP()

// Controladores de mensaje de CDPS32FeriadosDlg


BOOL CDPS32FeriadosDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Establecer el icono para este cuadro de diálogo. El marco de trabajo realiza esta operación
	//  automáticamente cuando la ventana principal de la aplicación no es un cuadro de diálogo
	SetIcon(m_hIcon, TRUE);			// Establecer icono grande
	SetIcon(m_hIcon, FALSE);		// Establecer icono pequeño

	// TODO: agregar aquí inicialización adicional

	return TRUE;  // Devuelve TRUE  a menos que establezca el foco en un control
}


// Si agrega un botón Minimizar al cuadro de diálogo, necesitará el siguiente código
//  para dibujar el icono. Para aplicaciones MFC que utilicen el modelo de documentos y vistas,
//  esta operación la realiza automáticamente el marco de trabajo.

void CDPS32FeriadosDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Contexto de dispositivo para dibujo

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrar icono en el rectángulo de cliente
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Dibujar el icono
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// El sistema llama a esta función para obtener el cursor que se muestra mientras el usuario arrastra
//  la ventana minimizada.
HCURSOR CDPS32FeriadosDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDPS32FeriadosDlg::OnBnClickedButtonAgregar()
{
	// BOTON AGREGAR
	
	CT2CA convertStr (dataTimerPicker_Agregar);
	std::string time(convertStr);

	std::string dia;
	std::string mes;
	std::string año;
	int iQueryNum = 0;

	auto vector = split(time,'/');

	dia = padLeft(vector[0],2,'0');
	mes = padLeft(vector[1],2,'0');
	año = "20" + vector[2].substr(vector[2].length()-2,2);

	auto fechaSQL = año + mes + dia;
	char szfechaSQL[32] = {0x00};
	sprintf( szfechaSQL, "%04i%02i%02i", stoi(año), stoi(mes), stoi(dia) );

	if(CheckHolidayDateInArrayFeriados(szfechaSQL))
	{
		m_editTexto = "EL FERIADO YA ESTA INGRESADO" ;
		UpdateData( FALSE );
		ProcessAppMsgQueue();

		AfxMessageBox( m_editTexto );
	}
	else
	{	
		std::stringstream ss;
		ss << "INSERT INTO FERIAD(FE_SUCURS ,FE_FECPRO) VALUES (0, '";
		ss << szfechaSQL << "') ;" ;
		std::string strQuery = ss.str();
		
		odbc.SetUserName( (PSTR)strUSER.c_str()     );
		odbc.SetPassword( (PSTR)strPASSWORD.c_str() );
		odbc.Connect    ( (PSTR)strDATABASE.c_str() ) ; 

		odbc.Query( (PSTR)strQuery.c_str(), &iQueryNum, FALSE, FALSE ) ;
		if(odbc.Error())
		{
			m_editTexto = "NO SE PUEDE ACTUALIZAR LA FECHA DE PROCESO SQL : "; 
			m_editTexto += odbc.GetErrorMessage(NULL);
			m_editTexto += odbc.GetLastSQLCommand(NULL);
			AfxMessageBox(m_editTexto);
			UpdateData(FALSE);
			ProcessAppMsgQueue();

			odbc.Disconnect();
			return;
		}
		else
		{
			odbc.Commit();
			m_editTexto = "SE AGREGADO CON EXITO" ;
			UpdateData( FALSE );
			ProcessAppMsgQueue();

			AfxMessageBox( m_editTexto );
		};
	}

	odbc.FreeSentence(iQueryNum);
}


void CDPS32FeriadosDlg::OnBnClickedButtonQuitar()
{
	// BOTON QUITAR
	
	CT2CA convertStr (dataTimerPicker_Quitar);
	std::string time(convertStr);

	std::string dia;
	std::string mes;
	std::string año;
	int iQueryNum = 0;

	auto vector = split(time,'/');

	dia = padLeft(vector[0],2,'0');
	mes = padLeft(vector[1],2,'0');
	año = "20" + vector[2].substr(vector[2].length()-2,2);

	auto fechaSQL = año + mes + dia;
	char szfechaSQL[32] = {0x00};
	sprintf( szfechaSQL, "%04i%02i%02i", stoi(año), stoi(mes), stoi(dia) );

	if(!CheckHolidayDateInArrayFeriados(szfechaSQL))
	{
		m_editTexto = "EL FERIADO NO ESTA INGRESADO" ;
		UpdateData( FALSE );
		ProcessAppMsgQueue();

		AfxMessageBox( m_editTexto );
	}
	else
	{
		std::stringstream ss;
		ss << "DELETE FROM [DTN_INTERFINANZAS].[dbo].[FERIAD] WHERE FE_FECPRO = '";
		ss << szfechaSQL << "' ;" ;
		std::string strQuery = ss.str();
		
		odbc.SetUserName( (PSTR)strUSER.c_str()     );
		odbc.SetPassword( (PSTR)strPASSWORD.c_str() );
		odbc.Connect    ( (PSTR)strDATABASE.c_str() ) ; 

		odbc.Query( (PSTR)strQuery.c_str(), &iQueryNum, FALSE, FALSE ) ;
		if(odbc.Error())
		{
			m_editTexto = "NO SE PUEDE ACTUALIZAR LA FECHA DE PROCESO SQL : "; 
			m_editTexto += odbc.GetErrorMessage(NULL);
			m_editTexto += odbc.GetLastSQLCommand(NULL);
			AfxMessageBox(m_editTexto);
			UpdateData(FALSE);
			ProcessAppMsgQueue();

			odbc.Disconnect();
			return;
		}
		else
		{
			odbc.Commit();
			m_editTexto = "SE QUITADO CON EXITO" ;
			UpdateData( FALSE );
			ProcessAppMsgQueue();

			AfxMessageBox( m_editTexto );
		};
	}

	odbc.FreeSentence(iQueryNum);
}


void CDPS32FeriadosDlg::OnDtnDatetimechangeDatetimepickerAgregar(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);

	GetDlgItemText(IDC_DATETIMEPICKER_Agregar , dataTimerPicker_Agregar); 

	*pResult = 0;
}


void CDPS32FeriadosDlg::OnDtnDatetimechangeDatetimepickerQuitar(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);

	GetDlgItemText(IDC_DATETIMEPICKER_Quitar , dataTimerPicker_Quitar); 

	*pResult = 0;
}


// WINDOWS APPLICATION QUEUE PROCESSOR */
int CDPS32FeriadosDlg::ProcessAppMsgQueue(void)
{
  /* WINDOWS APPLICATION message */
  MSG msgApp;
  /******************************************************************/
  /* procesar mensajes WINDOWS pendientes */
  while(PeekMessage(&msgApp, NULL, 0, 0, PM_REMOVE)) 
	{
	TranslateMessage(&msgApp);
	DispatchMessage(&msgApp);
	if(msgApp.message == WM_QUIT || msgApp.message == WM_CLOSE)
		return (-1);
	}//end-while
  return 0;
  /******************************************************************/
}

std::string padLeft(std::string& string, int pad, char padChar)
{
	std::string retValue = string;

	if( (pad - retValue.size()) > 0 )
		retValue.insert(retValue.begin(), pad - retValue.size(), padChar );

	return retValue;
}

std::string padRight(std::string& string, int pad, char padChar)
{
	std::string retValue = string;

	std::string::size_type sizePad = pad;

	if( (int)(sizePad - retValue.size()) > 0 )
		retValue.insert(retValue.end(), pad - retValue.size(), padChar );
	else retValue = string;

	return retValue;
}

std::vector<std::string> split(std::string& input, char separator)
{
	std::vector<std::string> retValue;

	std::string::size_type pos = 0, prev_pos = 0;
	
	while( (pos = input.find(separator, pos)) != std::string::npos )
    {
		std::string temp = input.substr(prev_pos, pos-prev_pos);

		retValue.push_back(temp);
		
		prev_pos = ++pos;
	}

	//Me fijo si queda uno al final
	if( (int)(input.size() - prev_pos) > 0)
		retValue.push_back( input.substr(prev_pos,input.size()-prev_pos));

	//Remove-Erase -> Elimino todos los strings vacios
	retValue.erase(	std::remove_if(retValue.begin(),retValue.end(),[](std::string a){return a.length() == 0;})
		,retValue.end());

	return retValue;
}

BOOL CDPS32FeriadosDlg::CheckHolidayDateInArrayFeriados( char *szDateTime )
{
    static stTABFER_ARRAY_t stTABFER;        /* Feriados Posibles */
    long                    nSearch = 0,     /* Indices */
                            nInx    = 0;
    int                     iQueryNum = _ODBC_READNEXT_SNTC_ ;
    char                    szYYYYMMDD[16+1] = {0x00}; /* "yyyymmdd" */
    char                    szFECHAFER[30+1]   = {0x00}; /* FECHA-FERIADO */
    rodbc                   odbcLoc;
	int						icount	= 0;

	FILE	*fFeriados;
	char	szLogFeriados[] = "FERIADOS_H2.log";
	fFeriados = fopen(szLogFeriados, "wt");
	fprintf(fFeriados,"LOG PARA ERROR CON FECHA DE FERIADOS EN HOLIDAY\n");  
 
    /* CONNECT SECONDARY DATABASE */
    odbcLoc.SetUserName( (PSTR)strUSER.c_str()     );
    odbcLoc.SetPassword( (PSTR)strPASSWORD.c_str() );
    odbcLoc.Connect    ( (PSTR)strDATABASE.c_str() ) ;  
    if(odbcLoc.Error())
	    return FALSE;

    /* HOLIDAY? - PREPARE QUERY */
    iQueryNum = _ODBC_READNEXT_SNTC_ ;		
    odbcLoc.Column( szFECHAFER, 1, 20+1, &iQueryNum ); //1
#ifdef _USE_EMBEBED_SQL_
    strQuery = "SELECT FE_FECPRO FROM FERIAD" ;
#else
      strQuery = "EXEC SP_FECHAS_DE_FERIADO" ;
#endif // _USE_EMBEBED_SQL_
    /* EXECUTE QUERY */
    odbcLoc.Query( (PSTR)strQuery.c_str(), &iQueryNum, TRUE, TRUE ) ;    
    /* FETCH CYCLE */
    for( // READ FIRST
        odbcLoc.Fetch( &iQueryNum ) , nInx = 0;
        // LOOP CONDITIONS
        nInx < sizeof(stTABFER.TABFER)/sizeof(stTABFER.TABFER[0])
        &&
        ( odbcLoc.GetReturnCode() == SQL_SUCCESS_WITH_INFO 
            ||  
            odbcLoc.GetReturnCode() == SQL_SUCCESS  
        ) ;
        // READ NEXT
        odbcLoc.Fetch( &iQueryNum )  , nInx++
        )
    {		
        // STR COPY
        DateTimeToDateYYYYMMDD(szFECHAFER,szYYYYMMDD);         /* "YYYYMMDD" */
        COPYSTRING( stTABFER.TABFER[nInx].chDATA, szYYYYMMDD);
		icount++;
    };
    /* OK! */
    odbcLoc.FreeSentence( iQueryNum );    
    odbcLoc.Disconnect();

  /* Check for Holiday */
  for( nSearch = 0; 
       nSearch < icount;
       nSearch++ )
  {
		fprintf(fFeriados,"chData: %i, szDateTime: %i \n", atoi(stTABFER.TABFER[nSearch].chDATA), atoi(szDateTime));
        if(strcmp(stTABFER.TABFER[nSearch].chDATA,szDateTime) == 0){ 
			fprintf(fFeriados,"BREAK\n");  
            break; /* OK ! */
		}
  };
  fclose(fFeriados);
  fFeriados = NULL;
  /* Holiday? */
  if( nSearch < icount )
       return TRUE ;
  else
       return FALSE ;
};