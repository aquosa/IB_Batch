/*------------------------------------------------------------------------------------*/
/* UMOVPBF - PROGRAMA DE CARGA MASIVA DE UMOV Y PBF A ESQUEMA DE BASE DATOS DTN       */
/*------------------------------------------------------------------------------------*/
/* 2006.10.20 , mdc. Para cargar los saldos y movimientos cuando una institucion      */
/* no posee ya una interfase on-line desde donde tomarlos, se re-utiliza lo q/tengan  */
/* para redes financieras como LINK o BANELCO (Base24-ATM) usando el PBF y el         */
/* UMOV . De alli se reconstruye el esquema a una base de datos propia, ya homologada */
/* en el primer cliente BATCH DATANET, y que de ahora en mas copiamos como propia.    */
/*------------------------------------------------------------------------------------*/
/* Las tablas a cargar son MAECUE, EXTCTA, NOVEDAD y CLIENTE.                         */
/*------------------------------------------------------------------------------------*/
// Tarea        Fecha        Autor Observaciones
// [ALFA]       2006.10.20   mdc   Para cargar los saldos y movimientos cuando una institucion 
// [ALFA]       2007.03.14   mdc   Tener en cuenta BASE DATOS por parametros, y 
//                                 igualar TIPCTA en NOVEDAD con el MAESTRO.
// [ALFA]       2007.03.27   mdc   _OFFSET_NUM_CUENTA_PBF_ , _OFFSET_NUM_CUENTA_UMOV_
// [ALFA]       2007.03.29   mdc   Parametricament MMDD como parte del nombre de entrada
// [ALFA]       2007.03.18   mdc   Tener en cuenta los ACUERDOS por PBF. Nuevas descripciones.
// [ALFA]       2007.03.19   mdc   Abreviaturas o Nros.conceptos en la DESCRIPCIOn de UMOV?   
// [ALFA]       2007.10.04   mdc   Agregada la estructura EXTRACT TLF solo con fines de parseo.
// [ALFA]       2007.10.30   mdc   bUMOV_USAR_ABREV_O_NRO = 'A'
// [ALFA]       2008.05.26   mdc   FIX HEADER vs. FECHA DEL DIA tomadas como diferidas
// [ALFA]       2008.09.08   mdc   FIX LOCALE's
// [ALFA]       2008.09.24   mdc   int convert_extract_ammount( char *szExtAmmount , const size_t cEXTR_AMT_LEN)
// [ALFA]       2009.04.20   mdc   CHECK ZEROES IN AMMOUNT 
// [BETA]       2011.04.30   mdc   Programa batch y libreria para programa visual.
// [BETA]       2011.06.21   mdc   Refresh CAF Link BE.
// [BETA]       2011.06.30   mdc   Tipo de Cuenta Corriente comun y especial (1 tipo, 2 sub-tipos)
// [BETA]       2011.11.04   mdc   Considerar 2 decimales implicitos. (por alguna razon, se perdieron...)
// [1.0.0.1]    2011.11.15   mdc   Considerar el SECUENCIAL de 25 bytes como nro de logg
// [1.0.0.2]    2012.02.10   mdc   Aumenta en 3 posiciones el campo importe.
// [1.0.0.3]    2012.07.02   mdc   Agrega detalles al archivo de trace/auditoria/trazabilidad
// [1.0.0.4]    2012.11.15   mdc   FIX : VALIDITY CONTROL : tipcta
// [1.0.0.5]    2012.12.11   mdc   FIX : REGIONAL SETTINGS "-" PARA SEPARADOR DE FECHA + EXCLUSION CONTROL
// (2.0.0.1)    2013.05.28   mdc   Fix Nro. Movimiento se agrega al UMOV, que ya no es el estandar////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/*--------------------------------------------------------------------------------------------------------*/


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <qusrinc/dpcsys.h> 
#include <qusrinc/typedefs.h>
#include <qusrinc/rodbc.h>
#include <qusrinc/extrctst.h>
#include <qusrinc/stdiol.h>
#include <qusrinc/pbfumov.h>
#include <fstream>
#include <sstream>

/* Libreria STD TEMPLATE STRING */
using namespace std;

/* Como los numeros de cuenta no son iguales en el PBF que en el UMOV, se offset-ea */
/* BANCO DE TIERRA DEL FUEGO no posee offsets desde donde cargar la cuenta .        */
#define  _OFFSET_NUM_CUENTA_PBF_   (0)
#define  _OFFSET_NUM_CUENTA_UMOV_  (0)


/********************************************************************/
/* ¿Abreviaturas o Codigos de Concepto?								*/
/********************************************************************/
#if defined( _SYSTEM_DATANET_TDFUEGO_ )
	#undef _UMOVPBF_USE_UMOV_DESCRIPCION_
	//#define _UMOVPBF_USE_UMOV_COD_CONCEP_		(TRUE)
	// JAF - 20151211 - No se utilizar definicion METRO - se deja la default de LINK
	//#define _UMOVPBF_USE_METRO_ 				(TRUE)
#elif defined( _SYSTEM_DATANET_CORRDNET_ )
	#undef _UMOVPBF_USE_UMOV_COD_CONCEP_
	#define _UMOVPBF_USE_UMOV_DESCRIPCION_		(TRUE)
#else
	#define _UMOVPBF_USE_UMOV_DESCRIPCION_ 		(TRUE)
	/********************************************************************/
	/* UMOV ESTANDAR DE RED LINK										*/
	/* UMOV NO ESTANDAR, AGREGA NRO.MOVIMIENTO INTERNO, NO SECUENCIA	*/
	//#define _UMOVPBF_USE_STANDARD_ 				(TRUE)				
	//#define _UMOVPBF_USE_METRO_ 				(TRUE)
#endif


/*---------------------------------------------------------------------------*/
/* Procesar_Batch_Umov_PBF:                                                  */
/*---------------------------------------------------------------------------*/
int Procesar_Batch_Umov_PBF(int argc, char * argv []) 
/*---------------------------------------------------------------------------*/
{
    rodbc           odbcSession;
    FILE            *hFile = NULL;
	/* fix TLF */
	const size_t    cnMaxBuffPBFUMOV = max(sizeof(uCMOVATM_LK),sizeof(uREFRESH_PBF));
	const size_t    cnMaxBuffer      = max(sizeof(msgEXTRACT),cnMaxBuffPBFUMOV);
	/* fix TLF */
	/*                                                 ....:....1....:....2....:....3 */
    char            szBuffer[ (cnMaxBuffer*2) + 1] = {"                              "},
                    szAux[32]                      = {"                              "};
	/* verlo como Refresh PBF , saldos */
    //uREFRESH_PBF    *pbfRecord  = (uREFRESH_PBF *)szBuffer;
	REFRESHPBFINTERF    *pbfRecord  = (REFRESHPBFINTERF *)szBuffer;

#ifdef _UMOVPBF_USE_STANDARD_
	/* verlo como UMOV estandar */
	uCMOVATM		*umovRecord = (uCMOVATM *)szBuffer;
	/* verlo como UMOV extendido al estandar */
    uCMOVATM_LK     *umovRecExt = (uCMOVATM_LK *)szBuffer;
#elif _UMOVPBF_USE_METRO_
	/* verlo como UMOV estandar */
	uCMOVATM_METRO	*umovRecord = (uCMOVATM_METRO *)szBuffer;
		/* verlo como UMOV extendido al estandar */
    uCMOVATM_LK     *umovRecExt = (uCMOVATM_LK *)szBuffer;
#else
	///* verlo como UMOV extendido al estandar */
	//uCMOVATM_LK		*umovRecord = (uCMOVATM_LK *)szBuffer;
	///* verlo como UMOV estandar */
 //   uCMOVATM_LK        *umovRecExt = (uCMOVATM_LK *)szBuffer;
	UMOVINTERF			*umovRecord = (UMOVINTERF *)szBuffer;
	UMOVINTERF			*umovRecExt = (UMOVINTERF *)szBuffer;
#endif
	/* indicativo de que UMOV usar */
	const BOOL      bUseUMOVStd = 
						#ifdef _UMOVPBF_USE_STANDARD_
								TRUE ;
						#else
								FALSE;
						#endif
	/* fix TLF */
	msgEXTRACT		*tlfRecord  = (msgEXTRACT *)szBuffer;
	/* fix TLF */
#ifdef _UMOVPBF_USE_STANDARD_
    uCMOVATM	    umovRecordSav , /* Saved Data-Record   */
					umovRecordHdr ; /* Saved Header-Record */
#elif _UMOVPBF_USE_METRO_
    uCMOVATM_METRO	umovRecordSav , /* Saved Data-Record   */
					umovRecordHdr ; /* Saved Header-Record */
#else
    //uCMOVATM_LK	    umovRecordSav , /* Saved Data-Record   */
				//	umovRecordHdr ; /* Saved Header-Record */
    UMOVINTERF	    umovRecordSav , /* Saved Data-Record   */
					umovRecordHdr ; /* Saved Header-Record */
#endif
    string          strQuery    = "", 
                    strAux      = "",
                    strDateHr   = "",
					strDateCi	= "",
					strBkpDate1, /* Saved string */
					strBkpDate2 = "" ; /* Saved string */
    INT             iSentence = 1,
                    iCounter,
					iErrCount ;
    SHORT           nStep     ,
                    nSize     ,
					nDigits   ; /* Digitos del Secuencial BT, hasta 25  */
    SHORT           nSlot      , /* Slot x Record */
  				    nSlotLimit ; /* Slot Limit = 1 or 10 */
    FIXEDINTEGER    fixNROLOG     , /* Nro.Log. UNIQUE = Primary Key */
					fixJulian     = 0; /* Julian TimeStamp */
    struct  tm      tmJulianTime = { 0,0,0,0,0,0 };
	BOOL			bClientInserted = FALSE;
	/***/
	time_t			tToday                 = time(NULL);
	struct tm		tmToday;		 
	char			szMMDD[4+1]            = {"1231"};
	char			szYYYYMMDD[8+1]        = {"19991231"};
	/***/
	char			szFilenameMMDD[256+1]  = {""};
	/***/

#if ( TRUE == _UMOVPBF_USE_UMOV_COD_CONCEP_  && _UMOVPBF_USE_UMOV_DESCRIPCION_ )
	char			bUMOV_USAR_ABREV_O_NRO = 'X' ; /* 'X'-Otro campo , no se usa codigo de concepto (secuencial) */
#elif ( TRUE == _UMOVPBF_USE_UMOV_COD_CONCEP_ )
	char			bUMOV_USAR_ABREV_O_NRO = 'N' ; /* 'N'-Numeros en el codigo de concepto */
#elif ( TRUE == _UMOVPBF_USE_UMOV_DESCRIPCION_ )
	char			bUMOV_USAR_ABREV_O_NRO = 'A' ; /* 'A'-Abreviaturas en el codigo de concepto */
#else
#error "Error, indicar si se usan abreviaturas o numeros de concepto."
#endif
	std::string sDenoOriginante;
	std::string sCuitOriginante;

	short			nUseCR_LF  = 0;
	short           nCount;
	BOOL			bSameDateInHeader; /* Hay movimientos con misma fecha en HEADER? Seran diferidos */
	BOOL            bUseDDMMYYYY; /* Usar DD/MM/YYYY o en cambio YYYY/MM/DD ? */
	short			nDaysBack;    /* Dias para atras si coincide con el header */
	size_t			nReadSize         = 0;
	// ReSharper disable CppEntityAssignedButNoRead
	long			lRetval;
	// ReSharper restore CppEntityAssignedButNoRead
    /* PRECONDITION : Local Settings USA : decimal_point "." - thousand_point "," */
    const char     *szLocale          = setlocale( LC_ALL, "LC_MONETARY=;LC_NUMERIC=;" ); 
    const struct    lconv *pstConv    = localeconv();
	/* SIGNO DEL IMPORTE */
	int             iSigno; /* Por omision, negativo */
	/* ARCHIVO DE TRAZA - EVIDENCIA - AUDITORIA */
	FILE			*fTracefile       = NULL;
	/* FIX : REGIONAL SETTINGS "-" PARA SEPARADOR DE FECHA */
	char			szDateTimeSeparator[1+1] = {"/"};
	BOOL            bReMapExclusion   = TRUE; /* remapea tipos invalidos a un unico tipo cta.cte. */
	
    /* argumentos son : UMOV-NAME, PBF-NAME */
	if( argc <= 1)
	{
		fprintf(stdout,"ERROR DE ARGUMENTOS, ESPECIFICAR NOMBRES DE PBF, UMOV, Y BASE DE DATOS\n\r");
		fprintf(stdout,"SINTAX: UMOVPBF.EXE <PBF-FILE> <UMOV-FILE> <DATABASE> <ABREVIATURA-O-NRO.COMPROB>\n\r");
		fprintf(stdout,"Ej. UMOVPBF.EXE PBF* UMOV* DATABASE | buscara 'MMDD' actuales en '*'\n\r");
		fprintf(stdout,"Ej. UMOVPBF.EXE PBF UMOV DATABASE A | procesara PBF y UMOV con Abreviaturas para los UMOV\n\r");
		fprintf(stdout,"Ej. UMOVPBF.EXE PBF UMOV DATABASE N | procesara PBF y UMOV con Conceptos para los UMOV\n\r");
		return (-1);
	}

	/*************************************************/
	/* PRECONDICION - TRACE FILE?                    */
	/*************************************************/
	if ( argc >= 7 )
	{ 
		// ReSharper disable CppDeprecatedEntity
		fTracefile = fopen( argv[6] , "w" ); // argv[6] = szOutputFile
		// ReSharper restore CppDeprecatedEntity
		if( NULL == fTracefile )
		{
			fTracefile = stdout ;
		}
	}
	/*************************************************/

	// JAF - 2016.09.22
	std::string sControlCORTOS(argv[2]);

	fprintf(fTracefile,"UMOV-PBF . PROGRAMA DE CARGA MASIVA DE UMOV Y PBF A ESQUEMA DE BASE DATOS DTN (%s)\n\r", argv[7-1] );
	fflush(fTracefile);

	std::string usuarioBase(argv[4]);
	std::string passBase(argv[5]);
	std::string odbcBase(argv[3]);
	
	odbcSession.SetUserName( (PSTR)usuarioBase.c_str() );   // argv[4] = userDB
	odbcSession.SetPassword( (PSTR)passBase.c_str() );		// argv[5] = passDB
    odbcSession.Connect( (PSTR)odbcBase.c_str() );			// argv[3] = baseDB

	//fprintf(fTracefile,"Usuario Base: [%s]\n\r", usuarioBase.c_str() );
	//fprintf(fTracefile,"Pass Base: [%s]\n\r", passBase.c_str() );
	//fprintf(fTracefile,"ServerODBC Base: [%s]\n\r", odbcBase.c_str() );
	
    if( odbcSession.Error() )
    {
		fprintf(fTracefile,"SIN PODER CONECTARSE A LA BASE DE DATOS\n\r");
		fclose(fTracefile);
		return (-2);
    }
	
	///* abreviaturas o numeros de conceptos en la DESCRIPCION de UMOV? */
 //   if ( argc >= 5 && ( 'N' == argv[5-1][0] || 'A' == argv[5-1][0] ) )
 //       bUMOV_USAR_ABREV_O_NRO = argv[5-1][0];

	/* mes y dia actuales */
	// ReSharper disable once CppDeprecatedEntity
	tmToday = *localtime( &tToday );
	sprintf( szMMDD, "%02i%02i", tmToday.tm_mon+1 , tmToday.tm_mday );
	/* año, mes y dia actuales */
	sprintf( szYYYYMMDD, "%04i%02i%02i", tmToday.tm_year + 1900 , tmToday.tm_mon+1 , tmToday.tm_mday );

	/* FIX chequeo de formato FECHA-HORA y MONETARIO */
    if(pstConv && (strcmp(pstConv->decimal_point,".")!=0 || strcmp(pstConv->thousands_sep,",")!=0))
    {		
		bUseDDMMYYYY = TRUE; /* siempre a VERDADERO por localizacion */
    }
	else
    {		
        bUseDDMMYYYY = FALSE; /* siempre a FALSO por omision */
    }

	bUseDDMMYYYY = FALSE;
    /* 2 pasos : paso 1 para PBF, paso 2 para UMOV */
    for( nStep = 1   ,    iCounter = 0, iErrCount = 0  ; 
         nStep < (argc-1) &&  nStep < 3 ; 		
         nStep++                        )
    {
        /* open file */
		if( strchr(argv[nStep],'*') )
		{
			strcpy(szFilenameMMDD, argv[nStep] );
			strcpy( strchr(szFilenameMMDD,'*') , szMMDD );
		}
		else
		{
			strcpy( szFilenameMMDD, argv[nStep] );
		}
		fprintf(fTracefile,"\n\rPROCESANDO ARCHIVO [%s]...\n\r", szFilenameMMDD);
		fflush(fTracefile);
		//hFile = fopen( szFilenameMMDD , "r" );
		//if(!hFile)
	    //   continue;

        /* record size ? PBF? UMOV LINK?*/
		if( 1 == nStep ) 
		{
			nSize = sizeof(REFRESHPBFINTERF);
		}
		else 
		{
			/* 2013.05.24, METROPOLIS extiende bytes al registro */
			nSize = ( bUseUMOVStd ) 
						? sizeof(uCMOVATM)    			
						: sizeof(UMOVINTERF) 
						;
		}

		/* FIX TLF */
		if( ( 1 == nStep ) 
			&&
			(  strstr( strupr(szFilenameMMDD), "TLF") != NULL 
			   ||
			   strstr( szFilenameMMDD, "EXTRACT") != NULL )
			)
		{
			/* Se restan los 3 bytes adicionales que incopore yo : ETX, EOF y NUL */
			nSize = sizeof(msgEXTRACT) - 
					sizeof(tlfRecord->chETX) - 
					sizeof(tlfRecord->chEOF) - 
					sizeof(tlfRecord->chNUL) ;
		}
		/* FIX TLF */
		/* FIX CR-LF */
		if ( nUseCR_LF && 1 != nStep && FALSE ) 		
			nUseCR_LF ++;
		/* FIX CR-LF */
        
        /* read until EOF */
		/* FIX : CR + LF */
		std::ifstream ifs;
		ifs.open (szFilenameMMDD, std::ifstream::in);

        //while ( fread( szBuffer, 1, nSize + nUseCR_LF, hFile ) > 0 ) //FML 20160411 Tarea -> Se cambia por la clase std
		while ( !ifs.eof() && ifs.good() )
        {
			ifs.getline(szBuffer, sizeof(szBuffer)) ;

            /* rec cntr */
            iCounter++;
            if( iCounter <= 1)
                fprintf(fTracefile,"INICIO DE INSERCION DE REGISTROS. AVANCE: \n\r" );		    
            if( ( iCounter % 100 ) == 0 )
                fprintf(fTracefile," <%i> ", iCounter );
			fflush(fTracefile);

			// 2013.08.08 - JAF - Agrego control si primer caracter es un espacio
			while ( 32 == szBuffer[0] )
				memmove ( szBuffer, szBuffer + 1, nSize );
			while ( 0x0A == szBuffer[0]  ||  0x0D == szBuffer[0] )
				memmove( szBuffer, szBuffer + 1, nSize);

			///* cr+lf */
			//if(  0x0A == szBuffer[0]  ||  0x0D == szBuffer[0] )
			//	memmove( szBuffer, szBuffer + 1, nSize);
			//if(  0x0A == szBuffer[0]  ||  0x0D == szBuffer[0] )
			//	memmove( szBuffer, szBuffer + 1, nSize);	
			///* invalid chr */

			for ( nCount = 0; szBuffer[0] < 0 && nCount < nSize; nCount++)
				memmove( szBuffer, szBuffer + 1, nSize);				
			/********/

            if( 1 == nStep )
            {
                if( strncmp(pbfRecord->REFRESH_FILE_HEADER.FH_REC_TYP, "FH", 2) == 0 ||
                    strncmp(pbfRecord->REFRESH_FILE_HEADER.FH_REC_TYP, "BH", 2) == 0 ||
                    strncmp(pbfRecord->REFRESH_FILE_HEADER.FH_REC_TYP, "BT", 2) == 0 ||
                    strncmp(pbfRecord->REFRESH_FILE_HEADER.FH_REC_TYP, "OH", 2) == 0 ||
                    strncmp(pbfRecord->REFRESH_FILE_HEADER.FH_REC_TYP, "OT", 2) == 0 ||
                    strncmp(pbfRecord->REFRESH_FILE_HEADER.FH_REC_TYP, "FT", 2) == 0 )
                {
                    continue;
                }
				/* FIX TLF */
				else if( strncmp(tlfRecord->header.PREFIX2, "TH", 2) == 0 )                    
                {
                    continue;
                }
				/* FIX TLF */

				// JAF - 2016-04-08 - PBI: xxx - CORRIMIENTO DE ARCHIVO, SE AGREGA CONTROL
				std::string sLineaMensaje(szBuffer);
				std::size_t posicion = sLineaMensaje.find("0116");
				if(posicion != 0 && posicion != string::npos)
					strncpy(szBuffer, sLineaMensaje.c_str()+posicion, sLineaMensaje.length()-posicion);

            }
            else if( 2 == nStep )
            {
                if( strncmp(umovRecord->CMOVATM_HEADER.CMOVATM_IDENTIFH, "HR", 2) == 0 ||
                    strncmp(umovRecord->CMOVATM_HEADER.CMOVATM_IDENTIFH, "TR", 2) == 0 ||
                    strncmp(umovRecord->CMOVATM_HEADER.CMOVATM_IDENTIFH, "FH", 2) == 0 ||
                    strncmp(umovRecord->CMOVATM_HEADER.CMOVATM_IDENTIFH, "FT", 2) == 0 )
                {
                    /* Save Header-Trailer */
                    umovRecordSav = (*umovRecord);
					umovRecordHdr = umovRecordSav;
                    continue;
                }
				/* FIX TLF */
				else if( strncmp(tlfRecord->header.PREFIX2, "TT", 2) == 0 )
                {
                    continue;
                }
				/* FIX TLF */
				/* FIX CORRIENTES */
				else
				{
					 /* HEADER-TRAILER*/
					 if( strncmp(umovRecord->CMOVATM_HEADER.CMOVATM_IDENTIFH, "00", 2) >= 0 
						 &&
						  strncmp(umovRecord->CMOVATM_HEADER.CMOVATM_IDENTIFH, "99", 2) <= 0 )
					 {
	                    /* Save Header-Trailer */
		                umovRecordSav = (*umovRecord);
					}
					 else
					 {
						 fprintf(fTracefile,"TIPO DE REGISTRO NO ESTANDAR.... (NO ESTA ENTRE '00' Y '99')\n\r" );		    
						 fflush(fTracefile);
						 fprintf(fTracefile,"\tTIPO DE REGISTRO INVALIDO NRO.[%i] : [%s]\n\r" , iCounter,  szBuffer  );
						 fflush(fTracefile);
						 continue;
					  }
				}
				/* FIX CORRIENTES */

            }
            else
                break;

            /* Slots per Record */
            nSlotLimit = ( 1 == nStep )
                            ? 1         /* PBF  = 1 slot   */
                            : 10;       /* UMOV = 10 slots */

            /* Slot Cycle ( INSERT CYCLE ) */
            for( nSlot = 0; nSlot < nSlotLimit; nSlot++ )
            {
                if( 1 == nStep )
                {
                    strQuery = "INSERT INTO MAECUE " ;
                    strQuery += "(MC_CODEMP,MC_SUCURS,MC_CODSIS,MC_TIPCTA,MC_MONEDA,MC_CUENTA,MC_ORDEN,";
					// 2013.08.08 - JAF - MC_COMEXT estaba comentado no podia hacer el insert
                    strQuery += "MC_NROCLI,MC_TIPORD,MC_CODTAS,MC_TASNOM,MC_CODCAP,MC_COMMAN,MC_COMEXT,";
                    strQuery += "MC_FECALT,MC_FECMOV,MC_FECACT,MC_FECPRO,";
                    strQuery += "MC_SALACT,MC_INTPAS,MC_INTACT,MC_FECCAP,MC_INTCAP,MC_SALCAP,MC_FECDEV,";
                    strQuery += "MC_DEVMES,MC_CODBLO,MC_IMPBLO,MC_FECBLO,MC_CANEXT,MC_CANCHE,MC_CODEXT,MC_ENVEXT," ;
                    strQuery += "MC_NUMERA,MC_CODBAJ,MC_FECBAJ,MC_ESTADO )" ;
                    strQuery += " VALUES " ;        
                    strQuery += "(0,0,0," ;

					/* ACCT TYPE */
					strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP, 0, 2) ;
					strQuery += ",";

					/**********************************************/					
					/* 2012.12.12   mdc   FIX : EXCLUSION CONTROL */
					/**********************************************/
					//FML 2017.07.28 No se reemplaza el, tipo de cuenta ahora si vienen cajas de ahorro
                    /*if ( bReMapExclusion && antoi(pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP,2) == _CEXTR_SAVINGS_ACCOUNT_ )
					{
						 fprintf(fTracefile,"TIPO DE CUENTA INVALIDA  #%i: [%i] CAJA DE AHORROS\n\r" , iCounter,  _CEXTR_SAVINGS_ACCOUNT_  );
						 fflush(fTracefile);
						 strncpy( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP, CEXTR_CHECKINGS_ACCOUNT, 2 );
					}
					else if ( bReMapExclusion && antoi(pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP,2) == _CEXTR_SAVINGS_ACCOUNT_USD_  )
					{
						 fprintf(fTracefile,"TIPO DE CUENTA INVALIDA  #%i: [%i] CAJA DE AHORROS USD\n\r" , iCounter,  _CEXTR_SAVINGS_ACCOUNT_USD_  );
						 fflush(fTracefile);
						 strncpy( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP, CEXTR_CHECKINGS_ACCOUNT_USD, 2 );
					}*/
					/**********************************************/
						
                    

					/* CURRENCY */
                    if (antoi(pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP,2) == _CEXTR_SAVINGS_ACCOUNT_
                        ||
                        antoi(pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP,2) == _CEXTR_CHECKINGS_ACCOUNT_  )
                        strQuery += B24_CURRENCY_ARGENTINA;
                    else if (antoi(pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP,2) == _CEXTR_CHECKINGS_ACCOUNT_USD_
                        ||
                        antoi(pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP,2) == _CEXTR_SAVINGS_ACCOUNT_USD_  )
                        strQuery += B24_CURRENCY_USA;
                    else
                        strQuery += B24_CURRENCY_ARGENTINA;
                    strQuery += ",";
                    
					/* ACCT : BRANCH + ACCT. NO. */
                    strAux = "";
					strAux.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_NUM.DR_NUM1, 0, 3) ;
                    strAux.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_NUM.DR_NUM2, 
								  _OFFSET_NUM_CUENTA_PBF_, 16 - _OFFSET_NUM_CUENTA_PBF_) ;

                    strQuery.append( strAux ) ;
                    strQuery += ",";
					/* ORDER */
                    strQuery += "0";
					/* CLIENT NO. IS FIXED 999999 */
                    strQuery += ",999999";
					/****************************************************************************************/
					/* TIPORD indica el sub-tipo de tipo de cuenta , que en el PBF viene indicado      */
					/* en el contador maximo de extracciones (99=normal, 0=cta.cte comun, 1=especial ) */
					if (  antoi(pbfRecord->REFRESH_BASE24.DR_WITH_ADV_AVAIL,2) == 99 )
						strQuery += ",0";
					else if ( antoi(pbfRecord->REFRESH_BASE24.DR_WITH_ADV_AVAIL,2) == 1 )
						strQuery += ",1";
					else
						strQuery += ",0";
					/****************************************************************************************/
					strQuery +=	",0,0,0,0,0,";
                    strQuery += "'1/01/1901','1/01/1901','1/01/1901','1/01/1901',";
                    
					iSigno = 
                    convert_extract_ammount( pbfRecord->REFRESH_BASE24.DR_LEDG_BAL,
                                            sizeof( pbfRecord->REFRESH_BASE24.DR_LEDG_BAL ) );
                    strQuery.append( pbfRecord->REFRESH_BASE24.DR_LEDG_BAL, 0, 10) ;
                    strQuery += ".";
                    strQuery.append( pbfRecord->REFRESH_BASE24.DR_LEDG_BAL, 10, 2) ;
                    strQuery += ",";
                    strQuery += "0,0,'1/01/1901',0,0,'1/01/1901',";                
                    strQuery += "0,0,0,'1/01/1901',0,0,0,0,0,0,'1/01/1901',0)" ;
                }
                else if( 2 == nStep )
                {

					/* FIX ATENCION, LA MASCARA DEL UMOV TIENE 2 DIGITOS MAS QUE EL PBF */
					/* POR LO TANTO HAY QUE IGNORAR LOS 2 PRIMEROS DIGITOS              */

                    strQuery = "INSERT INTO EXTCTA " ;
                    strQuery += "(EX_CODEMP,EX_SUCURS,EX_CODSIS,";
                    strQuery += "EX_TIPCTA,EX_MONEDA,EX_CUENTA,EX_ORDEN,";
                    strQuery += "EX_NROLOG,EX_LOGCTA,";
                    strQuery += "EX_NROCLI,EX_NROCOM,EX_NROCHE,EX_CODBAN,EX_CONCEP,";
                    strQuery += "EX_FECOPE,EX_FECIMP,EX_CODTRA,EX_TIPOPE,EX_IMPORT,";
                    strQuery += "EX_FECCON,EX_HORCON,EX_CODOPE,EX_DATEOR,EX_TIMEOR,";
                    strQuery += "EX_CODCAJ,EX_DATECA,EX_TIMECA,EX_CODERR,EX_CODLIQ,EX_FECCHE,EX_ASIIMP,";
                    strQuery += "EX_NROCAJ,EX_NROTER,EX_TIPMOV,EX_SUCORI,EX_PLACON,EX_CODCAM,";
                    //strQuery += "EX_SALACT,EX_ESTADO, EX_MARCADIFERIDO)" ;
					strQuery += "EX_SALACT,EX_ESTADO, EX_MARCADIFERIDO, EX_CORTOS, " ; 	// JAF - 2016.09.22
					strQuery += "EX_CUITOR,EX_DENOOR )"; // FML - 2017.06.09
					strQuery += " VALUES " ;
                    /**/
                    strQuery += "(0,0,0," ;

#if defined(  _SYSTEM_DATANET_CORRDNET_ )
					if( antoi(umovRecordSav.CMOVATM_HEADER.CMOVATM_TIPOCTA,2) == 0 )
					{
						strncpy( umovRecordSav.CMOVATM_HEADER.CMOVATM_TIPOCTA , CEXTR_CHECKINGS_ACCOUNT, 2);
						umovRecordHdr = umovRecordSav;
					}
#endif

					/**********************************************/					
					/* 2012.12.12   mdc   FIX : EXCLUSION CONTROL */
					/**********************************************/
                    // EAE ->
					/* 

					if ( bReMapExclusion && strncmp(umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA, CEXTR_SAVINGS_ACCOUNT, 2)==0 )
					{
						 fprintf(fTracefile,"TIPO DE CUENTA INVALIDA=[%2.2s]\n\r" ,  umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA  );
						 fflush(fTracefile);
						 strncpy( umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA, CEXTR_CHECKINGS_ACCOUNT, 2);
					}
					else 
					if ( bReMapExclusion && strncmp(umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA, CEXTR_SAVINGS_ACCOUNT_USD, 2)==0 )
					{
						 fprintf(fTracefile,"TIPO DE CUENTA INVALIDA=[%2.2s]\n\r" ,  umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA  );
						 fflush(fTracefile);
						 strncpy( umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA, CEXTR_CHECKINGS_ACCOUNT_USD, 2);
					}
					*/
					// EAE <-

					/*********************************************/

                    /* FIX : VALIDITY CONTROL */
                    if (strncmp(umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA, CEXTR_CHECKINGS_ACCOUNT_USD, 2)==0 ||
						strncmp(umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA, CEXTR_SAVINGS_ACCOUNT      , 2)==0 ||
						strncmp(umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA, CEXTR_SAVINGS_ACCOUNT_USD  , 2)==0 ||
						strncmp(umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA, CEXTR_CHECKINGS_ACCOUNT    , 2)==0 ||
						strncmp(umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA, CEXTR_TRANSFER_ACCOUNT_CBU , 2)==0 
						)
					{
						/* FIX */
						strQuery.append( umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA, 0, 2) ;
						strQuery += ",";
					}
					else
					{
						fprintf(fTracefile,"TIPO DE CUENTA INVALIDA=[%2.2s], SE ASUME DEFAULT=[%2.2s]\n\r" ,  
								umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA  ,
								CEXTR_CHECKINGS_ACCOUNT                       );
						fflush(fTracefile);
						/* DEFAULT */
						strQuery += CEXTR_CHECKINGS_ACCOUNT;
						strQuery += ",";
					}

					/* ACCT. TYPE */
                    if (antoi(umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA,2) == _CEXTR_SAVINGS_ACCOUNT_
                        ||
                        antoi(umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA,2) == _CEXTR_CHECKINGS_ACCOUNT_  )
                        strQuery += B24_CURRENCY_ARGENTINA;
                    else if (antoi(umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA,2) == _CEXTR_CHECKINGS_ACCOUNT_USD_
                        ||
                        antoi(umovRecordHdr.CMOVATM_HEADER.CMOVATM_TIPOCTA,2) == _CEXTR_SAVINGS_ACCOUNT_USD_  )
                        strQuery += B24_CURRENCY_USA;
                    else
                        strQuery += B24_CURRENCY_ARGENTINA;
                    strQuery += ",";
                    /* ACCT : BRANCH + ACCT. NO. */
                    strAux = "";
					/* BRANCH */
					strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_CUENTA.CMOVATM_SUC, 0, 3) ;
#if  ( 0 != _OFFSET_NUM_CUENTA_UMOV_ )
					/* FIX COPIA 2 VECES LA SUCURSAL (BRANCH) PARA MATCHEAR AL MAESTRO */
					strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_CUENTA.CMOVATM_SUC, 
								   3 - _OFFSET_NUM_CUENTA_UMOV_, _OFFSET_NUM_CUENTA_UMOV_) ;
#endif
					/* ACCT NO. */
                    strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_CUENTA.CMOVATM_NROCUEN, 
								  _OFFSET_NUM_CUENTA_PBF_, 15 - _OFFSET_NUM_CUENTA_PBF_ ) ;
                    strQuery.append( strAux ) ;
                    strQuery += ",";
                    strQuery += "0";
                    strQuery += ",";
                    /* NROLOG 12 DIGITS : 9 FOR ACCT.NO. , 3 FOR [JULIAN_DAY+SLOT ] */
                    fixNROLOG = antofix(umovRecord->CMOVATM_DATOS.CMOVATM_CUENTA.CMOVATM_NROCUEN,9)*1000 ;
                    /* JULIAN DAY IN YEAR */
					tmJulianTime.tm_isdst = 0;
					tmJulianTime.tm_hour  = 0;
					tmJulianTime.tm_min   = 0;
					tmJulianTime.tm_sec   = 0;
					tmJulianTime.tm_wday  = 0;
					tmJulianTime.tm_yday  = 0;
                    tmJulianTime.tm_year = antoi(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA,2)+100;
                    tmJulianTime.tm_mon  = antoi(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEMM,2)-1;
                    tmJulianTime.tm_mday = antoi(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA,2);

                    /* VALIDITY CONTROL */
                    if ( 0 == tmJulianTime.tm_mday
                         ||
                         0 > tmJulianTime.tm_mon
                         ||
                         100 > tmJulianTime.tm_year )
					{
                         /* BREAK SLOT CYCLE */
                         break;
					}

                    /* JULIAN DAY [0..365]*/
                    mktime( &tmJulianTime );
					
#if defined ( _UMOVPBF_USE_METRO_ )
                    /* NROLOG 12 DIGITS : 6 FOR MOVIMIENTO */
					fixNROLOG = antofix(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_NROMOVIM     , 
										sizeof(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[0].CMOVATM_NROMOVIM) ) ;
                    /* ASCII NRO LOG */
                    sprintf( szAux, FIXEDINTEGERMASK, fixNROLOG);
#else
                    /* NROLOG 12 DIGITS : 6 FOR ACCT.NO. , 6 FOR [JULIAN_DAY+SLOT ] */
                    fixNROLOG = antofix(umovRecord->CMOVATM_DATOS.CMOVATM_CUENTA.CMOVATM_NROCUEN,6)*1000000 ;
                    /* 3 DIGITS FOR JULIAN_DAY [0..365] */
                    fixNROLOG += tmJulianTime.tm_yday * 1000 ;  
                    /* 3 DIGITS FOR STEP + SLOT [0..9]*/
                    fixNROLOG += (iCounter*10) + nSlot ;  
                    /* ASCII NRO LOG */
                    sprintf( szAux, FIXEDINTEGERMASK, fixNROLOG);
#endif

					/* FIX NRO.LOG X ALFABETICO O X NUMERICO INFORMADO EN UMOV? */
					if ( 'A' == bUMOV_USAR_ABREV_O_NRO )
					{
	                    strQuery.append( strAux = szAux ) ;
					}
					else
					{
						strAux = "";
						if ( 'X' == bUMOV_USAR_ABREV_O_NRO )
						{

							/* FIX SECUENCIAL BT HASTA 25 DIGITOS */
#if defined( _UMOVPBF_USE_STANDARD_ ) || defined ( _UMOVPBF_USE_METRO_ )
							nDigits = strnumlen( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO, 
												sizeof(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO) );
							/* Superior a 5 digitos, hasta 10 digitos ? */
							if ( nDigits >= 5 && nDigits <= 10)
							{
								strAux.append(  umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO,
												0, 10 );
								strQuery.append( strAux ) ;						
							}
							else
							{
								/* SINO USAR ANTERIOR SECUENCIAMIENTO POR CUENTA+JULIAN-STAMP */
								strQuery.append( strAux = szAux ) ;
							}
#else							
							nDigits = strnumlen( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_SECUENCIAL, 
												sizeof(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_SECUENCIAL) );
							/* Superior a 20 digitos, hasta 25 digitos ? */
							if ( nDigits >= 20 && nDigits <= 25)
							{
								strAux.append(  umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_SECUENCIAL,
												0, 25 );
								strQuery.append( strAux ) ;						
							}
							else
							{
								/* SINO USAR ANTERIOR SECUENCIAMIENTO POR CUENTA+JULIAN-STAMP */
								strQuery.append( strAux = szAux ) ;
							}
#endif
						}
						else 
						{
							/* FIX HASTA 9 DIGITOS : 3 + 6 */
#if defined( _UMOVPBF_USE_STANDARD_ ) || defined ( _UMOVPBF_USE_METRO_ )
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO, 0, 9  );
#else
							//strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO    , 0, 3  );
							//strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO + 3, 1, 6  );
							//FML 2016-09-02 - Va el concepto completo para mapear con la tabla en la vista
							sCuitOriginante = std::string(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CUIT_ORIGINANTE, 0,11);
							sDenoOriginante = std::string(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_DENO_ORIGINANTE,0,40);
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO , 0, 9  );
#endif
							if ( atol(strAux.c_str()) > 0 )
								strQuery.append( strAux  ) ;						
							else
								/* SINO USAR ANTERIOR SECUENCIAMIENTO POR CUENTA+JULIAN-STAMP */
								strQuery.append( strAux = szAux ) ;
						};
					};

                    strQuery += ",";
#if defined( _UMOVPBF_USE_STANDARD_ ) || defined ( _UMOVPBF_USE_METRO_ )
					if( strnumlen( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO, 5 ) == 5 )
						strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO , 5 ) ;
					else
						strQuery.append( "0" );
                    strQuery += ",";
#else
					//FML 2016-08-31: Ahora toma 8 posiciones en vez de 5 para que entre el numero de cheques
					if( strnumlen( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_NRO_SOBRE_CH, 8 ) == 8 )
						strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_NRO_SOBRE_CH , 8 ) ;
					else
						strQuery.append( "0" );
                    strQuery += ",";
#endif

                    /* CLI, COMP, NRO_CHEQ, BANCO */
                    strQuery += "0,0," ;
					/* NROCHEQUE */
#if defined( _UMOVPBF_USE_STANDARD_ ) || defined ( _UMOVPBF_USE_METRO_ )
					if( strnumlen( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO, 5 ) >= 1 )
						strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO , 5 ) ;
					else
						strQuery.append( "0" );
                    strQuery += ",";
#else
					//FML 2016-08-31: Ahora toma 8 posiciones en vez de 5 para que entre el numero de cheques
					if( strnumlen( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_NRO_SOBRE_CH, 8 ) >= 1 )
						strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_NRO_SOBRE_CH , 8 ) ;
					else
						strQuery.append( "0" );
                    strQuery += ",";
#endif
					/* BANCO */
					strQuery += "0,";
					/* CONCEP */
                    strQuery += "'";

					/* FIX */
					if ( 'A' == bUMOV_USAR_ABREV_O_NRO || 'X' == bUMOV_USAR_ABREV_O_NRO )
					{
#if defined( _UMOVPBF_USE_STANDARD_ ) || defined ( _UMOVPBF_USE_METRO_ )
						strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO, 0, 10) ;
#else
						strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO, 0, 10) ;
#endif
					}
					else
					{
#if defined( _UMOVPBF_USE_STANDARD_ ) || defined ( _UMOVPBF_USE_METRO_ )
						strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.COD_CONCEP_NRO_COMPROB.CMOVATM_CODCONCEP, 0, 3  );
						strQuery += ",";
						strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.COD_CONCEP_NRO_COMPROB.CMOVATM_NROCOMPRO, 0, 7  );
#else
						//strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO    , 0, 3  );
						//strQuery += ",";
						//strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO + 3, 0, 7  );
						strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO , 0, 9  );
#endif
					};
					/* FIX */

                    strQuery += "'";
                    strQuery += ",";

                    /* DATE DD/MM/YYYY */  
					if( bUseDDMMYYYY )
					{
						/*** fix chequeo fuerte de fecha 6 posiciones ****/ 
						if( strnumlen(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEDD,2) == 2 &&
							strnumlen(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEMM,2) == 2 &&
							strnumlen(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA,2) == 2 )
						{
						/****/
							strAux = "";
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEDD, 0, 2) ;
							strAux += szDateTimeSeparator;
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEMM, 0, 2) ;
							strAux += szDateTimeSeparator;
							strAux += "20";
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA, 0, 2) ;
						}
						/*** fix chequeo fuerte de fecha 4 posiciones , MMDD ****/ 
						else if( strnumlen(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA,2) == 2 &&
							     strnumlen(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEMM,2) == 2 )
						{
							strAux = "";
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEMM, 0, 2) ;
							strAux += szDateTimeSeparator;
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA, 0, 2) ;
							strAux += szDateTimeSeparator;
							strAux += szYYYYMMDD;;
							strAux = strAux.substr(0,10);
						}
						/*****/
					}
					else
					{
						/*** fix chequeo fuerte de fecha 6 posiciones ****/ 
						if( strnumlen(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA,2) == 2 &&
							strnumlen(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEMM,2) == 2 &&
							strnumlen(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEDD,2) == 2 )
						{
						/****/
							strAux = "20";
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA, 0, 2) ;
							strAux += szDateTimeSeparator;
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEMM, 0, 2) ;
							strAux += szDateTimeSeparator;
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEDD, 0, 2) ;
						}
						/*** fix chequeo fuerte de fecha 4 posiciones , MMDD ****/ 
						else if( strnumlen(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA,2) == 2 &&
							     strnumlen(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEMM,2) == 2 )
						{
							strAux = szYYYYMMDD;
							strAux = strAux.substr(0,4);
							strAux += szDateTimeSeparator;
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA, 0, 2) ;
							strAux += szDateTimeSeparator;
							strAux.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEMM, 0, 2) ;
						}
						/*****/
					}
#if ( FALSE )
					fprintf(fTracefile,"FECHA PROCESO :  [%s]  vs. [%6.6s]\n\r" ,  
							(char *)strAux.c_str()  ,
							umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA);
#else
					fprintf(fTracefile,"FECHA PROCESO :  [%s]  vs. [%6.6s]\n\r" ,  
							(char *)strAux.c_str()  ,
							umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA);
#endif
					fflush(fTracefile);

					/* Backup date string (1) */
					strBkpDate1 = strAux;
					/* Backup date string (2) */
					strBkpDate2 = strAux;

					strQuery += "'";
					/*** FIX MISMA FECHA QUE EL HEADER? HACERLO COMO DIFERIDO ***/
					bSameDateInHeader = 
						( strncmp(umovRecordSav.CMOVATM_HEADER.CMOVATM_FECPROC.CMOVATM_AA,
								  umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA,
								  sizeof(umovRecordSav.CMOVATM_HEADER.CMOVATM_FECPROC) ) == 0							  
							) ;

					if( !bSameDateInHeader )
					{
						strQuery.append( strAux ) ; /* FEC-OPE */
					}
					else 
					{						
						/* Dias para atras si coincide con el header = 1 ... o + */
						nDaysBack = 1; 
						do 
						{
							/* JULIAN DAY IN YEAR */
							tmJulianTime.tm_isdst = 0;
							tmJulianTime.tm_hour  = 0;
							tmJulianTime.tm_min   = 0;
							tmJulianTime.tm_sec   = 0;
							tmJulianTime.tm_wday  = 0;
							tmJulianTime.tm_yday  = 0 ;
							tmJulianTime.tm_year = antoi(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA,2)+100;
							tmJulianTime.tm_mon  = antoi(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEMM,2)-1;
							tmJulianTime.tm_mday = antoi(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEDD,2)-nDaysBack;

							/* JULIAN DAY [0..365]*/
							mktime( &tmJulianTime );
							/* FORMAT DATE STRING IN ASCII */
							/* DATE DD/MM/YYYY */
							sprintf( szAux, "%02i/%02i/%04i",
										tmJulianTime.tm_mday       ,
										tmJulianTime.tm_mon + 1    ,
										tmJulianTime.tm_year + 1900);
							/* String vs. C-String format */
							strAux = szAux;

							/* VALIDITY CONTROL */
							if ( 0 == tmJulianTime.tm_mday /* SUNDAY? */
								 ||
								 6 == tmJulianTime.tm_mday /* SATURDAY? */
								)
							{
								nDaysBack++;
								/* COTINUE SLOT CYCLE */
								continue;
							}
							else if ( 0 > tmJulianTime.tm_mon
									 ||
									 100 > tmJulianTime.tm_year )	
							{
									 /* BREAK SLOT CYCLE */
								bSameDateInHeader = FALSE;
								break;
							}
							else
							{
								/* BREAK SLOT CYCLE */
								bSameDateInHeader = FALSE;
								break;
							}

						} while ( bSameDateInHeader );

						fprintf(fTracefile,"FECHA DIFERIDA AJUSTADA :  [%s]\n\r" ,  (char *)strAux.c_str()  );
						fflush(fTracefile);

						/* Backup date string (2) */
						strBkpDate2 = strAux;
						/* Append to Query */
						strQuery.append( strAux ) ; /* ¿ FEC-OPE = FEC-HEADER ? */
					};
					/************************************************************/
                    strQuery += "','";                    

					/* Restore Date String (1) */					
					strAux = strBkpDate1;

                    strQuery.append( strAux ) ; /* FEC-IMP */
                    strQuery += "',";                    
#if ( FALSE )
					strQuery += "'";
                    strQuery.append( strAux ) ;
                    strQuery += "',";
#endif
                    /* CODTRA,TIP-OPE */         
					/* AMMOUNT */                
                    if( '-' == umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMSIGNO )
                    {
						umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMSIGNO = '0';
						iSigno = -1;
                        /* TIP-OPE */
                        strQuery += "90,"; /* CHEQUE X/CAMARA , DEBITO, NEGATIVO */
                        /* COD-TRA */
                        strQuery += "-1,";
						/* AMMOUNT */                
						iSigno = 
						convert_extract_ammount( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE,
								   sizeof( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE ) );

                    }
                    else if ( '+' == umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMSIGNO)
                    {
						umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMSIGNO = '0';
						iSigno = +1;
                        /* TIP-OPE */
                        strQuery += "4,"; /* DEPOSITO CH, CREDITO, POSITIVO */
                        /* COD-TRA */
                        strQuery += "+1,";
						/* AMMOUNT */                
						iSigno = 
						convert_extract_ammount( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE,
                                   sizeof( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE ) );
						/* CHECK ZEROES IN AMMOUNT */
						if (strncmp(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE,
									"*************************", 
									sizeof( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE ) )
									== 0 )
						{
							/* AMMOUNT */                
#if defined( _UMOVPBF_USE_STANDARD_ ) || defined ( _UMOVPBF_USE_METRO_ )
							iSigno = 
								convert_extract_ammount( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO,
									   sizeof( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO ) );
#else
							iSigno = 
								convert_extract_ammount( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO,
									   sizeof( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO ) );
#endif
						}
						/***/
                    }
					else
					{
						iSigno = 
						convert_extract_ammount( &umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMSIGNO,
                                   sizeof( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE )+1);
						/* CHECK ZEROES IN AMMOUNT */
						if (strncmp(umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE,
									"******************", 
									sizeof( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE ) )
									== 0 )
						{
							/* AMMOUNT */                
#if defined( _UMOVPBF_USE_STANDARD_ ) || defined ( _UMOVPBF_USE_METRO_ )
							iSigno = 
								convert_extract_ammount( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO,
									   sizeof( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].DESCRIPCION.CMOVATM_CONCEPTO ) );
#else
							iSigno = 
								convert_extract_ammount( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO,
									   sizeof( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO ) );
#endif
						}
						/***/
						if (iSigno < 0)
						{
							/* TIP-OPE */
							strQuery += "4,"; /* DEPOSITO CH, CREDITO, POSITIVO */
							/* COD-TRA */
							strQuery += "+1,";
						}
						else
						{
							/* TIP-OPE */
							strQuery += "4,"; /* DEPOSITO CH, CREDITO, POSITIVO */
							/* COD-TRA */
							strQuery += "+1,";
						}
					}

					/* AMMOUNT */
					/***** FIX PICTURE 9(8)V99 ***/
					strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE, 
						0, 
						sizeof( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE )-2 );
                    strQuery += ".";
					strQuery.append( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE, 
						sizeof( umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE )-2, 
						2);
                    /***** FIX PICTURE 9(8)V99 ***/
				
					strDateHr = "20";
					strDateHr.append(umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECPROC.CMOVATM_AA,0,2);
					strDateHr.append(umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECPROC.CMOVATM_MM,0,2);
					strDateHr.append(umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECPROC.CMOVATM_DD,0,2);
					
					//EAE ->
					char szFechaParaControlDiferido[10+1] = {0x00};
					std::string sFechaTemp;
					strDateCi = "20";
					strDateCi.append(umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECCIE.CMOVATM_AA,0,2);
					strDateCi.append(umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECCIE.CMOVATM_MM,0,2);
					strDateCi.append(umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECCIE.CMOVATM_DD,0,2);

					sFechaTemp = "20";
					sFechaTemp.append(umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECCIE.CMOVATM_AA,0,2);
					sFechaTemp.append("/");
					sFechaTemp.append(umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECCIE.CMOVATM_MM,0,2);
					sFechaTemp.append("/");
					sFechaTemp.append(umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECCIE.CMOVATM_DD,0,2);
					strcat(szFechaParaControlDiferido, sFechaTemp.c_str());
					//sprintf(szFechaParaControlDiferido, "20%2s/%2s/%2s",
					//		umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECCIE.CMOVATM_AA,
					//		umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECCIE.CMOVATM_MM,
					//		umovRecordHdr.CMOVATM_HEADER.CMOVATM_FECCIE.CMOVATM_DD);

					//EAE ->
					strQuery += ",";

                    /* DATES */
                    strQuery += "'";
                    //strQuery.append( strAux ) ;
					strQuery.append( strDateCi ) ;
                    strQuery += "'";
                    strQuery += ",";
                    strQuery += "'";
                    strQuery.append( "0000" );
                    strQuery += "'";
                    strQuery += ",";
                    /* COD-OPE : negativo o positivo? db o cr ? */         
                    if( iSigno < 0 )
                        strQuery += "90,"; /* CHEQUE DB X CAMARA */
                    else
                        strQuery += "4,"; /* DEPOSITO CH */
                    /* DATE + HOUR */
                    strQuery += "'";
					/* Restore Date string (2) */
					strAux = strBkpDate2 ;
                    //strQuery.append( strAux ) ;
					strQuery.append( strAux ) ;
                    strQuery += "'";
                    strQuery += ",";
                    strQuery += "'";
                    strQuery.append( "0000" );
                    strQuery += "'";
                    strQuery += ",";
                    /* CAJA */
					/* FIX : REGIONAL SETTINGS "-" PARA SEPARADOR DE FECHA */
					if( !bUseDDMMYYYY )
					{	
						if( strcmp(szDateTimeSeparator,"/") == 0 )
							strQuery += "0,'1901/01/01','0000',0,0,'1901/01/01',0.0,";
						else
							strQuery += "0,'1901-01-01','0000',0,0,'1901-01-01',0.0,";
					}
					else
					{	
						if( strcmp(szDateTimeSeparator,"/") == 0 )
							strQuery += "0,'01/01/1901','0000',0,0,'01/01/1901',0.0,";
						else
							strQuery += "0,'01-01-1901',0,0,'01-01-1901',0.0,";
					}
					/************************************************/
                    strQuery += "0,0,0,0,0,0,";
                    // JAF - FML - 2016.0916 - CONTROL MARCA DIFERIDO
					strQuery += "0.0,1," ;

					
					if (strcmp(szFechaParaControlDiferido, strBkpDate2.c_str()) < 0)
						// strQuery += "'S')"; //2016.09.22
						strQuery += "'S',"; //2016.09.22
					else
						//strQuery += "'N')";
						strQuery += "'N',";

					if(sControlCORTOS.find("UMOVD") != std::string::npos)
						strQuery += "'N ',";
					else
						strQuery += "'S ',";

					strQuery += "\'"; 
					strQuery += sCuitOriginante.c_str();
					strQuery += "\' ,";

					
					strQuery += "\'";
					strQuery += sDenoOriginante.c_str();
					strQuery += "\' )";

                    
                }; /* if */

                iSentence = 1;
                odbcSession.Query( (char *)strQuery.c_str(), &iSentence );
                 if( odbcSession.Error() )
                {
                    odbcSession.ResetLastError();
                    if( 1 == nStep )
                    {                                                                       
                        strQuery = "UPDATE MAECUE " ;
                        strQuery += " SET MC_SALACT = ";
                        strQuery.append( pbfRecord->REFRESH_BASE24.DR_LEDG_BAL, 0, 10) ;
                        strQuery += ".";
                        strQuery.append( pbfRecord->REFRESH_BASE24.DR_LEDG_BAL, 10, 2) ;

						/****************************************************************************************/
						/* TIPORD indica el sub-tipo de tipo de cuenta , que en el PBF viene indicado      */
						/* en el contador maximo de extracciones (99=normal, 0=cta.cte comun, 1=especial ) */
                        strQuery += ", MC_TIPORD = ";
						if (  antoi(pbfRecord->REFRESH_BASE24.DR_WITH_ADV_AVAIL,2) == 99 )
							strQuery += "0";
						else if ( antoi(pbfRecord->REFRESH_BASE24.DR_WITH_ADV_AVAIL,2) == 1 )
							strQuery += "1";
						else
							strQuery += "0";
						/****************************************************************************************/


                        strQuery += " WHERE MC_CUENTA = ";
                        //strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_NUM.DR_NUM2, 0, 16) ;
	                    strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_NUM.DR_NUM1, 0, 3) ;
						strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_NUM.DR_NUM2, _OFFSET_NUM_CUENTA_PBF_, 16 - _OFFSET_NUM_CUENTA_PBF_) ;

                        iSentence = 2;
                        odbcSession.Query( (char *)strQuery.c_str(), &iSentence );
                        if( odbcSession.Error() )
                        {
							fprintf(fTracefile,"SIN PODER INSERTAR MAESTRO, <%s>.\n\r", odbcSession.GetErrorMessage(NULL) );
							fflush(fTracefile);
                            odbcSession.ResetLastError();
                        }; /* if */
                    } /* else if */
                    else if( 2 == nStep )
                    {       
						lRetval = odbcSession.GetStatus();
						if( odbcSession.GetReturnCode() == -1 )
						{
							if( strstr( odbcSession.GetErrorMessage(NULL), "PRIMARY KEY") )
							{
								fprintf(fTracefile,"MOVIMIENTO YA EXISTE, CONTINUANDO.\n\r" );		    
								fflush(fTracefile);
							}
							else if( strstr( odbcSession.GetErrorMessage(NULL), "datetime") 
									 ||
								     strstr( odbcSession.GetErrorMessage(NULL), "DATETIME") )
							{					
								/* FIX : REGIONAL SETTINGS "-" PARA SEPARADOR DE FECHA */
								if( !bUseDDMMYYYY &&  strcmp(szDateTimeSeparator,"/") == 0 )
								{
									strcpy( szDateTimeSeparator, "-"); /* MODIFICAR SEPARADOR DE FORMATO FECHA-HORA */
									bUseDDMMYYYY = FALSE; 
								}
								else if( !bUseDDMMYYYY &&  strcmp(szDateTimeSeparator,"-") == 0 )
								{
									strcpy( szDateTimeSeparator, "/"); /* MODIFICAR SEPARADOR DE FORMATO FECHA-HORA */
									bUseDDMMYYYY = TRUE; 
								}
								else if( bUseDDMMYYYY &&  strcmp(szDateTimeSeparator, "/") != 0 )
								{
									strcpy( szDateTimeSeparator, "-"); /* MODIFICAR SEPARADOR DE FORMATO FECHA-HORA */
									bUseDDMMYYYY = TRUE; /* MODIFICAR FORMATO FECHA-HORA dd/mm/yyyy */
								}
								else if( bUseDDMMYYYY )
								{
									strcpy( szDateTimeSeparator, "/"); /* MODIFICAR SEPARADOR DE FORMATO FECHA-HORA */
									bUseDDMMYYYY = FALSE; /* MODIFICAR FORMATO FECHA-HORA yyyy/mm/dd */
								}
								/******************************************************/
								fprintf(fTracefile,"SE MODIFICO EL FORMATO DE FECHA-HORA\n\r" ,  (char *)strQuery.c_str()  );
								/* fix etiqueta de error de insercion */
								fprintf(fTracefile,"SIN PODER INSERTAR MOVIMIENTO <%s>\n\r", odbcSession.GetErrorMessage(NULL) );
								fflush(fTracefile);
								iErrCount++;
								odbcSession.ResetLastError();									

							}
							else
							{
								fprintf(fTracefile,"SIN PODER INSERTAR MOVIMIENTO <%s>\n\r", odbcSession.GetErrorMessage(NULL) );		    
								fprintf(fTracefile,"\tSQL = [%s]\n\r" ,  (char *)strQuery.c_str()  );
								iErrCount++;
								fflush(fTracefile);
							}
						}
						else 
						{
		                  fprintf(fTracefile,"SIN PODER INSERTAR MOVIMIENTO <%s>\n\r", odbcSession.GetErrorMessage(NULL) );		    
						  fprintf(fTracefile,"\tSQL = [%s]\n\r" ,  (char *)strQuery.c_str()  );
						  iErrCount++;
						  fflush(fTracefile);
						}
                        odbcSession.ResetLastError();

					} ;/* if-step */

                }; /* if-Error */

				/* INSERTAR ACUERDOS VIGENTES PARA ESTE AÑO */
				if( 2 == nStep )
                {                                                                       
					/***********************************************************************************/
					fprintf(fTracefile, "\tCUENTA = [%-15.15s], MOVIMIENTO [%i] = "
						"[ %-2.2s/%-2.2s/%-2.2s"
						"; '%-10.10s'"
						"; %c"
						"; $ %-8.8s,%3.3s" 
						" ]\n\r" 
						, umovRecord->CMOVATM_DATOS.CMOVATM_CUENTA.CMOVATM_NROCUEN
						, nSlot
						, umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEAA
						, umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEMM  
						, umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_FECHA.CMOVATM_FEDD  
						, umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_CONCEPTO  
						, umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMSIGNO
						, umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE							
						, umovRecord->CMOVATM_DATOS.CMOVATM_TABLA_MOVIM[nSlot].CMOVATM_IMPORTE + 8
						);
					fflush(fTracefile);
					/***********************************************************************************/
                }; 

				
				/* INSERTAR ACUERDOS VIGENTES PARA ESTE AÑO */
				if( 1 == nStep )
                {                                                                       
                        strQuery = "INSERT INTO ACUERD ";
						strQuery += "( AC_NROCLI, AC_NROSOL, AC_CODSIS, AC_FECVTO, AC_FECALT, AC_TASEFE, AC_CODGAR, AC_PLATOT, AC_MONACU, AC_FECPRO ,";
						strQuery += "AC_COME03 , AC_COME02, AC_COME01 , AC_FECCAN , AC_ESTADO , AC_AUTO03 , AC_AUTO02, AC_AUTO01 ,";
						strQuery += "AC_TIPAPL , AC_TASNOM , AC_CODLIN , AC_TIPACU , ";
						strQuery += "AC_ORDEN , AC_CUENTA, AC_TIPCTA , ";
						strQuery += "AC_MONEDA , AC_SUCURS , AC_CODEMP )";
						strQuery += "VALUES ( '";						/* ******************************************* */
						/* COD-CLIENTE = NRO. CUENTA PARA ESTE ACUERDO */
	                    strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_NUM.DR_NUM1, 0, 3) ;
						strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_NUM.DR_NUM2, _OFFSET_NUM_CUENTA_PBF_, 16 - _OFFSET_NUM_CUENTA_PBF_) ;							
						/* ******************************************* */
						strQuery += "', 999999, 1, '";
						if ( bUseDDMMYYYY )
						{
							strQuery += "31/12/";
							strQuery += itoa( tmToday.tm_year + 1900 , szAux, 10 );
						}
						else
						{
							strQuery += itoa( tmToday.tm_year + 1900 , szAux, 10 );
							strQuery += "/12/31";
						}
						strQuery += "', " ;
						if ( bUseDDMMYYYY )
							strQuery += "'1/1/2001 00:00'" ;
						else
							strQuery += "'2001/1/1 00:00'" ;
						strQuery += ", 0.0001, 9999, 99, ";
						/* INICIO-MONTO DEL ACUERDO */
						strQuery.append( pbfRecord->REFRESH_BASE24.DR_OVRDRFT_LMT, 0, 10) ;
						strQuery += ".00";
						/* FIN-MONTO DEL ACUERDO */
						if ( bUseDDMMYYYY )
							strQuery += ", '1/1/2001' , ";
						else
							strQuery += ", '2001/1/1' , ";
						strQuery += "0 , 0 , 0, ";
						if ( bUseDDMMYYYY )
							strQuery += "'31/12/2030'";
						else
							strQuery += "'2030/12/31'";
						strQuery += ", '0' , 0 , 0 , 0,"; /* FIX ESTADO=0 */
						strQuery += "0 , 0 , 1 , 1 , ";
						strQuery += "0 , ";
						/* ACCT NO. */
						strQuery += "'";
						strQuery += strAux;						
						strQuery += "',";
						/* ACCT TYPE */
						strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP, 0, 2) ;
						strQuery += ",";
						/* CURRENCY */
						if (antoi(pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP,2) == _CEXTR_SAVINGS_ACCOUNT_
							||
							antoi(pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP,2) == _CEXTR_CHECKINGS_ACCOUNT_  )
							strQuery += B24_CURRENCY_ARGENTINA;
						else if (antoi(pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP,2) == _CEXTR_CHECKINGS_ACCOUNT_USD_
							||
							antoi(pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP,2) == _CEXTR_SAVINGS_ACCOUNT_USD_  )
							strQuery += B24_CURRENCY_USA;
						else
							strQuery += B24_CURRENCY_ARGENTINA;
						strQuery += ", 0 , 0)";

                        iSentence = 3;
                        odbcSession.Query( (char *)strQuery.c_str(), &iSentence );
                        if( odbcSession.Error() )
                        {
							lRetval = odbcSession.GetStatus();
							if( odbcSession.GetReturnCode() == -1 )
							{								
								if( strstr( odbcSession.GetErrorMessage(NULL), "PRIMARY KEY") )
								{									
								}
								else if( strstr( odbcSession.GetErrorMessage(NULL), "datetime") 
										 ||
									     strstr( odbcSession.GetErrorMessage(NULL), "DATETIME") )
								{					
									if( !bUseDDMMYYYY )
									{
										bUseDDMMYYYY = TRUE; /* MODIFICAR FORMATO FECHA-HORA */
									}
									else if( bUseDDMMYYYY )
									{
										bUseDDMMYYYY = FALSE; /* MODIFICAR FORMATO FECHA-HORA */
									}
									fprintf(fTracefile,"SE MODIFICO EL FORMATO DE FECHA-HORA\n\r" ,  (char *)strQuery.c_str()  );
									fprintf(fTracefile,"SIN PODER INSERTAR ACUERDO <%s>\n\r", odbcSession.GetErrorMessage(NULL) );
									fflush(fTracefile);
									odbcSession.ResetLastError();									

								}
								else
								{
									fprintf(fTracefile,"SIN PODER INSERTAR ACUERDO <%s>\n\r", odbcSession.GetErrorMessage(NULL) );		    
									fprintf(fTracefile,"CONTENIDO :  [%s]\n\r" ,  (char *)strQuery.c_str()  );
									fflush(fTracefile);
								}
							};
                            odbcSession.ResetLastError();
                        }; /* if */                
				}

				/* INSERTAR CLIENT GENERICO 999999 SI ES NECESARIO        */
				/* INSERTAR TODOS LOS DEMAS CLIENTES PARA REUNIR ACUERDOS */
				if( 1 == nStep && !odbcSession.Error() )
                {                           
						if ( !bClientInserted )
						{
							/* CLIENTE 999999 ES EL GENERICO POR OMISION */
							/* Y DEBE EXISTIR SIEMPRE                    */
							strQuery = "INSERT INTO CLIENT";
							strQuery += "( CL_NROCLI , CL_NOMCLI , CL_DOMCLI , CL_TELEFO , CL_NRODOC , CL_CATEGO, CL_NROPRO , CL_FECPRO , CL_FECEST , CL_TIPCLI ,";
							strQuery += " CL_FECBAJ , CL_FECALT )";
							strQuery += "VALUES ";
							strQuery += "( 999999, 'GENERICO', 'GENERICO', '', 999999, 1 , 1 , '01/01/2001', '01/01/2001'  , 1 , '01/01/1901', '01/01/2001')";
							iSentence = 4;
							odbcSession.Query( (char *)strQuery.c_str(), &iSentence );
							if( odbcSession.Error() )
							{
								lRetval = odbcSession.GetStatus();
								if( odbcSession.GetReturnCode() == -1 )
								{
									if( strstr( odbcSession.GetErrorMessage(NULL), "PRIMARY KEY") )
									{									
									}
									else
									{
										fprintf(fTracefile,"SIN PODER INSERTAR CLIENTE <%s>\n\r", odbcSession.GetErrorMessage(NULL) );		    
										fprintf(fTracefile,"CONTENIDO :  [%s]\n\r" ,  (char *)strQuery.c_str()  );
										fflush(fTracefile);
									}
								};
								odbcSession.ResetLastError();
							}; /* if */                
							/* FLAG DE MARCA DE 'GENERICO-999999' INSERTADO */
							bClientInserted = TRUE;
						};

						/* CLIENTE XXXXXX ES EL ESPECIFICO DE LA CUENTA */
                        strQuery = "INSERT INTO CLIENT";
						strQuery += "( CL_NROCLI , CL_NOMCLI , CL_DOMCLI , CL_TELEFO , CL_NRODOC , CL_CATEGO, CL_NROPRO , CL_FECPRO , CL_FECEST , CL_TIPCLI ,";
						strQuery += " CL_FECBAJ , CL_FECALT )";
						strQuery += "VALUES ";
						strQuery += "( '";
						/* ******************************************* */
						/* COD-CLIENTE = NRO. CUENTA PARA ESTE ACUERDO */
	                    strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_NUM.DR_NUM1, 0, 3) ;
						strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_NUM.DR_NUM2, _OFFSET_NUM_CUENTA_PBF_, 16 - _OFFSET_NUM_CUENTA_PBF_) ;							
						/* ******************************************* */
						strQuery += "', 'GENERICO', 'GENERICO', '', 999999, 1 , 1 , '01/01/2001', '01/01/2001'  , 1 , '01/01/1901', '01/01/2001')";
                        iSentence = 5;
                        odbcSession.Query( (char *)strQuery.c_str(), &iSentence );
                        if( odbcSession.Error() )
                        {
							lRetval = odbcSession.GetStatus();
							if( odbcSession.GetReturnCode() == -1 )
							{
								if( strstr( odbcSession.GetErrorMessage(NULL), "PRIMARY KEY") )
								{									
								}
								else
								{
									fprintf(fTracefile,"SIN PODER INSERTAR CLIENTE <%s>\n\r", odbcSession.GetErrorMessage(NULL) );		    
									fprintf(fTracefile,"CONTENIDO :  [%s]\n\r" ,  (char *)strQuery.c_str()  );
									fflush(fTracefile);
								}
							};
                            odbcSession.ResetLastError();
                        }; /* if */

#if ( FALSE )
						/* NOVEDAD DEL CLIENTE XXXXXX, ASUMIDO COMO ALTA AL PRINCIPIO */
                        strQuery = "INSERT INTO NOVEDA";
						strQuery += "( NV_TIPNOV , NV_TIPCTA, NV_CUENTA, NV_ORDEN, NV_FECBAJ )";
						strQuery += "VALUES ";
						strQuery += "( 1, " ; 
						/* ACCT TYPE */
						strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_TYP, 0, 2) ;
						strQuery += ", '";
						/* ******************************************* */
						/* COD-CLIENTE = NRO. CUENTA PARA ESTE ACUERDO */
	                    strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_NUM.DR_NUM1, 0, 3) ;
						strQuery.append( pbfRecord->REFRESH_BASE24.DR_PRIKEY.DR_NUM.DR_NUM2, _OFFSET_NUM_CUENTA_PBF_, 16 - _OFFSET_NUM_CUENTA_PBF_) ;							
						/* ******************************************* */
						strQuery += "', '0' , '1900-01-01' )";
                        iSentence = 6;
                        odbcSession.Query( (char *)strQuery.c_str(), &iSentence );
                        if( odbcSession.Error() )
                        {
							lRetval = odbcSession.GetStatus();
							if( odbcSession.GetReturnCode() == -1 )
							{
								if( strstr( odbcSession.GetErrorMessage(NULL), "PRIMARY KEY") )
								{									
								}
								else
								{
									fprintf(fTracefile,"SIN PODER INSERTAR NOVEDAD <%s>\n\r", odbcSession.GetErrorMessage(NULL) );		    
									fprintf(fTracefile,"CONTENIDO :  [%s]\n\r" ,  (char *)strQuery.c_str()  );
									fflush(fTracefile);
								}
							};
                            odbcSession.ResetLastError();
                        }; /* if */
#endif 

				};

            }; /* for SLOT */

        }; /* while FREAD */

        /* close file */
        //fclose(hFile);
        //hFile = NULL;
		ifs.close();

		/* display counter */
		if( 1 == nStep )
		{
			fprintf(fTracefile,"\r\n Registros PBF - MAESTRO CUENTAS   : <%i> ", iCounter );
			fprintf(fTracefile,"\r\n Errores en registros : <%i> "             , iErrCount );
		}
		else if( 2 == nStep )
		{
			fprintf(fTracefile,"\r\n Registros UMOV - DB/CR EN CUENTAS : <%i> ", iCounter );
			fprintf(fTracefile,"\r\n Errores en registros : <%i> "             , iErrCount );
		}
		else
			fprintf(fTracefile,"\r\n Registros : <%i> ", iCounter );
		fflush(fTracefile);


    };

	odbcSession.Disconnect();
	fprintf(fTracefile,"\r\n Fin");

	fclose(fTracefile);

	/* OK */
	return 0;
}; 
