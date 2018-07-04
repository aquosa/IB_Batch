
// DPS32FeriadosDlg.h: archivo de encabezado
//

#pragma once

#include <string>
#include <qusrinc/datanet.h>
#include <qusrinc/TRXDNET.h>
#include <qusrinc/databatc.h>
#include <qusrinc/Trxres.h>

#include <vector>

// Cuadro de diálogo de CDPS32FeriadosDlg
class CDPS32FeriadosDlg : public CDialog
{
// Construcción
public:
	CDPS32FeriadosDlg(CWnd* pParent = NULL);	// Constructor estándar

// Datos del cuadro de diálogo
	enum { IDD = IDD_DPS32FERIADOS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// Compatibilidad con DDX/DDV


// Implementación
protected:
	HICON m_hIcon;

	// Funciones de asignación de mensajes generadas
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	// Base de Datos
    rodbc odbc;
    std::string strDATABASE; 
    std::string strUSER; 
    std::string strPASSWORD;
    std::string strQuery;
	CString editInterCTACTE_CAJAHO;

    WORD wAPPLICAT;  /* backoffice application, IP port */   
	char strSERVER[128+1];
	char wsPort[6+1] ;
	char dBase[64+1];
	char *desenc;
	char wUSER[512];
	char wPASSWORD[512];

public:
	CString dataTimerPicker_Agregar;
	CString dataTimerPicker_Quitar;
	afx_msg void OnBnClickedButtonAgregar();
	afx_msg void OnBnClickedButtonQuitar();
	afx_msg void OnDtnDatetimechangeDatetimepickerAgregar(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepickerQuitar(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL CheckHolidayDateInArrayFeriados( char *szDateTime ) ;
	CString m_editTexto;

private:
	int ProcessAppMsgQueue(void);
		
};

std::string padLeft(std::string& string, int pad, char padChar);
std::string padRight(std::string& string, int pad, char padChar);
std::vector<std::string> split(std::string& input, char separator);