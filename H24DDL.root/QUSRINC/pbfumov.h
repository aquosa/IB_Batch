//////////////////////////////////////////////////////////////////////////////////////////////////
// IT24 OUTSOURCING S.A. / COA S.A.
// RECORD DEFINITION FOR PBF-MOV OF BASE24-ATM
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    2006.10.20      mdc     Base24-ATM: PBF UMOV 
// (Inicial)    2007.04.18      mdc     Redefinicion de DESCRIPCION con NRO.COMPROBANTE
// (Inicial)    2007.05.02      mdc     _CMOVATM_CODCONCEP_LEN_ (3) /* COD.CONCEP. LENGTH */
// (BETA)       2011.06.21      mdc     UMOV LK, con importe de +8 digitos S9(6)V99
// (BETA)       2011.09.19      mdc     CBU en registro PBF desde BT (REFRESH_BASE24_CBU_t)
// (1.0.0.1)    2011.11.15      mdc     Se agrega secuencial BT en interfase UMOV para evitar duplicados
// (Inicial)    2012.02.10      mdc     Aumenta en 3 posiciones el campo importe.
// (Inicial)    2013.02.28      mdc     ORIGINAL : CMOVATM_SALDO  en [8]    #if ( _USE_EXTENDED_AMMOUNT_ )
// (2.0.0.0)    2013.05.17      mdc     _UMOVPBF_USE_UMOV_DESCRIPCION_		(FALSE) y ademas SP modificado % 9971
// (2.0.0.1)    2013.05.28      mdc     Fix Nro. Movimiento se agrega al UMOV, que ya no es el estandar////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PBFUMOV_H_
#define _PBFUMOV_H_

// CURRENCY CODES FOR THE MONEY INVOLVED
#ifndef B24_CURRENCY_ARGENTINA
#define B24_CURRENCY_ARGENTINA	"32"   // PESOS ARGENTINA
#define B24_CURRENCY_USA		"840"  // DOLARES	USA
#define B24_CURRENCY_BRAZIL		"76"   // REALES BRAZIL
#define B24_CURRENCY_URUGUAY	"858"  // URUGUAYOS URUGUAY
#define B24_CURRENCY_LECOP		"996"  // LECOP ARGENTINA
#endif


// SI SE FUERA A USAR LA ABREVIATURA DE LA DESCRIPCION, INDICARLO CON ESTA MACRO.
#undef _UMOVPBF_USE_UMOV_DESCRIPCION_
// SI SE FUERA A USAR EL CODIGO DE CONCEPTO DEL BANCO O DE DATANET, INDICARLO CON ESTA MACRO.
#undef _UMOVPBF_USE_UMOV_COD_CONCEP_		
// POR OMISION, ES LA ABREVIATURA
#define _UMOVPBF_USE_UMOV_DESCRIPCION_		(FALSE)
#define _UMOVPBF_USE_UMOV_COD_CONCEP_		(TRUE)

/******************************************************************/
typedef union uREFRESH_PBF_t
{
   struct REFRESH_FILE_HEADER_t
   {
     char FH_REC_CNT                   [07]      ;              
     char FH_REC_TYP                   [02]      ;              
     char FH_REF_TYP                   [01]      ;              
     char FH_APPL                      [02]      ;              
     char FH_GRP                       [04]      ;              
     char FH_DAT                       [06]      ;              
     char FH_CNTL_NUM                  [04]      ;              
     char FH_LN                        [04]      ;              
     char FH_REL_NUM                   [02]      ;              
     char FH_CAF_ACCT_CNT              [02]      ;              
     char FH_PARTITION_NUM             [02]      ;              
     char FH_IMP_TIM                   [06]      ;              
     char FH_IMP_TYP                   [01]      ;              
     char FH_SEG_MAP                   [32]      ;              
     char FH_FILLER                    [41]      ;            
   } REFRESH_FILE_HEADER;

   struct REFRESH_ORG_HEADER_t
   {
     char OH_REC_CNT                   [07]      ;              
     char OH_REC_TYP                   [02]      ;              
     char OH_CRD_ISS                   [04]      ;              
     char OH_FILLER                    [103]     ;        
   } REFRESH_ORG_HEADER ;

   struct REFRESH_ORG_TRAILER_t
   {
     char OT_REC_CNT                   [07]      ;              
     char OT_REC_TYP                   [02]      ;              
     char OT_AMT                       [18]		 ;     
     char OT_NUM_RECS                  [07]      ;              
     char OT_FILLER                    [82]      ;              
   } REFRESH_ORG_TRAILER;

   struct REFRESH_FILE_TRAILER_t
   {
     char FT_REC_CNT                   [07]      ;              
     char FT_REC_TYP                   [02]      ;              
     char FT_NUM_RECS                  [07]      ;              
     char FT_FILLER                    [100]     ;              
   } REFRESH_FILE_TRAILER;

   struct REFRESH_BASE24_t
   {
     char DR_LGTH                      [04]      ;              
     char DR_CNT                       [07]      ;              
     struct DR_PRIKEY_t
     {
         char  DR_FIID                  [04]      ;              
         struct DR_NUM_t
         {
             char  DR_NUM1              [03]      ;              
             char  DR_NUM2              [16]      ;              
         } DR_NUM;
         char  DR_TYP                   [02]      ;              
     } DR_PRIKEY;
     char DR_ACCT_STAT                 [01]      ;              
     char DR_REC_TYP                   [01]      ;              
     char  DR_AVAIL_BAL                [12]    ;              
     char  DR_LEDG_BAL                 [12]    ;
     char DR_AMT_ON_HLD                [12]   ;              
     char DR_OVRDRFT_LMT               [10]      ;              
     char DR_WITH_ADV_AVAIL            [02]      ;              
     char DR_INT_OWE_AUST              [12]   ;              
     struct DR_PAYMENT_DAT_t
     {
         char  DR_PAYMENT_AA            [02]      ;              
         char  DR_PAYMENT_MM            [02]      ;              
         char  DR_PAYMENT_DD            [02]      ;              
     } DR_PAYMENT_DAT;
     char DR_OWE_DOLAR                 [10]   ;              
     char DR_FILLER                    [02]      ;              
   } REFRESH_BASE24;

   struct REFRESH_BASE24_CBU_t
   {
     char DR_LGTH                      [04]      ;              
     char DR_CNT                       [07]      ;              
     struct DR_PRIKEY_t
     {
         char  DR_FIID                  [04]      ;              
         struct DR_NUM_t
         {
             char  DR_NUM1              [03]      ;              
             char  DR_NUM2              [16]      ;              
         } DR_NUM;
         char  DR_TYP                   [02]      ;              
     } DR_PRIKEY;
     char DR_ACCT_STAT                 [01]  ;              
     char DR_REC_TYP                   [01]  ;              
     char  DR_AVAIL_BAL                [12]  ;              
     char  DR_LEDG_BAL                 [12]  ;
     char DR_AMT_ON_HLD                [12]  ;              
     char DR_OVRDRFT_LMT               [10]  ;              
     char DR_WITH_ADV_AVAIL            [02]  ;              
     char FILLER                       [8]   ;              
	 CHAR CBU                          [22]  ;              
   } REFRESH_BASE24_CBU;

} uREFRESH_PBF;


#define _CMOVATM_CODCONCEP_LEN_ (3) /* COD.CONCEP. LENGTH */

/******************************************************************/
typedef union uCMOVATM_t
{
  struct CMOVATM_HEADER_t
  {
     char CMOVATM_IDENTIFH             [13]      ;            
     char CMOVATM_CODBCO               [4]      ;            
     struct CMOVATM_FECPROC_t
     {
         char CMOVATM_AA               [2];
         char CMOVATM_MM               [2];
         char CMOVATM_DD               [2];
     } CMOVATM_FECPROC;
     char CMOVATM_TIPOCTA              [2]      ;            
         /*  CTA_CTE_PESOS          VALUE  01       */
         /*  CTA_CTE_DOLARES        VALUE  07       */            
         /*  AHORROS_PESOS          VALUE  11       */           
         /*  AHORROS_DOLARES        VALUE  15       */           
         /*  TARJETA_CREDITO        VALUE  31       */           
     char CMOVATM_FILLER1              [275]     ;            
  } CMOVATM_HEADER;
  struct CMOVATM_DATOS_t
  {
     struct CMOVATM_CUENTA_t
     {
         char CMOVATM_SUC              [3]      ;            
         char CMOVATM_NROCUEN          [15]      ;            
     } CMOVATM_CUENTA;
     char CMOVATM_SASIGNO                        ;            
#if ( _USE_EXTENDED_AMMOUNT_ )
     char CMOVATM_SALDO                [10]     ;            
#else
	 char CMOVATM_SALDO                [8]     ;            
#endif
     struct CMOVATM_TABLA_MOVIM_t
     { 
         struct CMOVATM_FECHA_t
         {
             char CMOVATM_FEAA         [2]      ;            
             char CMOVATM_FEMM         [2]      ;            
             char CMOVATM_FEDD         [2]      ;            
         } CMOVATM_FECHA;
		 /* 2007.04.18 - REDEFINES CONCEPTO : NRO.COMPROBANTE + COD.CONCEPT. */
		 union  DESCRIPCION_u
		 {
			char CMOVATM_CONCEPTO      [10]      ;
			struct COD_CONCEP_NRO_COMPROB_t
			{
				char CMOVATM_CODCONCEP[3]; /* COD.CONCEP.  */
				char CMOVATM_NROCOMPRO[7]; /* NRO.COMPROB. */
			} COD_CONCEP_NRO_COMPROB;
		 } DESCRIPCION;
		 /* 2007.04.18 - REDEFINES CONCEPTO : NRO.COMPROBANTE + COD.CONCEPT. */
         char CMOVATM_IMSIGNO                ;
#if ( _USE_EXTENDED_AMMOUNT_ )
         char CMOVATM_IMPORTE          [10]   ;            
#else
         char CMOVATM_IMPORTE          [8]   ;            
#endif
     } CMOVATM_TABLA_MOVIM[10];
     char CMOVATM_FILLER2              [1]      ;            
  } CMOVATM_DATOS;
  struct CMOVATM_TRAILER_t
  {
     char CMOVATM_IDENTIFT             [13]      ;            
     char CMOVATM_CANTREG              [8]      ;            
     char CMOVATM_FILLER3              [279]     ;            
  } CMOVATM_TRAILER;
} uCMOVATM;

/******************************************************************/
typedef union uCMOVATM_LK_t
{
  struct CMOVATM_HEADER_t
  {
     char CMOVATM_IDENTIFH             [13]      ;            
     char CMOVATM_CODBCO               [4]      ;            
     struct CMOVATM_FECPROC_t
     {
         char CMOVATM_AA               [2];
         char CMOVATM_MM               [2];
         char CMOVATM_DD               [2];
     } CMOVATM_FECPROC;
     char CMOVATM_TIPOCTA              [2]      ;            
     char CMOVATM_FILLER1              [275]     ;            
	char CMOVATM_FILLER2               [330]    ; 
  } CMOVATM_HEADER;
  struct CMOVATM_DATOS_t
  {
     struct CMOVATM_CUENTA_t
     {
         char CMOVATM_SUC              [3]      ;            
         char CMOVATM_NROCUEN          [15]      ;            
     } CMOVATM_CUENTA;
     char CMOVATM_SASIGNO                        ;            
     char CMOVATM_SALDO                [8]     ;            
     struct CMOVATM_TABLA_MOVIM_t
     { 
         struct CMOVATM_FECHA_t
         {
             char CMOVATM_FEAA         [2]      ;            
             char CMOVATM_FEMM         [2]      ;            
             char CMOVATM_FEDD         [2]      ;            
         } CMOVATM_FECHA;
         char CMOVATM_CONCEPTO         [10]      ;
         char CMOVATM_IMSIGNO                    ;
         char CMOVATM_IMPORTE          [8+3]     ;  // FIX 2012.02.10 mdc Aumenta +3 posiciones el campo importe.
		 char CMOVATM_SECUENCIAL       [25]      ; // YYYYMMDDmmmtttsssrrr
		 char CMOVATM_NRO_SOBRE_CH     [8]       ; // 99999 CHEQUE, SOBRE, ETC.
     } CMOVATM_TABLA_MOVIM[10];
     char CMOVATM_FILLER2              [23]      ;            
  } CMOVATM_DATOS;
  struct CMOVATM_TRAILER_t
  {
     char CMOVATM_IDENTIFT             [13]      ;            
     char CMOVATM_CANTREG              [8]      ;            
     char CMOVATM_FILLER3              [279]     ;            
	char CMOVATM_FILLER4               [330]    ; 
  } CMOVATM_TRAILER;
} uCMOVATM_LK;

/******************************************************************/
typedef union uCMOVATM_100_t
{
  struct CMOVATM_HEADER_t
  {
     char CMOVATM_IDENTIFH             [13]      ;            
     char CMOVATM_CODBCO               [4]      ;            
     struct CMOVATM_FECPROC_t
     {
         char CMOVATM_AA               [2];
         char CMOVATM_MM               [2];
         char CMOVATM_DD               [2];
     } CMOVATM_FECPROC;
     char CMOVATM_TIPOCTA              [2]      ;            
         /*  CTA_CTE_PESOS          VALUE  01       */
         /*  CTA_CTE_DOLARES        VALUE  07       */            
         /*  AHORROS_PESOS          VALUE  11       */           
         /*  AHORROS_DOLARES        VALUE  15       */           
         /*  TARJETA_CREDITO        VALUE  31       */           
     char CMOVATM_FILLER1              [275]     ;            
  } CMOVATM_HEADER;
  struct CMOVATM_DATOS_t
  {
     struct CMOVATM_CUENTA_t
     {
         char CMOVATM_SUC              [3]      ;            
         char CMOVATM_NROCUEN          [15]      ;            
     } CMOVATM_CUENTA;
     char CMOVATM_SASIGNO                        ;            
#if ( _USE_EXTENDED_AMMOUNT_ )
     char CMOVATM_SALDO                [10]     ;            
#else
	 char CMOVATM_SALDO                [8]     ;            
#endif
     struct CMOVATM_TABLA_MOVIM_t
     { 
         struct CMOVATM_FECHA_t
         {
             char CMOVATM_FEAA         [2]      ;            
             char CMOVATM_FEMM         [2]      ;            
             char CMOVATM_FEDD         [2]      ;            
         } CMOVATM_FECHA;
		 /* 2007.04.18 - REDEFINES CONCEPTO : NRO.COMPROBANTE + COD.CONCEPT. */
		 union  DESCRIPCION_u
		 {
			char CMOVATM_CONCEPTO      [10 *10]      ;	// 100 MOVIMIENTOS
			struct COD_CONCEP_NRO_COMPROB_t
			{
				char CMOVATM_CODCONCEP[3]; /* COD.CONCEP.  */
				char CMOVATM_NROCOMPRO[7]; /* NRO.COMPROB. */
			} COD_CONCEP_NRO_COMPROB;
		 } DESCRIPCION;
		 /* 2007.04.18 - REDEFINES CONCEPTO : NRO.COMPROBANTE + COD.CONCEPT. */
         char CMOVATM_IMSIGNO                ;
#if ( _USE_EXTENDED_AMMOUNT_ )
         char CMOVATM_IMPORTE          [10]   ;            
#else
         char CMOVATM_IMPORTE          [8]   ;            
#endif
     } CMOVATM_TABLA_MOVIM[10];
     char CMOVATM_FILLER2              [1]      ;            
  } CMOVATM_DATOS;
  struct CMOVATM_TRAILER_t
  {
     char CMOVATM_IDENTIFT             [13]      ;            
     char CMOVATM_CANTREG              [8]      ;            
     char CMOVATM_FILLER3              [279]     ;            
  } CMOVATM_TRAILER;
} uCMOVATM_100;

/******************************************************************/
typedef union uCMOVATM_METRO_t
{
  struct CMOVATM_HEADER_t
  {
     char CMOVATM_IDENTIFH             [13]      ;            
     char CMOVATM_CODBCO               [4]      ;            
     struct CMOVATM_FECPROC_t
     {
         char CMOVATM_AA               [2];
         char CMOVATM_MM               [2];
         char CMOVATM_DD               [2];
     } CMOVATM_FECPROC;
     char CMOVATM_TIPOCTA              [2]      ;            
         /*  CTA_CTE_PESOS          VALUE  01       */
         /*  CTA_CTE_DOLARES        VALUE  07       */            
         /*  AHORROS_PESOS          VALUE  11       */           
         /*  AHORROS_DOLARES        VALUE  15       */           
         /*  TARJETA_CREDITO        VALUE  31       */           
     char CMOVATM_FILLER1              [275]     ;  
	 /* 2013.05.24 - 50 bytes MAS DE LONGITUD DE REGISTRO */
     char CMOVATM_FILLER2              [5*10]    ;  
  } CMOVATM_HEADER;
  struct CMOVATM_DATOS_t
  {
     struct CMOVATM_CUENTA_t
     {
         char CMOVATM_SUC              [3]      ;            
         char CMOVATM_NROCUEN          [15]      ;            
     } CMOVATM_CUENTA;
     char CMOVATM_SASIGNO                        ;            
#if ( _USE_EXTENDED_AMMOUNT_ )
     char CMOVATM_SALDO                [10]     ;            
#else
	 char CMOVATM_SALDO                [8]     ;            
#endif
     struct CMOVATM_TABLA_MOVIM_t
     { 
         struct CMOVATM_FECHA_t
         {
             char CMOVATM_FEAA         [2]      ;            
             char CMOVATM_FEMM         [2]      ;            
             char CMOVATM_FEDD         [2]      ;            
         } CMOVATM_FECHA;
		 /* 2007.04.18 - REDEFINES CONCEPTO : NRO.COMPROBANTE + COD.CONCEPT. */
		 union  DESCRIPCION_u
		 {
			char CMOVATM_CONCEPTO      [10]      ;
			struct COD_CONCEP_NRO_COMPROB_t
			{
				char CMOVATM_CODCONCEP[3]; /* COD.CONCEP.  */
				char CMOVATM_NROCOMPRO[7]; /* NRO.COMPROB. */
			} COD_CONCEP_NRO_COMPROB;
		 } DESCRIPCION;
		 /* 2007.04.18 - REDEFINES CONCEPTO : NRO.COMPROBANTE + COD.CONCEPT. */
         char CMOVATM_IMSIGNO                ;
#if ( _USE_EXTENDED_AMMOUNT_ )
         char CMOVATM_IMPORTE          [10]  ;            
#else
         char CMOVATM_IMPORTE          [8]   ;            
#endif
		 /* 2013.05.24 - NRO MOVIMIENTO INTERNO - NO ES SECUENCIA */
         char CMOVATM_NROMOVIM         [5]   ;   
     } CMOVATM_TABLA_MOVIM[10];
     char CMOVATM_FILLER2              [1]   ;            
  } CMOVATM_DATOS;
  struct CMOVATM_TRAILER_t
  {
     char CMOVATM_IDENTIFT             [13]      ;            
     char CMOVATM_CANTREG              [8]      ;            
     char CMOVATM_FILLER3              [279]     ;            
	 /* 2013.05.24 - 50 bytes MAS DE LONGITUD DE REGISTRO */
     char CMOVATM_FILLER2              [5*10]    ;  
  } CMOVATM_TRAILER;
} uCMOVATM_METRO;


// ESTRUCTURA INTERFINANZAS...
typedef union UMOVINTERF_t
{
  struct CMOVATM_HEADER_t
  {
     char CMOVATM_IDENTIFH             [13]      ;            
     char CMOVATM_CODBCO               [4]      ;            
     struct CMOVATM_FECPROC_t
     {
         char CMOVATM_AA               [2];
         char CMOVATM_MM               [2];
         char CMOVATM_DD               [2];
     } CMOVATM_FECPROC;
     char CMOVATM_TIPOCTA              [2]      ;    
	 // EAE ->
	 struct CMOVATM_FECCIE_t
	 {
         char CMOVATM_AA               [2];
         char CMOVATM_MM               [2];
         char CMOVATM_DD               [2];
	 }CMOVATM_FECCIE;
     char CMOVATM_FILLER1              [269]     ;            
	 //char CMOVATM_FILLER1              [275]     ;            
	 // EAE <-
	char CMOVATM_FILLER2               [330]    ; 
  } CMOVATM_HEADER;
  struct CMOVATM_DATOS_t
  {
     struct CMOVATM_CUENTA_t
     {
         char CMOVATM_SUC              [3]      ;            
         char CMOVATM_NROCUEN          [15]      ;            
     } CMOVATM_CUENTA;
     char CMOVATM_SASIGNO                        ;            
     char CMOVATM_SALDO                [8+3]     ;            
     struct CMOVATM_TABLA_MOVIM_t
     { 
         struct CMOVATM_FECHA_t
         {
             char CMOVATM_FEAA         [2]      ;            
             char CMOVATM_FEMM         [2]      ;            
             char CMOVATM_FEDD         [2]      ;            
         } CMOVATM_FECHA;
         char CMOVATM_CONCEPTO         [10]      ;
         char CMOVATM_IMSIGNO                    ;
         char CMOVATM_IMPORTE          [8+3]     ;  // FIX 2012.02.10 mdc Aumenta +3 posiciones el campo importe.
		 char CMOVATM_SECUENCIAL       [25]      ; // YYYYMMDDmmmtttsssrrr
		 char CMOVATM_NRO_SOBRE_CH     [8]       ; // 99999 CHEQUE, SOBRE, ETC.
		 char CMOVATM_CUIT_ORIGINANTE  [11]		 ;
		 char CMOVATM_DENO_ORIGINANTE  [40]      ;
     } CMOVATM_TABLA_MOVIM[10];
     char CMOVATM_FILLER2              [20]      ;            
  } CMOVATM_DATOS;
  struct CMOVATM_TRAILER_t
  {
     char CMOVATM_IDENTIFT             [13]      ;            
     char CMOVATM_CANTREG              [8]      ;            
     char CMOVATM_FILLER3              [279]     ;            
	char CMOVATM_FILLER4               [330]    ; 
  } CMOVATM_TRAILER;
} UMOVINTERF;
typedef union REFRESHPBFINTERF_t
{
   struct REFRESH_FILE_HEADER_t
   {
     char FH_REC_CNT                   [07]      ;              
     char FH_REC_TYP                   [02]      ;              
     char FH_REF_TYP                   [01]      ;              
     char FH_APPL                      [02]      ;              
     char FH_GRP                       [04]      ;              
     char FH_DAT                       [06]      ;              
     char FH_CNTL_NUM                  [04]      ;              
     char FH_LN                        [04]      ;              
     char FH_REL_NUM                   [02]      ;              
     char FH_CAF_ACCT_CNT              [02]      ;              
     char FH_PARTITION_NUM             [02]      ;              
     char FH_IMP_TIM                   [06]      ;              
     char FH_IMP_TYP                   [01]      ;              
     char FH_SEG_MAP                   [32]      ;              
     char FH_FILLER                    [41]      ;            
   } REFRESH_FILE_HEADER;

   struct REFRESH_ORG_HEADER_t
   {
     char OH_REC_CNT                   [07]      ;              
     char OH_REC_TYP                   [02]      ;              
     char OH_CRD_ISS                   [04]      ;              
     char OH_FILLER                    [103]     ;        
   } REFRESH_ORG_HEADER ;

   struct REFRESH_ORG_TRAILER_t
   {
     char OT_REC_CNT                   [07]      ;              
     char OT_REC_TYP                   [02]      ;              
     char OT_AMT                       [18]		 ;     
     char OT_NUM_RECS                  [07]      ;              
     char OT_FILLER                    [82]      ;              
   } REFRESH_ORG_TRAILER;

   struct REFRESH_FILE_TRAILER_t
   {
     char FT_REC_CNT                   [07]      ;              
     char FT_REC_TYP                   [02]      ;              
     char FT_NUM_RECS                  [07]      ;              
     char FT_FILLER                    [100]     ;              
   } REFRESH_FILE_TRAILER;

   struct REFRESH_BASE24_t
   {
     char DR_LGTH                      [04]      ;              
     char DR_CNT                       [07]      ;              
     struct DR_PRIKEY_t
     {
         char  DR_FIID                  [04]      ;              
         struct DR_NUM_t
         {
             char  DR_NUM1              [03]      ;              
             char  DR_NUM2              [16]      ;              
         } DR_NUM;
         char  DR_TYP                   [02]      ;              
     } DR_PRIKEY;
     char DR_ACCT_STAT                 [01]      ;              
     char DR_REC_TYP                   [01]      ;              
     char  DR_AVAIL_BAL                [12]    ;              
     char  DR_LEDG_BAL                 [12]    ;
     char DR_AMT_ON_HLD                [12]   ;              
     char DR_OVRDRFT_LMT               [10]      ;              
     char DR_WITH_ADV_AVAIL            [02]      ;              
     char DR_INT_OWE_AUST              [12]   ;              
     struct DR_PAYMENT_DAT_t
     {
         char  DR_PAYMENT_AA            [02]      ;              
         char  DR_PAYMENT_MM            [02]      ;              
         char  DR_PAYMENT_DD            [02]      ;              
     } DR_PAYMENT_DAT;
     char DR_OWE_DOLAR                 [10]   ;              
     char DR_FILLER                    [02]      ;              
   } REFRESH_BASE24;

   struct REFRESH_BASE24_CBU_t
   {
     char DR_LGTH                      [04]      ;              
     char DR_CNT                       [07]      ;              
     struct DR_PRIKEY_t
     {
         char  DR_FIID                  [04]      ;              
         struct DR_NUM_t
         {
             char  DR_NUM1              [03]      ;              
             char  DR_NUM2              [16]      ;              
         } DR_NUM;
         char  DR_TYP                   [02]      ;              
     } DR_PRIKEY;
     char DR_ACCT_STAT                 [01]  ;              
     char DR_REC_TYP                   [01]  ;              
     char  DR_AVAIL_BAL                [12]  ;              
     char  DR_LEDG_BAL                 [12]  ;
     char DR_AMT_ON_HLD                [12]  ;              
     char DR_OVRDRFT_LMT               [10]  ;              
     char DR_WITH_ADV_AVAIL            [02]  ;              
     char FILLER                       [8]   ;              
	 CHAR CBU                          [22]  ;              
   } REFRESH_BASE24_CBU;

} REFRESHPBFINTERF;


/*---------------------------------------------------------------------------*/
/* Procesar_Batch_Umov_PBF:                                                  */
/*---------------------------------------------------------------------------*/
int EXPORT Procesar_Batch_Umov_PBF(int argc, char * argv []) ;
/*---------------------------------------------------------------------------*/

#endif /* PBFUMOV_H */

