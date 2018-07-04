/////////////////////////////////////////////////////////////////////////////////////////////////////////
// IT24 Sistemas S.A.
// DATANET-BATCH methods - ISO8583 conversion
//
// Tarea        Fecha           Autor   Observaciones
// (Alfa)       2006.01.24      mdc     Conversion DATANET-ISO8583 para impacto BATCH
// (Alfa)       2006.03.06      mdc     Correcciones segun BANCO DB/CR y tipo de cuenta
// (Alfa)       2006.03.28      mdc     e_codestad_dps_batch dpsState = DPS_B_ENVIADO_POR_LA_RED (x omision)
// [ALFA]       2006.04.03      mdc     DIFCRE no existe en el BATCH
// [ALFA]       2006.04.17      mdc     En ::GetEncryptionKeys(), se cambia el orden de recuperar claves
// [ALFA]       2006.05.15      mdc     .NET RUNTIME CHEK#2 : memcpy(fld43.stCrdAccptNameLoc.chFiller,"--------",sizeof fld43.stCrdAccptNameLoc.chFiller);
// [ALFA]       2006.07.27      mdc     "DPS-GENERIC-TERM" se reemplaza por "DPS-GENERIC-XX" incluyendo a la OPERACION 
// [ALFA]       2006.08.15      mdc     ISO_ACCT_LEN = x , DTN_ACCT_LEN = y
// [ALFA]       2006.09.13      mdc     "DPS-GENERIC-XX" agrega "S" si los CUITs son iguales. "N" si no.
// [ALFA]       2006.09.18      mdc     _SYSTEM_ATM_TDFUEGO_
// [ALFA]       2006.10.03      mdc     Modifica la forma de instanciar "DPS_i_batch *ptrDPS" ;
// [ALFA]       2006.10.09      mdc     Fix para que no contenga un NULL el nro. de cuenta al insertarse en su long. precisa */
// [BETA]       2007.09.25      mdc     FIX : precondicion : banco valido 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <qusrinc/dpcsys.h>
#ifdef _SYSTEM_DATANET_BATCH_	

// ANSI headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

// DATANET BATCH
#include <qusrinc/databatc.h>
#include <qusrinc/dnetbcra.h>
#include <qusrinc/copymacr.h>
#include <qusrinc/odbcdnet.h>
#include <qusrinc/trxdnet.h>

// Libreria utilitaria 
#include <qusrinc/stdiol.h>
// ISO 8583
#include <qusrinc/iso8583.h>
#include <qusrinc/isoapp.h>


// Libreria STD TEMPLATE STRING
#include <string>
using namespace std;


#if defined( _SYSTEM_DATANET_ ) || defined( _SYSTEM_DATANET_BATCH_ )
#define  _SYSTEM_DATANET_IN_USE_ (TRUE)
#else
#define _SYSTEM_DATANET_IN_USE_  (FALSE)
#endif

#define PADCOPYINTEGER_I( to, from ) sprintf(to,"%0*i" , sizeof(to), from );

// Constructor opcional
DPS_i_batch::DPS_i_batch( string strIpAddress, unsigned short ushIpPort) 
{ 
    strSrvIpAddress = strIpAddress;
    ushSrvIpPort    = ushIpPort;
};

///////////////////////////////////////////////////////////////////////
// Parsear mensaje DPRE y auditar en stream de salida
///////////////////////////////////////////////////////////////////////
int DPS_i_batch::DPRE_Test(char chBuffer[], int iBuffer, char* szOutFile, bool bTruncate, bool fromBatch)
{
    DPS_i_batch *ptr    = (DPS_i_batch *)chBuffer;
	/**/
	DPS_i_batch::data_tag *ptrdata  = (DPS_i_batch::data_tag *)chBuffer;
	/**/
	FILE        *hFile = NULL;	
    time_t tSystemTime      = time(NULL);              // System Time, in seconds since 1-Jan-70
    struct tm *ptmLocalTime = localtime(&tSystemTime); // Local System Time (GMT - h)
	struct tm *ptmGMTime    = gmtime(&tSystemTime);    // Greenwich Time (GMT - h)
    const char *pAscTime1   = asctime( ptmLocalTime );
    const char *pGMTime1    = asctime( ptmGMTime );
    const char *pAscTime2   = ctime( &tSystemTime );
    const char *pGMTime2    = ctime( &tSystemTime );	
	static long lCounter    = 0 ; /* contador de registros estatico (no local, vida global)*/

	// Precondition
	if(NULL == szOutFile || iBuffer <= 0 || NULL == chBuffer)
		return (-1);
	// Verify output file
	if ( bTruncate )
	{
		// Open truncating
		if ((hFile = fopen(szOutFile, "w")) ==NULL )
			return (-2);
	}
	else 
	{
		// Open appending
		if ((hFile = fopen(szOutFile, "a")) ==NULL )
			return (-2);
	};

	// En forma BATCH, no viene el header. Mover registro al ppio.
	if(fromBatch)
	{
		if( DPS_B_TIPO_REGISTRO_HEADER == ptrdata->TIPOREG )    // HEADER
			lCounter++ ; // Contar registro 2013.08.26
		else if( DPS_B_TIPO_REGISTRO_BODY == ptrdata->TIPOREG ) // BODY
			lCounter++ ;
		else if( DPS_B_TIPO_REGISTRO_FOOTER == ptrdata->TIPOREG ) // TRAILER
			lCounter++ ;
		else
			return (-5);
	} ;

    fprintf(hFile,"\r\nMENSAJE BATCH - DATANET");
    fprintf(hFile,"\r\n----------------------");
	fprintf(hFile,"\r\nBANDEB [%*.*s]",3,3,ptrdata->BANDEB	),	//Codigo Banco segun BCRA	ej."067":BANSUD
	fprintf(hFile,"\r\nFECSOL [%*.*s]",8,8,ptrdata->FECSOL	),	//Fecha de compensacion	Del dia o prox. Habil -aaaammdd-
	fprintf(hFile,"\r\nNUMTRA [%*.*s]",7,7,ptrdata->NUMTRA	),	//Numero de transferencia	
	fprintf(hFile,"\r\nNUMABO [%*.*s]",7,7,ptrdata->NUMABO	),	//Abonado	
	fprintf(hFile,"\r\nTIPOPE [%*.*s]",2,2,ptrdata->TIPOPE	),	//Tipo de operacion	Ver tabla I
	fprintf(hFile,"\r\nIMPORTE [%*.*s]",17,17,ptrdata->IMPORTE );//Importe de la transferencia	15 enteros y 2 decimales
    fprintf(hFile,"\r\nSUCDEB [%*.*s]",4,4,ptrdata->DATDEB.SUCDEB	),	//Sucursal del Banco que tratara el debito	No es necesariamente la de la cuenta
	fprintf(hFile,"\r\nNOMSOL [%*.*s]",29,29,ptrdata->DATDEB.NOMSOL	),	//Nombre de la cuenta	
	fprintf(hFile,"\r\nTIPCUE [%*.*s]",2,2,ptrdata->DATDEB.TIPCUE	),	//Tipo de cuenta	Ver tabla II
	fprintf(hFile,"\r\nNCUECM [%*.*s]",2,2,ptrdata->DATDEB.NCUECM	),	//Numero de cuenta corto	Interno Datanet
	fprintf(hFile,"\r\nNUMCTA [%*.*s]",17,17,ptrdata->DATDEB.NUMCTA	),	//Numero de cuenta	Formato Banco
	fprintf(hFile,"\r\nFSENDB [%*.*s]",6,6,ptrdata->DATDEB.FSENDB	),	//Fecha de envio de la transf.  al Banco	Aammdd
	fprintf(hFile,"\r\nHSENDB [%*.*s]",4,4,ptrdata->DATDEB.HSENDB	),	//Hora de envio	Hhmm
	fprintf(hFile,"\r\nOPEDB1 [%*.*s]",2,2,ptrdata->DATDEB.OPEDB1	),	//Identificacion del operador #1	Primer autorizante del Banco de db.
	fprintf(hFile,"\r\nOPEDB2 [%*.*s]",2,2,ptrdata->DATDEB.OPEDB2	),	//Identificacion del operador #2 	Segundo autorizante
	fprintf(hFile,"\r\nRECHDB [%*.*s]",4,4,ptrdata->DATDEB.RECHDB	);	//Motivo del rechazo del Banco de debito	Ver tabla III
    fprintf(hFile,"\r\nBANCRE [%*.*s]",3,3,ptrdata->DATCRE.BANCRE	),	//Codigo de Banco segun BCRA	
	fprintf(hFile,"\r\nSUCCRE [%*.*s]",4,4,ptrdata->DATCRE.SUCCRE	),	//Sucursal del Banco que tratara el credito	No es necesariamente la de la cuenta
	fprintf(hFile,"\r\nNOMBEN [%*.*s]",29,29,ptrdata->DATCRE.NOMBEN	),	//Nombre de la cuenta a acreditar	
	fprintf(hFile,"\r\nTIPCRE [%*.*s]",2,2,ptrdata->DATCRE.TIPCRE	),	//Tipo de cuenta	Ver tabla II
	fprintf(hFile,"\r\nCTACRE [%*.*s]",17,17,ptrdata->DATCRE.CTACRE	),	//Numero de cuenta	Formato Banco
	fprintf(hFile,"\r\nFSENCR [%*.*s]",6,6,ptrdata->DATCRE.FSENCR	),	//Fecha de envio de la transf.  al Banco	Aammdd
	fprintf(hFile,"\r\nHSENCR [%*.*s]",4,4,ptrdata->DATCRE.HSENCR	),	//Hora de envio	Hhmm
	fprintf(hFile,"\r\nOPECR1 [%*.*s]",2,2,ptrdata->DATCRE.OPECR1	),	//Identificacion del operador #1	Primer autorizante del Banco de cr.
	fprintf(hFile,"\r\nOPECR2 [%*.*s]",2,2,ptrdata->DATCRE.OPECR2	),	//Identificacion del operador #2 	Segundo autorizante
	fprintf(hFile,"\r\nRECHCR [%*.*s]",4,4,ptrdata->DATCRE.RECHCR	);	//Motivo del rechazo del Banco de credito	Ver tabla III
    fprintf(hFile,"\r\nOPECON [%*.*s]",2,2,ptrdata->OPECON	),	//Operador que confecciono la transferencia	
	fprintf(hFile,"\r\nOPEAU1 [%*.*s]",2,2,ptrdata->OPEAU1	),	//Id. Firmante #1 (primer autorizante de la Empresa	
	fprintf(hFile,"\r\nOPEAU2 [%*.*s]",2,2,ptrdata->OPEAU2	),	//Id. Firmante #2 (segundo autorizante de la Empresa	
	fprintf(hFile,"\r\nOPEAU3 [%*.*s]",2,2,ptrdata->OPEAU3	),	//Id. Firmante #3 (tercer autorizante de la Empresa	
	fprintf(hFile,"\r\nFECAUT [%*.*s]",6,6,ptrdata->FECAUT	),	//Fecha de autorizacion (envio del pago a la red)	Aammdd
	fprintf(hFile,"\r\nHORAUT [%*.*s]",4,4,ptrdata->HORAUT	),	//Hora de autorizacion (envio del pago a la red)	Hhmm
	fprintf(hFile,"\r\nESTADO [%*.*s]",2,2,ptrdata->ESTADO	),	//Estado de la transferencia	Ver tabla IV
	fprintf(hFile,"\r\nFECEST [%*.*s]",6,6,ptrdata->FECEST	),	//Fecha ultima modificacion de ESTADO	
	fprintf(hFile,"\r\nOBSER1 [%*.*s]",60,60,ptrdata->OBSER1	),	//Texto libre ingresado por el abonado	Opcional
	fprintf(hFile,"\r\nOBSER2 [%*.*s]",100,100,ptrdata->OBSER2	),	//Datos adicionales ingresados por el abonado	Opcional, varia segun TIPOPE. Ver Tabla V
	fprintf(hFile,"\r\nCODMAC2 [%*.*s]",12,12,ptrdata->CODMAC2	),	//Message Authentication Code (MAC)	Ver Anexo A
	fprintf(hFile,"\r\nCODMAC3 [%*.*s]",12,12,ptrdata->CODMAC3	),	//Message Authentication Code (MAC)	Ver Anexo A
	fprintf(hFile,"\r\nNUMREF [%*.*s]",7,7,ptrdata->NUMREF	),	//Numero de transferencia PC	Nro. De lote (N,3) + nro. de operacion dentro del lote (N,4) 
	fprintf(hFile,"\r\nNUMENV [%*.*s]",3,3,ptrdata->NUMENV	),	//Numero de envio del abonado	
	fprintf(hFile,"\r\nCONSOL [%c]",ptrdata->CONSOL[0]),	//Marca de consolidacion de movimientos ("s]" o "N")	"s]" se genera un unico movimiento en operaciones de debito con igual fecha de solicitud y  nro. de lote
	fprintf(hFile,"\r\nMARTIT [%c]",ptrdata->MARTIT[0]),	//Indica que la cuenta de debito y la cuenta de credito son de la misma empresa	
	fprintf(hFile,"\r\nPRIVEZ [%c]",ptrdata->PRIVEZ[0]),	//"s]" indica que al momento en que se envia el registro al Banco de debito no existe un pago previo aceptado por los dos Bancos que involucre a las cuentas intervinientes."N" indica que los Bancos aceptaron previamente un pago en el que intervenian las mismas cuentas	
	fprintf(hFile,"\r\nRIEABO [%c]",ptrdata->RIEABO[0]),	//"s]" indica que el pago supera el riesgo fijado"N" el pago no supera el riesgo	
	fprintf(hFile,"\r\nRIEBCO [%c]",ptrdata->RIEBCO[0]);	//"s]" indica que al ser aceptado el debito la transferencia supero el riesgo establecido"N" la transferencia no supera el riesgo pero tiene tratamiento del credito obligatorio" " indica que la transferencia quedo ejecutada por el debito (no supero el riesgo ni fue tratado el credito)	
    fprintf(hFile,"\r\nCODEST-1-2-3-4-5-6 [%2.2s,%2.2s,%2.2s,%2.2s,%2.2s,%2.2s]",
        ptrdata->TABEST[0].CODEST, 
        ptrdata->TABEST[1].CODEST, 
        ptrdata->TABEST[2].CODEST, 
        ptrdata->TABEST[3].CODEST, 
        ptrdata->TABEST[4].CODEST, 
        ptrdata->TABEST[5].CODEST );
	fprintf(hFile,"\r\nFECEST-1-2-3-4-5-6 [%6.6s,%6.6s,%6.6s,%6.6s,%6.6s,%6.6s]",
        ptrdata->TABEST[0].FECEST,
        ptrdata->TABEST[1].FECEST,
        ptrdata->TABEST[2].FECEST,
        ptrdata->TABEST[3].FECEST,
        ptrdata->TABEST[4].FECEST,
        ptrdata->TABEST[5].FECEST	), //Fecha de estado	
	fprintf(hFile,"\r\nHOREST-1-2-3-4-5-6 [%6.6s,%6.6s,%6.6s,%6.6s,%6.6s,%6.6s]",
        ptrdata->TABEST[0].HOREST, 
        ptrdata->TABEST[1].HOREST, 
        ptrdata->TABEST[2].HOREST,
        ptrdata->TABEST[3].HOREST, 
        ptrdata->TABEST[4].HOREST, 
        ptrdata->TABEST[5].HOREST	);	//Hora del estado	

    fprintf(hFile,"\r\n----------------------");
    fclose(hFile);

    // Ok
    return 0;
};


///////////////////////////////////////////////////////////////////////
// Convertir de DPRE a formato ISO8583
///////////////////////////////////////////////////////////////////////
int DPS_i_batch::DPRE_ISO8583_Convert(char chBuffer[] , int iBuffer, 
			                          char chConvert[], int iConvert,
                                      const short nISO_ACCT_LEN ,      // long. cuenta ISO
                                      const short nDTN_ACCT_LEN ,      // long. cuenta DTN
                                      const char  chTrunc_LEFT_RIGHT , // 'L' / 'R'
									  char        chDB_CR_OPTION ) // 'D' / 'C' / 'S' / 'R'
{
     /***********************************************************************************/
	 DPS_i_batch::data_tag *ptrDPS = (DPS_i_batch::data_tag *)chBuffer;
	 FILE      *hFile = NULL;	
     APPMSG    isoMsg;
     WORD      wExport = 0;
     /***************************************************************/
	 unsigned short	wLen            = 0;
	 time_t	        tToday          = time( NULL ); /* TODAY, NOW ! */
	 struct  tm     *tmToday        = localtime( &tToday );
     PCHAR          ptrField        = NULL;
	 // 2013.08.15 - JAF - Agrego field 32 para pasar Nro referencia
	 FIELD25		fld25;
	 FIELD32		fld32;
	 FIELD37		fld37;
	 FIELD35		fld35;
	 FIELD44		fld44;
	 FIELD54		fld54;
	 FIELD55		fld55;
	 FIELD60		fld60;     
     FIELD126		fld126;
	 FIELD61		fld61;
	 FIELD43	    fld43;
	 CONST PCHAR	pTxt="?REVERSAL FOR LATE/UNSOL RESP?";
     /***************************************************************/
	 string         strOpcode; /* STRING del codigo de operacion */
     string         strTipOpe; /* STRING del tipo de operacion   */
	 /***************************************************************/
     string         strAcctNum; /* STRING del numero de cuenta ajustado */
     FIELD102		fld102_103; /* ACCT 1, 2                            */
     /***************************************************************/

	 /* FIX : precondicion : inicio con CODIGO DE BANCO BCRA */
	 if( strncmp( (PSTR)ptrDPS, BCRA_ID_DEFAULT, 3) == 0)
	 {
		 if ( DPS_B_TIPO_REGISTRO_FOOTER != ptrDPS->TIPOREG )
		 {
			memmove( ptrDPS->BANDEB, ptrDPS, sizeof(DPS_i_batch::data_tag)-1);
		 }
	 }

	 /* FIX : precondicion : registro valido valido */
	 if( DPS_B_TIPO_REGISTRO_HEADER != ptrDPS->TIPOREG 
		 &&
		 DPS_B_TIPO_REGISTRO_BODY   != ptrDPS->TIPOREG 
		 &&
		 DPS_B_TIPO_REGISTRO_FOOTER != ptrDPS->TIPOREG 
		 )
	 {
		/* error de datos */
		return (-1);
	 }

	 /* FIX : precondicion : banco valido */
	 if(strncmp(ptrDPS->BANDEB,BCRA_ID_DEFAULT,3)       !=0   // BANCO DEBITO NO ES PARA EL QUE SE COMPILO
		 &&
		strncmp(ptrDPS->DATCRE.BANCRE,BCRA_ID_DEFAULT,3)!=0 ) // BANCO CREDITO TAMPOCO ES PARA EL QUE SE COMPILO
	 {
		 if ( DPS_B_TIPO_REGISTRO_FOOTER != ptrDPS->TIPOREG )
		 {
			/* error de datos */
			return (-1);
		 }
	 }

     /* precondicion */
     if(!( (chBuffer && iBuffer > 0 )
           && 
           ( chConvert && iConvert > 0 )
           &&
           nISO_ACCT_LEN,nDTN_ACCT_LEN > 0 
           &&
           nDTN_ACCT_LEN,nDTN_ACCT_LEN > 0 
           &&
           ( 'R' == chTrunc_LEFT_RIGHT || 'L' == chTrunc_LEFT_RIGHT )
         )
        )
	 {
        /* error de parametros */
        return (-2);
	 }

	 if ( DPS_B_TIPO_REGISTRO_HEADER == ptrDPS->TIPOREG)
	 {
		 strcpy(fld43.stCrdAccptNameLoc.chOwner,"HEADER DE TRANSFERENCIA"); // DATOS DE TERMINAL GENERICA
		 strcpy(fld43.stCrdAccptNameLoc.chCity,"               ");
		 strcpy(fld43.stCrdAccptNameLoc.chState,"   ");
	 	 strcpy(fld43.stCrdAccptNameLoc.chCountry,"  ");
		 memcpy(fld43.stCrdAccptNameLoc.chFiller,"--------",sizeof fld43.stCrdAccptNameLoc.chFiller);
	 }
	 else if (DPS_B_TIPO_REGISTRO_FOOTER == ptrDPS->TIPOREG )
	 {
		 		 strcpy(fld43.stCrdAccptNameLoc.chOwner,"FOOTER DE TRANSFERENCIA"); // DATOS DE TERMINAL GENERICA
		 strcpy(fld43.stCrdAccptNameLoc.chCity,"               ");
		 strcpy(fld43.stCrdAccptNameLoc.chState,"   ");
	 	 strcpy(fld43.stCrdAccptNameLoc.chCountry,"  ");
		 memcpy(fld43.stCrdAccptNameLoc.chFiller,"--------",sizeof fld43.stCrdAccptNameLoc.chFiller);
	 }
	 else
	 {
		 strcpy(fld43.stCrdAccptNameLoc.chOwner,"DPS24-GENERIC-PROCESS  "); // DATOS DE TERMINAL GENERICA
		 strcpy(fld43.stCrdAccptNameLoc.chCity,"BUENOS AIRES   ");
		 strcpy(fld43.stCrdAccptNameLoc.chState,"BUE");
	 	 strcpy(fld43.stCrdAccptNameLoc.chCountry,"AR");
		 memcpy(fld43.stCrdAccptNameLoc.chFiller,"--------",sizeof fld43.stCrdAccptNameLoc.chFiller);
	 }
	 /* campo 54 */
	 memset( &fld54, ' ', sizeof(fld54));
	 strcpy(fld54.stB24_0200_Def_Data.chAMT, "000000000000");	 
	 fld54.stB24_0200_Def_Data.chTIPO_DEP = ' ';
	 strcpy(fld54.stB24_0200_Def_Data.chINVOICES_QTY, "01");
	 strcpy(fld54.stB24_0200_Def_Data.chPIN_1, "FFFF");
	 strcpy(fld54.stB24_0200_Def_Data.chPIN_2, "FFFF");
	 strcpy(fld54.stB24_0200_Def_Data.chFILLER, "----------");

	 /* campo 55 */
	 memset( &fld55, ' ', sizeof(fld55));
	 // TRACK-2 : MOSTRAR TARJETA ADMINISTRATIVA : TODOS '9' 
	 if ( DPS_B_TIPO_REGISTRO_HEADER == ptrDPS->TIPOREG)
	 {
		 strncpy(fld55.stInterbancarias.chTrack2, "HEADERTRANS;9999999999999999", strlen("DPS-GENERIC;9999999999999999") );
	 }
	 else if (DPS_B_TIPO_REGISTRO_FOOTER == ptrDPS->TIPOREG )
	 {
		 strncpy(fld55.stInterbancarias.chTrack2, "FOOTERTRANS;9999999999999999", strlen("DPS-GENERIC;9999999999999999") );
	 }
	 else
	 {
		strncpy(fld55.stInterbancarias.chTrack2, "DPS-GENERIC;9999999999999999", strlen("DPS-GENERIC;9999999999999999") );
	 }

	 strncpy(fld55.stInterbancarias.chCA, "00", 2);
  	 strnset(fld55.stInterbancarias.stFrom_Acct.chAcct_num, ' ', sizeof fld55.stInterbancarias.stFrom_Acct.chAcct_num );
	 strnset(fld55.stInterbancarias.stTo_Acct.chAcct_num  , ' ', sizeof fld55.stInterbancarias.stFrom_Acct.chAcct_num );
	 fld55.stInterbancarias.filler[0] = 0x00;

	 /* field 60 - TERMINAL DATA */
     fld60.stATMTerm.chTermFIID[0]       = '0';
	 strncpy(fld60.stATMTerm.chTermFIID+1, ptrDPS->BANDEB, 3); // BANCO DEBITO = BANCO DEL ABONADO
	 strncpy(fld60.stATMTerm.chTermLNet  , "DPS ", 4); // LOGICAL NET ATM : DATANET-INTERBANKING
	 strncpy(fld60.stATMTerm.chTermTimeOffset, "-003", 4); // GMT-3
	 fld60.stATMTerm.chReserved[0]=0x00;

	 /* field 61 - CARD DATA */
     fld61.stATM_Data.chCrdFIID[0]       = '0'; // ID DEL BANCO DEL ATM : BANCO QUE DEBE APLICAR DB/CR
	 if(strncmp(ptrDPS->BANDEB,BCRA_ID_DEFAULT,3)==0)
		strncpy(fld61.stATM_Data.chCrdFIID+1, ptrDPS->BANDEB, 3); // BANCO DEBITO
	 else
		strncpy(fld61.stATM_Data.chCrdFIID+1, ptrDPS->DATCRE.BANCRE, 3); // BANCO CREDITO
	 fld61.stATM_Data.chCrdLNet[0]       = '0';
	 strncpy(fld61.stATM_Data.chCrdLNet, "DPS ", 4); // LOGICAL NET ATM : DATANET-INTERBANKING	 
	 strncpy(fld61.stATM_Data.chSaveAcctType, "DNET", 4);
	 fld61.stATM_Data.chAutorizer = '*';// Autorizador = NO CORRESPONDE
	 strcpy(fld61.stATM_Data.chOrigTranCde , "**");

	 /* campo 126 */
	 memset( &fld126, '0', sizeof(fld126));
	 strncpy(fld126.stCreditCardAcct.CANT_CTA,"01",2);   // SIN USO , TARJ.CREDITO
     fld126.stCreditCardAcct.uRedefTipo.TIPO_TRAN = '1'; // SIN USO , TARJ.CREDITO
	 fld126.stCreditCardAcct.SERVICE_CDE[0] = 0x00;

	 /* campo 32 */
	 // 2013.08.15 - JAF - Nro de referencia a Field32
	 memset(&fld32, '0', sizeof(fld32));
	 //ASSIGNFIELD(fld32.chNum, ptrDPS->NUMREF);
	 strncpy(fld32.chNum, ptrDPS->NUMREF, 7);

	 // Copia interna del buffer externo
	 this->data = (*ptrDPS);

	 // Formateo ISO 
	 isoMsg.InitMessage();

	 //
	 // ARMADO DE MENSAJE ISO : PRIMARY-KEY : TERMINAL+NUMERO_TRANSFERENCIA+FECHA_NEGOCIO_+HORA no debe duplicar
	 // el impacto aunque el proceso BATCH se ejecuta mas de 1 vez por error.
	 //
	 if( 'D' == chDB_CR_OPTION || 'C' == chDB_CR_OPTION )
	 {
		isoMsg.SetMsgTypeId( (PBYTE)CISO_MSGTYPE_FINANCIAL_ADV, 4); // TRANSACCION FORZADA 
	 }
	 else if( 'S' == chDB_CR_OPTION || 'R' == chDB_CR_OPTION )
	 {
		if( 'S' == chDB_CR_OPTION  ) /* S=REVERSA DEBITOS, R=REVERSA CREDITOS */
			chDB_CR_OPTION = 'D';
		else
			chDB_CR_OPTION = 'C';
		isoMsg.SetMsgTypeId( (PBYTE)CISO_MSGTYPE_REVERSAL, 4); // TRANSACCION REVERSADA
	 }
	 else
	 {
		isoMsg.SetMsgTypeId( (PBYTE)CISO_MSGTYPE_FINANCIAL_ADV, 4); // TRANSACCION NORMAL
	 }
	 isoMsg.InsertField( 2, &(wLen = 19), (PBYTE)"9999999999999999   ", 19 ); // TARJETA ADMINISTRATIVA 

	 /* cargar tran-code segun sea BANCO DEBITO, o CREDITO, o AMBOS */
     if( IS_ON_OUR_BANKCODE(ptrDPS->BANDEB) )
	 {
		 if ( antoi(ptrDPS->DATDEB.TIPCUE,2 ) == DPS_CAJA_DE_AHORROS_ARG )
			 strOpcode = B24_SAVINGS_ACCOUNT ;
		 else if ( antoi(ptrDPS->DATDEB.TIPCUE,2 ) == DPS_CUENTA_CORRIENTE_ARG )
			 strOpcode = B24_CHECKINGS_ACCOUNT ;
		 else if ( antoi(ptrDPS->DATDEB.TIPCUE,2 ) == DPS_CUENTA_CORRIENTE_USD )
			 strOpcode = B24_CHECKINGS_ACCOUNT_USD ;
		 else if ( antoi(ptrDPS->DATDEB.TIPCUE,2 ) == DPS_CAJA_DE_AHORROS_USD )
			 strOpcode = B24_SAVINGS_ACCOUNT_USD ;
		 else
			 strOpcode = B24_SAVINGS_ACCOUNT ;

		 if ( IS_ON_OUR_BANKCODE(ptrDPS->DATCRE.BANCRE))
		 {
 		 	 if ( antoi(ptrDPS->DATCRE.TIPCRE,2 ) == DPS_CAJA_DE_AHORROS_ARG )
				 strOpcode += B24_SAVINGS_ACCOUNT ;
			 else if ( antoi(ptrDPS->DATCRE.TIPCRE,2 ) == DPS_CUENTA_CORRIENTE_ARG )
				strOpcode += B24_CHECKINGS_ACCOUNT ;
			 else if ( antoi(ptrDPS->DATCRE.TIPCRE,2 ) == DPS_CUENTA_CORRIENTE_USD )
				strOpcode += B24_CHECKINGS_ACCOUNT_USD ;
			 else if ( antoi(ptrDPS->DATCRE.TIPCRE,2 ) == DPS_CAJA_DE_AHORROS_USD )
				strOpcode += B24_SAVINGS_ACCOUNT_USD ;
			 else
				strOpcode += B24_SAVINGS_ACCOUNT ;

 			 strOpcode = _B24_OPCODE_TRANSFER_ + strOpcode;

		 }
		 else
		 {/* FIX */
			 if ( 'D' == chDB_CR_OPTION )
				strOpcode = _B24_OPCODE_DEBIT_ORDER_XFER_ + strOpcode + "00";
			 else if ( 'C' == chDB_CR_OPTION )
				strOpcode = _B24_OPCODE_CREDIT_ORDER_XFER_ "00" + strOpcode;
			 else
				 strOpcode = _B24_OPCODE_DEBIT_ORDER_XFER_ + strOpcode + "00";
			 /* FIX */
		 }
	     isoMsg.InsertField( 3, &(wLen = 6), (PBYTE)strOpcode.c_str() , 6);
	 }
     else
	 {
		 if ( antoi(ptrDPS->DATCRE.TIPCRE,2 ) == DPS_CAJA_DE_AHORROS_ARG )
			 strOpcode = B24_SAVINGS_ACCOUNT ;
		 else if ( antoi(ptrDPS->DATCRE.TIPCRE,2 ) == DPS_CUENTA_CORRIENTE_ARG )
			 strOpcode = B24_CHECKINGS_ACCOUNT ;
		 else if ( antoi(ptrDPS->DATCRE.TIPCRE,2 ) == DPS_CUENTA_CORRIENTE_USD )
			 strOpcode = B24_CHECKINGS_ACCOUNT_USD ;
		 else if ( antoi(ptrDPS->DATCRE.TIPCRE,2 ) == DPS_CAJA_DE_AHORROS_USD )
			 strOpcode = B24_SAVINGS_ACCOUNT_USD ;
		 else
			 strOpcode = B24_SAVINGS_ACCOUNT ;

		 strOpcode = _B24_OPCODE_CREDIT_ORDER_XFER_ "00" + strOpcode;
         isoMsg.InsertField( 3, &(wLen = 6), (PBYTE)strOpcode.c_str() , 6);
	 }
	 isoMsg.InsertField( 4, &(wLen = 12), (PBYTE)ptrDPS->IMPORTE + (DPS_MAX_AMMOUNT_LENGTH-12), 12  );
	 isoMsg.InsertField( 5, &(wLen = 12), (PBYTE)"000000000000", 12 ); // HORA DE TRANSMISION : CERO HORAS.
	 isoMsg.InsertField( 6, &(wLen = 12), (PBYTE)"000000000000", 12 ); // HORA DE TRANSMISION : CERO HORAS.
	 isoMsg.InsertField( 7, &(wLen = 10), (PBYTE)ptrDPS->DATDEB.FSENDB+2, 10 ); // Excluido el año, mmdd
	 isoMsg.InsertField( 10, &(wLen = 10), (PBYTE)"00000000", 8 );     // HORA DE TRANSMISION : CERO HORAS.
  	 isoMsg.InsertField( 11, &(wLen = 7), (PBYTE)ptrDPS->NUMTRA, 7 );  // trace-num = NUMERO DE TRANSFERENCIA
	 isoMsg.InsertField( 12, &(wLen = 6), (PBYTE)ptrDPS->DATDEB.FSENDB, 6 ); // hhmmss
	 isoMsg.InsertField( 13, &(wLen = 4), (PBYTE)ptrDPS->FECSOL+4, 4 ); // mmdd
	 isoMsg.InsertField( 15, &(wLen = 4), (PBYTE)ptrDPS->FECSOL+4, 4 ); // mmdd
	 isoMsg.InsertField( 17, &(wLen = 4), (PBYTE)ptrDPS->FECSOL+4, 4 ); // mmdd
	 isoMsg.InsertField( 19, &(wLen = 3), (PBYTE)"032", 3 ); // MONEDA PESOS
	 isoMsg.InsertField( 24, &(wLen = 4), (PBYTE)fld60.stATMTerm.chTermFIID, 4 );
	 isoMsg.InsertField( 25, &(wLen = 1), (PBYTE)ptrDPS->CONSOL, 1);
	 
	 //isoMsg.InsertField( 32, &(wLen = 4), (PBYTE)fld60.stATMTerm.chTermLNet, 4 );
	 isoMsg.InsertField( 32, &(wLen = 7), (PBYTE)ptrDPS->NUMREF, 7);

	 isoMsg.InsertField( 34, &(wLen = sizeof FIELD34), (PBYTE)fld55.stInterbancarias.chTrack2, sizeof FIELD34 );
	 
	 memcpy( fld35.stTrack2.chData, fld55.stInterbancarias.chTrack2, sizeof fld35.stTrack2.chData);
	 isoMsg.InsertField( 35, &(wLen = sizeof fld35.stTrack2.chData), (PBYTE)fld35.stTrack2.chData, 
									  sizeof fld35.stTrack2.chData );
	 isoMsg.InsertField( 36, &(wLen = 30), (PBYTE)fld55.stInterbancarias.chTrack2, 30 );

	 memset( fld37.chNum, ' ', sizeof FIELD37);
     memcpy( fld37.chNum, ptrDPS->NUMTRA, sizeof ptrDPS->NUMTRA);
	 isoMsg.InsertField( 37, &(wLen = sizeof FIELD37), (PBYTE)fld37.chNum, sizeof FIELD37);  // NUMERO DE TRANSFERENCIA	 

     //////////////////////////////////////////////////////
     // TERMINAL GENERICA EN FUNCION DEL TIPO DE OPERACION
     strTipOpe.assign( ptrDPS->TIPOPE , 2);     
	 strOpcode  = "DPS-GENERIC-" ; // PREFIJO
     strOpcode += strTipOpe      ; // TIPO-OPERACION DNET ["01".."16"]
     // 2006.09.13 MDC /////////////////////////////////////////////////////
     // "S" o "N" si conciden o no los CUITS. + 1 FILLER.
     //if( strncmp(ptrDPS->CUITDEB, ptrDPS->CUITCRE, sizeof(ptrDPS->CUITCRE)) ) 
     //   strOpcode += "N " ;        // "N" + 1 FILLER
     //else
     //   strOpcode += "S " ;        // "S" + 1 FILLER         
     // 2006.09.13 MDC /////////////////////////////////////////////////////
     //FML 2017-09-21 Los cornudos estos quieren que mande hardcodeada una N porque sino duplican cosas
	 strOpcode += "N ";
     // Inserta el ID de TERMINAL
     isoMsg.InsertField( 41, &(wLen = sizeof(FIELD41)), (PBYTE)strOpcode.c_str(), strlen(strOpcode.c_str()) ); 
	 memset( &fld44, '0', sizeof FIELD44);
	 isoMsg.InsertField( 44, &(wLen = sizeof FIELD44), (PBYTE)&fld44, sizeof FIELD44 );
	 isoMsg.InsertField( 49, &(wLen = 3), (PBYTE)"032", 3 ); // MONEDA PESOS
	 isoMsg.InsertField( 52, &(wLen = 16), (PBYTE)"FFFFFFFFFFFFFFFF", 16 ); // OFFSET DE CLAVE : NO CORRESPONDE
	 isoMsg.InsertField( 54, &(wLen = strlen(fld54.chData)), (PBYTE)&fld54, strlen(fld54.chData) );
	 isoMsg.InsertField( 55, &(wLen = strlen(fld55.chData)), (PBYTE)&fld55, strlen(fld55.chData) );

	 isoMsg.InsertField( 60, &(wLen = strlen(fld60.chData)), (PBYTE)&fld60, strlen(fld60.chData) );
	 isoMsg.InsertField( 61, &(wLen = strlen(fld61.chData)), (PBYTE)&fld61, strlen(fld61.chData) );

	 // ******************************************************************************
	 // Solo en caso que difieran los PICTURE usados para una red ISO8583 (LINK) 
     // de los usados para la red DTN, deberan ajustarse los mismos : Se usan metodos
     // sobrecargados, implementados en la clase derivada , en cada proyecto
     // ******************************************************************************
     // CUENTA DEBITO 
     strAcctNum = ptrDPS->DATDEB.NUMCTA ;
     strAcctNum.resize( max(nISO_ACCT_LEN,max(nDTN_ACCT_LEN,17)) ); /* 17=maximo DTN*/     
     if( IS_ON_OUR_BANKCODE(ptrDPS->BANDEB) )
     {
		 /***/
		 if( nISO_ACCT_LEN > nDTN_ACCT_LEN )
		 {
			 strAcctNum = "0" + strAcctNum ;
		 }
		 /***/
         if( 'L' == chTrunc_LEFT_RIGHT )
            strAcctNum = strAcctNum.substr( max(nISO_ACCT_LEN,nDTN_ACCT_LEN)
                                            -
                                            min(nISO_ACCT_LEN,nDTN_ACCT_LEN)
                                            , 
                                            strAcctNum.length() );     
         else
            strAcctNum = strAcctNum.substr( 0 , 
                                            strAcctNum.length() 
                                            -
                                            ( max(nISO_ACCT_LEN,nDTN_ACCT_LEN)
                                              -
                                              min(nISO_ACCT_LEN,nDTN_ACCT_LEN) )
                                            );     
     };
     /****************************************************************************************/
     /*isoMsg.InsertField( 102, &(wLen = 17), (PBYTE)strAcctNum.c_str(), 17 );               */
     /* fix para que no contenga un NULL el nro. de cuenta al insertarse en su long. precisa */
     /****************************************************************************************/
     /*                  ....:....1....:....2....:... - 28=maximo ISO                        */
     strAcctNum.append( "                            " ); 
     isoMsg.InsertField( 102, &(wLen = sizeof(FIELD102)), (PBYTE)strAcctNum.c_str(), sizeof(FIELD102) );
     // ****************************************************************************** //
     
     // ****************************************************************************** //
     // CUENTA CREDITO
     strAcctNum = ptrDPS->DATCRE.CTACRE ;     
     strAcctNum.resize( max(nISO_ACCT_LEN,max(nDTN_ACCT_LEN,17)) ); /* 17=maximo DTN*/     
     if( IS_ON_OUR_BANKCODE(ptrDPS->DATCRE.BANCRE) ) 
     {
		 /***/
		 if( nISO_ACCT_LEN > nDTN_ACCT_LEN )
		 {
			 strAcctNum = "0" + strAcctNum ;
		 }
         if( 'L' == chTrunc_LEFT_RIGHT )
            strAcctNum = strAcctNum.substr( max(nISO_ACCT_LEN,nDTN_ACCT_LEN)
                                            -
                                            min(nISO_ACCT_LEN,nDTN_ACCT_LEN)
                                            , 
                                            strAcctNum.length() );     
         else
            strAcctNum = strAcctNum.substr( 0 , 
                                            strAcctNum.length() 
                                            -
                                            ( max(nISO_ACCT_LEN,nDTN_ACCT_LEN)
                                              -
                                              min(nISO_ACCT_LEN,nDTN_ACCT_LEN) )
                                            );     
     };
     /****************************************************************************************/
	 /*isoMsg.InsertField( 103, &(wLen = 17), (PBYTE)strAcctNum.c_str(), 17 );               */
     /* fix para que no contenga un NULL el nro. de cuenta al insertarse en su long. precisa */     
     /****************************************************************************************/
     /*                  ....:....1....:....2....:... - 28=maximo ISO                        */
     strAcctNum.append( "                            " ); 
     isoMsg.InsertField( 103, &(wLen = sizeof(FIELD103)), (PBYTE)strAcctNum.c_str(), sizeof(FIELD103) );
	 // ****************************************************************************** //

	 isoMsg.InsertField( 126, &(wLen = strlen(fld126.chData)), (PBYTE)&fld126, 
		 min(sizeof(fld126.stCreditCardAcct),strlen(fld126.chData)) );

	 if(DPS_B_TIPO_REGISTRO_BODY == ptrDPS->TIPOREG )
	 {
		 std::string field125, fCuitDB(ptrDPS->CUITDEB), fCuitCR(ptrDPS->CUITCRE), fNomben(ptrDPS->DATCRE.NOMBEN), fNomsol(ptrDPS->DATDEB.NOMSOL);
		 field125 = fCuitDB.substr(0,11);
		 field125 += fCuitCR.substr(0,11);
		 field125 += fNomsol.substr(0,24);
		 field125 += fNomben.substr(0,24);
		 //isoMsg.InsertField( 125, &(wLen = field125.length()), (PBYTE)field125.c_str(), 372);
		 isoMsg.InsertField( 125, &(wLen = field125.length()), (PBYTE)field125.c_str(), field125.length());
	 }
	 else
	 {
		 std::string field127, fNomben(ptrDPS->DATCRE.NOMBEN), fNomsol(ptrDPS->DATDEB.NOMSOL);
		 field127 = fNomsol.substr(0,29);
		 field127 += fNomben.substr(0,29);
		 isoMsg.InsertField( 127, &(wLen = field127.length()), (PBYTE)field127.c_str(), sizeof(field127.c_str()));
	 }

    // Exportacion de mensaje interno ISO hacia afuera de la funcion
    wExport = iConvert;
    isoMsg.Export( (PBYTE)chConvert , &wExport);
    iConvert = wExport ;

	return 0;

};

///////////////////////////////////////////////////////////////////////
// Controles de trasacciones en general
WORD  DPS_i_batch::DPRE_ProcessInit(char chBuffer[], int iBuffer, boolean_t bIsAuth,	// Proceso base, validacion		
									PSTR strDatabase, PSTR strDBUser, PSTR strDBbPassword,
									PWORD pwProcRespCde , PBYTE pbMAC_Used)
{	
	ODBC_CONNECTOR_DNET      odbcDNET( strDBUser, strDBbPassword);
	DPS_in_out				 dpsMsg;
    // FIX a la forma de referenciar el ptr a buffer de mensaje BATCH
    struct DPS_i_batch::data_tag *ptrDPS = (DPS_i_batch::data_tag *)chBuffer;
    // FIX
	TrxResBATCH				 trxDNET( &odbcDNET );
	WORD					 wResult = 0;
	e_codestad_dps_batch     dpsState = DPS_B_ENVIADO_POR_LA_RED;

	odbcDNET.ConnectDataBase( strDatabase, strDBUser, strDBbPassword);

	// Importar mensaje BATCH como si fuera ONLINE
    data = (*ptrDPS);
	dpsMsg.Import( (PBYTE)chBuffer, iBuffer);

	// Copiarlo al atributo de la clase de resolucion DNET;	
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.header.TPNAME   ,"DPRE");//Nombre TP emisor	ej."DPRE" : Datanet
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.header.NDNAME   ,"DNET");//Nombre nodo emisor	ej."DNET" : Datanet
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.header.HORA     ,"0000");
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.header.SECUENCIA,"00");
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.FRNAME,"DNET");//Nombre del nodo originante DNET
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.TONAME,"GENE");//Nombre nodo receptor	ej."BSUD" : Bansud

	// Controlo si es header o footer y le asigo el que sea. 2013.08.26
	if ( IS_ON_OUR_BANKCODE(ptrDPS->BANDEB) )
	{
		EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.RECTYPE,"TDE");	//Tipo de registro	TDE = debito TCE = credito		
	}
	else
	{
		EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.RECTYPE,"TCE");	//Tipo de registro	TDE = debito TCE = credito
	}


	if ( DPS_B_TIPO_REGISTRO_HEADER == ptrDPS->TIPOREG )
	{
		EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.RECTYPE,"HEA")	;
	}
	else if ( DPS_B_TIPO_REGISTRO_FOOTER == ptrDPS->TIPOREG )
	{
		EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.RECTYPE,"FOO");
	}

	trxDNET.dpsMsg.input.data.ACTIVITY[0] = 'N';
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.COUNTER, "FF");
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.FILLER1, "000000");
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.BANDEB , ptrDPS->BANDEB);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.FECSOL , ptrDPS->FECSOL);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.NUMTRA , ptrDPS->NUMTRA);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.NUMABO , ptrDPS->NUMABO);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.TIPOPE , ptrDPS->TIPOPE);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.IMPORTE, ptrDPS->IMPORTE);
	COPYSTRUCT(trxDNET.dpsMsg.input.data.DATDEB , ptrDPS->DATDEB);
	COPYSTRUCT(trxDNET.dpsMsg.input.data.DATCRE , ptrDPS->DATCRE);

	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.OPECON, ptrDPS->OPECON);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.OPEAU1, ptrDPS->OPEAU1);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.OPEAU2, ptrDPS->OPEAU2);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.OPEAU3, ptrDPS->OPEAU3);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.FECAUT, ptrDPS->FECAUT);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.HORAUT, ptrDPS->HORAUT);
	
	// ESTADO al registro y a la variable local de soporte
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.ESTADO, ptrDPS->ESTADO);
	dpsState = (e_codestad_dps_batch)antoi( ptrDPS->ESTADO, sizeof ptrDPS->ESTADO );
	
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.FECEST, ptrDPS->FECEST);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.OBSER1, ptrDPS->OBSER1);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.OBSER2, ptrDPS->OBSER2);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Segun el ESTADO de envio, y a veces el RIESGO BANCO, se verifica o no el MAC-2 o 3
	// 20 --> Demorada bco de debito         ==> se verifica el mac 2 (mac puesto por el banco de débito).
	// 40 --> Pendiente de crédito           ==> se verifica el mac 2 (mac puesto por el banco de débito).
	// 50 --> Enviada al bco de credito      ==> se verifica el mac 2 (mac puesto por el banco de débito).
	// 70 --> Rechazo Banco de debito        ==> se verifica el mac 2 (mac puesto por el banco de débito).	
	// 60  +  Riesgo Banco = ' '   --> Ejecutada ==> se verifica el mac 2 (mac puesto por el banco de débito).	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(  DPS_B_DEMORADA_POR_EL_BANCO_DE_DEBITO == dpsState ||
		 DPS_B_VALOR_AL_COBRO                  == dpsState ||
		 DPS_B_ENVIADA_AL_BANCO_DE_CREDITO	   == dpsState ||
		 DPS_B_RECHAZO_DEL_BANCO_DE_DEBITO     == dpsState ||
		 ( DPS_B_EJECUTADA == dpsState  && 	' ' == ptrDPS->RIEBCO[0] ) 
		)
	{
	  EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.CODMAC , ptrDPS->CODMAC3);
	  BLANKFIELD(trxDNET.dpsMsg.input.data.FILLER2);
	  COPYFIELD(trxDNET.dpsMsg.input.data.FILLER2, ptrDPS->CODMAC2);
	}
	// 80 --> Rechazo Banco de crédito       ==> se verifica el mac 3 (mac puesto por el banco de crédito).
	// 60  +  Riesgo Banco <> ' '  --> Ejecutada ==> se verifica el mac 3 (mac puesto por el banco de crédito).
	else if( DPS_B_RECHAZO_DEL_BANCO_DE_CREDITO == dpsState 
		     ||
			(DPS_B_EJECUTADA == dpsState  && ' ' != ptrDPS->RIEBCO[0])
		   )
	{
	  EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.CODMAC , ptrDPS->CODMAC3);
	  BLANKFIELD(trxDNET.dpsMsg.input.data.FILLER2);
	  COPYFIELD(trxDNET.dpsMsg.input.data.FILLER2, ptrDPS->CODMAC2);
	}
	// Sino.... MAC-3 por defecto
	else
	{
	  EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.CODMAC , ptrDPS->CODMAC3);
	  BLANKFIELD(trxDNET.dpsMsg.input.data.FILLER2);
	  COPYFIELD(trxDNET.dpsMsg.input.data.FILLER2, ptrDPS->CODMAC2);
	}
	/*********************************************************************/
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.NUMREF, ptrDPS->NUMREF);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.NUMENV, ptrDPS->NUMENV);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.CONSOL, ptrDPS->CONSOL);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.MARTIT, ptrDPS->MARTIT);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.PRIVEZ, ptrDPS->PRIVEZ);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.RIEABO, ptrDPS->RIEABO);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.RIEBCO, ptrDPS->RIEBCO);
	
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.CUITDEB, ptrDPS->CUITDEB);
	EXACTCOPYFIELD(trxDNET.dpsMsg.input.data.CUITCRE, ptrDPS->CUITCRE);

	COPYSTRUCT(trxDNET.dpsMsg.input.data.TABEST , ptrDPS->TABEST);

	// Prevalidar todas las condiciones iniciales (controles 1 al 8)   
	// Si es header o footer no hacer controles. 2013.08.26
	if ( DPS_B_TIPO_REGISTRO_HEADER == ptrDPS->TIPOREG 
		 ||
		 DPS_B_TIPO_REGISTRO_FOOTER == ptrDPS->TIPOREG 
		)
	{
		return ( wResult = DPS_APROBADA_SIN_RECHAZO );
	}
	else 
	{
		wResult = trxDNET.ProcessInit( pbMAC_Used );
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Segun el ESTADO de envio, y a veces el RIESGO BANCO, se verifica o no el MAC-2 o 3
	//////////////////////////////////////////////////////////////////////////////////////
	// 00 --> Recibida por la red            ==> no se verifica el mac (no fueron tratadas por los bancos).
	// 10 --> Enviada al bco de débito       ==> no se verifica el mac (no fueron tratadas por los bancos).
	// 15 --> Pendiente autoriz. cta credito ==> no se verifica el mac (no fueron tratadas por los bancos). 
	// 20 --> Demorada bco de debito         ==> se verifica el mac 2 (mac puesto por el banco de débito).
	// 30 --> Tratamiento interrumpido       ==> no se verifica el mac (no fueron tratadas por los bancos).
	//////////////////////////////////////////////////////////////////////////////////////	
	if( DPS_B_ENVIADO_POR_LA_RED				   == dpsState ||
		DPS_B_ENVIADA_AL_BANCO_DE_DEBITO		   == dpsState ||
		DPS_B_PENDIENTE_AUTORIZ_CTA_CREDITO		   == dpsState ||
		DPS_B_DEMORADA_POR_EL_BANCO_DE_DEBITO      == dpsState ||
		DPS_B_REVERSO_DEL_DEBITO_EXCEDE_RIESGO_RED == dpsState )
	{
		(*pwProcRespCde) = trxDNET.TransactionStatus();
		return ( wResult = DPS_NO_EXISTEN_RESPUESTAS_HACIA_LA_RED );
	}
	// 70 --> Rechazo Banco de debito        ==> se verifica el mac 2 (mac puesto por el banco de débito).
	// 80 --> Rechazo Banco de crédito       ==> se verifica el mac 3 (mac puesto por el banco de crédito).
	// 90 --> Rechazo cta credito            ==> no se verifica el mac (no fueron tratadas por los bancos).  
	else if( DPS_B_RECHAZO_DEL_BANCO_DE_DEBITO  == dpsState ||
			 DPS_B_RECHAZO_DEL_BANCO_DE_CREDITO == dpsState ||
			 DPS_B_RECHAZO_CUENTA_CREDITO       == dpsState )  
	{
		(*pwProcRespCde) = trxDNET.TransactionStatus();
		return ( wResult = DPS_PASADA_A_PROCESO_BATCH );
	}
	// 40 --> Pendiente de crédito           ==> se verifica el mac 2 (mac puesto por el banco de débito).
	// 50 --> Enviada al bco de credito      ==> se verifica el mac 2 (mac puesto por el banco de débito).
	// 60  +  Riesgo Banco = ' '   --> Ejecutada ==> se verifica el mac 2 (mac puesto por el banco de débito).
	// 60  +  Riesgo Banco <> ' '  --> Ejecutada ==> se verifica el mac 3 (mac puesto por el banco de crédito).
	else if ( DPS_B_VALOR_AL_COBRO              == dpsState ||
			  DPS_B_ENVIADA_AL_BANCO_DE_CREDITO	== dpsState ||
			  DPS_B_EJECUTADA                   == dpsState )
	{
		(*pwProcRespCde) = trxDNET.TransactionStatus();
		return ( wResult = DPS_APROBADA_SIN_RECHAZO );
	}
	// Default PROC-INIT
	else
	{		
		(*pwProcRespCde) = trxDNET.TransactionStatus();
		return ( wResult );
	};
}

///////////////////////////////////////////////////////////////////////
// Authorizar mensaje ISO8583, ya conver	tido
///////////////////////////////////////////////////////////////////////
int DPS_i_batch::ISO8583_Authorize( char pcbBuffer[] , PWORD pwRespCde)
{
	APPMSG          isoMsg;
	boolean_t       bProcResult = is_false;
	enumFormatType  efFormatMsg = CNTL_FORMAT_ISO8583; // Formato de mensaje actual
	INT				iBuffLen = ( pcbBuffer ) 
								? strlen( (PCSTR)pcbBuffer ) 
								: 0;
    WORD			wRespCodeLen = 2;
    ///////////////////////////////////////////////////////////////////

    // Precondicion: Mensaje ISO-8583 valido
    // Import ISO-8583 Message
    if(!isoMsg.Import( (PBYTE)pcbBuffer, iBuffLen))
        // Error
        return (FALSE);
        
    // Is Valid Message?
    if(!isoMsg.IsValidMessage())
        // Error
        return (FALSE);

    if( isoMsg.IsATMTransaction() || !isoMsg.IsATMTransaction() ) // POS? - ATM?
    {
        //////////////////////////////////////////////////////////
        // Clase especializada de manejo ISO8583 segun backoffice
#if defined( _SYSTEM_ATM_TDFUEGO_ ) 
		TrxResTDFUEGO	*pTrxRes = new TrxResTDFUEGO ( strSrvIpAddress, ushSrvIpPort );
#elif defined( _SYSTEM_ATM_FINANSUR_ ) 
		TrxResFINAN	*pTrxRes = new TrxResFINAN ( );
#elif defined( _SYSTEM_ATM_ ) 
		TrxResolution	*pTrxRes = new TrxResolution ( );
#else
		TrxResATM_POS   *pTrxRes = new TrxResATM_POS ( );        
#endif
		//////////////////////////////////////////////////////////

        // Procesar si el host esta ONLINE y DISPONIBLE
        if( pTrxRes )
        {
            // Assign the MSG
			pTrxRes->SetISOMessage( (PBYTE)pcbBuffer, iBuffLen );        
			// Process the MSG
            bProcResult = pTrxRes->ProcessTransaction();

			// Processed?
			if( bProcResult )			
			{
				/* FIX Result? */				
				INT  iRc    = pTrxRes->TransactionStatus();
				CHAR szRc[2+1] = {"00"};				
				//PADCOPYINTEGER_I( szRc, iRc );
				sprintf(szRc,"%02i" , iRc );
				/* FIX Result? */
				wRespCodeLen = 2;
				/* FIX Result? */
				isoMsg.SetField( 39, &wRespCodeLen, (PBYTE)szRc, wRespCodeLen);
				/* Map Responde Codes */
				/* fix defaults BTFuego */
				if (  1 == iRc )
					(*pwRespCde) = DPS_APROBADA_SIN_RECHAZO ;
				else if (  2 == iRc )
					(*pwRespCde) = DPS_OPERACION_NO_HABILITADA_EN_BANCO ;
				else if (  3 == iRc )
					(*pwRespCde) = DPS_SALDO_INSUFICIENTE ;
				else if (  4 == iRc )
					(*pwRespCde) = DPS_CUENTA_INVALIDA ;
				else if (  5 == iRc )
					(*pwRespCde) = DPS_CUENTA_BLOQUEADA ;
				else if (  6 == iRc )
					(*pwRespCde) = DPS_CUENTA_EMBARGADA ;
				else if (  7 == iRc )
					(*pwRespCde) = DPS_CUENTA_EMBARGADA ;
				else if (  8 == iRc )
					(*pwRespCde) = DPS_TIPO_DE_CUENTA_INVALIDA ;
				else if (  9 == iRc )
					(*pwRespCde) = DPS_CUENTA_INVALIDA ;
				else if ( 10 == iRc )
					(*pwRespCde) = DPS_SALDO_INSUFICIENTE ;
				else if ( 11 == iRc )
					(*pwRespCde) = DPS_CUENTA_INVALIDA ;
				else if ( 12 == iRc )
					(*pwRespCde) = DPS_CUENTA_BLOQUEADA ;
				else if ( 13 == iRc )
					(*pwRespCde) = DPS_CUENTA_EMBARGADA ;
				else if ( 14 == iRc )
					(*pwRespCde) = DPS_CUENTA_EMBARGADA ;
				else if ( 15 == iRc )
					(*pwRespCde) = DPS_CUENTA_INVALIDA ;
				else if ( 16 == iRc )
					(*pwRespCde) = DPS_OPERACION_INEXISTENTE ;
				else if ( 17 == iRc )
					(*pwRespCde) = DPS_FUERZA_MAYOR;
				else if ( 19 == iRc )
					(*pwRespCde) = DPS_CUENTA_INVALIDA ;
				/* faltan mas */
				/* fix defaults BTFuego */
				else if ( CISO_APPROVED_I == iRc )
					(*pwRespCde) = DPS_APROBADA_SIN_RECHAZO;
				else if ( CISO_APPROVED_VIP_I == iRc )
					(*pwRespCde) = DPS_APROBADA_SIN_RECHAZO;
				else if ( CISO_EXTERNAL_DECLINE_I == iRc )
					(*pwRespCde) = DPS_NO_RESPONDE_CONSULTA_ESA_SUC;
				else if ( atoi(CISO_WITHDRAWAL_LIMIT_EXC)== iRc )
					(*pwRespCde) = DPS_SALDO_INSUFICIENTE;
				else if ( CISO_TRANSACTION_NOT_SUPP_I == iRc )
					(*pwRespCde) = DPS_MAC_ERRONEO;
				else if (  atoi(CISO_INVALID_ACCOUNT) == iRc )
					(*pwRespCde) = DPS_CUENTA_INVALIDA ;
				else 
					(*pwRespCde) = DPS_APLICACION_NO_DISPONIBLE;
				/* Map Responde Codes */
			}
			else
			{
				wRespCodeLen = 2;
				isoMsg.SetField( 39, &wRespCodeLen, (PBYTE)CISO_EXTERNAL_DECLINE, wRespCodeLen);
				(*pwRespCde) = CISO_EXTERNAL_DECLINE_I;
			}


			delete pTrxRes ;     
			pTrxRes = NULL;

        }
        else 
        {
            bProcResult = is_false; // error        
        };
      
    }
    else
    {
        // Invalid Transaction 

        ///////////////////////////////////////////////////////////////////////////////////////
        // Reversals of Balance Inquiry must be replied as APPROVED
        if(isoMsg.IsATMBalanceInquiry() && isoMsg.IsValidReversal())
        {
			wRespCodeLen = 2;
            isoMsg.SetField( 39, &wRespCodeLen, (PBYTE)CISO_APPROVED, wRespCodeLen);
            (*pwRespCde) = CISO_APPROVED_I;
        }
        ///////////////////////////////////////////////////////////////////////////////////////
        else if(!isoMsg.IsValidField(39))
        {
			wRespCodeLen = 2;
            isoMsg.InsertField( 39, &wRespCodeLen, (PBYTE)CISO_TRANSACTION_NOT_SUPP, wRespCodeLen);
            (*pwRespCde) = CISO_TRANSACTION_NOT_SUPP_I;
        }
        else
        {
			wRespCodeLen = 2;
            isoMsg.SetField( 39, &wRespCodeLen, (PBYTE)CISO_TRANSACTION_NOT_SUPP, wRespCodeLen);
            (*pwRespCde) = CISO_TRANSACTION_NOT_SUPP_I;
        };
        

        // Set Message Type Response { 0210,230,430 }
        if( !isoMsg.SetMsgTypeIdResponse() )
            return (FALSE);
        // Procesada, pero con error
        bProcResult = is_true;
    }; // end-if-IsTransaction...
    //////////////////////////////////////////////////////////////////////////////
    // Fin del Bloque discriminador de Tipo de Transaccion    
    //////////////////////////////////////////////////////////////////////////////

    
    // Pudo procesarse?
    if(!bProcResult)
        // Error
        return (FALSE);
    else    
		// Y retornar verdadero, por transaccion procesada (con error o sin error)
		return (TRUE);
};

////////////////////////////////////////////////////////////////////////////////////////////

#endif // _SYSTEM_DATANET_BATCH_	
