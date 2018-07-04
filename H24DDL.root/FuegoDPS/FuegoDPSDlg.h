// FuegoDPSDlg.h: archivo de encabezado
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// Header STRING STD
#include <string>
#include <map>
using namespace std;
// Headers del Sistema DPS-BATCH
#include <qusrinc/datanet.h>
#include <qusrinc/TRXDNET.h>
#include <qusrinc/databatc.h>
#include <qusrinc/Trxres.h>
#include "afxdtctl.h"
#include "afxcmn.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////


// Cuadro de diálogo de CFuegoDPSDlg
class CFuegoDPSDlg : public CDialog
{
// Construcción
public:
	CFuegoDPSDlg(CWnd* pParent = NULL);	// Constructor estándar

// Datos del cuadro de diálogo
	enum { IDD = IDD_FUEGODPS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// Compatibilidad con DDX/DDV


// Implementación
protected:
	HICON m_hIcon;

    ////////////////////////////
    rodbc         odbcPri ,
                  odbcSec ;  
    string        strDATABASE  ; 
    string        strUSER      ; 
    string        strPASSWORD  ;
    string        strQuery;
    ////////////////////////////

    WORD          wAPPLICAT  ;  /* backoffice application, IP port */   
	char strSERVER[128+1];
	char wsPort[6+1] ;
	char dBase[64+1];
	char *desenc;
	char wUSER[512];
	char wPASSWORD[512];
	

    ////////////////////////////

	// Funciones de asignación de mensajes generadas
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:

    /*********************************/
    //CDateTimeCtrl m_datetimePicker;
    CTime         ctDate    ,
                  ctDate2   ;
    CString       strDate ;
    //CDateTimeCtrl m_datetimePicker2;
    /*********************************/
    BOOL m_bReprocesamiento;
    BOOL m_checkConvertASCII;
    CString editInterCTACTE_CAJAHO;
    /*********************************/
    void         DoSaldosHis( e_dps_interfases eType , BOOL bTruncate );
    void         DoSaldos( e_dps_interfases eType , BOOL bTruncate );
    void         DoMovimientos(  e_dps_interfases eType , BOOL bTruncate ) ;
	BOOL         CheckHolidayDateInArray( char *szDateTime ) ;
    /*********************************/

	std::string		TransformarLeyenda( std::string sLeyendaFind, std::string sTipoMovimiento);
	void			CargarLeyendas();
	std::map<std::string, std::string> mListadoLeyendas;
	
	/************************************************************************************/
	/* 2013.08.07 - JAF - AGREGO METODOS DE IMPACTO DE PBF Y UMOV						*/
	/************************************************************************************/
	int		Impactar_PBF(char *szTrxFilename    , char *szOutputFile );
	int		Impactar_UMOV(char *szTrxFilename, char *szOutputFile );

    afx_msg void OnBnClickedSaldos();
    afx_msg void OnBnClickedSaldosHis();
    afx_msg void OnBnClickedMovimientos();
    afx_msg void OnBnClickedImpactar();
    afx_msg void OnBnClickedCheckRepro();
    afx_msg void OnBnClickedCheckAscii();
    
    CString m_editTexto;
    CProgressCtrl m_progressBar;
    CComboBox m_comboHacia;
    CComboBox m_comboDesde;
    afx_msg void OnCbnDropdownComboFiles1();
    CDateTimeCtrl m_datetimePicker;
    CDateTimeCtrl m_datetimePicker2;
	afx_msg void OnBnClickedOk();

	
	
};
