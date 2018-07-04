///////////////////////////////////////////////////////////////////////////////////////////
//
// COA SA / IT24 OUTSOURCING
// DATANET-BATCH D.O.S. program for TDF - Batch interfases to INTERBANKING
//
// Tarea        Fecha           Autor   Observaciones
// [ALFA]       2006.09.18      mdc     Inicial basado en Finansur Batch DPS.
// [ALFA]       2006.10.10      mdc     #define _ISO_ACCT_PICTURE_LENGHT_   (28)      
// [ALFA]       2006.10.24      mdc     Check CutOver time 15:00hs 
// [ALFA]       2006.10.30      mdc     PADCOPYFIXEDINT_ASCII() reemplazo al anterior.
// [ALFA]       2006.10.31      mdc     FIX A PROMEDIOS MENSUALES X MES 
// [ALFA]       2006.11.07      mdc     FIX PARA "INSERT INTO NOVEDAD " 
// [ALFA]       2006.11.13      mdc     FIX PARA DISCRIMINAR C.C. DE C.A.
// [ALFA]       2006.11.14      mdc     FIX PARA FECHA ORIGINAL EN DIFERIDOS
// [ALFA]       2006.11.17      mdc     FIX AHODIF "NUMCOR" COMPUTATIONAL-3
// [ALFA]       2007.03.14      mdc     Tener en cuenta BASE DATOS por parametros, y 
//                                      igualar TIPCTA en NOVEDAD con el MAESTRO.
// [ALFA]       2007.03.18      mdc     #include <qusrinc/pbfumov.h>
// [ALFA]       2007.03.18      mdc     ELIMINAR RANGO POSIBLE DE CR+LF (0D+0A)
// [ALFA]       2007.04.27      mdc     FIX ODBC DRIVER , cambia tipo CHAR(19) x FLOAT 
// [ALFA]       2007.05.02      mdc     FIX NUEVOS MAPEOS COD.OPE. : aszCODOPE_CC, aszCODOPE_CA .
// [ALFA]       2012.12.04      mdc     FIX anular mapeos tras migracion del core.
// [ALFA]       2012.12.04      mdc     FIX agregar razon social del abonado (abonado_cuenta + abonado_clave)
// [FIX]		2013.08.08		JAF		MODIFICO CONFIG PARA BCO. INTERF
// [FIX]		2013.08.08		JAF		AGREGO SETEO DE CONFIG REGIONAL Y AVISO SI NO SE GENERAN MOVIMIENTOS - GCP ID: 14136
// [FIX]		2015.12.16		mdc		_SYSTEM_DATANET_INTERFINANZAS_ , picture de 12.
// [FIX]		2015.12.22		mdc		AHSADO, campos sobrantes quitados, fix.
///////////////////////////////////////////////////////////////////////////////////////////
//
// FuegoDPSDlg.cpp: archivo de implementación
//

#include "stdafx.h"
#include "FuegoDPS.h"
#include "FuegoDPSDlg.h"
//#include ".\fuegodpsdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Headers ANSI y non-ANSI
#include <sqlext.h>
#include <time.h>
#include <locale.h>


// Header STRING STD
#include <string>
#include <map>
#include <algorithm> 
#include <cctype>
#include <math.h>
#include <sstream>
#include <functional>
#include <iomanip>
#include <list>

using namespace std;

// Headers del Sistema DPS-BATCH
#include <qusrinc/datanet.h>
#include <qusrinc/TRXDNET.h>
#define _USE_FIXED_INTEGER_
#include <qusrinc/copymacr.h>
#include <qusrinc/databatc.h>
//#include ".\fuegodpsdlg.h"

#include <qusrinc/ebc2asci.h>
#include <qusrinc/extrctst.h>
#include <qusrinc/pbfumov.h>


#include "QUSRINC\opciones.h"

/*** picture propio para la instalacion especifica de cliente ***/
#define _SYSTEM_DATANET_INTERFINANZAS_

/*********************************************/
#ifdef _SYSTEM_DATANET_INTERFINANZAS_
#define _DTN_ACCT_PICTURE_LENGHT_   (3+9)   /* verificado : suc.logica + numero       */
#define _ISO_ACCT_PICTURE_LENGHT_   (3+9+1) /* y se agrega la suc.fisica como prefijo */
#define _ACCT_INTERNAL_LENGHT_      (3+9)   /* unicamente al momento del db/cr        */
/*********************************************/
#elif defined( _SYSTEM_DATANET_TDFUEGO_)
#define _DTN_ACCT_PICTURE_LENGHT_   (3+2+9)   /* verificado : suc.logica + numero       */
#define _ISO_ACCT_PICTURE_LENGHT_   (3+2+9+1) /* y se agrega la suc.fisica como prefijo */
#define _ACCT_INTERNAL_LENGHT_      (3+2+9)   /* unicamente al momento del db/cr        */
/*********************************************/
#else
#error 'FALTA DEFINIR LA LONGITUD DE CUENTA, EXTERNA E INTERNA'
#endif


//*************************************************************************//
typedef struct stNUMCTA_tag
  {
    char chNUMCTA [_DTN_ACCT_PICTURE_LENGHT_];
    char chOVERLAY[_DTN_ACCT_PICTURE_LENGHT_]; // FILLER para poner el 0x00    
  } stNUMCTA_t;
//*************************************************************************//
// RECORD TYPE 
typedef enum  enumRECTYPE { RT_MOVCUE, RT_MOVDIF, RT_AHOMOV, RT_AHODIF, RT_NONE,RT_AHMODO,RT_AHDIDO} ;
//*************************************************************************//

//**********************************************************************************//
// TABLA DE CODIGOS DE OPERACION BATCH PROPIOS A INTERBANKING                       //
//**********************************************************************************//
typedef struct aszREGOPE_tag
{
    char *INTERNO, *MAPEADO ;
} aszREGOPE_t;


// FIX anular mapeos tras migracion del core. Eliminar tablas obsoletas .
#if ( TRUE )

const aszREGOPE_t aszCODOPE_CC[] = { 0x00 };
const aszREGOPE_t aszCODOPE_CA[] = { 0x00 };

#else

//**********************************************************************************//
const aszREGOPE_t aszCODOPE_CC[] =
	{
	"INT","MAP",
	"001","045",
	"002","044",
	"003","043",
	"004","461",
	"005","S25",
	"006","M55",
	"007","819",
	"008","817",
	"009","O99",
	"010","M35",
	"011","943",
	"012","894",
	"013","413",
	"014","769",
	"015","033",
	"016","U21",
	"017","575",
	"018","754",
	"019","753",
	"020","765",
	"021","775",
	"022","830",
	"023","668",
	"024","170",
	"025","921",
	"026","N03",
	"027","947",
	"028","O99",
	"029","777",
	"030","587",
	"031","779",
	"032","170",
	"033","813",
	"034","S08",
	"035","S08",
	"036","962",
	"037","869",
	"038","814",
	"039","026",
	"040","107",
	"041","869",
	"042","M32",
	"043","350",
	"044","S36",
	"045","692",
	"046","584",
	"047","Q02",
	"048","750",
	"049","869",
	"050","Q07",
	"051","O89",
	"052","O59",
	"053","674",
	"054","R23",
	"055","P31",
	"056","M35",
	"057","897",
	"058","833",
	"059","S69",
	"060","S69",
	"061","765",
	"062","797",
	"063","O59",
	"064","672",
	"065","S34",
	"066","S09",
	"067","M42",
	"068","M42",
	"069","S31",
	"070","467",
	"072","674",
	"073","758",
	"074","758",
	"075","758",
	"076","758",
	"077","758",
	"078","758",
	"079","959",
	"080","839",
	"081","765",
	"082","P69",
	"083","N33",
	"084","S36",
	"085","592",
	"086","P05",
	"087","984",
	"088","T72",
	"089","981",
	"090","N26",
	"091","914",
	"092","M55",
	"093","T55",
	"100","961",
	"101","797",
	"102","765",
	"109","Q52",
	"110","959",
	"111","T44",
	"112","959",
	"113","959",
	"114","959",
	"115","959",
	"116","959",
	"117","T43",
	"118","959",
	"119","959",
	"120","959",
	"121","959",
	"122","O62",
	"123","O64",
	"124","959",
	"125","T44",
	"126","959",
	"127","959",
	"128","959",
	"129","959",
	"130","959",
	"131","959",
	"132","959",
	"133","959",
	"134","959",
	"135","959",
	"136","959",
	"137","959",
	"138","959",
	"139","S36",
	"141","959",
	"142","S36",
	"143","959",
	"144","959",
	"145","959",
	"146","959",
	"147","959",
	"148","S36",
	"149","959",
	"150","959",
	"151","959",
	"152","959",
	"153","959",
	"154","959",
	"155","959",
	"156","959",
	"157","834",
	"158","959",
	"159","959",
	"160","959",
	"161","959",
	"162","959",
	"163","144",
	"164","144",
	"165","959",
	"166","959",
	"167","959",
	"168","959",
	"169","959",
	"170","959",
	"171","637",
	"172","869",
	"173","S36",
	"174","949",
	"175","784",
	"176","966",
	"177","819",
	"178","817",
	"179","959",
	"180","S36",
	"181","959",
	"182","959",
	"183","959",
	"184","959",
	"186","959",
	"187","959",
	"188","959",
	"189","959",
	"190","959",
	"191","959",
	"192","959",
	"193","959",
	"194","959",
	"195","959",
	"196","959",
	"197","S09",
	"198","M42",
	"199","966",
	"200","S25",
	"201","S25",
	"202","S25",
	"203","S25",
	"204","S25",
	"205","S25",
	"206","S25",
	"207","S25",
	"208","S25",
	"209","S25",
	"210","S25",
	"211","S25",
	"212","S25",
	"213","S25",
	"214","S25",
	"215","S25",
	"216","S25",
	"217","S25",
	"218","S25",
	"219","S25",
	"220","S25",
	"221","S25",
	"222","S25",
	"223","S25",
	"224","S25",
	"225","S25",
	"226","S25",
	"227","S25",
	"228","S25",
	"250","M97",
	"300","S42",
	"301","S42",
	"302","S42",
	"303","S42",
	"304","S42",
	"305","S42",
	"306","S42",
	"307","S42",
	"308","S42",
	"309","S42",
	"310","S42",
	"311","S42",
	"312","S42",
	"313","S42",
	"314","S42",
	"315","S42",
	"316","S42",
	"317","S42",
	"318","S42",
	"319","S42",
	"320","S42",
	"321","S42",
	"322","S42",
	"323","S42",
	"324","S42",
	"325","S42",
	"326","S42",
	"327","S42",
	"328","S42",
	"329","S42",
	"330","S42",
	"331","S42",
	"332","S42",
	"333","S42",
	"334","S42",
	"335","S42",
	"336","S42",
	"337","S42",
	"338","S42",
	"339","S42",
	"340","S42",
	"341","S42",
	"342","S42",
	"343","S42",
	"344","S42",
	"345","S42",
	"346","S42",
	"347","S42",
	"348","S42",
	"349","S42",
	"350","S42",
	"351","S42",
	"352","S42",
	"353","S42",
	"354","S42",
	"355","S42",
	"358","S42",
	"397","959",
	"398","959",
	"399","959",
	"400","959",
	"401","959",
	"402","959",
	"403","959",
	"404","959",
	"405","959",
	"406","959",
	"407","959",
	"408","959",
	"409","959",
	"410","959",
	"411","959",
	"412","959",
	"413","959",
	"414","959",
	"415","959",
	"416","959",
	"417","959",
	"418","959",
	"419","959",
	"420","959",
	"421","959",
	"422","959",
	"423","959",
	"424","959",
	"425","959",
	"426","959",
	"427","959",
	"428","959",
	"429","959",
	"430","959",
	"431","959",
	"432","959",
	"433","959",
	"434","959",
	"435","959",
	"436","959",
	"437","959",
	"438","959",
	"439","959",
	"440","959",
	"441","959",
	"442","959",
	"443","959",
	"444","959",
	"445","M97",
	"446","984",
	"447","959",
	"448","959",
	"449","959",
	"450","789",
	"451","959",
	"452","959",
	"453","959",
	"454","959",
	"456","959",
	"457","959",
	"458","959",
	"459","959",
	"460","959",
	"461","959",
	"462","959",
	"463","959",
	"464","N29",
	"465","959",
	"466","959",
	"467","959",
	"468","959",
	"469","959",
	"470","959",
	"471","Q05",
	"472","959",
	"473","959",
	"474","959",
	"475","959",
	"476","823",
	"477","959",
	"478","959",
	"479","959",
	"482","959",
	"483","959",
	"484","959",
	"485","959",
	"486","959",
	"487","524",
	"488","832",
	"489","959",
	"490","959",
	"491","959",
	"492","959",
	"493","959",
	"494","959",
	"495","959",
	"496","959",
	"497","959",
	"498","959",
	"499","959",
	"500","755",
	"501","011",
	"503","M87",
	"504","M87",
	"505","M87",
	"506","S22",
	"507","947",
	"508","864",
	"509","M68",
	"510","772",
	"511","O93",
	"512","T35",
	"513","872",
	"514","M30",
	"515","O99",
	"516","R05",
	"517","S32",
	"518","962",
	"519","M71",
	"520","N71",
	"521","M83",
	"522","N35",
	"523","P26",
	"524","870",
	"525","755",
	"526","959",
	"527","Q00",
	"528","Q02",
	"529","981",
	"530","M11",
	"531","N21",
	"532","N22",
	"533","952",
	"534","N45",
	"535","N45",
	"536","M32",
	"537","R80",
	"538","O90",
	"539","820",
	"540","772",
	"541","S32",
	"542","S33",
	"543","S33",
	"544","M83",
	"545","M83",
	"546","O64",
	"547","T08",
	"548","M83",
	"549","O65",
	"550","T08",
	"551","837",
	"552","P45",
	"553","M81",
	"554","T42",
	"555","M83",
	"556","T42",
	"557","T35",
	"558","N35",
	"559","M83",
	"560","M83",
	"572","R23",
	"573","N02",
	"574","N29",
	"575","N30",
	"576","825",
	"577","T59",
	"578","Q76",
	"585","M83",
	"586","947",
	"587","R05",
	"588","M83",
	"589","R05",
	"590","O62",
	"591","O64",
	"593","814",
	"594","350",
	"595","N45",
	"596","S32",
	"597","S32",
	"598","350",
	"599","350",
	"600","N30",
	"601","P05",
	"602","P57",
	"603","837",
	"604","837",
	"605","837",
	"606","837",
	"607","837",
	"608","837",
	"613","T42",
	"614","T42",
	"615","S25"
	};
//**********************************************************************************//
const aszREGOPE_t aszCODOPE_CA[] =
{
	"INT","MAP",
	"001","672",
	"002","P31",
	"003","947",
	"004","461",
	"005","S25",
	"006","756",
	"008","674",
	"009","O99",
	"010","461",
	"011","527",
	"012","T35",
	"013","531",
	"014","461",
	"015","461",
	"016","155",
	"017","R54",
	"018","765",
	"019","765",
	"020","765",
	"021","775",
	"022","S22",
	"024","106",
	"026","533",
	"027","673",
	"028","023",
	"029","959",
	"030","587",
	"031","839",
	"032","170",
	"033","959",
	"034","959",
	"035","959",
	"036","B04",
	"037","144",
	"038","959",
	"040","107",
	"041","578",
	"043","R40",
	"044","R41",
	"045","581",
	"046","584",
	"047","587",
	"048","M37",
	"049","861",
	"050","T41",
	"051","984",
	"052","T72",
	"053","674",
	"054","860",
	"055","488",
	"056","S69",
	"057","S69",
	"065","S34",
	"066","S28",
	"067","M42",
	"068","M42",
	"069","S31",
	"085","592",
	"100","530",
	"101","797",
	"102","673",
	"109","404",
	"110","959",
	"111","T44",
	"112","T43",
	"113","959",
	"114","T43",
	"115","959",
	"116","959",
	"117","T43",
	"118","959",
	"119","959",
	"121","959",
	"122","O62",
	"123","O64",
	"124","959",
	"125","T44",
	"126","959",
	"127","959",
	"128","959",
	"129","495",
	"130","959",
	"131","959",
	"132","T43",
	"133","959",
	"134","959",
	"135","462",
	"136","959",
	"137","959",
	"138","113",
	"139","468",
	"140","839",
	"141","959",
	"142","835",
	"143","959",
	"144","959",
	"145","959",
	"146","T43",
	"147","592",
	"148","N07",
	"149","T43",
	"150","T43",
	"151","959",
	"152","959",
	"153","959",
	"154","T43",
	"155","959",
	"156","959",
	"157","834",
	"158","959",
	"159","T43",
	"160","959",
	"161","959",
	"162","959",
	"165","959",
	"166","959",
	"167","T43",
	"168","T43",
	"169","959",
	"170","662",
	"171","637",
	"172","350",
	"173","Q02",
	"174","949",
	"175","601",
	"176","966",
	"177","819",
	"178","817",
	"179","T44",
	"180","524",
	"181","959",
	"182","959",
	"183","959",
	"184","959",
	"185","P30",
	"186","T43",
	"187","T43",
	"188","959",
	"189","959",
	"190","959",
	"191","959",
	"200","T43",
	"201","M97",
	"202","M97",
	"203","496",
	"204","M97",
	"205","959",
	"206","M97",
	"207","959",
	"208","959",
	"209","959",
	"210","959",
	"211","510",
	"212","551",
	"213","959",
	"214","959",
	"215","S67",
	"216","R24",
	"217","T45",
	"218","959",
	"219","860",
	"220","511",
	"221","959",
	"222","959",
	"223","959",
	"224","M97",
	"225","M97",
	"226","M97",
	"227","959",
	"250","M97",
	"300","959",
	"301","959",
	"302","959",
	"303","T43",
	"304","T43",
	"305","959",
	"306","959",
	"307","959",
	"308","959",
	"309","959",
	"310","959",
	"311","959",
	"312","959",
	"313","959",
	"314","170",
	"315","959",
	"316","T45",
	"317","959",
	"318","959",
	"319","959",
	"320","T45",
	"321","959",
	"322","959",
	"323","959",
	"324","959",
	"325","959",
	"326","T43",
	"327","T44",
	"328","959",
	"329","959",
	"330","959",
	"331","959",
	"332","959",
	"333","959",
	"334","170",
	"335","959",
	"336","959",
	"337","170",
	"338","959",
	"339","959",
	"340","959",
	"341","959",
	"342","959",
	"343","959",
	"344","959",
	"345","959",
	"346","959",
	"347","959",
	"348","959",
	"349","959",
	"350","959",
	"351","959",
	"352","959",
	"353","959",
	"354","959",
	"355","959",
	"358","959",
	"397","M97",
	"398","M97",
	"399","M97",
	"400","M97",
	"401","M97",
	"402","T45",
	"403","959",
	"404","959",
	"405","T44",
	"406","959",
	"407","959",
	"408","959",
	"409","959",
	"410","959",
	"411","959",
	"412","959",
	"413","959",
	"414","959",
	"415","959",
	"416","959",
	"417","959",
	"418","959",
	"419","959",
	"420","959",
	"421","959",
	"422","959",
	"423","959",
	"424","959",
	"425","959",
	"426","959",
	"427","T43",
	"428","T43",
	"429","M97",
	"430","M97",
	"431","170",
	"432","959",
	"433","959",
	"434","959",
	"435","959",
	"436","959",
	"437","959",
	"438","959",
	"439","959",
	"440","959",
	"441","T45",
	"442","959",
	"443","959",
	"444","M97",
	"445","M97",
	"446","984",
	"447","959",
	"448","959",
	"449","959",
	"450","784",
	"451","959",
	"452","959",
	"453","959",
	"454","959",
	"456","959",
	"457","959",
	"458","959",
	"459","959",
	"460","959",
	"461","959",
	"462","S06",
	"465","959",
	"466","959",
	"467","M97",
	"468","959",
	"469","959",
	"470","859",
	"471","959",
	"472","959",
	"473","959",
	"474","959",
	"475","959",
	"477","959",
	"478","959",
	"479","959",
	"480","817",
	"481","819",
	"482","959",
	"483","959",
	"484","959",
	"485","959",
	"486","107",
	"487","524",
	"488","832",
	"489","959",
	"490","959",
	"491","M97",
	"492","959",
	"493","959",
	"494","959",
	"495","959",
	"496","959",
	"497","959",
	"498","959",
	"499","959",
	"500","755",
	"501","011",
	"503","013",
	"504","014",
	"505","015",
	"506","116",
	"507","P20",
	"508","130",
	"509","826",
	"510","M83",
	"511","400",
	"512","T35",
	"513","143",
	"514","R73",
	"515","872",
	"517","174",
	"518","T08",
	"519","462",
	"520","871",
	"521","M83",
	"522","N95",
	"523","461",
	"524","N95",
	"525","868",
	"528","T08",
	"530","488",
	"531","N21",
	"532","N22",
	"533","952",
	"534","588",
	"535","588",
	"536","M32",
	"537","O60",
	"541","859",
	"542","859",
	"543","859",
	"544","614",
	"545","592",
	"546","859",
	"547","N30",
	"548","107",
	"549","B04",
	"550","837",
	"551","107",
	"552","604",
	"554","963",
	"555","M83",
	"556","T42",
	"557","T42",
	"558","480",
	"559","107",
	"575","N30",
	"585","592",
	"586","350",
	"587","R05",
	"588","107",
	"589","960",
	"590","O62",
	"591","O64",
	"592","637",
	"593","814",
	"594","350",
	"595","R07",
	"596","190",
	"597","190",
	"600","N30",
	"601","P05",
	"602","981",
	"603","837",
	"604","837",
	"605","837",
	"606","837",
	"607","155",
	"608","837",
	"609","136",
	"610","401",
	"611","P26"
};
//**********************************************************************************//
#endif


//**********************************************************************************//
// szOPERACION  : interal operation
// szDESCRIPCION : ---not in use---future use---
// recType       : SAVING, CHECKINGS,... (C.A., C.C.,...)
//**********************************************************************************//
int SearchLocalCodOpe( char *szOPERACION , char *szDESCRIPCION, enumRECTYPE recType )
{
	register int nInx	   = 0 ;
	register int nLimit    = 0 ;
	aszREGOPE_t *ptrCODOPE = NULL;

	// Precondition 1
	if( !szOPERACION || !szDESCRIPCION )		
		return -1;
	
	// Precondition 2
	if( RT_AHOMOV == recType || RT_AHODIF == recType || RT_AHMODO == recType || RT_AHDIDO == recType )		
	{
		ptrCODOPE = (aszREGOPE_t *)aszCODOPE_CA;
		nLimit    = sizeof(aszCODOPE_CA)/sizeof(aszCODOPE_CA[0]);
	}
	else
	{
		ptrCODOPE = (aszREGOPE_t *)aszCODOPE_CC;
		nLimit    = sizeof(aszCODOPE_CC)/sizeof(aszCODOPE_CC[0]);
	}

    // Search Loop in Array 
	for( int nInx = 0; nInx < nLimit; nInx++ )
    {
		if ( atoi(ptrCODOPE[nInx].INTERNO) == atoi(szOPERACION) 
			 && 
			 atoi(ptrCODOPE[nInx].INTERNO) > 0 
			 &&
			 strlen(ptrCODOPE[nInx].MAPEADO) > 0
			 &&
			 strlen(ptrCODOPE[nInx].MAPEADO) <= 3
		   )
		{
			// External Copy
			strcpy( szOPERACION, ptrCODOPE[nInx].MAPEADO );
			// Postcondicion
			return nInx;
		};
	};

	// Postcondition
	return 0;
};

// WINDOWS APPLICATION QUEUE PROCESSOR */
int ProcessAppMsgQueue(void)
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

// Cuadro de diálogo CAboutDlg utilizado para el comando Acerca de

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Datos del cuadro de diálogo
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV

// Implementación
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Cuadro de diálogo de CFuegoDPSDlg



CFuegoDPSDlg::CFuegoDPSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFuegoDPSDlg::IDD, pParent)
    , m_bReprocesamiento(FALSE)
    , m_checkConvertASCII(FALSE)	// 2013.08.08 - JAF - Default FALSE
    , editInterCTACTE_CAJAHO(_T("CC/CA"))
    , m_editTexto(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFuegoDPSDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_REPRO, m_bReprocesamiento);
    DDX_Check(pDX, IDC_CHECK_ASCII, m_checkConvertASCII);
    DDX_Text(pDX, IDC_EDIT_INTERFASE_CTACTE_CAJAHO, editInterCTACTE_CAJAHO);
    DDX_Text(pDX, IDC_EDIT1, m_editTexto);
    DDX_Control(pDX, IDC_PROGRESS, m_progressBar);
    DDX_Control(pDX, IDC_COMBO_FILES2, m_comboHacia);
    DDX_Control(pDX, IDC_COMBO_FILES1, m_comboDesde);
    DDX_Control(pDX, IDC_DATETIMEPICKER1, m_datetimePicker);
    DDX_Control(pDX, IDC_DATETIMEPICKER2, m_datetimePicker2);
}

BEGIN_MESSAGE_MAP(CFuegoDPSDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDEMITIR_SALDOS, OnBnClickedSaldos)
    ON_BN_CLICKED(IDEMITIR_SALDOS_HIS, OnBnClickedSaldosHis)
    ON_BN_CLICKED(IDEMITIR_MOVIMIENTOS, OnBnClickedMovimientos)
    ON_BN_CLICKED(IDCARGAR_IMPACTAR, OnBnClickedImpactar)
    ON_BN_CLICKED(IDC_CHECK_REPRO, OnBnClickedCheckRepro)
    ON_BN_CLICKED(IDC_CHECK_ASCII, OnBnClickedCheckAscii)
    ON_CBN_DROPDOWN(IDC_COMBO_FILES1, OnCbnDropdownComboFiles1)
END_MESSAGE_MAP()

// Controladores de mensaje de CFuegoDPSDlg

BOOL CFuegoDPSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	char strRuta[1024] = {'\0'};
	char datos[512];


	// Agregar el elemento de menú "Acerca de..." al menú del sistema.

	// IDM_ABOUTBOX debe estar en el intervalo de comandos del sistema.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Establecer el icono para este cuadro de diálogo. El marco de trabajo realiza esta operación
	//  automáticamente cuando la ventana principal de la aplicación no es un cuadro de diálogo
	SetIcon(m_hIcon, TRUE);			// Establecer icono grande
	SetIcon(m_hIcon, FALSE);		// Establecer icono pequeño

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
		return FALSE;
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
		return FALSE;
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

// EAE ->
/*
	//*************************************************************************************
	// JAF - VALORES DE CONFIGURACION REGIONAL - GCP ID: 14136							  *
	//*************************************************************************************
	
	setlocale( LC_ALL, "en-US" );
	setlocale( LC_NUMERIC, "[decimal_point]=." );
	setlocale( LC_NUMERIC, "[thousands_sep]=," );

	//////////////////////////////////////////////////////////////////////////////////
    //* Carga parametros de la REGISTRY con valores por default *
    strSERVER   = AfxGetApp()->GetProfileString("DATANET","Server"     ,  "127.0.0.1" );
    wAPPLICAT   = atoi( AfxGetApp()->GetProfileString("DATANET","Application",  "1000" ) );
    strDATABASE = AfxGetApp()->GetProfileString("DATANET","Database"   ,  "TEST" );
    strUSER     = AfxGetApp()->GetProfileString("DATANET","User"       ,  "test" );
    strPASSWORD = AfxGetApp()->GetProfileString("DATANET","Key"        ,  "test" );
    // CC , CA o ambas CC/CA /////////////////////////////////////////////////////////
    editInterCTACTE_CAJAHO = AfxGetApp()->GetProfileString("DATANET","Interfases",  "CC" );
	//////////////////////////////////////////////////////////////////////////////////
*/
//EAE <-

	return TRUE;  // Devuelve TRUE  a menos que establezca el foco en un control
}

void CFuegoDPSDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Si agrega un botón Minimizar al cuadro de diálogo, necesitará el siguiente código
//  para dibujar el icono. Para aplicaciones MFC que utilicen el modelo de documentos y vistas,
//  esta operación la realiza automáticamente el marco de trabajo.

void CFuegoDPSDlg::OnPaint() 
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
HCURSOR CFuegoDPSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/******************************************************************/
void OutputLoggMessage( char *szOutputFile , const char *cszTexto )
{
	FILE		*hFile = NULL;
    hFile = fopen( szOutputFile, "a+");
    if( hFile )
    {
        fprintf( hFile, "%s", cszTexto );
        fclose( hFile );
        hFile = NULL;
    }
}
/******************************************************************/

void CFuegoDPSDlg::OnBnClickedSaldos()
{
    /* CC */
    if( "CC" == editInterCTACTE_CAJAHO )
    {
        /* interfase saldos cta.cte */
        DoSaldos ( DPS_INTER_SALDOS , TRUE );
    };
    /* CA */
    if( "CA" == editInterCTACTE_CAJAHO )
    {
        /* interfase saldos caj.aho. */
        DoSaldos ( DPS_INTER_AHOSAL , TRUE );
    };
    /* CC/CA */
    if( "CC/CA" == editInterCTACTE_CAJAHO || "CA/CC" == editInterCTACTE_CAJAHO)
    {
        /* interfase saldos cta.cte */
        DoSaldos ( DPS_INTER_SALDOS , TRUE );
        /* interfase saldos caj.aho. */
        DoSaldos ( DPS_INTER_AHOSAL , FALSE ); /* sin truncar */
        /* interfase saldos caj.aho. DOLAR*/
        DoSaldos ( DPS_INTER_AHSADO , FALSE ); /* sin truncar */
    };
}

void CFuegoDPSDlg::OnBnClickedSaldosHis()
{
    /* CC */
    if( "CC" == editInterCTACTE_CAJAHO ) 
    {
        /* interfase saldos historicos cta.cte */
        DoSaldosHis ( DPS_INTER_SALHIS , FALSE );
    }
    /* CA */
    if( "CA" == editInterCTACTE_CAJAHO )
    {
        /* interfase saldos historicos caj.aho. */
        DoSaldosHis ( DPS_INTER_AHOHIS , FALSE );
    }
    /* CC/CA */
    if( "CC/CA" == editInterCTACTE_CAJAHO || "CA/CC" == editInterCTACTE_CAJAHO)
    {
        /* interfase saldos historicos cta.cte. */
        DoSaldosHis ( DPS_INTER_SALHIS , FALSE );
        /* interfase saldos historicos caj.aho. */
        DoSaldosHis ( DPS_INTER_AHOHIS , FALSE );
        /* interfase saldos historicos caj.aho.DOLAR */
        DoSaldosHis ( DPS_INTER_AHHIDO , FALSE );

	}
}

void CFuegoDPSDlg::OnBnClickedMovimientos()
{
	CargarLeyendas();
    /* CC */
    if( "CC" == editInterCTACTE_CAJAHO )
    {
        /* interfase mov. del dia cta.cte */
        DoMovimientos ( DPS_INTER_MOVCUE , TRUE  );
        /* interfase mov. dif. cta.cte */
        DoMovimientos ( DPS_INTER_MOVDIF , FALSE );
    };
    /* CA */
    if( "CA" == editInterCTACTE_CAJAHO )
    {
        /* interfase mov. del dia caj.aho. */
        DoMovimientos ( DPS_INTER_AHOMOV , TRUE );
        /* interfase mov. dif. dia caj.aho. */
        DoMovimientos ( DPS_INTER_AHODIF , FALSE );
    };
    if( "CC/CA" == editInterCTACTE_CAJAHO || "CA/CC" == editInterCTACTE_CAJAHO )
    {
        /* interfase mov. del dia cta.cte */
        DoMovimientos ( DPS_INTER_MOVCUE , TRUE  );
        /* interfase mov. dif. cta.cte */
        DoMovimientos ( DPS_INTER_MOVDIF , FALSE );
        /* interfase mov. del dia caj.aho. */
        DoMovimientos ( DPS_INTER_AHOMOV , FALSE );
		/* interfase mov. del dia caj.aho. DOLAR */
        DoMovimientos ( DPS_INTER_AHMODO , FALSE );
        /* interfase mov. dif. dia caj.aho. */
        DoMovimientos ( DPS_INTER_AHODIF , FALSE );
        /* interfase mov. dif. dia caj.aho. DOLAR */
        DoMovimientos ( DPS_INTER_AHDIDO , FALSE );
    };
}

void CFuegoDPSDlg::OnBnClickedImpactar()
{
	char		szTrxFilename[256] = {0x00},
				szOutputFile[256]  = {0x00};	
	int			iRetval = 0;
	FILE		*hFile = NULL,
                *hFOut = NULL;
	char		szInputBuffer[1024*10] = {0x00};
	char		szConverted[1024*10]   = {0x00};
    char		szAux[32] = {0x00};
	/* SIZE_T type */
	size_t		nLen      = 0,				
				nTrxStep  = 0;
	/****/
    long        nCounter  = 0;
	size_t		nSize     = 0;	                
	size_t      nCRLFSize   = 0, /** CR+LF  = 0 o 2 bytes        **/
			    nFILLERSize = 1; /** FILLER = 0 si se necesitara **/
    /**************************************************/
	/* Variables de Soporte DPS BATCH                 */
    /**************************************************/
	DPS_i_batch	          dpsBatchTRA ( strSERVER , wAPPLICAT );		
    /**************************************************/
    DPIBCOMM              dpsBatchABO;
    /**************************************************/
    /* Use interfase ? TRANSFER, ABONADO, OFFERING... */
    enum { INTERFASE_TRA, INTERFASE_ABO, INTERFASE_OFF } enInterfase = INTERFASE_TRA; /*DEFAULT*/
    /**************************************************/
	WORD                  wRespCde     = 0,
						  wProcRespCde = 0;	
    BYTE                  bMAC_Used    = 0;
    /* Selection - QUERY */
    int                   iSel      = 0,
                          iQueryNum = 0;
    string                str;
    // CycleFlag - AvoidProcessing
    BOOL				  bCycleFlag = FALSE;

    /*****************************************************/
    m_datetimePicker.GetTime( ctDate );  
    strDate.Format( "%02i/%02i/%04i" , ctDate.GetDay(),
                                       ctDate.GetMonth(),
                                       ctDate.GetYear() );    
    /*****************************************************/

	/* Casting a estructura para acceder con mas facilidad a ciertos atributos */
	DPS_i_batch::data_tag *ptrDATA = (DPS_i_batch::data_tag *)szInputBuffer;

    // Precondition
	iSel = m_comboDesde.GetCurSel();
	if(iSel >= 0)
		m_comboDesde.GetLBText( iSel, szTrxFilename );
	else
		m_comboDesde.GetWindowText( szTrxFilename , sizeof(szTrxFilename)-1);

    // Precondition
	hFile = fopen(szTrxFilename, "rt");
	if(!hFile)
    {
        AfxMessageBox("NO SE PUDO ABRIR EL ARCHIVO DE ENTRADA");
		return;
    }
    else
    {

        // Close original EBCDIC file 
        fclose( hFile ) ;
        hFile = NULL;
        
        // EBCDIC-2-ASCII 
        if( m_checkConvertASCII )
        {
            ///////////////////////////////////////
            // Archivo de salida ASCII ".ASC"
            strcpy( szOutputFile, szTrxFilename );
            strcat( szOutputFile, ".ASC" );
            ///////////////////////////////////////
            // Conversion EBCDIC-ASCII 
			iRetval = CONVERT_ASCII_EBCDIC_FILE( szTrxFilename, EBCDIC_2_ASCII, sizeof(DPS_i_batch::data_tag), &nCounter ,
                                                 szOutputFile ) ;
            ///////////////////////////////////////
            // 2 bytes CR+LF
            nCRLFSize = 0;
        }
        else
        {
            ///////////////////////////////////////
            // Archivo de salida de Logg
            strcpy( szOutputFile, szTrxFilename );
            strcat( szOutputFile, ".TXT" );
            // no bytes for bytes CR+LF
            nCRLFSize = 0;
            // Archivo de entrada, ya que esta en ASCII....
            strcpy( szOutputFile, szTrxFilename );            
        };
        //////////////////////////////////
		hFile = fopen(szOutputFile, "rt");
        //////////////////////////////////
	    if(!hFile)
        {
            AfxMessageBox("NO SE PUDO ABRIR EL ARCHIVO DE ENTRADA");
		    return;
        }
        else
        {
            m_editTexto = "ARCHIVO 'ASCII' ABIERTO, REGISTROS = ";
            m_editTexto = m_editTexto + itoa( nCounter, szAux, 10) ;
        };

        strcpy(szOutputFile, szTrxFilename);
        strcat(szOutputFile, ".OUT");
    };

    UpdateData( FALSE );
    ProcessAppMsgQueue();

    /*********************************************************/
    /* READ LENGTH                                           */
    /*********************************************************/
	std::string sNombreArchivo(szTrxFilename);
	//if( !strncmp( strupr(szTrxFilename), "TRA", 3) )
	if( std::size_t found = sNombreArchivo.find("TRA") != std::string::npos )
    {
        nLen = sizeof(dpsBatchTRA.data)+nCRLFSize+nFILLERSize;
        enInterfase = INTERFASE_TRA;
    }
    else if( std::size_t found = sNombreArchivo.find("ABO") != std::string::npos )
    {
		nCRLFSize = 2; /** ABONADOS sí vienen con un CR+LF **/
        nLen = sizeof(dpsBatchABO.data)+nCRLFSize;
        enInterfase = INTERFASE_ABO;

        /* CONNECT DATABASE */
        odbcPri.SetUserName( (PSTR)strUSER.c_str()     );
        odbcPri.SetPassword( (PSTR)strPASSWORD.c_str() );
        odbcPri.Connect    ( (PSTR)strDATABASE.c_str() ) ;  
        if(odbcPri.Error())
        {
	        m_editTexto =  "NO SE PUEDE CONECTAR A BASE DE DATOS : ";                
	        m_editTexto += odbcPri.GetErrorMessage(NULL);
	        m_editTexto += odbcPri.GetLastSQLCommand(NULL);
            AfxMessageBox(m_editTexto);
            UpdateData( FALSE );
	        ProcessAppMsgQueue();
	        return;
        };

    }
	else if(std::size_t found2 = sNombreArchivo.find("CUENTA") != std::string::npos)
	{
		nCRLFSize = 2; /** ABONADOS sí vienen con un CR+LF **/
        nLen = sizeof(dpsBatchABO.data)+nCRLFSize;
        enInterfase = INTERFASE_ABO;

        /* CONNECT DATABASE */
        odbcPri.SetUserName( (PSTR)strUSER.c_str()     );
        odbcPri.SetPassword( (PSTR)strPASSWORD.c_str() );
        odbcPri.Connect    ( (PSTR)strDATABASE.c_str() ) ;  
        if(odbcPri.Error())
        {
	        m_editTexto =  "NO SE PUEDE CONECTAR A BASE DE DATOS : ";                
	        m_editTexto += odbcPri.GetErrorMessage(NULL);
	        m_editTexto += odbcPri.GetLastSQLCommand(NULL);
            AfxMessageBox(m_editTexto);
            UpdateData( FALSE );
	        ProcessAppMsgQueue();
	        return;
        };
	}
	else if( !strncmp( szTrxFilename, "PBF", 3) )
	{
		// Impactar PBF
		iRetval = Impactar_PBF(	szTrxFilename, 
								szOutputFile );
	}
	else if( !strncmp( szTrxFilename, "UMOV", 4) )
	{
		// Impactar UMOV
		iRetval = Impactar_UMOV( szTrxFilename, 
								 szOutputFile );
	}
    else /* default */
    {
        nLen = sizeof(dpsBatchTRA.data)+nCRLFSize+nFILLERSize;
        enInterfase = INTERFASE_TRA;
    };
    /*********************************************************/

	/* LECTURA AVANZADA */
	if ( INTERFASE_ABO == enInterfase  )
	{
		fgets( szInputBuffer, sizeof(szInputBuffer), hFile )  ;
		nSize = strlen( szInputBuffer );
	}
	else
		nSize = fread( szInputBuffer, 1, nLen, hFile )  ;

    // Read Cicle
	for(nCounter = 1; hFile && nSize > 0 ; nCounter++ )
	{	
        
        UpdateData( FALSE );
        ProcessAppMsgQueue();

        if( 1 == nCounter && strstr( szInputBuffer, "TRANSFER") && enInterfase != INTERFASE_TRA)
        {
                m_editTexto = "Interfase de TRANSFERENCIAS incompatible con archivo";
				iRetval = AfxMessageBox( m_editTexto , MB_OK | MB_ICONQUESTION );
                break;
        };

        /********************************************************************/
        /* ABONADOS                                                         */
        /********************************************************************/
        if( INTERFASE_ABO == enInterfase )
        {
            m_progressBar.StepIt();

            ///////////////////////////////////////////////////////////////////
            // Auto-Adjust CR+LF
            while ( 0x0D == szInputBuffer[0] 
                    ||
                    0x0A == szInputBuffer[0] 
                    ||
                    0x20 == szInputBuffer[0] )
			{                
				memmove( szInputBuffer, szInputBuffer+1, sizeof(dpsBatchABO.data) );
				nLen++;
			}
            ///////////////////////////////////////////////////////////////////

            dpsBatchABO.data = ((DPIBCOMM *)szInputBuffer)->data;

			// EOF ? 
            if(1 < nCounter && strncmp(dpsBatchABO.data.CODBAN, BCRA_ID_DEFAULT, 3)  )
            {				
                break;
            }

            /*****************************************************/
            /* PREPARE QUERY - 'ABO_CTA' (DPS ONLINE)            */
            /*****************************************************/
            iQueryNum = _ODBC_INSERT_SNTC_ + 1;		
            /* QUERY */
            strQuery =  "INSERT INTO DN_ABONADOS_CUENTAS " ;
            strQuery += "(CUENTA,TIPO_CUENTA,ABONADO) " ;
            strQuery += "VALUES " ;
            strQuery += "('" ;
            str       = dpsBatchABO.data.NUMCTA;
            strQuery += str.substr(0,17);
            strQuery += "','" ;
            str       = dpsBatchABO.data.TIPCUE;
            strQuery += str.substr(0,2);
            strQuery += "','" ;
            str       = dpsBatchABO.data.NUMABO;
            strQuery += str.substr(0,7);
            strQuery += "') " ;
            /* EXECUTE QUERY */
            odbcPri.Query( (PSTR)strQuery.c_str(), &iQueryNum, FALSE, FALSE ) ;
            if(odbcPri.Error())
            {
                    odbcPri.ResetLastError();
	                m_editTexto = "NO SE PUEDE INSERTAR LA RELACION ABONADO-CUENTA: "; 
	                m_editTexto += odbcPri.GetErrorMessage(NULL);
	                m_editTexto += odbcPri.GetLastSQLCommand(NULL);

                    UpdateData( FALSE );
	                ProcessAppMsgQueue( );	                
                    
	                /* NEXT STEP : 'NOVEDA' */
            };
			odbcPri.ResetLastError();
            odbcPri.FreeSentence( iQueryNum );
			odbcPri.ResetLastError();

            /*****************************************************/
            /* PREPARE QUERY - 'ABO_CLAVE' (DPS ONLINE)          */
            /*****************************************************/
            iQueryNum = _ODBC_INSERT_SNTC_ + 2;		
            /* QUERY */
            strQuery =  "INSERT INTO DN_ABONADOS_CLAVES " ;
            strQuery += "(ABONADO,CLAVE,RAZON_SOCIAL,ESTADO) " ;
            strQuery += "VALUES " ;
            strQuery += "('" ;
            str       = dpsBatchABO.data.NUMABO;
            strQuery += str.substr(0,7);
            strQuery += "','*******','GENERICO','A') " ;
            /* EXECUTE QUERY */
            odbcPri.Query( (PSTR)strQuery.c_str(), &iQueryNum, FALSE, FALSE ) ;
            if(odbcPri.Error())
            {
                    odbcPri.ResetLastError();
	                m_editTexto = "NO SE PUEDE INSERTAR EL ABONADO: "; 
	                m_editTexto += odbcPri.GetErrorMessage(NULL);
	                m_editTexto += odbcPri.GetLastSQLCommand(NULL);

                    UpdateData( FALSE );
	                ProcessAppMsgQueue( );	                
                    
	                /* NEXT STEP : 'NOVEDA' */
            };
			odbcPri.ResetLastError();
            odbcPri.FreeSentence( iQueryNum );
			odbcPri.ResetLastError();


            /*****************************************************/
            /* PREPARE QUERY - 'NOVEDA' (DPS BATCH)              */
            /*****************************************************/
            iQueryNum = _ODBC_INSERT_SNTC_ + 3 ;		
            /* QUERY */
            strQuery =  "INSERT INTO NOVEDA " ;
            strQuery += "(NV_TIPNOV,NV_TIPCTA,NV_CUENTA,NV_ORDEN,";
            strQuery += " NV_FECALT,NV_FECBAJ,NV_FECPRO,NV_USERID,NV_USERID_BAJ)" ;
            strQuery += "VALUES " ;
            strQuery += "('1','" ;            
            /* TIPO DE CUENTA */
            str       = dpsBatchABO.data.TIPCUE;
            /* FIX DE TIPO DE CUENTA : MAPEAR TIPOS DATANET A TIPOS BASE24-LINK/BANELCO */
            switch ( antoi(dpsBatchABO.data.TIPCUE,sizeof(dpsBatchABO.data.TIPCUE))) 
            {
            case DPS_CUENTA_CORRIENTE_ARG /* 01 */ : str = CEXTR_CHECKINGS_ACCOUNT ; break;
            case DPS_CAJA_DE_AHORROS_ARG  /* 02 */ : str = CEXTR_SAVINGS_ACCOUNT ; break;
            case DPS_CUENTA_CORRIENTE_USD /* 13 */ : str = CEXTR_CHECKINGS_ACCOUNT_USD ; break;
            case DPS_CAJA_DE_AHORROS_USD  /* 15 */ : str = CEXTR_SAVINGS_ACCOUNT_USD ; break;
            default : str = CEXTR_CHECKINGS_ACCOUNT;
            };
            /* FIX DE TIPO DE CUENTA : MAPEAR TIPOS DATANET A TIPOS BASE24-LINK/BANELCO */
            
            strQuery += str.substr(0,2);
            strQuery += "','" ;
            str       = dpsBatchABO.data.NUMCTA;
            strQuery += str.substr(0,17);
            strQuery += "','0','";            
			/*****************************/
			/* FIX fecha ANSI yyyy/mm/dd para algunos motores con compatibilidad y localizacion */
            str       = strDate;	
			/*****************************/
			CString strANSIDate;
			strANSIDate.Format( "%04i/%02i/%02i" , ctDate.GetYear(), ctDate.GetMonth(), ctDate.GetDay() );    			
            str       = strANSIDate;	
			/*****************************/
            strQuery += str.substr(0,10);
            strQuery += "','1900-01-01','1900-01-01','ADMIN','0')" ;            
            /* EXECUTE QUERY */
            odbcPri.Query( (PSTR)strQuery.c_str(), &iQueryNum, FALSE, FALSE ) ;
            if(odbcPri.Error())
            {
                    odbcPri.ResetLastError();
	                m_editTexto = "NO SE PUEDE INSERTAR LA NOVEDAD : "; 
	                m_editTexto += odbcPri.GetErrorMessage(NULL);
	                m_editTexto += odbcPri.GetLastSQLCommand(NULL);
                    
                    UpdateData( FALSE );
	                ProcessAppMsgQueue( );	                
                    odbcPri.FreeSentence( iQueryNum );
	                /* next statement */
            };
			odbcPri.ResetLastError();
            odbcPri.FreeSentence( iQueryNum );
			odbcPri.ResetLastError();

        }
        /********************************************************************/
        /* TRANSFER DB/CR                                                   */
        /********************************************************************/
        else if( INTERFASE_TRA == enInterfase )
        {
			// DPRE Tester/Parser
			// Parsear mensaje y auditar en un stream de salida
			// bool esHeader = (nCounter == 1) && (szInputBuffer[0] == DPS_B_TIPO_REGISTRO_HEADER);

			if( (iRetval = dpsBatchTRA.DPRE_Test( szInputBuffer, (int)strlen(szInputBuffer), 
			                szOutputFile, ( 1 == nCounter ) ? true : false  , true )
			                ) != 0 )
			//	|| esHeader)
			{
			    // CycleFlag
			    bCycleFlag = TRUE;

			    ///////////////////////////////////////////////////////////////////
			    // Auto-Adjust CR+LF
			    while ( 0x0D == szInputBuffer[0] 
			            ||
			            0x0A == szInputBuffer[0] 
			            ||
			            0x20 == szInputBuffer[0] )
			        memmove( szInputBuffer, szInputBuffer+1, sizeof(dpsBatchTRA.data) );
			    ///////////////////////////////////////////////////////////////////

			    // Report Header
			    if( 1 == nCounter && DPS_B_TIPO_REGISTRO_HEADER == ptrDATA->TIPOREG)
			    {
				    m_progressBar.StepIt();
			        long lAux = antoi( ptrDATA->NUMTRA, 8);
			        m_editTexto = "FECHA DEL HEADER = ";                
			        m_editTexto = m_editTexto + itoa( lAux, szAux, 10) ;
			        m_editTexto = m_editTexto + ", CONFIRME SI/NO SU PROCESAMIENTO.";                

			        UpdateData( FALSE );
			        ProcessAppMsgQueue();
			        
				    iRetval = AfxMessageBox( m_editTexto , MB_OKCANCEL | MB_ICONQUESTION );

			        if( 1 != iRetval )
			        {
			            m_editTexto = m_editTexto + " = CANCELADO POR USUARIO.";                
			            break;
			        };
			    }
			    // Report Footer
			    else if(1 != nCounter  && DPS_B_TIPO_REGISTRO_FOOTER == ptrDATA->TIPOREG)
			    {
					m_editTexto = "PROCESANDO FOOTER"; 

					// No error, seguir procesando
					bCycleFlag = FALSE;					

			    }
			    // Data Record
			    else if(1 != nCounter && DPS_B_TIPO_REGISTRO_BODY == ptrDATA->TIPOREG)
			    {
			        // No error, continue
			        bCycleFlag = FALSE;
			    }
			    // Error
			    else
			    {
			        m_editTexto = "REGISTRO ERRONEO DETECTADO ";
			        m_editTexto = m_editTexto + (LPCSTR)ptrDATA->BANDEB ;
				    AfxMessageBox( m_editTexto );
			        m_editTexto = " ";
			    }
			}
			else
			{  
				// NO ERROR
				bCycleFlag = FALSE;
			}

			if( !bCycleFlag )
			{
				// Prevalidar mensaje DPRE BATCH contra base de datos del backoffice / local
				wRespCde = dpsBatchTRA.DPRE_ProcessInit(
										szInputBuffer, 
										(int)strlen(szInputBuffer), 
										is_false           ,
										(PSTR)strDATABASE.c_str(), 
										(PSTR)strUSER.c_str()    , 
										(PSTR)strPASSWORD.c_str(),
										&wProcRespCde      , 
										&bMAC_Used         ) ;
			};

			if( !bCycleFlag )
			{
				// Verificar resultado de pre-validaciones 
				if( DPS_APROBADA_SIN_RECHAZO  != wRespCde	)
				{            
					m_editTexto = m_editTexto + ".LA TRANSFERENCIA NO PREVALIDA CONTROLES, REG. #" ;
					m_editTexto = m_editTexto + itoa( nCounter, szAux, 10) ;
					m_editTexto += ". COD.RTA.=[" ;
					m_editTexto += itoa( wRespCde, szAux, 10) ;            
					m_editTexto += "]. ERROR=[" ;
					m_editTexto += itoa( wProcRespCde, szAux, 10) ;            
					m_editTexto += "]. CONTINUANDO...\r\n" ; 
					/************************************************/
					OutputLoggMessage( szOutputFile , (LPCSTR)m_editTexto );
					/************************************************/
					if( DPS_MAC_ERRONEO == wProcRespCde )
					{
						m_editTexto = m_editTexto + "MAC ERRONEA DETECTADA, NO SE IMPACTO.\r\n" ;
						/************************************************/
						OutputLoggMessage( szOutputFile , (LPCSTR)m_editTexto );
						/************************************************/
					}
					else if( DPS_FALTAN_DATOS_EN_DETALLE_OPERACION == wProcRespCde )
					{
						m_editTexto = m_editTexto + "FALTA ABONADO/BANCO, NO SE IMPACTO.\r\n" ;
						/************************************************/
						OutputLoggMessage( szOutputFile , (LPCSTR)m_editTexto );
						/************************************************/
					}
					bCycleFlag = TRUE;
				}
			}
    		
			if( !bCycleFlag )
			{
				if( DPS_MAC_ERRONEO == wProcRespCde )
				{
						m_editTexto = m_editTexto + ".MAC ERRONEA DETECTADA , NO SE IMPACTA LA TEF.\r\n" ;
						/************************************************/
						OutputLoggMessage( szOutputFile , (LPCSTR)m_editTexto );
						/************************************************/
						m_editTexto = m_editTexto + (LPCSTR)ptrDATA->BANDEB;

						// JAF - Cambio la bandera de las tefs rechazadas por mac invalida para que no vayan al core.
						bCycleFlag = TRUE;
				}
			}

			if( !bCycleFlag )
			{
				// Convertir a ISO8583 para poder reutilizar el autorizador Host24
				if( (iRetval = dpsBatchTRA.DPRE_ISO8583_Convert(szInputBuffer, sizeof(szInputBuffer) ,
										szConverted  , sizeof(szConverted) ,
										_ISO_ACCT_PICTURE_LENGHT_, _DTN_ACCT_PICTURE_LENGHT_, 'R' )
										) != 0 )
				{
					m_editTexto = "CONVERTIDA A 'ISO8583' CON ERROR.\r\n" ;
					m_editTexto = m_editTexto + itoa( nCounter, szAux, 10) ;			
					AfxMessageBox( m_editTexto );
					m_editTexto = " ";
					/************************************************/
					OutputLoggMessage( szOutputFile , (LPCSTR)m_editTexto );
					/************************************************/
					bCycleFlag = TRUE;
				}
			}

			if( !bCycleFlag )
			{
				// ISO Tester/Parser
				if( (iRetval = ISO_Test( (PBYTE)szConverted, sizeof(szConverted), true, 
			    			"DNET-ISO.txt", false )
							) != 0 )
				{
					m_editTexto = "PARSEADA CON ERROR SEGUN ISO8583. " ;
					m_editTexto = m_editTexto + itoa( nCounter, szAux, 10) ;			
					m_editTexto += ", CONTINUANDO...\r\n" ; 
					AfxMessageBox( m_editTexto );
					m_editTexto = " ";
					bCycleFlag = TRUE;
				}
			}

			if( !bCycleFlag )
			{
				// Autorizar como si fuera una transaccion ATM Host24
				if( (iRetval = dpsBatchTRA.ISO8583_Authorize( szConverted , &wRespCde)) != TRUE )
				{
					m_editTexto = "SIN IMPACTAR, ERROR DE PROCESAMIENTO. " ;
					m_editTexto = m_editTexto + itoa( nCounter, szAux, 10) ;            
					m_editTexto += ", CONTINUANDO...\r\n" ; 
					/************************************************/
					OutputLoggMessage( szOutputFile , (LPCSTR)m_editTexto );
					/************************************************/
					bCycleFlag = TRUE;

					// salgo por error de comunicacion con el Core. y mando mensaje de error de comunicacion con el Core.
					AfxMessageBox( "Error de comunicacion con el CORE... Se aborta el procesamiento" );
					break;
					
				}
			}
			
			////////////////////////////////////////////////////////////
			if( !bCycleFlag )
			{
				m_editTexto = "IMPACTADA OK. CONTINUANDO.\r\n" ;
				m_editTexto = m_editTexto + itoa( nCounter, szAux, 10) ;
				/************************************************/
				OutputLoggMessage( szOutputFile , (LPCSTR)m_editTexto );
				/************************************************/
			}
			
        }; /* end-if-TRANSFER */
        /********************************************************************/

		/* LECTURA SIGUIENTE Y CORTE DE CICLO  */
		if ( INTERFASE_ABO == enInterfase  )
		{
			szInputBuffer[0] = 0x00;
			if( fgets( szInputBuffer, sizeof(szInputBuffer), hFile )  )
				nSize = strlen( szInputBuffer );
			else
				nSize = 0;
		}
		else
			nSize = fread( szInputBuffer, 1, nLen, hFile )  ;

	}; /* end-for */

    m_editTexto  = "FIN DEL PROCESAMIENTO. REGISTROS : " ;
    m_editTexto  = m_editTexto + itoa( nCounter, szAux, 10) ;
    m_editTexto += ".\r\n";
    /************************************************/
    OutputLoggMessage( szOutputFile , (LPCSTR)m_editTexto );
    /************************************************/

    UpdateData( FALSE );
    ProcessAppMsgQueue();
    
    fclose( hFile );
	hFile = NULL;

}

void CFuegoDPSDlg::OnBnClickedCheckRepro()
{
    if(!m_bReprocesamiento)
        m_bReprocesamiento = TRUE;
    else
        m_bReprocesamiento = FALSE;
    UpdateData( TRUE );
}

void CFuegoDPSDlg::OnBnClickedCheckAscii()
{
    if( m_checkConvertASCII )
        m_checkConvertASCII = FALSE;
    else
        m_checkConvertASCII = TRUE;
    UpdateData( FALSE );
}

void CFuegoDPSDlg::DoSaldosHis( e_dps_interfases eType , BOOL bTruncate )
{
  struct HEADER_SALHIS_tag hdrSALHIS;
  struct ENREGI_SALHIS_tag recSALHIS;

  struct ENREGI_AHOHIS_tag recAHOHIS;
  struct FOOTER_SALHIS_tag ftrSALHIS;

  struct ENREGI_AHHIDO_tag recAHHIDO;
//  struct FOOTER_SALHIS_tag ftrSALHIS;

  FILE	      *fLogg      ,
              *fBALANCES  ;
  int           iQueryNum    = 0,
                iQueryNumSec = 1;
  long          lRetval   = 0;
  char          szCUENTA[20+1]     = {0x00};  
  char          szCUENTABKP[20+1]  = {0x00};  
  char          szSALACT[19+1]     = {0x00};  
  /***********************************************************************/
  char          szSALHIST[19+1]    = {0x00}; /* SALDO HISTORICO A FECHA */
  char          szSUMACRDB[19+1]   = {0x00}; /* SUMA DE DEBITOS Y CREDITOS */
  char          szSALNUEVO[19+1]   = {0x00};
  char          szSALNUEBKP[19+1]  = {0x00};
  /***********************************************************************/
  char          szFECHAOPE[30+1]   = {0x00};
  char          szSUMADB[19+1]     = {0x00};
  char          szSUMACR[19+1]     = {0x00};
  char          szFECHAHDR[30+1]   = {0x00}; /* FECHA-HEADER  = ULTIMA FECHA-SALDO-INFORMADA + 1 */
  char          szFECHAINI[30+1]   = {0x00}; /* FECHA-INICIAL = FECHA-HASTA - 70 DIAS ATRAS      */
  char          szFECHAPRO[30+1]   = {0x00}; /* FECHA-PROCESO = FECHA-HEADER - 1                 */
  char          szFECHAFER[30+1]   = {0x00}; /* FECHA-FERIADO */
  struct stPKDECIMAL_tag
  {
      char          chPKDECIMAL[9]; /* PIC S9(15)V99 = 17 bytes ( 16 + signo ) = empaquetados 9 bytes */
      char          chOVERFLOW[11]; /* OVERFLOW AREA */
  } stPKDECIMAL ; /* 20 BYTES = 160 bits */
  char          szYYYYMMDD[16+1] = {0x00};
  long          lCounter  = 0;
  bool          bHeader   = FALSE;
  const size_t  cnSize    = sizeof(recSALHIS);  
  char		    szAux[32] = {0x00};

  /****************************************/
  /* Auxiliares por CUENTA y SALDO diario */  
  const long    clMAX_SALDACT_TBL  = sizeof(recSALHIS.stTABDIA.TABDIA)/sizeof(recSALHIS.stTABDIA.TABDIA[0]); /* n=50 */
  /*********************************************/
  /* Auxiliares para verificar si es dia habil */
  struct tm tmWorkingDay  = {0,0,0,0,0,0,0,0,0};
  time_t    tWorkingDay   = 0;        
  struct tm tmHistoricDay = {0,0,0,0,0,0,0,0,0};
  time_t    tHistoricDay  = 0;        
  /*********************************************/
  char          szLogFilename[]     = "SALHIS.DATANET.log";
  char          szDatFilename[]     = "LARGOS.dat";
  char          szBcraFilename[256] = { "LARGOS.999" };
  /*******************************************************/
  /* Tabla Auxiliar de propagacion de Saldos Historicos  */
  stTABDIA_ARRAY_t stTABDIA;
  stREGDIA_t       stREGDIA;
  long            nInx    = 0,
                  nJnx    = 0,
                  nSearch = 0;
  const BOOL      bUsePostBalance  = FALSE; /* BALANCE POST FORMADO? */
  /*******************************************************/
  stNUMCTA_t   stNUMCTA = { 0x00, 0x00 };
  /****************************************************/


  /* PRECONDITION */
  if ( bTruncate )
    fLogg = fopen(szLogFilename, "wt");
  else
    fLogg = fopen(szLogFilename, "at");
  if(!fLogg)
  {
	  m_editTexto = "NO SE PUEDE CREAR EL ARCHIVO DE LOGG";                
      AfxMessageBox(m_editTexto);
      UpdateData( FALSE );
      ProcessAppMsgQueue();
	  return;
  }
  else
	  fprintf(fLogg,"\r\nBANCO INTERFINANZAS - SALDOS PARA DATANET - VIA ODBC\r\n");

  /* PRECONDITION : APPEND FILE 'LARGOS' (SALDOS & SALHIS) */
  if ( bTruncate )
    fBALANCES = fopen(szDatFilename, "wt");
  else
    fBALANCES = fopen(szDatFilename, "at");
  if(!fBALANCES)
  {
    /* PRECONDITION : CREATE FILE 'LARGOS' (SALDOS & SALHIS) */
    fBALANCES = fopen(szDatFilename, "wt"); 
    if(!fBALANCES)
    {
	    m_editTexto = "NO SE PUEDE CREAR EL ARCHIVO DE DATOS DE SALIDA DE SALDOS";                
        AfxMessageBox(m_editTexto);
        UpdateData( FALSE );
	    ProcessAppMsgQueue();

	    fprintf(fLogg,"\r\n");
	    fprintf(fLogg, (LPCSTR)m_editTexto);
	    fprintf(fLogg,"\r\n");
        fflush( fLogg );        
	    return;
    };
  };

  /* CONNECT PRIMARY DATABASE */
  odbcPri.SetUserName( (PSTR)strUSER.c_str()     );
  odbcPri.SetPassword( (PSTR)strPASSWORD.c_str() );
  odbcPri.Connect    ( (PSTR)strDATABASE.c_str() ) ;  
  if(odbcPri.Error())
  {
	  m_editTexto =  "NO SE PUEDE CONECTAR A BASE DE DATOS : ";                
	  m_editTexto += odbcPri.GetErrorMessage(NULL);
	  m_editTexto += odbcPri.GetLastSQLCommand(NULL);
      AfxMessageBox(m_editTexto);
      UpdateData( FALSE );
	  ProcessAppMsgQueue();

	  fprintf(fLogg,"\r\n");
	  fprintf(fLogg, (LPCSTR)m_editTexto);
	  fprintf(fLogg,"\r\n");	  
      fflush( fLogg ); 
	  return;
  };

  /* CONNECT SECONDARY DATABASE */
  odbcSec.SetUserName( (PSTR)strUSER.c_str()     );
  odbcSec.SetPassword( (PSTR)strPASSWORD.c_str() );
  odbcSec.Connect    ( (PSTR)strDATABASE.c_str() ) ;  
  if(odbcSec.Error())
  {
	  m_editTexto =  "NO SE PUEDE CONECTAR A BASE DE DATOS : ";                
	  m_editTexto += odbcSec.GetErrorMessage(NULL);
	  m_editTexto += odbcSec.GetLastSQLCommand(NULL);
      AfxMessageBox(m_editTexto);
      UpdateData( FALSE );
	  ProcessAppMsgQueue();

	  fprintf(fLogg,"\r\n");
	  fprintf(fLogg, (LPCSTR)m_editTexto);
	  fprintf(fLogg,"\r\n");	  
      fflush( fLogg ); 
	  return;
  };

  /* PRECONDITION : REGISTER SIZE = 2000 + 6 */
  if( sizeof( recSALHIS ) != 2000 + 6)
  {
	  AfxMessageBox("EL PROGRAMA NO HA SIDO COMPILADO CON ALINEACION A 2 BYTES. ABORTANDO.");
	  odbcPri.Disconnect();
	  return;
  }
  /* PRECONDITION : REGISTER SIZE = 2000 + 6 */
  if( sizeof( recAHOHIS ) != 2000 + 6)
  {
	  AfxMessageBox("EL PROGRAMA NO HA SIDO COMPILADO CON ALINEACION A 2 BYTES. ABORTANDO.");
	  odbcPri.Disconnect();
	  return;
  }

  /* PRECONDITION : REGISTER SIZE = 2000 + 6 */
  if( sizeof( recAHHIDO ) != 2000 + 6)
  {
	  AfxMessageBox("EL PROGRAMA NO HA SIDO COMPILADO CON ALINEACION A 2 BYTES. ABORTANDO.");
	  odbcPri.Disconnect();
	  return;
  }


  /* DATE TO FINISH */
  m_datetimePicker2.GetTime( ctDate );  
  strDate.Format( "%04i-%02i-%02i" , ctDate.GetYear(),
                                     ctDate.GetMonth(),
                                     ctDate.GetDay() );

  COPYSTRING( szFECHAOPE, (LPCSTR)strDate );
  COPYSTRING( szFECHAHDR, (LPCSTR)strDate );   
  
  /* DATE TO PROCESS = DATE TO PROCESS - 1 */
  tmWorkingDay.tm_year = ctDate.GetYear() - 1900; /* Base 1900 */
  tmWorkingDay.tm_mon  = ctDate.GetMonth() - 1;   /* [0,11] */
  tmWorkingDay.tm_mday = ctDate.GetDay() ;        /* [1,31] */  
  tWorkingDay = mktime( &tmWorkingDay );
  /* LOCAL TIME - (JIS format) */
  tWorkingDay -= 24*60*60 ; /* seconds in 24hs */
  tmWorkingDay = * ( localtime( &tWorkingDay ) );
  strDate.Format( "%04i-%02i-%02i" , tmWorkingDay.tm_year + 1900,
                                     tmWorkingDay.tm_mon + 1    ,
                                     tmWorkingDay.tm_mday       );  
  COPYSTRING( szFECHAPRO, (LPCSTR)strDate );

  /* DATE TO BEGIN = DATE TO PROCESS - 90 DAYS AGO */
  tmWorkingDay.tm_year = ctDate.GetYear() - 1900; /* Base 1900 */
  tmWorkingDay.tm_mon  = ctDate.GetMonth() - 1;   /* [0,11] */
  tmWorkingDay.tm_mday = ctDate.GetDay() - 90;    /* [1,31] */  
  tWorkingDay = mktime( &tmWorkingDay );
  strDate.Format( "%04i-%02i-%02i" , tmWorkingDay.tm_year + 1900,
                                     tmWorkingDay.tm_mon + 1    ,
                                     tmWorkingDay.tm_mday       );  
  COPYSTRING( szFECHAINI, (LPCSTR)strDate );

  /*******************************************/
  /* RESET FROM-DATE                         */
  /*******************************************/
  CTime ctDateTemp( tWorkingDay );
  m_datetimePicker.SetTime( &ctDateTemp );   /* FROM */
  UpdateData( FALSE );
  ProcessAppMsgQueue();
  /*******************************************/


  /****************************************************************************************/
  /* PREPARE QUERY */
  iQueryNum = _ODBC_UPDATE_SNTC_ ;		
  /* QUERY */
#ifdef _USE_EMBEBED_SQL_
  strQuery =  "UPDATE DN_PARAMETROS SET DN_FECHA_PROCESO = '"  ;
  strQuery += szFECHAOPE ;            
  strQuery += "', DN_FECHA_HASTA='";
  strQuery += szFECHAPRO ;
  strQuery += "', DN_FECHA_DESDE='";
  strQuery += szFECHAINI;
  strQuery += "' ";
  strQuery += "  WHERE DN_ID=1" ;
#else
  strQuery = "EXEC SP_UPDATE_DN_PARAMETROS '" ;
  strQuery += szFECHAOPE ;            
  strQuery += "', '";
  strQuery += szFECHAINI ;
  strQuery += "', '";
  strQuery += szFECHAPRO;
  strQuery += "', 1, 180 "; /* REGISTRO-DESDE = 6 MESES PARA ATRAS (180 DIAS) . REGISTRO-HASTA = AYER (1 DIA)*/
#endif

  /* EXECUTE QUERY */
  odbcPri.Query( (PSTR)strQuery.c_str(), &iQueryNum, FALSE, FALSE ) ;
  if(odbcPri.Error())
  {
	    m_editTexto = "NO SE PUEDE ACTUALIZAR LA FECHA DE PROCESO SQL : "; 
	    m_editTexto += odbcPri.GetErrorMessage(NULL);
	    m_editTexto += odbcPri.GetLastSQLCommand(NULL);
        AfxMessageBox(m_editTexto);
        UpdateData( FALSE );
	    ProcessAppMsgQueue( );

	    odbcPri.Disconnect();
	    return;
  };
  odbcPri.FreeSentence( iQueryNum );
  /****************************************************************************************/

  /****************************************************************************************/
  /* MSG LOGG */
  m_editTexto = "RECUPERANDO SALDOS HISTORICOS A FECHA..."; 
  UpdateData( FALSE );
  ProcessAppMsgQueue();

  /* QUERY 'VIEW_SALDOS_HISTORICOS_A_FECHA' */
  /* PREPARE QUERY  */
  iQueryNum = _ODBC_READ_SNTC_ ;		
  odbcPri.Column( szCUENTA    , SQL_C_CHAR, 20+1, &iQueryNum ); //1
  odbcPri.Column( szSALACT    , SQL_C_CHAR, 19+1, &iQueryNum ); //2


#ifdef _USE_EMBEBED_SQL_
  strQuery = " SELECT convert(char(1),mc_tipcta)+convert(varchar(8),mc_cuenta)+convert(char(1),mc_orden) AS CUENTA,"
	         "        mc_salact as SALDO "
	         " FROM MAECUE";
  /****************************************************/
  if( DPS_INTER_SALHIS == eType )
      strQuery += " WHERE mc_tipcta IN(1,7)";
  else
      strQuery += " WHERE mc_tipcta IN(11,15)";
  /****************************************************/
  strQuery += " ORDER BY mc_tipcta,mc_cuenta,mc_orden" ;
#else
  strQuery = " EXEC SP_VIEW_MAESTRO_DE_CUENTAS " ;
  /****************************************************/
   
  if( DPS_INTER_SALHIS == eType )
      strQuery += " 1 " ; // Antes " 1,7";
  else if( DPS_INTER_AHOHIS == eType )
      strQuery += " 11"; // Antes " 11,15";
  else
      strQuery += " 15"; // Antes " 11,15";

  /****************************************************/
#endif

  /* EXECUTE QUERY */
  odbcPri.Query( (PSTR)strQuery.c_str(), &iQueryNum, TRUE, TRUE ) ;
  if(odbcPri.Error())
  {
	  m_editTexto = "NO SE PUEDE EJECUTAR LA CONSULTA SQL : "; 
	  m_editTexto += odbcPri.GetErrorMessage(NULL);
	  m_editTexto += odbcPri.GetLastSQLCommand(NULL);
      AfxMessageBox(m_editTexto);
      UpdateData( FALSE );
	  ProcessAppMsgQueue( );

	  fprintf(fLogg,"\r\n");
	  fprintf(fLogg, (LPCSTR)m_editTexto);
	  fprintf(fLogg,"\r\n");	  
      fflush( fLogg ); 

	  odbcPri.Disconnect();
	  return;
  };

  /* Progress Bar */
  m_progressBar.SetRange32(0, 1 + 1 + 1 ); /* header + footer + 1 data record */
  m_progressBar.SetStep( 1 );	

  m_editTexto = "PROCESANDO..."; 
  UpdateData( FALSE );
  ProcessAppMsgQueue();

  /*****************************************************/
  /* FETCH CYCLE */
  for( // READ FIRST
       odbcPri.Fetch( &iQueryNum ) ;
       // LOOP CONDITIONS
       odbcPri.GetReturnCode() == SQL_SUCCESS_WITH_INFO 
       ||  
       odbcPri.GetReturnCode() == SQL_SUCCESS  ;
       // READ NEXT
       odbcPri.Fetch( &iQueryNum ) 
       )
  {		
		/* Internal ODBC status */
		lRetval = odbcPri.GetReturnCode();                              

        /* Counter , Table Index */
        lCounter++ ;
        m_progressBar.StepIt() ;

		/* CHECK FOR FILE HEADER */
		if( !bHeader )
		{
			bHeader = TRUE;
			BLANKSTRUCT( hdrSALHIS );
			COPYSTRING( hdrSALHIS.ASTERISCO1   , "*I*"           );
			COPYSTRING( hdrSALHIS.BANCO        , BCRA_ID_DEFAULT );
            /*****************************/
            if( DPS_INTER_SALHIS == eType )
			    COPYSTRING( hdrSALHIS.TEXTO        , "SALHIS"        )
            else if( DPS_INTER_AHOHIS == eType ) 
                COPYSTRING( hdrSALHIS.TEXTO        , "AHOHIS"        )
			else 
				COPYSTRING( hdrSALHIS.TEXTO        , "AHHIDO"        );
            /*****************************/
			
            /*
            El ultimo dia que se carga es el del dia hábil anterior al que dice en el registro inicial, 
            o sea, si en el registro inicial tiene la fecha yymmdd (que podria ser lunes), el ultimo saldo 
            que se tiene que informar es el yymmdd-1 (que podria haber sido viernes).
            */
            DateTimeToDateYYYYMMDD(szFECHAHDR,szYYYYMMDD); /* "YYYYMMDD" */
            tmWorkingDay.tm_year = antoi(szYYYYMMDD,4)   - 1900 ;
            tmWorkingDay.tm_mon  = antoi(szYYYYMMDD+4,2) - 1    ;
            tmWorkingDay.tm_mday = antoi(szYYYYMMDD+4+2,2) ;    /* yymmdd = today */
            tWorkingDay          = mktime( &tmWorkingDay );
            /* Check WORKING DAY : tm_wday = days since Sunday - [0,6] */
            if( 6 == tmWorkingDay.tm_wday ) /* SATURDAY=6 */
                tmWorkingDay.tm_mday = antoi(szYYYYMMDD+4+2,2) + 2 ; /* yymmdd+2 = MONDAY   */
            else if( 0 == tmWorkingDay.tm_wday ) /* SUNDAY=0 */
                tmWorkingDay.tm_mday = antoi(szYYYYMMDD+4+2,2) + 1 ; /* yymmdd+1 = MONDAY   */
            else 
            {
                /* Check CutOver time 15:00hs */
                if( tmWorkingDay.tm_hour != 0 && tmWorkingDay.tm_hour < 15 )
                    tmWorkingDay.tm_mday = antoi(szYYYYMMDD+4+2,2) - 1 ; /* yymmdd-1 = TODAY */
                else
                    tmWorkingDay.tm_mday = antoi(szYYYYMMDD+4+2,2) ;     /* yymmdd = TODAY */
            }

            /* New DATE */
            tWorkingDay  = mktime( &tmWorkingDay );        
            /* yyymmdd */
            sprintf( hdrSALHIS.FECHA_INICIAL, "%02i%02i%02i",
                    tmWorkingDay.tm_year % 100,
                    tmWorkingDay.tm_mon + 1   ,
                    tmWorkingDay.tm_mday      );
            /* SPACE TO 1ST. CHAR */
			hdrSALHIS.FILLER[0] = ' ';

			fwrite( &hdrSALHIS, sizeof( hdrSALHIS), 1, fBALANCES );
			fflush( fBALANCES );        

		};
        
        /* BEGIN FORMATTING DATA RECORD */
        /********************************/
        if( DPS_INTER_SALHIS == eType )
            COPYSTRING( recSALHIS.SALHIS_TEXTO , "SALHIS"  )  
        else if( DPS_INTER_AHOHIS == eType )
            COPYSTRING( recSALHIS.SALHIS_TEXTO , "AHOHIS"  )
		else
			COPYSTRING( recSALHIS.SALHIS_TEXTO , "AHHIDO"  );      

        /********************************/
        RIGHTCOPYFIELD(recSALHIS.CODBAN,BCRA_ID_DEFAULT);

        /***********************************************/
        COPYSTRING( szCUENTABKP, szCUENTA );
        /***********************************************/
        /* Modificacion al PICTURE de la CUENTA        */
        /***********************************************/            
        PADCOPYFIXEDINT_ASCII(stNUMCTA.chNUMCTA, szCUENTA );
	    RIGHTCOPYFIELD(recSALHIS.NUMCTA,stNUMCTA.chNUMCTA  );
        /***********************************************/

        /* TABLE OF DAILY BALANCES #0-#50 */
		/* DEFAULT ALL ZEROES (in PACKED DECIMAL format) SLOT #0 */
		memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;
		EXACTCOPYFIELD(stREGDIA.SALDIA,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3
		COPYSTRING(stREGDIA.FECDIA, "      " ); // SPACES
		EXACTCOPYFIELD(stREGDIA.TOTDEB,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3
		EXACTCOPYFIELD(stREGDIA.TOTCRE,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3
		for ( nInx = 0; nInx < clMAX_SALDACT_TBL; nInx++)
        {
                recSALHIS.stTABDIA.TABDIA[nInx] = stREGDIA;
        }; /* end-for */
        
        /* TABLE OF MONTHLY BALANCES */
        /* DEFAULT ALL ZEROES (in PACKED DECIMAL format) SLOT #0 */
        /* MONTH #1 = INDEX = 0 */
		COPYSTRING( recSALHIS.stTABMES.TABMES[0].FECMES, "  " ); // SPACES			
		memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;
		EXACTCOPYFIELD(recSALHIS.stTABMES.TABMES[0].SALMEN,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3					
        /* FROM MONTH #12 (INDEX=11) TO #1 (INDEX=0) */
		for ( nInx = 0; 
              nInx < sizeof(recSALHIS.stTABMES.TABMES)/sizeof(recSALHIS.stTABMES.TABMES[0]); 
              nInx++ )
		{                     
            /* AUX VAR */
            short shMonth = 0;
            /* TODAY MONTH */
            tHistoricDay = time(NULL);
            tmHistoricDay = *gmtime( &tHistoricDay );
            /* CURRENT MONTH 1-12 - INDEX */
            if ((tmHistoricDay.tm_mon+1) > (nInx+1)) 
                shMonth = (tmHistoricDay.tm_mon+1) - (nInx+1)  ;
            else
            {
                shMonth = 12 - ((nInx+1) - (tmHistoricDay.tm_mon+1)) ;
                /* FIX a pedido de EDS : solo meses del año en curso */
                shMonth = 0  ;
            }
            /* MONTHLY MEDIA = 0 */
            sprintf( recSALHIS.stTABMES.TABMES[0].FECMES, "%02i", shMonth ); // LAST MONTH  - INDEX
            /* LAST POSITION = LAST MONTH  */
            /* FIRST POSITION = 1 YEAR AGO */
			recSALHIS.stTABMES.TABMES[sizeof(recSALHIS.stTABMES.TABMES)/sizeof(recSALHIS.stTABMES.TABMES[0])-1-nInx] 
                    = 
                    recSALHIS.stTABMES.TABMES[0];					
		};/* end-for */
        /* FIND FIRST MONTH CONTAINING DATA : MOVE TO FIRST POSITION */
		for ( nInx = 0; 
              nInx < sizeof(recSALHIS.stTABMES.TABMES)/sizeof(recSALHIS.stTABMES.TABMES[0]); 
              nInx++ )
		{   
            if( antoi(recSALHIS.stTABMES.TABMES[nInx].FECMES,2) != 0) 
            {
                /* REORDER */
		        for (nJnx = 0; 
                     nJnx < sizeof(recSALHIS.stTABMES.TABMES)/sizeof(recSALHIS.stTABMES.TABMES[0])-nInx; 
                     nJnx++ )
                {
                     recSALHIS.stTABMES.TABMES[nJnx] = recSALHIS.stTABMES.TABMES[nInx+nJnx];
                }
                /* RESET TRAILING SLOTS */
		        for (nJnx = sizeof(recSALHIS.stTABMES.TABMES)/sizeof(recSALHIS.stTABMES.TABMES[0])-nInx; 
                     nJnx < sizeof(recSALHIS.stTABMES.TABMES)/sizeof(recSALHIS.stTABMES.TABMES[0]); 
                     nJnx++ )
                {
		            COPYSTRING( recSALHIS.stTABMES.TABMES[nJnx].FECMES, "00" ); // ZEROES
		            EXACTCOPYFIELD(recSALHIS.stTABMES.TABMES[nJnx].SALMEN,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3					
                }
                /* BREAK */
                break;
            };
        };
        /* FILLER */
        BLANKSTRUCT( recSALHIS.FILLER );

        /**************************************************************/
        /* TODAY = SLOT #0                                            */
        /**************************************************************/
        DateTimeToDateYYYYMMDD(szFECHAPRO,szYYYYMMDD); /* FECHA PROCESO (AYER) */
		memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;				
		EXACTCOPYFIELD(stREGDIA.SALDIA,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3
		COPYSTRING( stREGDIA.FECDIA, szYYYYMMDD + 2 ); // YYMMDD
		EXACTCOPYFIELD(stREGDIA.TOTDEB,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3
		EXACTCOPYFIELD(stREGDIA.TOTCRE,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3

        /**********************************************/
        /* TABLE OF BALANCES - PROPAGATE EMPTY SLOTS  */
        /**********************************************/
        /* Inx = Day Counter                          */
        /* Jnx = WeekEnd Counter                      */
        /* Inx + Jnx = Working Day Counter            */
        /**********************************************/
		for ( nInx = 0, nJnx = 0; nInx < clMAX_SALDACT_TBL; nInx++ )
		{
            /* Today - Inx - Jnx = NEW DATE IN ARRAY */                    
            tmWorkingDay.tm_year = antoi(szYYYYMMDD,4)     - 1900 ;
            tmWorkingDay.tm_mon  = antoi(szYYYYMMDD+4,2)   - 1    ;
            tmWorkingDay.tm_mday = antoi(szYYYYMMDD+4+2,2) - nInx - nJnx ;
            /* Validate Date */
            tWorkingDay          = mktime( &tmWorkingDay );        
            /* Check WORKING DAY : tm_wday = days since Sunday - [0,6] */
            if( 6 == tmWorkingDay.tm_wday ) /* SATURDAY=6 */
            {
                tmWorkingDay.tm_mday = tmWorkingDay.tm_mday - 1 ;   /* yymmdd-1 = FRIDAY */
                nJnx++; /* Weekend Counter */
                /* New DATE */
                tWorkingDay  = mktime( &tmWorkingDay );        
            }
            else if( 0 == tmWorkingDay.tm_wday ) /* SUNDAY=0 */
            {
                tmWorkingDay.tm_mday = tmWorkingDay.tm_mday - 2 ;   /* yymmdd-2 = FRIDAY */
                nJnx = nJnx + 2; /* Weekend Counter */
                /* New DATE */
                tWorkingDay  = mktime( &tmWorkingDay );        
            };

            /* FORMAT DATE STRING IN ASCII */
            sprintf( szAux, "%04i%02i%02i",
                        tmWorkingDay.tm_year + 1900,
                        tmWorkingDay.tm_mon + 1    ,
                        tmWorkingDay.tm_mday       );
            /* Is Holiday ? YYYYMMDD */
            while ( CheckHolidayDateInArray( szAux ) )
            {                    
                /* NEW DATE = PREVIOUS DAY */
                if( 1 == tmWorkingDay.tm_wday ) /* MONDAY=1 */
                {
                    tmWorkingDay.tm_mday = tmWorkingDay.tm_mday - 3 ;   /* yymmdd-3 = FRIDAY */
                    nJnx++;          /* Holiday Counter */
                    nJnx = nJnx + 2; /* Weekend Counter */
                    /* New DATE */
                    tWorkingDay  = mktime( &tmWorkingDay );        
                }
                else 
                {
                    tmWorkingDay.tm_mday = tmWorkingDay.tm_mday - 1 ;   /* yymmdd-1 = previous */
                    nJnx++; /* Holiday Counter */
                    /* New DATE */
                    tWorkingDay  = mktime( &tmWorkingDay );        
                };
                /* FORMAT DATE STRING IN ASCII */
                sprintf( szAux, "%04i%02i%02i",
                            tmWorkingDay.tm_year + 1900,
                            tmWorkingDay.tm_mon + 1    ,
                            tmWorkingDay.tm_mday       );
            }; /* end-Weekend? */

            /* RESET SLOT - INVERSE ORDER : OLDEST FIRST = 1, NEWEST LAST = 50 */
            recSALHIS.stTABDIA.TABDIA[ clMAX_SALDACT_TBL - nInx - 1] = stREGDIA;
            /* FORMAT SLOT DATE : INVERSE ORDER : OLDEST FIRST = 1, NEWEST LAST = 50 */
            sprintf( szAux, "%02i%02i%02i",
                        tmWorkingDay.tm_year % 100,
                        tmWorkingDay.tm_mon + 1   ,
                        tmWorkingDay.tm_mday      );
            EXACTCOPYFIELD( recSALHIS.stTABDIA.TABDIA[ clMAX_SALDACT_TBL - nInx - 1].FECDIA, szAux );

		};/* end-for-I */

        /* ARRAY BACKUP */
        stTABDIA = recSALHIS.stTABDIA ;

        /* RESET BACKUP VARIABLES  */
		memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;				
		EXACTCOPYFIELD(stREGDIA.SALDIA,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3
		COPYSTRING( stREGDIA.FECDIA, "      " ); // YYMMDD
		EXACTCOPYFIELD(stREGDIA.TOTDEB,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3
		EXACTCOPYFIELD(stREGDIA.TOTCRE,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3
        COPYSTRING( szSALHIST  , "0.0");
        COPYSTRING( szSUMADB   , szSALHIST);
        COPYSTRING( szSUMACR   , szSALHIST);
        COPYSTRING( szSALNUEVO , szSALHIST);
        COPYSTRING( szSALNUEBKP, szSALHIST);
        /* RESET BACKUP VARIABLES  */

        /*****************************************************************/
        /* RECALCULATE BALANCES IN ARRAY                                 */
        /*****************************************************************/
		for ( nInx = 0, nJnx = 0, nSearch = 0; /* FIX EMPTY SLOTS */
              nInx < clMAX_SALDACT_TBL && nJnx < clMAX_SALDACT_TBL; 
              nInx++ )
		{
            /* FORMAT DATE-TIME */            
            DateYYYYMMDDToDateTime( stTABDIA.TABDIA[nInx].FECDIA, szYYYYMMDD, '-'); 
            /* RESET COLUMN VARIABLES  */
            COPYSTRING( szSALHIST, "0.0");
            COPYSTRING( szSUMADB , szSALHIST);
            COPYSTRING( szSUMACR , szSALHIST);

            /* PREPARE QUERY */
            odbcSec.ResetLastError();
            iQueryNumSec = _ODBC_EXECUTE_SNTC_ + eType + 1;		
            odbcSec.Column( szSALHIST , SQL_C_CHAR, 19+1, &iQueryNumSec ); //1
            odbcSec.Column( szSUMADB  , SQL_C_CHAR, 19+1, &iQueryNumSec ); //2
            odbcSec.Column( szSUMACR  , SQL_C_CHAR, 19+1, &iQueryNumSec ); //3
            odbcSec.Column( szSALNUEVO, SQL_C_CHAR, 19+1, &iQueryNumSec ); //4

            /* FORMAT QUERY */

#ifdef _USE_EMBEBED_SQL_
#else
            strQuery = "EXEC SP_SALDOS_Y_SUMAS_DBCR_POR_FECHA_main '" ;
            strQuery += szYYYYMMDD ;            
            strQuery += "', '";
            strQuery += szCUENTABKP ;
            strQuery += "', '";
			strQuery += szFECHAHDR;
			strQuery += "' ";
#endif
            /* EXECUTE QUERY */            
            odbcSec.Query( (PSTR)strQuery.c_str(), &iQueryNumSec, TRUE, TRUE ) ;
            if(odbcSec.Error())
            {
                /* FREE SENTENCE     */
                odbcSec.FreeSentence( iQueryNumSec );
                /* CYCLE TO NEXT DAY */
                continue;
            };
            odbcSec.Fetch( &iQueryNumSec );
            if( odbcSec.Error() || odbcSec.GetReturnCode() == SQL_NO_DATA )
            {
                /* PROPAGATE LAST BALANCE  */
		        NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
			        szSALNUEBKP, strlen(szSALNUEBKP), 0x00, is_true ) ;
		        EXACTCOPYFIELD(recSALHIS.stTABDIA.TABDIA[nInx].SALDIA , stPKDECIMAL.chPKDECIMAL); 
                /* FREE SENTENCE     */
                odbcSec.FreeSentence( iQueryNumSec );
                /* CYCLE TO NEXT DAY */
                continue;
            };
            odbcSec.FreeSentence( iQueryNumSec );

            /* COUNT SLOT - FIX EMPTY SLOTS */
            nSearch++;

            /* SAVE NEW BALANCE */
            COPYSTRING( szSALNUEBKP, szSALNUEVO );

            /* COPY VALUES TO SLOT */
            if( bUsePostBalance )
			    NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
				    szSALNUEVO, strlen(szSALNUEVO), 0x00, is_true ) ;
            else
			    NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
				    szSALHIST, strlen(szSALHIST), 0x00, is_true ) ;
			EXACTCOPYFIELD(recSALHIS.stTABDIA.TABDIA[nInx].SALDIA , stPKDECIMAL.chPKDECIMAL); 

            /* SIN SIGNO porque ya es campo discrimado para DEBITOS */
            if( '-' == szSUMADB[0] )
		        szSUMADB[0] = '0';
			NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
				szSUMADB, strlen(szSUMADB), 0x00, is_true ) ;
            EXACTCOPYFIELD(recSALHIS.stTABDIA.TABDIA[nInx].TOTDEB , stPKDECIMAL.chPKDECIMAL); 

			NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
				szSUMACR, strlen(szSUMACR), 0x00, is_true ) ;
            EXACTCOPYFIELD(recSALHIS.stTABDIA.TABDIA[nInx].TOTCRE , stPKDECIMAL.chPKDECIMAL); 

            UpdateData( FALSE );
	        ProcessAppMsgQueue();

        };/* end-for-I */

        /*******************/
        /* FIX EMPTY SLOTS */
        /*******************/
        if( 0 == nSearch )
        {
            /* SAVE NEW BALANCE */
            COPYSTRING( szSALNUEVO , szSALACT );
            /* SIN SIGNO porque ya es campo discrimado para DEBITOS */
            if( '-' == szSALACT[0] )
                szSALNUEVO[0] = '0';
            /* COPY VALUES TO SLOT */
            if( bUsePostBalance )
			    NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
				    szSALNUEVO, strlen(szSALNUEVO), 0x00, is_true ) ;
            else
			    NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
				    szSALHIST, strlen(szSALHIST), 0x00, is_true ) ;
            /* DAY BALANCE */
            EXACTCOPYFIELD(recSALHIS.stTABDIA.TABDIA[clMAX_SALDACT_TBL-1].SALDIA , stPKDECIMAL.chPKDECIMAL); 
            /* ORIGINAL DB/CR MOV. */
            if( '-' == szSALACT[0] )
                EXACTCOPYFIELD(recSALHIS.stTABDIA.TABDIA[clMAX_SALDACT_TBL-1].TOTDEB , stPKDECIMAL.chPKDECIMAL)
            else
                EXACTCOPYFIELD(recSALHIS.stTABDIA.TABDIA[clMAX_SALDACT_TBL-1].TOTCRE , stPKDECIMAL.chPKDECIMAL); 			
        };

        /* FORMAT CAJ.AHO. INTERFASE */
        if( DPS_INTER_SALHIS != eType )
        {
            if(DPS_INTER_AHOHIS != eType)
			{
				BLANKSTRUCT( recAHHIDO );
				EXACTCOPYFIELD( recAHHIDO.AHOHIS_TEXTO, recSALHIS.SALHIS_TEXTO );
				EXACTCOPYFIELD( recAHHIDO.CODBAN, recSALHIS.CODBAN );
				EXACTCOPYFIELD( recAHHIDO.NUMCTA, recSALHIS.NUMCTA );
				recAHHIDO.stTABDIA = recSALHIS.stTABDIA;				
			}
			else
			{
				BLANKSTRUCT( recAHOHIS );
				EXACTCOPYFIELD( recAHOHIS.AHOHIS_TEXTO, recSALHIS.SALHIS_TEXTO );
				EXACTCOPYFIELD( recAHOHIS.CODBAN, recSALHIS.CODBAN );
				EXACTCOPYFIELD( recAHOHIS.NUMCTA, recSALHIS.NUMCTA );
				recAHOHIS.stTABDIA = recSALHIS.stTABDIA;
			}
        };

            
        /* I/O OPERATION */
        if( DPS_INTER_SALHIS == eType )
		    fwrite( &recSALHIS, sizeof( recSALHIS), 1, fBALANCES );
        else if( DPS_INTER_AHOHIS == eType )
            fwrite( &recAHOHIS, sizeof( recAHOHIS), 1, fBALANCES );
		else
			fwrite( &recAHHIDO, sizeof( recAHHIDO), 1, fBALANCES );
		fflush( fBALANCES );    

		fprintf(fLogg,"\r\n\r\nDATANET - REGISTRO BATCH DE SALDOS HISTORICOS");
		fprintf(fLogg,"\r\n---------------------------------------------");
		fprintf(fLogg,"\r\nHEADER [%*.*s]",6,6,recSALHIS.SALHIS_TEXTO	);        
		fprintf(fLogg,"\r\nBANCO [%*.*s]",3,3,recSALHIS.CODBAN	);        
		fprintf(fLogg,"\r\nCUENTA [%*.*s]",17,17,recSALHIS.NUMCTA);        
        fprintf(fLogg,"\r\nTABLA DE SALDOS, TOTALES DB Y CR:");
		fflush( fLogg );        
		for ( size_t nInx = 0; nInx < clMAX_SALDACT_TBL; nInx++ )
		{
            fprintf(fLogg,"\r\n\t%02i-FECDIA [%6.6s] ", nInx , recSALHIS.stTABDIA.TABDIA[nInx].FECDIA );
            PackedDecimalToString( (PBYTE)recSALHIS.stTABDIA.TABDIA[nInx].SALDIA,9, 
			    (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
	        fprintf(fLogg,"   SALDIA [%*.*s]", 18,18, stPKDECIMAL.chPKDECIMAL );
            PackedDecimalToString( (PBYTE)recSALHIS.stTABDIA.TABDIA[nInx].TOTDEB,9, 
			    (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
	        fprintf(fLogg,"   TOTDEB [%*.*s]", 18,18, stPKDECIMAL.chPKDECIMAL );
            PackedDecimalToString( (PBYTE)recSALHIS.stTABDIA.TABDIA[nInx].TOTCRE,9, 
			    (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
	        fprintf(fLogg,"   TOTCRE [%*.*s]", 18,18, stPKDECIMAL.chPKDECIMAL );
        }; /* end-for */
    	fflush( fLogg );        
        /* DISPLAY PARA TABLA DE MESES */
		for ( size_t nInx = 0; 
              nInx < sizeof(recSALHIS.stTABMES.TABMES)/sizeof(recSALHIS.stTABMES.TABMES[0]); 
              nInx++ )
		{
            fprintf(fLogg,"\r\n\t%02i-MES [%2.2s] ", nInx , recSALHIS.stTABMES.TABMES[nInx].FECMES );	        
            PackedDecimalToString( (PBYTE)recSALHIS.stTABMES.TABMES[nInx].SALMEN,9, 
			    (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
            fprintf(fLogg,"   SALPROM [%*.*s]", 18,18, stPKDECIMAL.chPKDECIMAL );
        }; /* end-for */
        /* DISPLAY PARA TABLA DE MESES */
    	fflush( fLogg );        

		m_editTexto = m_editTexto + " #" + szCUENTA; 
		UpdateData( FALSE );
		ProcessAppMsgQueue();                

  }; /* END FETCH CYCLE */
  /*****************************************************/

  /* FILE FOOTER - ALWAYS */
  if( bHeader )
  {        
      BLANKSTRUCT( ftrSALHIS );
      COPYSTRING( ftrSALHIS.ASTERISCO1   , "*F*"           );
      COPYSTRING( ftrSALHIS.BANCO        , BCRA_ID_DEFAULT );
      /*************************************************/
      if( DPS_INTER_SALHIS == eType )
        COPYSTRING( ftrSALHIS.TEXTO        , "SALHIS"        )
      else if( DPS_INTER_AHOHIS == eType )
        COPYSTRING( ftrSALHIS.TEXTO        , "AHOHIS"        )
	  else
		COPYSTRING( ftrSALHIS.TEXTO        , "AHHIDO"        );      
      /*************************************************/
	  ftrSALHIS.FILLER[0] = ' ';

      fwrite( &ftrSALHIS, sizeof( ftrSALHIS), 1, fBALANCES );
      fflush( fBALANCES );        
  }

  /* POSTCONDITION */
  fclose( fLogg) ;
  fLogg     = NULL;
  fclose( fBALANCES ) ;
  fBALANCES = NULL;

  /* MESSAGE */
  m_editTexto = "FIN DE LA EJECUCION, REGISTROS : " ;
  m_editTexto = m_editTexto + itoa( lCounter, szAux, 10) ;			
  UpdateData( FALSE );
  ProcessAppMsgQueue();

  /* ASCII-2-EBCDIC */
  if( m_checkConvertASCII )
  {
    /******************************************/
    strcpy( szBcraFilename, "LARGOS." );
    strcat( szBcraFilename, BCRA_ID_INTERF );
    /******************************************/
    lRetval = CONVERT_ASCII_EBCDIC_FILE( szDatFilename, ASCII_2_EBCDIC_NO_CRLF, sizeof(ftrSALHIS), &lCounter ,
                                         szBcraFilename ) ;
  };


  /* MESSAGE */
  m_editTexto = m_editTexto + " - CONVERSION EBCDIC, REGISTROS : " ;
  m_editTexto = m_editTexto + itoa( lCounter, szAux, 10) ;			
  UpdateData( FALSE );
  ProcessAppMsgQueue();

  AfxMessageBox( m_editTexto );


  m_progressBar.SetRange32(0, 0); /* NO COUNTERS IN PROGRESS BAR */
  // Disconnect DB
  odbcPri.Disconnect();
  // Disconnect DB
  odbcSec.Disconnect();

  // Set Cur File
  m_comboHacia.SetWindowText( szLogFilename );
  if(!m_comboHacia.FindStringExact( 1, szLogFilename ))
  	 m_comboHacia.AddString( szLogFilename );	

}

void CFuegoDPSDlg::DoMovimientos( e_dps_interfases eType , BOOL bTruncate ) 
{
    /**/
    struct HEADER_MOVCUE_tag hdrMOVCUE;
    struct ENREGI_MOVCUE_tag recMOVCUE;
    struct FOOTER_MOVCUE_tag ftrMOVCUE;
    /**/
    struct HEADER_MOVDIF_tag hdrMOVDIF;
    struct ENREGI_MOVDIF_tag recMOVDIF;

    struct ENREGI_AHODIF_tag recAHODIF; /* FIX AHODIF */
    struct FOOTER_MOVDIF_tag ftrMOVDIF;

    struct ENREGI_AHDIDO_tag recAHDIDO; /* FIX AHODIF */
    //struct FOOTER_MOVDIF_tag ftrAHDIDO;

	

    /**/
    FILE *fLogg      = NULL,
         *fMOVCUE    = NULL; /* MOVCUE & MOVDIF */
    /**/
    int           iQueryNum = 0;
    long          lRetval   = 0;
    char          szCUENTA[20+1]      = {0x00};
    char          szIMPORTE[19+1]     = {0x00};
    char          szSALDO[19+1]       = {0x00};
    char          szSECUENCIAL[12+1]  = {0x00};
    char          szOPERACION[4+1]    = {0x00};
    char          szDEBCRE[2+1]       = {0x00};
    char          szFECHACON[30+1]    = {0x00}; /* FECHA CONTABLE DEL MOVIMIENTO */
    char          szFECHAVAL[30+1]    = {0x00}; /* FECHA VALOR DEL MOVIMIENTO  */
    char          szFECHAOPE[30+1]    = {0x00}; /* FECHA OPERACION DEL MOVIMIENTO */
    char          szFECHAPRO[30+1]    = {0x00}; /* FECHA DEL PROCESO/PROGRAMA */
    char          szFECHAINI[30+1]    = {0x00}; /* FECHA INICIAL DEL RANGO DESDE/HASTA */
    char          szFECHAINI_DIF[30+1]= {0x00}; /* FECHA INICIAL DEL RANGO DESDE/HASTA (MOVDIF) */    
    char          szFECHAEND[30+1]    = {0x00}; /* FECHA FINAL   DEL RANGO DESDE/HASTA */
    char          szSUCURSAL[3+1]     = {0x00};  
    char          szDEPOSITO[8+1]     = {0x00};  
    char          szACUERDO[19+1]     = {0x00};  
    char          szNOMBRE[25+1]      = {0x00};  
    char          szDESCRIPCION[25+1]       = {0x00};
	char		  szCODOPEIB[3+1]	  = {0x00};
	char		  szDESCRLAR[25]    = {0x00};
	char		  szMarcaDifer[1+1]   = {0x00};
	char		  szMuestroCORTOS[1+1] = {0x00};
	char		  szCUITORIGINANTE[11+1] = {" "};
	char		  szDENOMINACIONOR[40+1] = {" "};
	char		  szCBU			  [22] = {" "};
	char		  szCODOPEORIGINAL[20+1] = {" "};
    /* FIX STACK CORRUPTED VISUAL .NET (64 BYTES ANTERIORMENTE) */
    char          szDESCRIPCION_LARGA[256+1] = {0x00};
	char          szTIPCTA[5+1]       = {0x00};
	char          szFECHA_APER[30+1]     = {0x00};
	char          szRAZON_SOCIAL[256+1]  = {0x00};
    struct stPKDECIMAL_tag
    {
        char          chPKDECIMAL[9]; /* PIC S9(15)V99 = 17 bytes ( 16 + signo ) = empaquetados 9 bytes */
        char          chOVERFLOW[11]; /* OVERFLOW AREA */
    } stPKDECIMAL ; /* 20 BYTES = 160 bits */
    char          szYYYYMMDD[8+1] = {0x00};
    long          lCounter   = 0,
                  lSaved     = 0,
                  lDaysRange = 1;
    bool          bHeader   = FALSE;
    const size_t  cnSize    = sizeof(recMOVCUE);
    char		  szAux[32] = {0x00};
    USHORT       *ptrUSHORT = NULL;      /* Computational-3 Conversion : 2 bytes integer */
    CONST BOOL    bMaskForEBCDIC = TRUE; /* Computational-3 Conversion : EBCDIC mapped?  */
    /******************************************/
    char          szBcraFilename[256] = { "CORTOS.999" };
    char          szDatFilename[]     = "CORTOS.dat" ;
    /******************************************/
    /*********************************************/
    TrxResBATCH	trxBatch;
    /******************************************************************************/
    /* Auxiliares para verificar si es dia habil */
    struct tm    tmWorkingDay    = {0,0,0,0,0,0,0,0,0},
                 tmBkpWorkingDay = {0,0,0,0,0,0,0,0,0};
    time_t       tWorkingDay    = 0,
                 tBkpWorkingDay = 0;        
    string       strYYYYMMDD  = "yyyymmdd";
    FIXEDINTEGER fixAmmnt   = 0;
    /* PRECONDITION : Local Settings USA : decimal_point "." - thousand_point "," */
    const char *szLocale = setlocale( LC_ALL, "LC_MONETARY=;LC_NUMERIC=;" ); 
    const struct lconv *pstConv = localeconv();
    /******************************************************************************/
    /* TIPO DE REGISTRO A EMITIR */
    enumRECTYPE  recTYPE     = RT_MOVCUE ;
    /******************************************************************************/
    boolean_t    bUSE_BALANCE_RECALC = is_true; /* SI SE AJUSTAN SALDOS HISTORICOS  */
    /******************************************************************************/
    stNUMCTA_t   stNUMCTA = { 0x00, 0x00 };
    /****************************************************/
    static char   szFECHAINI_SAV[30+1]= {0x00}; /* FECHA INICIAL BACKUP PARA N CICLOS (MOVDIF) */
    /**********************************************************************/
	/* DESCRIPCION ABREVIADA? O SINO NRO.INTERNO + COMPROBANTE?           */
	const BOOL    bUSE_UMOV_DESCRIPCION  = _UMOVPBF_USE_UMOV_DESCRIPCION_ ;
	/**********************************************************************/
	/* FIX ODBC DRIVER , cambia tipo CHAR(19) x FLOAT */
	float         fIMPORTE			  = 0.0f;
	const BOOL	  bUseRealAmmount     = FALSE;
	/* FIX ODBC DRIVER , cambia tipo CHAR(19) x FLOAT */
	/**********************************************************************/
	char		  szCODOPE[25+1]	  = {"000"};
	/* FIX anular mapeos internos de codigos de operacion, 2012.12.05 mdc    */
	const BOOL	  bUseInternalCodOpe  = FALSE;
	/**********************************************************************/

	FILE	*fFeriados;
	char	szLogFeriados[] = "FERIADOS.log";

	if( bTruncate )
        fFeriados = fopen(szLogFeriados, "wt");
    else
        fFeriados = fopen(szLogFeriados, "at");

	fprintf(fFeriados,"-----------------------------\n");
	fprintf(fFeriados,"-----------------------------\n");
	fprintf(fFeriados,"LOG PARA ERROR CON FECHA DE FERIADOS\n");  

    /* PRECONDITION */
    if( bTruncate )
        fLogg = fopen("MOVCUE.DATANET.log", "wt");
    else
        fLogg = fopen("MOVCUE.DATANET.log", "at");
    if(!fLogg)
    {
	    m_editTexto = "NO SE PUEDE CREAR EL ARCHIVO DE LOGG";                
        AfxMessageBox(m_editTexto);
        UpdateData( FALSE );
        ProcessAppMsgQueue();
	    return;
    }
    else
	    fprintf(fLogg,"\r\nBANCO INTERFINANZAS - MOVIMIENTOS DE CUENTAS PARA DATANET - VIA ODBC\r\n");

    /* PRECONDITION : TRUNCATE FILE 'CORTOS' (MOVCUE & MOVDIF) */
    if( bTruncate )
        fMOVCUE = fopen( szDatFilename , "wt");
    else
        fMOVCUE = fopen( szDatFilename , "at");
    if(!fMOVCUE)
    {
	    m_editTexto = "NO SE PUEDE CREAR EL ARCHIVO DE MOVIMIENTOS DE CUENTAS";                
        AfxMessageBox(m_editTexto);
        UpdateData( FALSE );
	    ProcessAppMsgQueue();

	    fprintf(fLogg,"\r\n");
	    fprintf(fLogg, (LPCSTR)m_editTexto);
	    fprintf(fLogg,"\r\n");
        fflush( fLogg );        
	    return;
    };

    /* CONNECT DATABASE */
    odbcPri.Disconnect() ;  
    odbcPri.SetUserName( (PSTR)strUSER.c_str()     );
    odbcPri.SetPassword( (PSTR)strPASSWORD.c_str() );
    odbcPri.Connect    ( (PSTR)strDATABASE.c_str() ) ;  
    if(odbcPri.Error())
    {
	    m_editTexto =  "NO SE PUEDE CONECTAR A BASE DE DATOS : ";                
	    m_editTexto += odbcPri.GetErrorMessage(NULL);
	    m_editTexto += odbcPri.GetLastSQLCommand(NULL);
        AfxMessageBox(m_editTexto);
        UpdateData( FALSE );
	    ProcessAppMsgQueue();

	    fprintf(fLogg,"\r\n");
	    fprintf(fLogg, (LPCSTR)m_editTexto);
	    fprintf(fLogg,"\r\n");	  
        fflush( fLogg ); 
	    return;
    };

    /* PRECONDITION : REGISTER SIZE = 160 + 6 */
    //if( cnSize != 160 + 6)
	/*if( cnSize != 400 )
    {
	    odbcPri.Disconnect();
	    AfxMessageBox("EL PROGRAMA NO HA SIDO COMPILADO CON ALINEACION A 2 BYTES. ABORTANDO.");
	    return;
    }*/
       
    /*****************************************************/
    m_datetimePicker.GetTime ( ctDate2 );   /* BACKUP FROM */
    m_datetimePicker2.GetTime( ctDate );    /* BACKUP TO   */    
    if( !m_bReprocesamiento )
    {
        m_datetimePicker.SetTime( &ctDate );    /* FROM = TO */
        UpdateData( FALSE );
        ProcessAppMsgQueue();
    };
    /*****************************************************/
    m_datetimePicker.GetTime( ctDate2 ); /* FORMAT FROM-DATE */ 
    strDate.Format( "%04i-%02i-%02i" , ctDate2.GetYear(),
                                        ctDate2.GetMonth(),
                                        ctDate2.GetDay() );
    /* SAVE PROCESSING DATE */
    COPYSTRING( szFECHAPRO, (LPCSTR)strDate );  
    /* SAVE STARTING DATE */
    COPYSTRING( szFECHAINI, (LPCSTR)strDate );
    
    /*****************************************************/
    m_datetimePicker2.GetTime( ctDate );  /* FORMAT TO-DATE */ 
    strDate.Format( "%04i-%02i-%02i" , ctDate.GetYear(),
                                        ctDate.GetMonth(),
                                        ctDate.GetDay() );

	std::string sFechaHastaPosta(strDate);
    /* SAVE FINISHING DATE */
    COPYSTRING( szFECHAEND, (LPCSTR)strDate );
    COPYSTRING( szFECHACON, (LPCSTR)strDate );  
    COPYSTRING( szFECHAPRO, (LPCSTR)strDate );
    /*****************************************************/

    /* DATE TO BEGIN ? */
    /* DATE TO BEGIN = DATE TO PROCESS - 1 WORKING-DAY AGO */
    tmWorkingDay.tm_year = ctDate.GetYear()  - 1900; /* Base 1900 */
    tmWorkingDay.tm_mon  = ctDate.GetMonth() - 1;   /* [0,11] */    
    tmWorkingDay.tm_mday = ctDate.GetDay()   - 1;   /* [1,31] */  
    /* NEW DATE */
    tWorkingDay = mktime( &tmWorkingDay );
    /* CHECK WEEK-END - SUNDAY=0 , SATURDAY=6 */ 
//EAE 20160218 ->	
	lDaysRange = 0;
	fprintf(fFeriados,"-----------------------------\n");
	fprintf(fFeriados,"CALCULO DEL RANGO DEL FERIADO\n");
	while (true)
	{
		tWorkingDay = mktime( &tmWorkingDay );
		sprintf( szAux, "%04i%02i%02i",
                        tmWorkingDay.tm_year + 1900,
                        tmWorkingDay.tm_mon + 1    ,
                        tmWorkingDay.tm_mday       );

		fprintf(fFeriados,"--> lDaysRange: %i, szAux: %i \n", lDaysRange, atoi(szAux));

		if(0 == tmWorkingDay.tm_wday || 6 == tmWorkingDay.tm_wday || CheckHolidayDateInArray(szAux) )
		{
			tmWorkingDay.tm_mday -= 1;
			lDaysRange += 1;
		}
		else
		{break;}
	}
//EAE <-

	/* HOLIDAY? - FORMAT DATE STRING IN ASCII */
	char szYYYYMMDDaux[16+1] = {0x00};						/* "yyyymmdd" */
	DateTimeToDateYYYYMMDD(szFECHAPRO, szYYYYMMDDaux);   /* "YYYYMMDD" */
    if( CheckHolidayDateInArray( szYYYYMMDDaux ) )
	{
		/* MESSAGE */
		m_editTexto = "LA FECHA INGRESADA ES INVALIDA" ;
		UpdateData( FALSE );
		ProcessAppMsgQueue();

		AfxMessageBox( m_editTexto );
		return;
	}
	
	/* NEW DATE */
    tWorkingDay = mktime( &tmWorkingDay );    
    /* SAVE PREVIOUS DATE */
    strDate.Format( "%04i-%02i-%02i" ,  tmWorkingDay.tm_year + 1900,
                                        tmWorkingDay.tm_mon + 1    ,
                                        tmWorkingDay.tm_mday       );  
    if( !m_bReprocesamiento )
    {
        /* SAVE STARTING DATE */
        COPYSTRING( szFECHAINI, (LPCSTR)strDate );    
        /* SAVE STARTING DATE (MOVDIF) */
        COPYSTRING( szFECHAINI_DIF, (LPCSTR)strDate );    
        /* SAVE STARTING DATE (MOVDIF) ONLY IF NULL */
        if ( strlen( szFECHAINI_SAV ) == 0 )
            COPYSTRING( szFECHAINI_SAV, (LPCSTR)strDate );    
        /* SAVE FINISHING DATE */
        COPYSTRING( szFECHAEND, szFECHAINI );
    };

	fprintf(fFeriados,"-----------------------------\n");
	fprintf(fFeriados,"LOS PARAMETROS FINALES OBTENIDOS\n");
	fprintf(fFeriados,"--> lDaysRange: %i, szFECHAPRO: %0.31s, szFECHAINI: %0.31s, szFECHAEND: %0.31s \n", lDaysRange, szFECHAPRO, szFECHAINI, szFECHAEND);
	
    /********************************************************************/
    /* REPROCESSING?                                                    */
    /********************************************************************/
    if( !m_bReprocesamiento )
    {
        CTime ctDateTemp( tWorkingDay );
        m_datetimePicker.SetTime( &ctDateTemp );   /* FROM */
        UpdateData( FALSE );
        ProcessAppMsgQueue();
    }
    else
    {
        /* DAYS RANGE (FINISH-START)*/
        tmBkpWorkingDay.tm_year = ctDate2.GetYear() - 1900; /* Base 1900 */
        tmBkpWorkingDay.tm_mon  = ctDate2.GetMonth() - 1;   /* [0,11] */    
        tmBkpWorkingDay.tm_mday = ctDate2.GetDay() ;        /* [1,31] */  
        /* NEW DATE */
        tBkpWorkingDay = mktime( &tmBkpWorkingDay );
        if( tmBkpWorkingDay.tm_yday > tmWorkingDay.tm_yday )
            lDaysRange = tmBkpWorkingDay.tm_yday - tmWorkingDay.tm_yday;
        else
            lDaysRange = tmWorkingDay.tm_yday - tmBkpWorkingDay.tm_yday ;
        /* SAVE PREVIOUS DATE */
        strDate.Format( "%04i-%02i-%02i" ,  tmBkpWorkingDay.tm_year + 1900,
                                            tmBkpWorkingDay.tm_mon + 1    ,
                                            tmBkpWorkingDay.tm_mday       );  
        /* SAVE STARTING DATE */
        COPYSTRING( szFECHAINI, (LPCSTR)strDate );    

        /* FINISHING DATE (MOVDIF) 1 DAY BEFORE END-DATE */
        tmBkpWorkingDay.tm_year = ctDate.GetYear()  - 1900; /* Base 1900 */
        tmBkpWorkingDay.tm_mon  = ctDate.GetMonth() - 1;    /* [0,11] */    
        tmBkpWorkingDay.tm_mday = ctDate.GetDay()   - 1;    /* [1,31] */  
        /* NEW DATE (MOVDIF) */
        tBkpWorkingDay = mktime( &tmBkpWorkingDay );
        /* SAVE PREVIOUS DATE (MOVDIF) */
        strDate.Format( "%04i-%02i-%02i" ,  tmBkpWorkingDay.tm_year + 1900,
                                            tmBkpWorkingDay.tm_mon + 1    ,
                                            tmBkpWorkingDay.tm_mday       );  
        /* SAVE STARTING DATE (MOVDIF) */
        COPYSTRING( szFECHAINI_DIF, (LPCSTR)strDate );
        /* SAVE STARTING DATE (MOVDIF) ONLY IF NULL */
        if ( strlen( szFECHAINI_SAV ) == 0 )
            COPYSTRING( szFECHAINI_SAV, (LPCSTR)strDate );    

    };
    /********************************************************************/


    /*****************************************************/
    /* PREPARE QUERY                                     */
    /*****************************************************/
    iQueryNum = _ODBC_UPDATE_SNTC_ ;		
    /* QUERY */
    #ifdef _USE_EMBEBED_SQL_
    strQuery =  "UPDATE DN_PARAMETROS SET DN_FECHA_PROCESO = '" + szFECHAPRO ;
    strQuery += "' , DN_FECHA_DESDE = '" ;
    strQuery += szFECHAINI ;  
    strQuery += "' , DN_FECHA_HASTA = '" ;
    strQuery += szFECHAEND ;
    strQuery += "' WHERE DN_ID=1" ;
    #else
    strQuery = "EXEC SP_UPDATE_DN_PARAMETROS '" ;
    strQuery += szFECHAPRO ; 
    strQuery += "', '" ;
    strQuery += szFECHAINI ;  
    strQuery += "', '" ;
    strQuery += szFECHAEND ;  
    strQuery += "', 1 , " ;
    strQuery += itoa( 1 + lDaysRange, szAux, 10);
    #endif
    /* EXECUTE QUERY */
    odbcPri.Query( (PSTR)strQuery.c_str(), &iQueryNum, FALSE, FALSE ) ;
    if(odbcPri.Error())
    {
	        m_editTexto = "NO SE PUEDE ACTUALIZAR LA FECHA DE PROCESO SQL : "; 
	        m_editTexto += odbcPri.GetErrorMessage(NULL);
	        m_editTexto += odbcPri.GetLastSQLCommand(NULL);
            AfxMessageBox(m_editTexto);
            UpdateData( FALSE );
	        ProcessAppMsgQueue( );

	        odbcPri.Disconnect();
	        return;
    };
    odbcPri.FreeSentence( iQueryNum );
    /****************************************************************************************/

    /*******************************************************/
    /* MOVCUE or MOVDIF? AHOMOV or AHODIF ? */
    if ( DPS_INTER_MOVCUE == eType )
        recTYPE = RT_MOVCUE ;
    else if ( DPS_INTER_MOVDIF == eType )
        recTYPE = RT_MOVDIF ;
    else if ( DPS_INTER_AHOMOV == eType )
        recTYPE = RT_AHOMOV ;
    else if ( DPS_INTER_AHODIF == eType )
        recTYPE = RT_AHODIF ;
    else if ( DPS_INTER_AHMODO == eType )
        recTYPE = RT_AHMODO ;
    else if ( DPS_INTER_AHDIDO == eType )
        recTYPE = RT_AHDIDO ;
    else
    {
        AfxMessageBox("OPCION INVALIDA DE INTERFASE. SIN PROCESAR.");
        return ;
    }

    /*******************************************************/
    /* PREPARE SELECT QUERY  */
    iQueryNum = _ODBC_READ_SNTC_ + recTYPE;		
    odbcPri.Column( szCUENTA     , SQL_C_CHAR, 20+1, &iQueryNum ); //1
    odbcPri.Column( szFECHACON   , SQL_C_CHAR, 30+1, &iQueryNum ); //2
    odbcPri.Column( szSECUENCIAL , SQL_C_CHAR, 12+1, &iQueryNum ); //3
    odbcPri.Column( szSALDO      , SQL_C_CHAR, 19+1, &iQueryNum ); //4    
	/* FIX ODBC DRIVER , cambia tipo CHAR(19) x FLOAT */
	if( bUseRealAmmount )
		odbcPri.Column( &fIMPORTE    , SQL_C_FLOAT, 19+1, &iQueryNum ); //5  
	else
		odbcPri.Column( szIMPORTE    , SQL_C_CHAR, 19+1, &iQueryNum ); //5  
    odbcPri.Column( szDEBCRE     , SQL_C_CHAR,  2+1, &iQueryNum ); //6  
    odbcPri.Column( szFECHAOPE   , SQL_C_CHAR, 30+1, &iQueryNum ); //7
    odbcPri.Column( szSUCURSAL   , SQL_C_CHAR,  3+1, &iQueryNum ); //8
    odbcPri.Column( szDEPOSITO   , SQL_C_CHAR,  8+1, &iQueryNum ); //9
    odbcPri.Column( szDESCRIPCION, SQL_C_CHAR, 25+1, &iQueryNum ); //10
    odbcPri.Column( szOPERACION  , SQL_C_CHAR,  4+1, &iQueryNum ); //11    
    odbcPri.Column( szACUERDO    , SQL_C_CHAR, 19+1, &iQueryNum ); //12
    odbcPri.Column( szNOMBRE     , SQL_C_CHAR, 25+1, &iQueryNum ); //13    
    odbcPri.Column( szFECHAVAL   , SQL_C_CHAR, 30+1, &iQueryNum ); //14
	odbcPri.Column( szFECHA_APER  , SQL_C_CHAR, 30+1, &iQueryNum ); //4
	odbcPri.Column( szTIPCTA	  , SQL_C_CHAR,  5+1, &iQueryNum ); //4
	odbcPri.Column( szRAZON_SOCIAL, SQL_C_CHAR, 50+1, &iQueryNum ); //5
	odbcPri.Column( szCODOPEIB   , SQL_C_CHAR,  3+1, &iQueryNum ); //14
	odbcPri.Column( szDESCRLAR   , SQL_C_CHAR, 30+1, &iQueryNum ); //14
	odbcPri.Column( szMarcaDifer , SQL_C_CHAR, 1+1, &iQueryNum ); // 22
	odbcPri.Column( szMuestroCORTOS, SQL_C_CHAR, 1+1, &iQueryNum ); //23
	odbcPri.Column( szCUITORIGINANTE, SQL_C_CHAR, 11+1, &iQueryNum ); //23
	odbcPri.Column( szDENOMINACIONOR, SQL_C_CHAR, 40+1, &iQueryNum ); //23
	odbcPri.Column( szCODOPEORIGINAL, SQL_C_CHAR, 20+1, &iQueryNum);
#ifdef _USE_EMBEBED_SQL_
    strQuery = " SELECT * FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS " ;
    /****************************************************/
    if( DPS_INTER_MOVCUE == eType || DPS_INTER_MOVDIF == eType )
          strQuery += " 1,7";
    else
          strQuery += " 11,15";
    /****************************************************/
    strQuery += " ORDER BY FECHA, CUENTA, SECUENCIAL " ;
#else
    strQuery = " EXEC SP_VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS " ;
    /****************************************************/
    if( DPS_INTER_MOVCUE == eType || DPS_INTER_MOVDIF == eType )
          strQuery += " 1" ; // Antes " 1,7";
    else if( DPS_INTER_AHOMOV == eType || DPS_INTER_AHODIF == eType )
          strQuery += " 11"; // Antes " 11,15";
	else
          strQuery += " 15"; // Antes " 11,15";

	/****************************************************/
#endif
        


    /* EXECUTE QUERY */
    odbcPri.Query( (PSTR)strQuery.c_str(), &iQueryNum, TRUE, TRUE ) ;
    if(odbcPri.Error())
    {
	    m_editTexto = "NO SE PUEDE EJECUTAR LA CONSULTA SQL : "; 
	    m_editTexto += odbcPri.GetErrorMessage(NULL);
	    m_editTexto += odbcPri.GetLastSQLCommand(NULL);
        AfxMessageBox(m_editTexto);
        UpdateData( FALSE );
	    ProcessAppMsgQueue( );

	    fprintf(fLogg,"\r\n");
	    fprintf(fLogg, (LPCSTR)m_editTexto);
	    fprintf(fLogg,"\r\n");	  
        fflush( fLogg ); 
        	
	    odbcPri.Disconnect();
	    return;
    };


    /* Progress Bar */
    m_progressBar.SetRange32(0, 1 + 1 + 1 ); /* header + footer + 1 data record */
    m_progressBar.SetStep( 1 );	

    m_editTexto = "PROCESANDO..."; 
    UpdateData( FALSE );
    ProcessAppMsgQueue();

    /*****************************************************/
	fprintf(fFeriados,"-----------------------------\n");
	fprintf(fFeriados,"FETCH CYCLE\n");
    /* FETCH CYCLE */
    for( odbcPri.Fetch( &iQueryNum ) ;
        (odbcPri.GetReturnCode() == SQL_SUCCESS_WITH_INFO 
            ||  
            odbcPri.GetReturnCode() == SQL_SUCCESS) ;
            odbcPri.Fetch( &iQueryNum )          )
    {		
            /* Counter */
            lCounter++ ;
            m_progressBar.StepIt() ;

			/* FIX ODBC DRIVER , cambia tipo CHAR(19) x FLOAT */
			if( bUseRealAmmount )
				sprintf( szIMPORTE, "%15.2f", fIMPORTE) ;
			else
				fIMPORTE = atof( szIMPORTE ) ;
			/* FIX ODBC DRIVER , cambia tipo CHAR(19) x FLOAT */
			
		    /* Internal ODBC status */
		    lRetval = odbcPri.GetReturnCode();

            DateTimeToDateYYYYMMDD(szFECHAOPE,szYYYYMMDD); /* "YYYYMMDD" */
            tmWorkingDay.tm_year = antoi(szYYYYMMDD,4)   - 1900;
            tmWorkingDay.tm_mon  = antoi(szYYYYMMDD+4,2) - 1   ;
            tmWorkingDay.tm_mday = antoi(szYYYYMMDD+4+2,2)     ;        
            /* New DATE */
            tWorkingDay  = mktime( &tmWorkingDay );        
            /* SUNDAY=0 , SATURDAY=6 */
            if( 0 == tmWorkingDay.tm_wday  || 6 == tmWorkingDay.tm_wday )
                continue;

            /* HOLIDAY? - FORMAT DATE STRING IN ASCII */
            sprintf( szAux, "%04i%02i%02i",
                        tmWorkingDay.tm_year + 1900,
                        tmWorkingDay.tm_mon + 1    ,
                        tmWorkingDay.tm_mday       );
            /* Is Holiday ? YYYYMMDD */
			fprintf(fFeriados,"--> lCounter: %i, szAux: %i \n", lCounter, atoi(szAux));
            if( CheckHolidayDateInArray( szAux ) )
                continue;


            /**************************************************************/
            /* FORMAT RECORD - MOVCUE OR MOVDIF ?                         */
            /**************************************************************/
            if( ( RT_MOVCUE == recTYPE || RT_AHOMOV == recTYPE || RT_AHMODO == recTYPE) && 
                strlen(szFECHACON) > 0         && 
                strlen(szFECHAOPE) > 0         && 
                ( strncmp(szFECHACON,szFECHAOPE,10) == 0 /* POSTING-DATE = OPERATION-DATE */
                    ||                                     /* OR */
                    //strncmp(szFECHACON,szFECHAINI,10) == 0 /* POSTING-DATE = INI-PROCESSING-DATE */
                    //||                                     /* OR */
                    ( strncmp(szFECHACON,szFECHAINI,10) >= 0 /* POSTING-DATE >= INI-PROCESSING-DATE */
                    &&
                    strncmp(szFECHACON,szFECHAEND,10) <  0 /* POSTING-DATE <  END-PROCESSING-DATE */
                    )
                    ||                                     /* OR */
                    (
                    strncmp(szFECHAVAL,szFECHAINI,10) == 0 /* VALUED-DATE  = INI-PROCESSING-DATE */
                    &&
                    strncmp(szFECHACON,szFECHAOPE,10) < 0  /* POSTING-DATE > OPERATION-DATE */
                    &&                                     /* AND */
                    strncmp(szFECHACON,szFECHAINI,10) != 0 /* POSTING-DATE <> INI-PROCESSING-DATE */
                    )
					||
					(
					strncmp(szFECHAVAL, szFECHACON, 10) < 0
					)
                )
                )
            {
				fprintf(fFeriados,"ENTRO EN EL IF MOVCUE OR MOVDIF\n");

				if (strncmp(szFECHACON, szFECHAOPE, 10) < 0)
					continue;

                ///* JUST FOR REPROCESSING - VALUED DATE */
                //if( m_bReprocesamiento 
                //    &&
                //    !( strncmp(szFECHACON,szFECHAINI,10) >= 0 /* PROC-DATE >= INI-PROCESSING-DATE */
                //        &&
                //        strncmp(szFECHACON,szFECHAEND,10) <  0 /* PROC-DATE <  END-PROCESSING-DATE */
                //        )
                //    )
                //    continue;

                lSaved++;

                memset( &recMOVCUE  , ' ' , sizeof recMOVCUE);
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL);
                
		        if( RT_MOVCUE == recTYPE )
                    COPYSTRING( recMOVCUE.MOVCUE_TEXTO, "MOVCUE"  )
                else if(RT_AHOMOV == recTYPE)
                    COPYSTRING( recMOVCUE.MOVCUE_TEXTO, "AHOMOV"  )
				else
					COPYSTRING( recMOVCUE.MOVCUE_TEXTO, "AHMODO"  );

		        RIGHTCOPYFIELD(recMOVCUE.CODBAN,BCRA_ID_DEFAULT);
                /***********************************************/
                /* Modificacion al PICTURE de la CUENTA        */
                /***********************************************/                                     
                PADCOPYFIXEDINT_ASCII(stNUMCTA.chNUMCTA, szCUENTA );
	            RIGHTCOPYFIELD(recMOVCUE.NUMCTA,stNUMCTA.chNUMCTA  );
                /***********************************************/
                
                /**************************************************/
                /* IN CASE OF PREVIOUS DATE, MUST BE PROCESS DATE */
                if( strncmp(szFECHAOPE,szFECHACON,10) < 0 ) /* yyyy-mm-dd */
                {
                        DateTimeToDateYYYYMMDD(szFECHACON,szYYYYMMDD);		
                }
                /* IN CASE OF FUTURE-PAST DATES , IT DEPENS ON BALANCE RECALC. */
                else if( strncmp(szFECHAVAL,szFECHACON,10) < 0 )  /* yyyy-mm-dd */
                {
                    if( bUSE_BALANCE_RECALC )
                        DateTimeToDateYYYYMMDD(szFECHAVAL,szYYYYMMDD);
                    else
                        DateTimeToDateYYYYMMDD(szFECHACON,szYYYYMMDD);		
                }
                else if( strncmp(szFECHAVAL,szFECHACON,10) > 0 ) /* yyyy-mm-dd */
                {
                    if( bUSE_BALANCE_RECALC )
                        DateTimeToDateYYYYMMDD(szFECHACON,szYYYYMMDD);
                    else
                        DateTimeToDateYYYYMMDD(szFECHAVAL,szYYYYMMDD);		
                }
                else
                {
                    DateTimeToDateYYYYMMDD(szFECHAOPE,szYYYYMMDD);		
                };

				if( strncmp(szFECHACON, szFECHAOPE, 10) > 0 )
					DateTimeToDateYYYYMMDD(szFECHAOPE, szYYYYMMDD);

		        /**************************************************/
	            RIGHTCOPYFIELD(recMOVCUE.FECMOV,szYYYYMMDD + 2 );
		        DateTimeToDateYYYYMMDD(szFECHACON,szYYYYMMDD);
                /******************************************************
                    Para el caso en que se ajusta saldos, FECMOV tiene 
                    que ser igual que FECVAL, y FECPRO siempre es la 
                    fecha de proceso real.                                                 
                ******************************************************/

// EAE 151228 -> 
				//if( !bUSE_BALANCE_RECALC )
    //            {
		              //DateTimeToDateYYYYMMDD(szFECHAVAL,szYYYYMMDD);		
					  DateTimeToDateYYYYMMDD(szFECHAOPE,szYYYYMMDD);		
			          RIGHTCOPYFIELD(recMOVCUE.FECVAL,szYYYYMMDD + 2 );
    //            }
    //            else
    //                RIGHTCOPYFIELD(recMOVCUE.FECVAL,recMOVCUE.FECMOV);
    //            /*****************************************************/
// EAE <-
		        recMOVCUE.DEBCRE = ( atoi(szDEBCRE) <= 0 ) 
								        ? 'D'  
								        : 'C' ;
		        /* En caso de que el IMPORTE fuera packed-decimal (COMP-3) */
                NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
			        szIMPORTE, strlen(szIMPORTE), 0x00, is_true ) ;
                /* En caso de que el IMPORTE fuera ASCII ISO-8583 */
                if(pstConv && (strcmp(pstConv->decimal_point,".")!=0 || 
		                strcmp(pstConv->thousands_sep,",")!=0))
                {		
		            strrepchar(szIMPORTE, (short)strlen(szIMPORTE), '.', ',');
                };
				/* FIX */
				//JAF
				// 2016-09-21 - JAF - saco punto de decimal, no hago atof porque redondea a varios decimales y da mal el impore
				std::string sImporte(szIMPORTE);
				
				if(sImporte.find(".") != std::string::npos)
					sImporte.replace(sImporte.find(".") ,1, "");
				
				if(sImporte.find(",") != std::string::npos)
					sImporte.replace(sImporte.find(",") ,1, "");
				
				fIMPORTE = atof(sImporte.c_str());

				//fIMPORTE  = atof( szIMPORTE ) ;
				//fIMPORTE *= 100.0f ;				
                fixAmmnt = (FIXEDINTEGER) fIMPORTE;
				/* FIX */
            	
				std::stringstream ss;
				ss << std::setfill('0') << std::setw(17) << std::right << sImporte;
				sImporte = ss.str();

                /* Aqui, el IMPORTE ha quedado alineado y solo con 2 decimales virtuales */
		        //PADCOPYFIXEDINT(recMOVCUE.IMPORTE, fixAmmnt );
				if(sImporte.find("-") != std::string::npos)
					sImporte.replace(sImporte.find("-") ,1, "0");
				
				if(sImporte.find("+") != std::string::npos)
					sImporte.replace(sImporte.find("+") ,1, "0");

				sprintf(recMOVCUE.IMPORTE, "%s", sImporte.c_str());
		        /* SIN SIGNO porque para eso esta el campo DEBCRE */
                if( '+' == recMOVCUE.IMPORTE[0] || '-' == recMOVCUE.IMPORTE[0] )
		            recMOVCUE.IMPORTE[0] = '0';
            	
                /****************************************************************/
		        /* ENTERO BINARIO que no es packed-decimal */
	            ptrUSHORT = (USHORT *)&recMOVCUE.NUMCOR ;
                /* DEBERIA SER CORRELATIVO E INCREMENTAL + 1, */
                /* PERO USAMOS EL SECUENCIAL Y HASTA 65535    */
		        (*ptrUSHORT) = (USHORT) (((ULONG)atol(szSECUENCIAL)) + lCounter)  << sizeof(USHORT)  ;
                /* MASK FOR EBCDIC ? */
                if( bMaskForEBCDIC )
                {
	                recMOVCUE.NUMCOR[0] = GetEBCDIC_ASCII( recMOVCUE.NUMCOR[0] ); 
                    recMOVCUE.NUMCOR[1] = GetEBCDIC_ASCII( recMOVCUE.NUMCOR[1] ); 
                    /* ELIMINAR RANGO POSIBLE DE CR+LF (0D+0A) */
                    if( 0x0A == recMOVCUE.NUMCOR[0] ) 
                        recMOVCUE.NUMCOR[0] |= 0xFF;
                    if( 0x0A == recMOVCUE.NUMCOR[1] ) 
                        recMOVCUE.NUMCOR[1] |= 0xFF;
                };
                /****************************************************************/

				if( bUSE_UMOV_DESCRIPCION )
				{
					/* COMPROBANTE ES EL SECUENCIAL */
					PADCOPYFIXEDINT_ASCII(recMOVCUE.NROCOM ,szSECUENCIAL )
				}
				else
				{
					/* consulta a la nueva tabla por la descripcion traeme leyenda y codope */

					/* SAVE ORIGINAL COD.OPE. */
					COPYSTRING( szCODOPE, szDESCRIPCION );
					szCODOPE[_CMOVATM_CODCONCEP_LEN_] = 0x00;
					/* SAVE ORIGINAL COD.OPE. */

					char sTodosCeros[8+1] = {0x00};
					strcpy(sTodosCeros, "00000000");

					//FML 2016-08-26: Para movimientos de cheque se usa 
					if(strcmp(szDEPOSITO,"99999999") != 0)
					{
						PADCOPYFIXEDINT_ASCII(recMOVCUE.NROCOM , szDEPOSITO);
					}
					else
					{
						/* COMPROBANTE ES PARAMETRICO CONTENIDO EN LA DESCRIPCION, CON COMA */
						if( strchr(szDESCRIPCION,',') )
							PADCOPYFIXEDINT_ASCII(recMOVCUE.NROCOM , strchr(szDESCRIPCION,',')+1 )
						else if( strlen(szDESCRIPCION) > 3 )
							PADCOPYFIXEDINT_ASCII(recMOVCUE.NROCOM , szDESCRIPCION + 3 )
						else
							PADCOPYFIXEDINT_ASCII(recMOVCUE.NROCOM , szSECUENCIAL );

						// 2016-10-03 - JAF - C.C.C.C.C.
						PADCOPYFIXEDINT_ASCII(recMOVCUE.NROCOM , sTodosCeros);
					}
				}; // _UMOVPBF_USE_UMOV_DESCRIPCION_

                COPYSTRING( szOPERACION , " ");
                COPYSTRING( szDESCRIPCION_LARGA , " " );

				/* 2016-07-12 - JAF - MAPEO DESCRIPCION SEGUN CODIGO QUE VENGA DE PINES */
				std::string sLeyenda(szDESCRIPCION);
				std::size_t pos = sLeyenda.find(",");
				if(pos != std::string::npos)
					sLeyenda.replace(pos, 1, "");

				trxBatch.SearchInternalCodOpe( szOPERACION , szDESCRIPCION, 
                                            ( atoi(szDEBCRE) > 0 ) ? "C" : "D" , 
                                            szOPERACION , szDESCRIPCION_LARGA     );

				std::string sDescripcionPINES = TransformarLeyenda( sLeyenda , (atoi(szDEBCRE) > 0 ) ? "C" : "D");
				strcpy(szDESCRIPCION_LARGA, sDescripcionPINES.c_str());

				
                /** fix **************************************/
				if( FALSE == bUseInternalCodOpe )
				{
                    //COPYSTRING(recMOVCUE.CODOPE, szCODOPE  )
					//FML 2016-09-02
					COPYSTRING(recMOVCUE.CODOPE, szCODOPEIB);
				}
				else
					/** fix **************************************/
					if( atoi(szOPERACION) > 0 )
						PADCOPYINTEGER(recMOVCUE.CODOPE, szOPERACION  )
					else
						COPYSTRING(recMOVCUE.CODOPE, szOPERACION  );                
					/** fix **************************************/

				/** fix ****************/
				if( bUseInternalCodOpe )
				{
					/***********************************************************/
					/* FIX MAPEOS PROPIOS DEL BANCO PARA EL CAMPO COD.OPE.     */
					/***********************************************************/
					COPYSTRING( szOPERACION, szCODOPE );
					if( SearchLocalCodOpe( szOPERACION , szDESCRIPCION, recTYPE ) > 0 )
					{
						EXACTCOPYFIELD(recMOVCUE.CODOPE, szOPERACION );                
					};					
				}; // if-bUseInternalCodOpe
				/** fix ****************/

		        EXACTCOPYFIELD(recMOVCUE.NUMEXT, "0000"  );

                /* EXTRACT DATE */
                if( !m_bReprocesamiento )
		            DateTimeToDateYYYYMMDD(szFECHAINI,szYYYYMMDD);		
                else
                {
                    /* VALUE > POSTING */
                    if( strncmp(szFECHACON,szFECHAVAL,10) < 0)
                        DateTimeToDateYYYYMMDD(szFECHAVAL,szYYYYMMDD); 
                    else
                        DateTimeToDateYYYYMMDD(szFECHACON,szYYYYMMDD); 
                }
	            //RIGHTCOPYFIELD(recMOVCUE.FECEXT, szYYYYMMDD + 2 );
				//FML 2017-10-27: Oski pidio que la fecha de extracto sea la misma que la del movimiento
				RIGHTCOPYFIELD(recMOVCUE.FECEXT, recMOVCUE.FECMOV); 
                /* PROCESSING DATE */
                if( !m_bReprocesamiento )
		            DateTimeToDateYYYYMMDD(szFECHAINI,szYYYYMMDD);		
                else
                {
                    /* VALUE > POSTING */
                    if( strncmp(szFECHACON,szFECHAVAL,10) < 0 )
                        DateTimeToDateYYYYMMDD(szFECHAVAL,szYYYYMMDD); 
                    else
                        DateTimeToDateYYYYMMDD(szFECHACON,szYYYYMMDD); 
                }
	            RIGHTCOPYFIELD(recMOVCUE.FECPRO, szYYYYMMDD + 2 );
				/**begin-fix PROCESSING DATE **/
				char          szYYYYMMDD_FECPRO[16+1] = {0x00};
                
				//EAE 151228 ->
				//DateTimeToDateYYYYMMDD(szFECHAPRO  , szYYYYMMDD_FECPRO); /* "YYYYMMDD" */
				DateTimeToDateYYYYMMDD(szFECHACON  , szYYYYMMDD_FECPRO); /* "YYYYMMDD" */				
				// EAE <-
				
				RIGHTCOPYFIELD(recMOVCUE.FECPRO, szYYYYMMDD_FECPRO + 2 );
				/**end-fix PROCESSING DATE **/

                /* BRANCH */
	            RIGHTCOPYFIELD(recMOVCUE.SUCORI, szSUCURSAL );
                if ( atoi( szDEPOSITO ) == 0)
                {
                    RIGHTCOPYFIELD(recMOVCUE.DEPOSI, "        " ); /* SPACES */
                }
                else
                {
					
					//FML 2016-08-26: Para movimientos de cheque se usa 
					PADCOPYINTEGER(recMOVCUE.DEPOSI, "99999" );
					//PADCOPYINTEGER(recMOVCUE.DEPOSI, szDEPOSITO );
                };

				// Siempre va Deposi en 0
				RIGHTCOPYFIELD(recMOVCUE.DEPOSI, "        " );

		        recMOVCUE.MARCOM[0] = ' ';
		        //RIGHTCOPYFIELD(recMOVCUE.DESCRI, szDESCRIPCION_LARGA );
				//RIGHTCOPYFIELD(recMOVCUE.DESCRI, szDESCRLAR );
				for(int i=0;i<sizeof(szDESCRLAR);i++)
					if(szDESCRLAR[i]=='\0') 
						szDESCRLAR[i]=' ';
				//A6200
				/*for(int i=0;i<sizeof(szCUITORIGINANTE);i++)
					if(szCUITORIGINANTE[i]=='\0') 
						szCUITORIGINANTE[i]=' ';*/
				for(int i=0;i<sizeof(szDENOMINACIONOR);i++)
					if(szDENOMINACIONOR[i]=='\0') 
						szDENOMINACIONOR[i]=' ';
				for(int i=0;i<sizeof(szCBU);i++)
					//if(szCBU[i]=='\0') 
						szCBU[i]=' ';				
				/*for(int i=0;i<sizeof(szCODOPEORIGINAL);i++)
					if(szCODOPEORIGINAL[i]=='\0') 
						szCODOPEORIGINAL[i]=' ';*/

				
				COPYSTRING(recMOVCUE.DESCRI, szDESCRLAR);
				COPYSTRING(recMOVCUE.OPEBCO, szCODOPEORIGINAL);
				COPYSTRING(recMOVCUE.CUITOR, szCUITORIGINANTE);
				COPYSTRING(recMOVCUE.DENOMI, szDENOMINACIONOR);
				COPYSTRING(recMOVCUE.CBU,	szCBU);
				recMOVCUE.FILLER[0] = ' ';	

		        /* LOG */
                fprintf(fLogg,"\r\n--------------------------------------");
		        fprintf(fLogg,"\r\nDATANET - REGISTRO MOVIMIENTOS CUENTAS");
		        fprintf(fLogg,"\r\n--------------------------------------");
		        fprintf(fLogg,"\r\nHEADER [%*.*s]",6,6,recMOVCUE.MOVCUE_TEXTO	);        
		        fprintf(fLogg,"\r\nBANCO [%*.*s]",3,3,recMOVCUE.CODBAN	);        
		        fprintf(fLogg,"\r\nCUENTA [%*.*s]",17,17,recMOVCUE.NUMCTA);        
		        fprintf(fLogg,"\r\nIMPORTE [%*.*s]",17,17,recMOVCUE.IMPORTE);        
		        fprintf(fLogg,"\r\nFECHA VALOR [%*.*s]",6,6,recMOVCUE.FECVAL);        
                fprintf(fLogg,"\r\nFECHA MOVIMIENTO [%*.*s]",6,6,recMOVCUE.FECMOV);        
                fprintf(fLogg,"\r\nFECHA EXTRACTO [%*.*s]",6,6,recMOVCUE.FECEXT);                            
		        fprintf(fLogg,"\r\nFECHA PROCESO [%*.*s]",6,6,recMOVCUE.FECPRO);                            
		        fprintf(fLogg,"\r\nCORRELATIVO [%*.*i]",2,2, *((short *)&recMOVCUE.NUMCOR) );
		        fprintf(fLogg,"\r\nCOMPROBANTE [%*.*s]",12,12,recMOVCUE.NROCOM);
				fprintf(fLogg,"\r\nCOD.OPERACION [%3.3s] vs. [%3.3s]",recMOVCUE.CODOPE, szCODOPE );
		        fprintf(fLogg,"\r\nDESCRIPCION [%*.*s]",25,25,recMOVCUE.DESCRI);        
				fprintf(fLogg,"\r\nOPERACION BANCO[%*.*s]",5,5,recMOVCUE.OPEBCO);        
				fprintf(fLogg,"\r\nCUIT ORIGINANTE [%*.*s]",11,11,recMOVCUE.CUITOR);        
				fprintf(fLogg,"\r\nDENOMINACION ORIGINANTE[%*.*s]",40,40,recMOVCUE.DENOMI);        

                fflush( fLogg );        

				/*******************************************************/
				for(int i=0; i<sizeof(szDESCRLAR); i++)
					szDESCRLAR[i]=' ';

            
            }
            /**************************************************************/
            else if( (RT_MOVDIF == recTYPE || RT_AHODIF == recTYPE || RT_AHDIDO == recTYPE)  &&  
                        strlen(szFECHACON) > 0         && 
                        strlen(szFECHAOPE) > 0         && 
                    ( strncmp(szFECHACON,szFECHAOPE,10) < 0  /* POSTING-DATE < OPERATION-DATE */
                        //&&                                     /* AND */
                        //strncmp(szFECHACON,szFECHAINI,10) != 0 /* POSTING-DATE <> INI-PROCESSING-DATE */
                    )
					//&& (strncmp(szMuestroCORTOS, "S ", 2) == 0)
                    )
            /**************************************************************/
            {
                /* JUST FOR REPROCESSING - VALUED DATE */
                //if( m_bReprocesamiento 
                //    &&
                //    strncmp(szFECHACON,szFECHAINI_DIF,10) <= 0 /* PROC-DATE < INI-PROCESSING-DATE */
                //    )
                //    continue;

				char szMuestroTemp[1+1] = {0x00};
				strncpy(szMuestroTemp, szMuestroCORTOS, 1);
				std::string sMuestroTemp(szMuestroTemp);
				
				if(sMuestroTemp != "S")
					continue;


				std::string sFechaOperacion(szFECHAOPE);
				if( sFechaHastaPosta != sFechaOperacion.substr(0,10)) 
					continue;

                lSaved++; 

                memset( &recMOVDIF  , ' ' , sizeof recMOVDIF);
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL);
                
                if( RT_MOVDIF == recTYPE )
		            COPYSTRING( recMOVDIF.MOVDIF_TEXTO, "MOVDIF"  )
                else if( RT_AHODIF == recTYPE )
                    COPYSTRING( recMOVDIF.MOVDIF_TEXTO, "AHODIF"  )
				else 
					COPYSTRING( recMOVDIF.MOVDIF_TEXTO, "AHDIDO"  );  

		        RIGHTCOPYFIELD(recMOVDIF.CODBAN,BCRA_ID_DEFAULT);
                /***********************************************/
                /* Modificacion al PICTURE de la CUENTA        */
                /***********************************************/                                     
                PADCOPYFIXEDINT_ASCII(stNUMCTA.chNUMCTA, szCUENTA );
	            RIGHTCOPYFIELD(recMOVDIF.NUMCTA,stNUMCTA.chNUMCTA  );
                /***********************************************/

                DateTimeToDateYYYYMMDD(szFECHAOPE,szYYYYMMDD);		
	            RIGHTCOPYFIELD(recMOVDIF.FECIMP,szYYYYMMDD + 2 );
		        recMOVDIF.DEBCRE = ( atoi(szDEBCRE) <= 0 ) 
								        ? 'D'  
								        : 'C' ;
                
                if( strncmp(szFECHAOPE,szFECHACON,10) < 0 ) /* yyyy-mm-dd */
                {
                    /* Fecha-Operacion vs. Fecha de Envio de Movimientos */
                    if( strncmp(szFECHAOPE,szFECHAINI,10) < 0 ) /* yyyy-mm-dd */
                        DateTimeToDateYYYYMMDD(szFECHAINI,szYYYYMMDD);		
                    else
                        DateTimeToDateYYYYMMDD(szFECHAOPE,szYYYYMMDD);		
                    /* Al contrario que en MOVCUE, en MOVDIF debe detallarse la        */
                    /* fecha original de imputacion, y no la del dia siendo procesado: */
                    DateTimeToDateYYYYMMDD(szFECHAOPE,szYYYYMMDD);		
                }
                else
                    DateTimeToDateYYYYMMDD(szFECHACON,szYYYYMMDD);			            
                /*********************************************************************/
                /* FIX S/EDS : VOLVER A TOMAR 'FECHAINI' PORQUE SE PASO A 'FECHAOPE' */                
                if( !m_bReprocesamiento )
                {
                    DateTimeToDateYYYYMMDD(szFECHAINI      ,szYYYYMMDD);
                    DateTimeToDateYYYYMMDD(szFECHAINI_SAV,szYYYYMMDD);
                }
                /* FIX S/EDS : VOLVER A TOMAR 'FECHAINI' PORQUE SE PASO A 'FECHAOPE' */
                else
                {                    
                    DateTimeToDateYYYYMMDD(szFECHAINI_SAV,szYYYYMMDD);
                }
                RIGHTCOPYFIELD(recMOVDIF.FECPRO , szYYYYMMDD + 2 );
                /*********************************************************************/
                
                if( RT_AHODIF == recTYPE )
                {   
                    /* NO ES ASCII 2 bytes SINO BINARIO 2 bytes */
                    PADCOPYINTEGER(recMOVDIF.NUMCOR , itoa( lCounter, szAux, 10) );
		            /* ENTERO BINARIO que no es packed-decimal */
	                ptrUSHORT = (USHORT *)&recAHODIF.NUMCOR ;
					/* DEBERIA SER CORRELATIVO E INCREMENTAL + 1, */
					/* PERO USAMOS EL SECUENCIAL Y HASTA 65535    */
					(*ptrUSHORT) = (USHORT) (((ULONG)atol(szSECUENCIAL)) + lCounter)  << sizeof(USHORT) ;
                    /* MASK FOR EBCDIC ? */
                    if( bMaskForEBCDIC )
                    {
	                    recAHODIF.NUMCOR[0] = GetEBCDIC_ASCII( recAHODIF.NUMCOR[0] ); 
                        recAHODIF.NUMCOR[1] = GetEBCDIC_ASCII( recAHODIF.NUMCOR[1] ); 
						/* ELIMINAR RANGO POSIBLE DE CR+LF (0D+0A) */
						if( 0x0A == recAHODIF.NUMCOR[0] ) 
							recMOVCUE.NUMCOR[0] |= 0xFF;
						if( 0x0A == recAHODIF.NUMCOR[1] ) 
							recMOVCUE.NUMCOR[1] |= 0xFF;
                    };                    
                }
				else if( RT_AHDIDO == recTYPE )
                {   
                    /* NO ES ASCII 2 bytes SINO BINARIO 2 bytes */
                    PADCOPYINTEGER(recMOVDIF.NUMCOR , itoa( lCounter, szAux, 10) );
		            /* ENTERO BINARIO que no es packed-decimal */
	                ptrUSHORT = (USHORT *)&recAHODIF.NUMCOR ;
					/* DEBERIA SER CORRELATIVO E INCREMENTAL + 1, */
					/* PERO USAMOS EL SECUENCIAL Y HASTA 65535    */
					(*ptrUSHORT) = (USHORT) (((ULONG)atol(szSECUENCIAL)) + lCounter)  << sizeof(USHORT) ;
                    /* MASK FOR EBCDIC ? */
                    if( bMaskForEBCDIC )
                    {
	                    recAHDIDO.NUMCOR[0] = GetEBCDIC_ASCII( recAHDIDO.NUMCOR[0] ); 
                        recAHDIDO.NUMCOR[1] = GetEBCDIC_ASCII( recAHDIDO.NUMCOR[1] ); 
						/* ELIMINAR RANGO POSIBLE DE CR+LF (0D+0A) */
						if( 0x0A == recAHDIDO.NUMCOR[0] ) 
							recMOVCUE.NUMCOR[0] |= 0xFF;
						if( 0x0A == recAHDIDO.NUMCOR[1] ) 
							recMOVCUE.NUMCOR[1] |= 0xFF;
                    };                    
                }							
				else
                {
                    PADCOPYINTEGER(recMOVDIF.NUMCOR , itoa( lCounter, szAux, 10) );
                };


                /**************************************************/
		        /* SIN SIGNO porque para eso esta el campo DEBCRE */
                if( '+' == szIMPORTE[0] || '-' == szIMPORTE[0] )
		            szIMPORTE[0] = '0';
                /**************************************************/
		        /* En caso de que el IMPORTE fuera packed-decimal (COMP-3) */
                NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
			        szIMPORTE, strlen(szIMPORTE), 0x00, is_true ) ;
                EXACTCOPYFIELD(recMOVDIF.IMPORTE,stPKDECIMAL.chPKDECIMAL);    // S9(15)V99 COMP-3	            					
		        /* En caso de que el IMPORTE fuera ASCII ISO-8583 */
                if(pstConv && (strcmp(pstConv->decimal_point,".")!=0 || 
		                strcmp(pstConv->thousands_sep,",")!=0))
                {		
		            strrepchar(szIMPORTE, (short)strlen(szIMPORTE), '.', ',');
                };
                fixAmmnt = (FIXEDINTEGER)( atof( szIMPORTE ) * 100.0f );
            		            	
		        
				if( bUSE_UMOV_DESCRIPCION )
				{
					/* COMPROBANTE ES EL SECUENCIAL */
					PADCOPYFIXEDINT_ASCII(recMOVDIF.NROCOM ,szSECUENCIAL );
				}
				else
				{
					/* COMPROBANTE ES PARAMETRICO CONTENIDO EN LA DESCRIPCION, CON COMA */
					if( strchr(szDESCRIPCION,',') )
						PADCOPYFIXEDINT_ASCII(recMOVDIF.NROCOM , strchr(szDESCRIPCION,',')+1 )
					else if( strlen(szDESCRIPCION) > 3 )
						PADCOPYFIXEDINT_ASCII(recMOVDIF.NROCOM , szDESCRIPCION + 3 )
					else
						PADCOPYFIXEDINT_ASCII(recMOVDIF.NROCOM , szSECUENCIAL );
				}; // _UMOVPBF_USE_UMOV_DESCRIPCION_

				// 2016-10-03 - JAF - C.C.C.C.C.
				char szTodosCeros2[8+1] = {0x00};
				strcpy(szTodosCeros2, "00000000");


				if (strcmp(szDEPOSITO,"99999999") != 0)
				{
					PADCOPYFIXEDINT_ASCII(recMOVDIF.NROCOM ,szDEPOSITO );
				}
	            else
				{
					PADCOPYFIXEDINT_ASCII(recMOVDIF.NROCOM ,szTodosCeros2);
				}

				/* 2016-07-12 - JAF - MAPEO DESCRIPCION SEGUN CODIGO QUE VENGA DE PINES */
				std::string sLeyenda(szDESCRIPCION);
				std::size_t pos = sLeyenda.find(",");
				if(pos != std::string::npos)
					sLeyenda.replace(pos, 1, "");

				trxBatch.SearchInternalCodOpe( szOPERACION , szDESCRIPCION, 
                                            ( atoi(szDEBCRE) > 0 ) ? "C" : "D" , 
                                            szOPERACION , szDESCRIPCION_LARGA     );

				std::string sDescripcionPINES = TransformarLeyenda( sLeyenda , (atoi(szDEBCRE) > 0 ) ? "C" : "D");
				strcpy(szDESCRIPCION_LARGA, sDescripcionPINES.c_str());

                //COPYSTRING( szOPERACION , " ");
                //COPYSTRING( szDESCRIPCION_LARGA , " " );
                //trxBatch.SearchInternalCodOpe( szOPERACION , szDESCRIPCION, 
                //                            ( atoi(szDEBCRE) > 0 ) ? "C" : "D" , 
                //                            szOPERACION , szDESCRIPCION_LARGA     );	


                /** fix **************************************/
				if( FALSE == bUseInternalCodOpe )
				{
					memcpy(recMOVDIF.CODOPE, szCODOPEIB, 3);
                    //strncpy(recMOVDIF.CODOPE, szCODOPEIB, 3 );
				}
				else
					/** fix **************************************/
					if( atoi(szOPERACION) > 0 )
						PADCOPYINTEGER(recMOVDIF.CODOPE, szOPERACION  )
					else
						COPYSTRING(recMOVDIF.CODOPE, szOPERACION  );
					/** fix **************************************/


		        EXACTCOPYFIELD(recMOVDIF.CANMOV  , "01"  );
	            PADCOPYINTEGER(recMOVDIF.SUCORI, szSUCURSAL );
                if ( atoi( szDEPOSITO ) == 0)
                {

                    RIGHTCOPYFIELD(recMOVDIF.DEPOSI, "        " ); /* SPACES */
                }
                else
                {
		            PADCOPYINTEGER(recMOVDIF.DEPOSI, szDEPOSITO );
                };

				RIGHTCOPYFIELD(recMOVDIF.DEPOSI, "        " ); 

				
		        recMOVDIF.CONTRA[0] = ' '; /* los valores posibles son " " o "1" si es contraasiento */
		        //RIGHTCOPYFIELD(recMOVDIF.DESCRI, szDESCRIPCION_LARGA );
				RIGHTCOPYFIELD(recMOVDIF.DESCRI, szDESCRLAR);
				COPYSTRING(recMOVDIF.OPEBCO, szCODOPEORIGINAL);
				recMOVDIF.FILLER[0] = ' ';	


		        /* LOG */
                fprintf(fLogg,"\r\n-----------------------------------------");
		        fprintf(fLogg,"\r\nDATANET - REGISTRO MOVIMIENTOS DIFERIDOS");
		        fprintf(fLogg,"\r\n-----------------------------------------");
		        fprintf(fLogg,"\r\nHEADER [%*.*s]",6,6,recMOVDIF.MOVDIF_TEXTO	);        
		        fprintf(fLogg,"\r\nBANCO [%*.*s]",3,3,recMOVDIF.CODBAN	);        
		        fprintf(fLogg,"\r\nCUENTA [%*.*s]",17,17,recMOVDIF.NUMCTA);        

                PackedDecimalToString( (PBYTE)recMOVDIF.IMPORTE,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00 , is_true  );
		        fprintf(fLogg,"\r\nIMPORTE [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );

		        fprintf(fLogg,"\r\nFECHA IMPUTACION          [%*.*s]",6,6,recMOVDIF.FECIMP);        
		        fprintf(fLogg,"\r\nFECHA OPERACION-PROCESADO [%*.*s]",6,6,recMOVDIF.FECPRO);        
                if( RT_AHODIF == recTYPE )
                    fprintf(fLogg,"\r\nCORRELATIVO AHO.DIF.[%*.*i]",2,2, *((short *)&recAHODIF.NUMCOR) ); 
				else if( RT_AHDIDO == recTYPE )
					fprintf(fLogg,"\r\nCORRELATIVO AHO.DIF.[%*.*i]",2,2, *((short *)&recAHODIF.NUMCOR) ); 
				else
		            fprintf(fLogg,"\r\nCORRELATIVO [%*.*s]",4,4, recMOVDIF.NUMCOR );

		        fprintf(fLogg,"\r\nCOMPROBANTE [%*.*s]",12,12,recMOVDIF.NROCOM);
		        fprintf(fLogg,"\r\nDESCRIPCION [%*.*s]",25,25,recMOVDIF.DESCRI);        
                fflush( fLogg );        

            } /* REC-TYPE-MOVCUE */
            else
            {
		        /* MSG QUEUE */
		        ProcessAppMsgQueue();
                continue;
            };
            /**************************************************************/
        	    
            /* FILE HEADER */
            if( !bHeader )
            {
                bHeader = TRUE;
                BLANKSTRUCT( hdrMOVCUE );
                COPYSTRING( hdrMOVCUE.ASTERISCO1   , "*I*"           );
                COPYSTRING( hdrMOVCUE.BANCO        , BCRA_ID_DEFAULT );
                /* FORMAT RECORD - MOVCUE OR MOVDIF ? */
                if( RT_MOVCUE == recTYPE )
                    COPYSTRING( hdrMOVCUE.TEXTO        , "MOVCUE"        )
                else if( RT_MOVDIF == recTYPE )
                    COPYSTRING( hdrMOVCUE.TEXTO        , "MOVDIF"        )
                else if( RT_AHOMOV == recTYPE )
                    COPYSTRING( hdrMOVCUE.TEXTO        , "AHOMOV"        )
				else if( RT_AHODIF == recTYPE )
                    COPYSTRING( hdrMOVCUE.TEXTO        , "AHODIF"        )
                else if( RT_AHMODO == recTYPE )
                    COPYSTRING( hdrMOVCUE.TEXTO        , "AHMODO"        ) 
				else if( RT_AHDIDO == recTYPE )
                    COPYSTRING( hdrMOVCUE.TEXTO        , "AHDIDO"        )
				else 
                    COPYSTRING( hdrMOVCUE.TEXTO        , "******"        );
                /*
                Cuando se envian las interfases se supone que se estan enviando los registros del dia anterior, 
                por lo tanto si la fecha de proceso de los registros es yymmdd, en el registro inicial de MOVCUE 
                tendra que ir con fecha yymmdd+1, y la segunda fecha tiene que venir con yymmdd. 
                La segunda fecha es muy importe para el caso que se tenga que hacer un reproceso, ya que el sistema
                toma esa fecha como "fecha desde" para eliminar todo los registros a partir de esa fecha y volver 
                a reprocesar.
                */
                DateTimeToDateYYYYMMDD(szFECHAPRO  , szYYYYMMDD); /* "YYYYMMDD" */
                COPYSTRING( hdrMOVCUE.FECHA_PROCESO, szYYYYMMDD + 2);            
                
                if( !m_bReprocesamiento )
                {
                    /***********************************************************/
                    if( strncmp(szFECHAEND,szFECHAPRO,10) < 0 ) /* yyyy-mm-dd */
                        DateTimeToDateYYYYMMDD(szFECHAEND, szYYYYMMDD); /* "YYYYMMDD" */
                    /***********************************************************/
                    else if( strncmp(szFECHAPRO,szFECHAOPE,10) > 0 ) /* yyyy-mm-dd */
                        DateTimeToDateYYYYMMDD(szFECHAPRO, szYYYYMMDD); /* "YYYYMMDD" */
                    else
                        DateTimeToDateYYYYMMDD(szFECHAOPE, szYYYYMMDD); /* "YYYYMMDD" */
                }
                else /* REPROCESSING */
                {
                    DateTimeToDateYYYYMMDD(szFECHAINI, szYYYYMMDD); /* "YYYYMMDD" */
                };

                COPYSTRING( hdrMOVCUE.FECHA_DESDE  , szYYYYMMDD + 2 ); 
                hdrMOVCUE.FILLER[0] = ' ';

	            fwrite( &hdrMOVCUE, sizeof(hdrMOVCUE), 1, fMOVCUE );
                fflush( fMOVCUE );        
            }

            /* FILE BODY - REGISTER */
            if( RT_MOVCUE == recTYPE )      fwrite( &recMOVCUE, sizeof( recMOVCUE), 1, fMOVCUE );
            else if( RT_MOVDIF == recTYPE ) fwrite( &recMOVDIF, sizeof( recMOVDIF), 1, fMOVCUE );
            else if( RT_AHOMOV == recTYPE ) fwrite( &recMOVCUE, sizeof( recMOVCUE), 1, fMOVCUE );
			else if (RT_AHMODO == recTYPE ) fwrite( &recMOVCUE, sizeof( recMOVCUE), 1, fMOVCUE );
            else if( RT_AHODIF == recTYPE ) 
                {                                
	            EXACTCOPYFIELD(recAHODIF.AHODIF_TEXTO, recMOVDIF.MOVDIF_TEXTO ) ;
	            EXACTCOPYFIELD(recAHODIF.CODBAN, recMOVDIF.CODBAN	);
	            EXACTCOPYFIELD(recAHODIF.NUMCTA, recMOVDIF.NUMCTA	);	
	            EXACTCOPYFIELD(recAHODIF.FECIMP, recMOVDIF.FECIMP	);
	            recAHODIF.DEBCRE = recMOVDIF.DEBCRE ;
                EXACTCOPYFIELD(recAHODIF.FECPRO, recMOVDIF.FECPRO	);		            
                EXACTCOPYFIELD(recAHODIF.IMPORTE, recMOVDIF.IMPORTE	);
	            EXACTCOPYFIELD(recAHODIF.NROCOM, recMOVDIF.NROCOM	);
	            EXACTCOPYFIELD(recAHODIF.CODOPE, recMOVDIF.CODOPE	);
	            EXACTCOPYFIELD(recAHODIF.CANMOV, recMOVDIF.CANMOV	);
	            EXACTCOPYFIELD(recAHODIF.SUCORI, recMOVDIF.SUCORI   );
	            EXACTCOPYFIELD(recAHODIF.DEPOSI, recMOVDIF.DEPOSI	);
	            EXACTCOPYFIELD(recAHODIF.CONTRA, recMOVDIF.CONTRA   );
	            EXACTCOPYFIELD(recAHODIF.DESCRI, recMOVDIF.DESCRI	);	            
                BLANKSTRUCT( recAHODIF.FILLER );
                fwrite( &recAHODIF, sizeof( recAHODIF), 1, fMOVCUE );
                } 
            else if( RT_AHDIDO == recTYPE ) 
                {                                
	            EXACTCOPYFIELD(recAHDIDO.AHDIDO_TEXTO, recMOVDIF.MOVDIF_TEXTO ) ;
	            EXACTCOPYFIELD(recAHDIDO.CODBAN, recMOVDIF.CODBAN	);
	            EXACTCOPYFIELD(recAHDIDO.NUMCTA, recMOVDIF.NUMCTA	);	
	            EXACTCOPYFIELD(recAHDIDO.FECIMP, recMOVDIF.FECIMP	);
	            recAHDIDO.DEBCRE = recMOVDIF.DEBCRE ;
                EXACTCOPYFIELD(recAHDIDO.FECPRO, recMOVDIF.FECPRO	);		            
                EXACTCOPYFIELD(recAHDIDO.IMPORTE, recMOVDIF.IMPORTE	);
	            EXACTCOPYFIELD(recAHDIDO.NROCOM, recMOVDIF.NROCOM	);
	            EXACTCOPYFIELD(recAHDIDO.CODOPE, recMOVDIF.CODOPE	);
	            EXACTCOPYFIELD(recAHDIDO.CANMOV, recMOVDIF.CANMOV	);
	            EXACTCOPYFIELD(recAHDIDO.SUCORI, recMOVDIF.SUCORI   );
	            EXACTCOPYFIELD(recAHDIDO.DEPOSI, recMOVDIF.DEPOSI	);
	            EXACTCOPYFIELD(recAHDIDO.CONTRA, recMOVDIF.CONTRA   );
	            EXACTCOPYFIELD(recAHDIDO.DESCRI, recMOVDIF.DESCRI	);	            
                BLANKSTRUCT( recAHDIDO.FILLER );
                fwrite( &recAHDIDO, sizeof( recAHDIDO), 1, fMOVCUE );
                } 
            else ;
            fflush( fMOVCUE );    
                            
		    /* MSG QUEUE */
		    ProcessAppMsgQueue();

            /* Nulify */
            szCUENTA[0]      = szIMPORTE[0]     = szSALDO[0]    = szSECUENCIAL[0]  = szOPERACION[0] = 
            szDEBCRE[0]      = szFECHACON[0]    = szFECHAOPE[0] = szSUCURSAL[0]    = szDEPOSITO[0]  = 
            szDESCRIPCION[0] = szDESCRIPCION_LARGA[0] = 0x00;

    }; /* END FETCH CYCLE */
    /*****************************************************/

    /* FILE FOOTER? CHECK HEADER 1ST TIME */        
    if( !bHeader ) //&& RT_MOVDIF != recTYPE )
    {
        bHeader = TRUE;
        BLANKSTRUCT( hdrMOVCUE );
        COPYSTRING( hdrMOVCUE.ASTERISCO1   , "*I*"           );
        COPYSTRING( hdrMOVCUE.BANCO        , BCRA_ID_DEFAULT );
        /* FORMAT RECORD - MOVCUE OR MOVDIF ? */
        if( RT_MOVCUE == recTYPE )
            COPYSTRING( hdrMOVCUE.TEXTO        , "MOVCUE"        )
        else if( RT_MOVDIF == recTYPE )
            COPYSTRING( hdrMOVCUE.TEXTO        , "MOVDIF"        )
        else if( RT_AHOMOV == recTYPE )
            COPYSTRING( hdrMOVCUE.TEXTO        , "AHOMOV"        )
        else if( RT_AHODIF == recTYPE )
            COPYSTRING( hdrMOVCUE.TEXTO        , "AHODIF"        )
        else if( RT_AHMODO == recTYPE )
            COPYSTRING( hdrMOVCUE.TEXTO        , "AHMODO"        )        
        else if( RT_AHDIDO == recTYPE )
            COPYSTRING( hdrMOVCUE.TEXTO        , "AHDIDO"        )
		else 
            COPYSTRING( hdrMOVCUE.TEXTO        , "******"        );
        DateTimeToDateYYYYMMDD(szFECHAPRO  , szYYYYMMDD); /* "YYYYMMDD" */
        COPYSTRING( hdrMOVCUE.FECHA_PROCESO, szYYYYMMDD + 2);            
        /* FIX SIN MOVIMIENTOS */        
        if( !m_bReprocesamiento )
        {
            /***********************************************************/
            if( strncmp(szFECHAEND,szFECHAPRO,10) < 0 ) /* yyyy-mm-dd */
                DateTimeToDateYYYYMMDD(szFECHAEND, szYYYYMMDD); /* "YYYYMMDD" */
            /***********************************************************/
            else if( strncmp(szFECHAPRO,szFECHAOPE,10) > 0 ) /* yyyy-mm-dd */
                DateTimeToDateYYYYMMDD(szFECHAPRO, szYYYYMMDD); /* "YYYYMMDD" */
            else
                DateTimeToDateYYYYMMDD(szFECHAOPE, szYYYYMMDD); /* "YYYYMMDD" */
        }
        else /* REPROCESSING */
        {
            DateTimeToDateYYYYMMDD(szFECHAINI, szYYYYMMDD); /* "YYYYMMDD" */
        };
        /* FIX SIN MOVIMIENTOS */
        COPYSTRING( hdrMOVCUE.FECHA_DESDE  , szYYYYMMDD + 2 ); 
        hdrMOVCUE.FILLER[0] = ' ';

	    fwrite( &hdrMOVCUE, sizeof(hdrMOVCUE), 1, fMOVCUE );
        fflush( fMOVCUE );        
    }
    if( bHeader )
    {        
        BLANKSTRUCT( ftrMOVCUE );
        COPYSTRING( ftrMOVCUE.ASTERISCO1, "*F*"           );
        COPYSTRING( ftrMOVCUE.BANCO     , BCRA_ID_DEFAULT );
        /* FORMAT RECORD - MOVCUE OR MOVDIF ? */
        if( RT_MOVCUE == recTYPE )
            COPYSTRING( ftrMOVCUE.TEXTO        , "MOVCUE"        )      
        else if( RT_MOVDIF == recTYPE )
            COPYSTRING( ftrMOVCUE.TEXTO        , "MOVDIF"        )
        else if( RT_AHOMOV == recTYPE )
            COPYSTRING( ftrMOVCUE.TEXTO        , "AHOMOV"        )
        else if( RT_AHODIF == recTYPE )
            COPYSTRING( ftrMOVCUE.TEXTO        , "AHODIF"        )
        else if( RT_AHMODO == recTYPE )
            COPYSTRING( ftrMOVCUE.TEXTO        , "AHMODO"        )    
        else if( RT_AHDIDO == recTYPE )
            COPYSTRING( ftrMOVCUE.TEXTO        , "AHDIDO"        )
        else 
            COPYSTRING( ftrMOVCUE.TEXTO        , "******"        );
	    ftrMOVCUE.FILLER[0] = ' ';

        fwrite( &ftrMOVCUE, sizeof( ftrMOVCUE), 1, fMOVCUE );
        fflush( fMOVCUE );        
    };

    /* FREE CURRENT SENTENCE */
    odbcPri.FreeSentence( iQueryNum );
             
    /* POSTCONDITION */
    fclose( fLogg) ;
    fLogg   = NULL;
    fclose( fMOVCUE) ;
    fMOVCUE = NULL;

	fclose(fFeriados);
	fFeriados = NULL;

    /* ASCII-2-EBCDIC */
    if( m_checkConvertASCII )
    {
        /******************************************/
        strcpy( szBcraFilename, "CORTOS." );
        strcat( szBcraFilename, BCRA_ID_INTERF );
        /******************************************/
        lRetval = CONVERT_ASCII_EBCDIC_FILE( szDatFilename, ASCII_2_EBCDIC_NO_CRLF, sizeof(ftrMOVCUE), 
                    &lCounter , szBcraFilename ) ;
    };


    /* MESSAGE */
    m_editTexto = "FIN DE LA EJECUCION, REGISTROS LEIDOS: " ;
    m_editTexto += itoa( lCounter, szAux, 10) ;			
    m_editTexto += ", GRABADOS : " ;
    m_editTexto += itoa( lSaved, szAux, 10) ;			
    UpdateData( FALSE );
    ProcessAppMsgQueue();

    AfxMessageBox( m_editTexto );

	/********************************************************************************************/
	/* JAF - AGREGO MESSAGEBOX SI LA CANTIDAD DE REGISTROS ES 0	- GCP ID: 14136					*/
	/********************************************************************************************/
	if( lSaved == 0 )
	{
		AfxMessageBox("ADVERTENCIA. NO SE HA GRABADO NINGUN MOVIMIENTO");
	};


    m_progressBar.SetRange32(0, 0); /* NO COUNTERS IN PROGRESS BAR */
    // Disconnect DB
    odbcPri.Disconnect();

};

void CFuegoDPSDlg::DoSaldos( e_dps_interfases eType , BOOL bTruncate ) 
{
  struct HEADER_SALDOS_tag hdrSALDOS;
  struct ENREGI_SALDOS_tag recSALDOS;
  struct ENREGI_AHOSAL_tag recAHOSAL;
  struct ENREGI_AHSADO_tag recAHSADO;

  struct FOOTER_SALDOS_tag ftrSALDOS;  
  FILE	      *fLogg      ,
              *fBALANCES  ;
  int           iQueryNum = 0;
  long          lRetval   = 0;
  char          szCUENTA[20+1]    = {0x00};  
  char          szSALACT[19+1]    = {0x00};    
  char          szACUERDO[19+1]   = {0x00};
  char          szNOMBRE[30+1]    = {0x00};
  char          szFECHAOPE[30+1]  = {0x00};
  struct stPKDECIMAL_tag
  {
      char          chPKDECIMAL[9]; /* PIC S9(15)V99 = 17 bytes ( 16 + signo ) = empaquetados 9 bytes */
      char          chOVERFLOW[11]; /* OVERFLOW AREA */
  } stPKDECIMAL ; /* 20 BYTES = 160 bits */
  char          szYYYYMMDD[8+1] = {0x00};
  long          lCounter = 0;
  bool          bHeader  = FALSE;
  size_t        cnSize   = sizeof(recSALDOS);
  char		    szAux[32] = {0x00};
  char          szLogFilename[]    = "SALDOS.DATANET.log";
  char          szHisLogFilename[] = "SALHIS.DATANET.log";
  char          szDatFilename[]    = "LARGOS.dat";
  char          szBcraFilename[256]= { "LARGOS.999" };
  /****************************************************/
  struct tm    tmWorkingDay = {0,0,0,0,0,0,0,0,0};
  time_t       tWorkingDay  = 0;        
  /****************************************************/  
  /* TIPO DE REGISTRO A EMITIR */
  enum         enumRECTYPE { RT_SALDOS, RT_AHOSAL, RT_NONE, RT_AHSADO } ;
  enumRECTYPE  recTYPE     = RT_SALDOS;
  /****************************************************/
  stNUMCTA_t   stNUMCTA = { 0x00, 0x00 };
  /****************************************************/
  char          szFECHA_APER[30+1]     = {0x00};
  char          szRAZON_SOCIAL[256+1]  = {0x00};
  /****************************************************/


  /* PRECONDITION */
  if( bTruncate )
  {
    remove( szHisLogFilename );
    remove( szLogFilename );
    fLogg = fopen(szLogFilename, "wt");
  }
  else
    fLogg = fopen(szLogFilename, "at");
  if(!fLogg)
  {
	  m_editTexto = "NO SE PUEDE CREAR EL ARCHIVO DE LOGG";                
      AfxMessageBox(m_editTexto);
      UpdateData( FALSE );
      ProcessAppMsgQueue();
	  return;
  }
  else
	  fprintf(fLogg,"\r\nBANCO INTERFINANZAS - SALDOS PARA DATANET - VIA ODBC\r\n");

  /* PRECONDITION : TRUNCATE FILE 'LARGOS' (SALDOS & SALHIS) */
  if( bTruncate )
    fBALANCES = fopen(szDatFilename, "wt");
  else
    fBALANCES = fopen(szDatFilename, "at");
  if(!fBALANCES)
  {
	  m_editTexto = "NO SE PUEDE CREAR EL ARCHIVO DE DATOS DE SALIDA DE SALDOS";                
      AfxMessageBox(m_editTexto);
      UpdateData( FALSE );
	  ProcessAppMsgQueue();

	  fprintf(fLogg,"\r\n");
	  fprintf(fLogg, (LPCSTR)m_editTexto);
	  fprintf(fLogg,"\r\n");
      fflush( fLogg );        
	  return;
  };

  /* CONNECT DATABASE */
  odbcPri.SetUserName( (PSTR)strUSER.c_str()     );
  odbcPri.SetPassword( (PSTR)strPASSWORD.c_str() );
  odbcPri.Connect    ( (PSTR)strDATABASE.c_str() ) ;  
  if(odbcPri.Error())
  {
	  m_editTexto =  "NO SE PUEDE CONECTAR A BASE DE DATOS : ";                
	  m_editTexto += odbcPri.GetErrorMessage(NULL);
	  m_editTexto += odbcPri.GetLastSQLCommand(NULL);
      AfxMessageBox(m_editTexto);
      UpdateData( FALSE );
	  ProcessAppMsgQueue();

	  fprintf(fLogg,"\r\n");
	  fprintf(fLogg, (LPCSTR)m_editTexto);
	  fprintf(fLogg,"\r\n");	  
      fflush( fLogg ); 
	  return;
  };

  /* PRECONDITION : REGISTER SIZE = 2000 + 6 */
  cnSize   = sizeof(recSALDOS);
  if( cnSize != 2000 + 6)
  {
	  AfxMessageBox("EL PROGRAMA NO HA SIDO COMPILADO CON ALINEACION A 2 BYTES. ABORTANDO.");
	  odbcPri.Disconnect();
	  return;
  }
  /* PRECONDITION : REGISTER SIZE = 2000 + 6 */
  cnSize   = sizeof(recAHOSAL);
  if( cnSize != 2000 + 6)
  {
	  AfxMessageBox("EL PROGRAMA NO HA SIDO COMPILADO CON ALINEACION A 2 BYTES. ABORTANDO.");
	  odbcPri.Disconnect();
	  return;
  }

  /* DATE TO PROCESS */
  m_datetimePicker2.GetTime( ctDate );  
  tmWorkingDay.tm_year = ctDate.GetYear() - 1900; /* Base 1900 */
  tmWorkingDay.tm_mon  = ctDate.GetMonth() - 1;   /* [0,11] */
  tmWorkingDay.tm_mday = ctDate.GetDay() - 90;    /* [1,31] */  
  tWorkingDay = mktime( &tmWorkingDay );
  strDate.Format( "%04i-%02i-%02i" , ctDate.GetYear(),
                                     ctDate.GetMonth(),
                                     ctDate.GetDay() );  
  strcpy( szFECHAOPE, (LPCSTR)strDate );

  /*******************************************/
  /* RESET FROM-DATE                         */
  /*******************************************/
  CTime ctDateTemp( tWorkingDay );
  m_datetimePicker.SetTime( &ctDateTemp );   /* FROM */
  UpdateData( FALSE );
  ProcessAppMsgQueue();
  /*******************************************/

#ifdef _USE_EMBEBED_SQL_
  strQuery =  "UPDATE DN_PARAMETROS SET DN_FECHA_PROCESO = '"  ;
  strQuery += szFECHAOPE ;            
  strQuery += "', DN_FECHA_HASTA='";
  strQuery += szFECHAOPE ;
  strQuery += "', DN_FECHA_DESDE='";
  strQuery += szFECHAOPE;
  strQuery += "' ";
  strQuery += "  WHERE DN_ID=1" ;
#else
  strQuery = "EXEC SP_UPDATE_DN_PARAMETROS '" ;
  strQuery += szFECHAOPE ;            
  strQuery += "', '";
  strQuery += szFECHAOPE ;
  strQuery += "', '";
  strQuery += szFECHAOPE;
  strQuery += "', 0, 1 "; /* REGISTRO-DESDE = 0 DIAS PARA ATRAS, REG-HASTA = 1 DIA PARA ATRAS */
#endif
  /* PREPARE QUERY  */
  iQueryNum = _ODBC_UPDATE_SNTC_ ;		
  /* EXECUTE QUERY */
  odbcPri.Query( (PSTR)strQuery.c_str(), &iQueryNum, FALSE, FALSE ) ;
  if(odbcPri.Error())
  {
	    m_editTexto = "NO SE PUEDE ACTUALIZAR LA FECHA DE PROCESO SQL : "; 
	    m_editTexto += odbcPri.GetErrorMessage(NULL);
	    m_editTexto += odbcPri.GetLastSQLCommand(NULL);
        AfxMessageBox(m_editTexto);
        UpdateData( FALSE );
	    ProcessAppMsgQueue( );

	    odbcPri.Disconnect();
	    return;
  };
  odbcPri.FreeSentence( iQueryNum );
  /****************************************************************************************/


  /*******************************************************/  
  /* Comenzando por SALDOS (Ctas.Ctes)                   */
  /* Y siguiendo por AHOSAL (Cajas Ahorro)               */
  /*******************************************************/
  if( DPS_INTER_SALDOS == eType )
    recTYPE = RT_SALDOS ;  
  else if( DPS_INTER_AHOSAL == eType )
    recTYPE = RT_AHOSAL ;  
  else if (DPS_INTER_AHSADO == eType)
	  recTYPE = RT_AHSADO ;  
  else
      return ;  

    /* PREPARE QUERY  */
    iQueryNum = _ODBC_READ_SNTC_ + recTYPE ;		
    odbcPri.Column( szCUENTA    , SQL_C_CHAR, 20+1, &iQueryNum ); //1
    odbcPri.Column( szSALACT    , SQL_C_CHAR, 19+1, &iQueryNum ); //2    
    odbcPri.Column( szACUERDO   , SQL_C_CHAR, 19+1, &iQueryNum ); //5
    odbcPri.Column( szNOMBRE    , SQL_C_CHAR, 30+1, &iQueryNum ); //6
    odbcPri.Column( szFECHA_APER  , SQL_C_CHAR, 30+1, &iQueryNum ); //4
	/* Fix 2012.12.04   mdc  Agregar razon social del abonado (abonado_cuenta + abonado_clave) */
    odbcPri.Column( szRAZON_SOCIAL, SQL_C_CHAR, 50+1, &iQueryNum ); //5

    /* QUERY */
#ifdef _USE_EMBEBED_SQL_
    strQuery = "SELECT CUENTA, SALDO, ACUERDO, NOMBRE FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS"
	            " WHERE ACUERDO IS NOT NULL"
	            " UNION "
	            " SELECT CUENTA, SALDO, 0 AS ACUERDO, NOMBRE FROM VIEW_MOVIMIENTOS_ULTIMOS_N_DIAS"
	            " WHERE ACUERDO IS NULL " ;
    /****************************************************/
    if( DPS_INTER_SALDOS == eType )
        strQuery += " AND MC_TIPCTA IN (1,7)";
    else
        strQuery += " AND MC_TIPCTA IN (11,15)";
    /****************************************************/

    strQuery += " GROUP BY CUENTA, SALDO, ACUERDO, NOMBRE";
    strQuery += " ORDER BY CUENTA, SALDO";
    
#else

    strQuery = " EXEC SP_VIEW_SALDOS_CON_MOVIMIENTOS_AGRUPADOS ";
    /****************************************************/
    if( DPS_INTER_SALDOS == eType)
          strQuery += " 1" ; // Antes " 1,7";
    else if( DPS_INTER_AHOSAL == eType)
          strQuery += " 11"; // Antes " 11,15";
	else
          strQuery += " 15"; // Antes " 11,15";
    /****************************************************/
#endif


    /* EXECUTE QUERY */
    odbcPri.Query( (PSTR)strQuery.c_str(), &iQueryNum, TRUE, TRUE ) ;
    if(odbcPri.Error())
    {
	    m_editTexto = "NO SE PUEDE EJECUTAR LA CONSULTA SQL : "; 
	    m_editTexto += odbcPri.GetErrorMessage(NULL);
	    m_editTexto += odbcPri.GetLastSQLCommand(NULL);
        AfxMessageBox(m_editTexto);
        UpdateData( FALSE );
	    ProcessAppMsgQueue( );

	    fprintf(fLogg,"\r\n");
	    fprintf(fLogg, (LPCSTR)m_editTexto);
	    fprintf(fLogg,"\r\n");	  
        fflush( fLogg ); 

	    odbcPri.Disconnect();
	    return;
    };


    /* Progress Bar */
    m_progressBar.SetRange32(0, 1 + 1 + 1 ); /* header + footer + 1 data record */
    m_progressBar.SetStep( 1 );	

    m_editTexto = "PROCESANDO..."; 
    UpdateData( FALSE );
    ProcessAppMsgQueue();

    /*****************************************************/
    /* FETCH CYCLE */
    for( odbcPri.Fetch( &iQueryNum ) ;
        (odbcPri.GetReturnCode() == SQL_SUCCESS_WITH_INFO 
            ||  
            odbcPri.GetReturnCode() == SQL_SUCCESS) ;
            odbcPri.Fetch( &iQueryNum )          )
    {		
            /* Counter */
            lCounter++ ;
            m_progressBar.StepIt() ;

		    /* Internal ODBC status */
		    lRetval = odbcPri.GetReturnCode();
            
            /* FORMAT RECORD */
            if( RT_SALDOS == recTYPE )
            {
                memset( &recSALDOS  , ' ' , sizeof recSALDOS);
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL);

                COPYSTRING( recSALDOS.SALDOS_TEXTO , "SALDOS"  );      
                RIGHTCOPYFIELD(recSALDOS.CODBAN,BCRA_ID_DEFAULT);
                /***********************************************/
                /* Modificacion al PICTURE de la CUENTA        */
                /***********************************************/                                     
                PADCOPYFIXEDINT_ASCII(stNUMCTA.chNUMCTA, szCUENTA );
	            RIGHTCOPYFIELD(recSALDOS.NUMCTA,stNUMCTA.chNUMCTA  );
                /***********************************************/
	            RIGHTCOPYFIELD(recSALDOS.CODMON," $ "          );

				if( strlen( szRAZON_SOCIAL ) > 0 )
					RIGHTCOPYFIELD(recSALDOS.NOMCUE,szRAZON_SOCIAL )
				else
					RIGHTCOPYFIELD(recSALDOS.NOMCUE,szNOMBRE       );

	            RIGHTCOPYFIELD(recSALDOS.NUMEDI,stNUMCTA.chNUMCTA       );
                NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
			        szSALACT, strlen(szSALACT), 0x00, is_true ) ;
	            EXACTCOPYFIELD(recSALDOS.SALCON,stPKDECIMAL.chPKDECIMAL);    // S9(15)V99 COMP-3	
                EXACTCOPYFIELD(recSALDOS.SALINI,stPKDECIMAL.chPKDECIMAL);    // S9(15)V99 COMP-3	
	            EXACTCOPYFIELD(recSALDOS.SALOPE,stPKDECIMAL.chPKDECIMAL);    // S9(15)V99 COMP-3	
                NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
			        szACUERDO, strlen(szACUERDO), 0x00 , is_true ) ;
	            EXACTCOPYFIELD(recSALDOS.TOTACU,stPKDECIMAL.chPKDECIMAL); // S9(15)V99 COMP-3	
                /* DEFAULT ALL ZEROES (in PACKED DECIMAL format) */
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;
	            EXACTCOPYFIELD(recSALDOS.MARGEN,stPKDECIMAL.chPKDECIMAL); // S9(15)V99 COMP-3	
                DateTimeToDateYYYYMMDD(szFECHAOPE,szYYYYMMDD);
	            EXACTCOPYFIELD(recSALDOS.FEULMO, szYYYYMMDD + 2); // YYMMDD
	            EXACTCOPYFIELD(recSALDOS.HOULMO, "000000"  );
	            EXACTCOPYFIELD(recSALDOS.FECAPE, "060101"  ); // APERTURA PRE-FIJADA
                /* DEFAULT ALL ZEROES (in PACKED DECIMAL format) */
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;
                EXACTCOPYFIELD(recSALDOS.INTDEV,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3	
	            EXACTCOPYFIELD(recSALDOS.PAGPROV,stPKDECIMAL.chPKDECIMAL); // S9(15)V99 COMP-3	
                EXACTCOPYFIELD(recSALDOS.CHECOM,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3	
	            EXACTCOPYFIELD(recSALDOS.IMPCRE,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3	
	            EXACTCOPYFIELD(recSALDOS.IMPDBE,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3		    
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[1] = 0x0c;
	            EXACTCOPYFIELD(recSALDOS.ORDNOP,stPKDECIMAL.chPKDECIMAL);  // COMP-3


		        fprintf(fLogg,"\r\nDATANET - REGISTRO BATCH DE SALDOS PARSEADO");
		        fprintf(fLogg,"\r\n--------------------------------------------");
		        fprintf(fLogg,"\r\nHEADER [%*.*s]",6,6,recSALDOS.SALDOS_TEXTO	);        
		        fprintf(fLogg,"\r\nBANCO [%*.*s]",3,3,recSALDOS.CODBAN	);        
		        fprintf(fLogg,"\r\nCUENTA [%*.*s]",17,17,recSALDOS.NUMCTA);        
		        fprintf(fLogg,"\r\nCODMON [%*.*s]",3,3,recSALDOS.CODMON);                
	            fprintf(fLogg,"\r\nNOMCUE [%*.*s]",29,29,recSALDOS.NOMCUE  );
                PackedDecimalToString( (PBYTE)recSALDOS.SALCON,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
	            fprintf(fLogg,"\r\nSALCON [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
                PackedDecimalToString( (PBYTE)recSALDOS.SALOPE,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00 , is_true  );
		        fprintf(fLogg,"\r\nSALOPE [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
                PackedDecimalToString( (PBYTE)recSALDOS.SALINI,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
		        fprintf(fLogg,"\r\nSALINI [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
                PackedDecimalToString( (PBYTE)recSALDOS.TOTACU,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00 , is_true );
		        fprintf(fLogg,"\r\nTOTACU [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
                PackedDecimalToString( (PBYTE)recSALDOS.MARGEN,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00 , is_true );
		        fprintf(fLogg,"\r\nMARGEN [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
	            fprintf(fLogg,"\r\nFEULMO [%*.*s]",6,6,recSALDOS.FEULMO  );
	            fprintf(fLogg,"\r\nHOULMO [%*.*s]",6,6,recSALDOS.HOULMO  );
	            fprintf(fLogg,"\r\nFECAPE [%*.*s]",6,6,recSALDOS.FECAPE  );
                PackedDecimalToString( (PBYTE)recSALDOS.INTDEV,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00 , is_true );
		        fprintf(fLogg,"\r\nINTDEV [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
                PackedDecimalToString( (PBYTE)recSALDOS.PAGPROV,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00 , is_true );
		        fprintf(fLogg,"\r\nPAGPROV [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
		        PackedDecimalToString( (PBYTE)recSALDOS.ORDNOP,2, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,4,0x00, is_true  );
		        fprintf(fLogg,"\r\nORDNOP [%*.*s]",2,2,stPKDECIMAL.chPKDECIMAL );
		        PackedDecimalToString( (PBYTE)recSALDOS.CHECOM,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
		        fprintf(fLogg,"\r\nCHECOM [%*.*s]",18,18,stPKDECIMAL.chPKDECIMAL );		
		        PackedDecimalToString( (PBYTE)recSALDOS.IMPCRE,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
		        fprintf(fLogg,"\r\nIMPCRE [%*.*s]",18,18,stPKDECIMAL.chPKDECIMAL );		
		        PackedDecimalToString( (PBYTE)recSALDOS.IMPDBE,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
		        fprintf(fLogg,"\r\nIMPDBE [%*.*s]",18,18,stPKDECIMAL.chPKDECIMAL );
                fflush( fLogg );        
            }
            else if( RT_AHOSAL == recTYPE )
            {
                memset( &recAHOSAL  , ' ' , sizeof recSALDOS);
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL);

                COPYSTRING( recAHOSAL.AHOSAL_TEXTO , "AHOSAL"  );      
                RIGHTCOPYFIELD(recAHOSAL.CODBAN,BCRA_ID_DEFAULT);
                /***********************************************/
                /* Modificacion al PICTURE de la CUENTA        */
                /***********************************************/                                     
                PADCOPYFIXEDINT_ASCII(stNUMCTA.chNUMCTA, szCUENTA );
	            RIGHTCOPYFIELD(recAHOSAL.NUMCTA,stNUMCTA.chNUMCTA  );
                /***********************************************/	                
                RIGHTCOPYFIELD(recAHOSAL.TIPCAH,"0"          );
                RIGHTCOPYFIELD(recAHOSAL.CODMON," $ "          );
	            
				if( strlen( szRAZON_SOCIAL ) > 0 )
					RIGHTCOPYFIELD(recAHOSAL.NOMCUE,szRAZON_SOCIAL )
				else
					RIGHTCOPYFIELD(recAHOSAL.NOMCUE,szNOMBRE       );
	            
				RIGHTCOPYFIELD(recAHOSAL.NUMEDI,stNUMCTA.chNUMCTA );
                NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
			        szSALACT, strlen(szSALACT), 0x00, is_true ) ;
	            EXACTCOPYFIELD(recAHOSAL.SALCON,stPKDECIMAL.chPKDECIMAL);    // S9(15)V99 COMP-3	
                EXACTCOPYFIELD(recAHOSAL.SALINI,stPKDECIMAL.chPKDECIMAL);    // S9(15)V99 COMP-3	
                EXACTCOPYFIELD(recAHOSAL.SALDIS,stPKDECIMAL.chPKDECIMAL);    // S9(15)V99 COMP-3	
                DateTimeToDateYYYYMMDD(szFECHAOPE,szYYYYMMDD);
	            EXACTCOPYFIELD(recAHOSAL.FEULMO, szYYYYMMDD + 2); // YYMMDD
	            EXACTCOPYFIELD(recAHOSAL.HOULMO, "000000"  );
	            EXACTCOPYFIELD(recAHOSAL.FECAPE, "060101" ); // APERTURA PRE-FIJADA
                /* DEFAULT ALL ZEROES (in PACKED DECIMAL format) */
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;
                EXACTCOPYFIELD(recAHOSAL.INTDEV,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3	
                RIGHTCOPYFIELD(recAHOSAL.EXTRAC,"001"          );	                
                RIGHTCOPYFIELD(recAHOSAL.DEPCHE,"001"          );
	            EXACTCOPYFIELD(recAHOSAL.FEPRCA, szYYYYMMDD + 2); // YYMMDD 
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[3] = 0x0c;
                /* DEFAULT ALL ZEROES (in PACKED DECIMAL format) */
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;
	            EXACTCOPYFIELD(recAHOSAL.TASNOM,stPKDECIMAL.chPKDECIMAL);  // COMP-3	                
                EXACTCOPYFIELD(recAHOSAL.TASEFE,stPKDECIMAL.chPKDECIMAL);  // COMP-3	                
                /* DEFAULT ALL ZEROES (in PACKED DECIMAL format) */
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;
                EXACTCOPYFIELD(recAHOSAL.SALIND,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3	                

		        fprintf(fLogg,"\r\nDATANET - REGISTRO BATCH DE SALDOS C.AHO. PARSEADO");
		        fprintf(fLogg,"\r\n--------------------------------------------------");
		        fprintf(fLogg,"\r\nHEADER [%*.*s]",6,6,recAHOSAL.AHOSAL_TEXTO	);        
		        fprintf(fLogg,"\r\nBANCO [%*.*s]",3,3,recAHOSAL.CODBAN	);        
		        fprintf(fLogg,"\r\nCUENTA [%*.*s]",17,17,recAHOSAL.NUMCTA);        
		        fprintf(fLogg,"\r\nCODMON [%*.*s]",3,3,recAHOSAL.CODMON);                
	            fprintf(fLogg,"\r\nNOMCUE [%*.*s]",29,29,recAHOSAL.NOMCUE  );
                PackedDecimalToString( (PBYTE)recAHOSAL.SALCON,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
	            fprintf(fLogg,"\r\nSALCON [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
                PackedDecimalToString( (PBYTE)recAHOSAL.SALINI,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
		        fprintf(fLogg,"\r\nSALINI [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
                PackedDecimalToString( (PBYTE)recAHOSAL.SALDIS,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00 , is_true  );
		        fprintf(fLogg,"\r\nSALDIS [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
                fflush( fLogg ); 

            }
			else
            {
                memset( &recAHSADO  , ' ' , sizeof recSALDOS);
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL);

                COPYSTRING( recAHSADO.AHSADO_TEXTO , "AHSADO"  );      
                RIGHTCOPYFIELD(recAHSADO.CODBAN,BCRA_ID_DEFAULT);
                /***********************************************/
                /* Modificacion al PICTURE de la CUENTA        */
                /***********************************************/                                     
                PADCOPYFIXEDINT_ASCII(stNUMCTA.chNUMCTA, szCUENTA );
	            RIGHTCOPYFIELD(recAHSADO.NUMCTA,stNUMCTA.chNUMCTA  );
                /***********************************************/	                
                // RIGHTCOPYFIELD(recAHSADO.TIPCAH,"0"          ); //  fix 2015.12.22, mdc.
                RIGHTCOPYFIELD(recAHSADO.CODMON,"USD"          );  //  fix 2015.12.22, mdc.
	            
				if( strlen( szRAZON_SOCIAL ) > 0 )
					RIGHTCOPYFIELD(recAHSADO.NOMCUE,szRAZON_SOCIAL )
				else
					RIGHTCOPYFIELD(recAHSADO.NOMCUE,szNOMBRE       );
	            
				RIGHTCOPYFIELD(recAHSADO.NUMEDI,stNUMCTA.chNUMCTA );
                NumericToPackedDecimal( (PBYTE)&stPKDECIMAL.chPKDECIMAL, 9, 
			        szSALACT, strlen(szSALACT), 0x00, is_true ) ;
	            EXACTCOPYFIELD(recAHSADO.SALCON,stPKDECIMAL.chPKDECIMAL);    // S9(15)V99 COMP-3	
                EXACTCOPYFIELD(recAHSADO.SALINI,stPKDECIMAL.chPKDECIMAL);    // S9(15)V99 COMP-3	
                EXACTCOPYFIELD(recAHSADO.SALDIS,stPKDECIMAL.chPKDECIMAL);    // S9(15)V99 COMP-3	
                DateTimeToDateYYYYMMDD(szFECHAOPE,szYYYYMMDD);
	            EXACTCOPYFIELD(recAHSADO.FEULMO, szYYYYMMDD + 2); // YYMMDD
	            EXACTCOPYFIELD(recAHSADO.HOULMO, "000000"  );
	            EXACTCOPYFIELD(recAHSADO.FECAPE, "060101" ); // APERTURA PRE-FIJADA
                /* DEFAULT ALL ZEROES (in PACKED DECIMAL format) */
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;
                EXACTCOPYFIELD(recAHSADO.INTDEV,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3	
                // RIGHTCOPYFIELD(recAHSADO.EXTRAC,"001"          ); //  fix 2015.12.22, mdc.
                RIGHTCOPYFIELD(recAHSADO.DEPCHE,"001"          );
	            EXACTCOPYFIELD(recAHSADO.FEPRCA, szYYYYMMDD + 2); // YYMMDD 
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[3] = 0x0c;
                /* DEFAULT ALL ZEROES (in PACKED DECIMAL format) */
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;
	            EXACTCOPYFIELD(recAHSADO.TASNOM,stPKDECIMAL.chPKDECIMAL);  // COMP-3	                
                EXACTCOPYFIELD(recAHSADO.TASEFE,stPKDECIMAL.chPKDECIMAL);  // COMP-3	                
                /* DEFAULT ALL ZEROES (in PACKED DECIMAL format) */
                memset( &stPKDECIMAL, 0x00, sizeof stPKDECIMAL); stPKDECIMAL.chPKDECIMAL[8] = 0x0c;
                // EXACTCOPYFIELD(recAHSADO.SALIND,stPKDECIMAL.chPKDECIMAL);  // S9(15)V99 COMP-3 //  fix 2015.12.22, mdc.
				EXACTCOPYFIELD(recAHSADO.TIPCAH,"1");

		        fprintf(fLogg,"\r\nDATANET - REGISTRO BATCH DE SALDOS C.AHO. DOLARES");
		        fprintf(fLogg,"\r\n-------------------------------------------------");
		        fprintf(fLogg,"\r\nHEADER [%*.*s]",6,6,recAHSADO.AHSADO_TEXTO	);        
		        fprintf(fLogg,"\r\nBANCO [%*.*s]",3,3,recAHSADO.CODBAN	);        
		        fprintf(fLogg,"\r\nCUENTA [%*.*s]",17,17,recAHSADO.NUMCTA);        
		        fprintf(fLogg,"\r\nCODMON [%*.*s]",3,3,recAHSADO.CODMON);                
	            fprintf(fLogg,"\r\nNOMCUE [%*.*s]",29,29,recAHSADO.NOMCUE  );
                PackedDecimalToString( (PBYTE)recAHSADO.SALCON,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
	            fprintf(fLogg,"\r\nSALCON [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
                PackedDecimalToString( (PBYTE)recAHSADO.SALINI,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00, is_true  );
		        fprintf(fLogg,"\r\nSALINI [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
                PackedDecimalToString( (PBYTE)recAHSADO.SALDIS,9, 
			        (PSTR)stPKDECIMAL.chPKDECIMAL,18,0x00 , is_true  );
		        fprintf(fLogg,"\r\nSALDIS [%*.*s]",18,18, stPKDECIMAL.chPKDECIMAL );
                fflush( fLogg ); 

            };

        	    
            /* FILE HEADER */
            if( !bHeader )
            {
                bHeader = TRUE;
                BLANKSTRUCT( hdrSALDOS );
                COPYSTRING( hdrSALDOS.ASTERISCO1   , "*I*"           );
                COPYSTRING( hdrSALDOS.BANCO        , BCRA_ID_DEFAULT );
                if( RT_SALDOS == recTYPE )
                {
                    COPYSTRING( hdrSALDOS.SALDOS_TEXTO , "SALDOS"        );
                }
                else if( RT_AHOSAL == recTYPE )
                {
                    COPYSTRING( hdrSALDOS.SALDOS_TEXTO , "AHOSAL"        );
                }
				else
                {
                    COPYSTRING( hdrSALDOS.SALDOS_TEXTO , "AHSADO"        );
                }

                COPYSTRING( hdrSALDOS.FECHA_INICIAL, szYYYYMMDD  + 2 ); // yymmdd
                hdrSALDOS.FILLER[0] = ' ';


	            fwrite( &hdrSALDOS, sizeof( hdrSALDOS), 1, fBALANCES );
                fflush( fBALANCES );        
            }

            /* FILE BODY - REGISTER */                
            if( RT_SALDOS == recTYPE )
    	        fwrite( &recSALDOS, sizeof( recSALDOS), 1, fBALANCES );
            else if( RT_AHOSAL == recTYPE )
                fwrite( &recAHOSAL, sizeof( recAHOSAL), 1, fBALANCES );
			else
				fwrite( &recAHSADO, sizeof( recAHSADO), 1, fBALANCES );

            fflush( fBALANCES );    
            
		    // MSG QUEUE
		    ProcessAppMsgQueue();

    }; /* END FETCH CYCLE */
    /*****************************************************/

    /* FILE FOOTER */
    if( bHeader )
    {        
        BLANKSTRUCT( ftrSALDOS );
        COPYSTRING( ftrSALDOS.ASTERISCO1   , "*F*"           );
        COPYSTRING( ftrSALDOS.BANCO        , BCRA_ID_DEFAULT );
        if( RT_SALDOS == recTYPE )
        {
            COPYSTRING( ftrSALDOS.SALDOS_TEXTO , "SALDOS"        );                      
        }
        else if( RT_AHOSAL == recTYPE )
        {
            COPYSTRING( ftrSALDOS.SALDOS_TEXTO , "AHOSAL"        );                      
        }
		else
        {
            COPYSTRING( ftrSALDOS.SALDOS_TEXTO , "AHSADO"        );                      
        };
        ftrSALDOS.FILLER[0] = ' ';
        fwrite( &ftrSALDOS, sizeof( ftrSALDOS), 1, fBALANCES );	            
        fflush( fBALANCES );        
    };

  /* FREE CURRENT SENTENCE */
  odbcPri.FreeSentence( iQueryNum );


  /* POSTCONDITION */
  fclose( fLogg) ;
  fLogg     = NULL;
  fclose( fBALANCES) ;
  fBALANCES = NULL;

  /* ASCII-2-EBCDIC */
  if( m_checkConvertASCII )
  {
    /******************************************/
    strcpy( szBcraFilename, "LARGOS." );
    strcat( szBcraFilename, BCRA_ID_INTERF );
    /******************************************/
    lRetval = CONVERT_ASCII_EBCDIC_FILE( szDatFilename, ASCII_2_EBCDIC_NO_CRLF, sizeof(ftrSALDOS), &lCounter ,
                                         szBcraFilename ) ;
  }


  /* MESSAGE */
  m_editTexto = "FIN DE LA EJECUCION, REGISTROS : " ;
  m_editTexto = m_editTexto + itoa( lCounter, szAux, 10) ;			
  UpdateData( FALSE );
  ProcessAppMsgQueue();

  AfxMessageBox( m_editTexto );


  m_progressBar.SetRange32(0, 0); /* NO COUNTERS IN PROGRESS BAR */
  // Disconnect DB
  odbcPri.Disconnect();

  // Set Cur File
  m_comboHacia.SetWindowText( szLogFilename );
  if(!m_comboHacia.FindStringExact( 1, szLogFilename ))
  	 m_comboHacia.AddString( szLogFilename );	


}

void CFuegoDPSDlg::OnCbnDropdownComboFiles1()
{
	WIN32_FIND_DATA  w32FData ;	
	CHAR			 szFile[256]={"*.*"};
	HANDLE           hHandle    = NULL;
    INT              iStep      = 0;

    if(m_comboDesde.GetCount())
	    m_comboDesde.ResetContent();	

    for ( iStep = 0; iStep < 5; iStep++ )
    {
        /**********************************/
        if( m_checkConvertASCII )
        {
            if( 0 == iStep )
                strcpy( szFile , "*TRA*.BIN" );
            else if ( 1 == iStep )
                strcpy( szFile , "ABO*.BIN" );
			else if ( 2 == iStep )
				strcpy( szFile, "PBF*.BIN" );
			else if ( 3 == iStep )
				strcpy( szFile, "*CUENTA*.BIN" );
			else
				strcpy( szFile, "UMOV*.BIN" );
        }
        else
        {
            if( 0 == iStep )
                strcpy( szFile , "*TRA*.DAT" );
            else if( 1 == iStep )
                strcpy( szFile , "ABO*.DAT" );
			else if( 2 == iStep )
				strcpy( szFile, "PBF*.DAT" );
			else if ( 3 == iStep )
				strcpy( szFile, "*CUENTA*.DAT" );
			else
				strcpy( szFile, "UMOV*.DAT" );
        }
        /**********************************/

	    SetCurrentDirectory(".\\");		
	    memset( &w32FData, 0, sizeof w32FData);
	    hHandle = FindFirstFile( szFile, &w32FData  );
        /*** bug ***/
        if( hHandle )
            strcpy(szFile, w32FData.cFileName );
        /*** bug ***/
	    while(hHandle)
	    {
			if( strlen( szFile ) > 0 )
				m_comboDesde.AddString( szFile );
		    if( FindNextFile( hHandle, &w32FData  ) )
			    strcpy(szFile, w32FData.cFileName );
		    else
		    {
			    hHandle = NULL;
			    break;
		    };
	    };
    };
	
}

///////////////////////////////////////////////////////		
BOOL CFuegoDPSDlg::CheckHolidayDateInArray( char *szDateTime )
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
	char	szLogFeriados[] = "FERIADOS_H.log";
	fFeriados = fopen(szLogFeriados, "at");
	fprintf(fFeriados,"LOG PARA ERROR CON FECHA DE FERIADOS EN HOLIDAY\n");  
 
	/*******************************************************/
    /* MSG LOGG */
    m_editTexto = "CARGANDO FERIADOS..."; 
    UpdateData( FALSE );
    ProcessAppMsgQueue();
    /*******************************************************/

    /* CONNECT SECONDARY DATABASE */
    odbcLoc.SetUserName( (PSTR)strUSER.c_str()     );
    odbcLoc.SetPassword( (PSTR)strPASSWORD.c_str() );
    odbcLoc.Connect    ( (PSTR)strDATABASE.c_str() ) ;  
    if(odbcLoc.Error())
	    return FALSE;

    /* HOLIDAY? - PREPARE QUERY */
    iQueryNum = _ODBC_READNEXT_SNTC_ ;		
    odbcLoc.Column( szFECHAFER, SQL_C_CHAR, 20+1, &iQueryNum ); //1
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

    /*******************************************************/
    /* MSG LOGG */
    m_editTexto = m_editTexto + "LISTO. CONTINUANDO..."; 
    UpdateData( FALSE );
    ProcessAppMsgQueue();
    /*******************************************************/
	
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
///////////////////////////////////////////////////////		

/************************************************/
/* Impactar PBF                                 */
/************************************************/
/* szTrxFilename = nombre de archivo a procesar */
/* szOutputFile  = nombre de archivo de salida  */
/************************************************/
int CFuegoDPSDlg::Impactar_PBF(char *szTrxFilename, char *szOutputFile )
{
	/******************************************************************************/
	/* Argumentos de libreria "UmovPBF.EXE PBF UMOV DATABASE USER PASSW"          */
	/******************************************************************************/	
	char		*argv[7]     = {0,0,0,0,0,0,0};
	char		szRetval[10] = {0x00};
	int			iRetval      = 0;

	// Inicio
	m_editTexto  = "INICIANDO PROCESAMIENTO PBF... " ;
	m_editTexto  = m_editTexto ;
	m_editTexto += "\r\n";
	/************************************************/
	remove( szOutputFile );
	OutputLoggMessage( szOutputFile , (LPCSTR)m_editTexto );
	/************************************************/
	argv[0] = "UmovPbf.EXE " ;	
	argv[1] = szTrxFilename ;
	argv[2] = ".";
	argv[3] = (PSTR) strDATABASE.c_str() ;
	argv[4] = (PSTR) strUSER.c_str();
	argv[5] = (PSTR) strPASSWORD.c_str();
	/************************************************/
	argv[6] = szOutputFile; /* fix arcvhivo de trazabilidad en importador 1 a 1 */
	/************************************************/
	
	/*************************************************************/
	iRetval = Procesar_Batch_Umov_PBF( 7 , argv ) ;
	/*************************************************************/
	
	if( iRetval < 0)
	{
		m_editTexto =  "Error de procesamiento del archivo = ";
		m_editTexto += itoa(iRetval, szRetval, 10);
		m_editTexto += "\n( -2 : No se pudo conectar a la base de datos)";
		m_editTexto += "\n( -1 : Argumentos incorrectos)";
		AfxMessageBox( m_editTexto );
	}
	// Retorno
	return iRetval;
};

/************************************************/
/* Impactar UMOV                                */
/************************************************/
/* szTrxFilename = nombre de archivo a procesar */
/* szOutputFile  = nombre de archivo de salida  */
/************************************************/
int CFuegoDPSDlg::Impactar_UMOV(char *szTrxFilename, char *szOutputFile )
{
	/******************************************************************************/
	/* Argumentos de libreria "UmovPBF.EXE PBF UMOV DATABASE USER PASSW"          */
	/******************************************************************************/	
	char		*argv[7]     = {0,0,0,0,0,0};
	char		szRetval[10] = {0x00};
	int			iRetval      = 0;

	m_editTexto  = "INICIANDO PROCESAMIENTO UMOV... " ;
	m_editTexto  = m_editTexto ;
	m_editTexto += "\r\n";
	/************************************************/
	remove( szOutputFile );
	OutputLoggMessage( szOutputFile , (LPCSTR)m_editTexto );
	/************************************************/
	argv[0] = "UmovPbf.EXE " ;	
	argv[1] = "." ;
	argv[2] = szTrxFilename;
	argv[3] = (PSTR) strDATABASE.c_str() ;
	argv[4] = (PSTR) strUSER.c_str();
	argv[5] = (PSTR) strPASSWORD.c_str();
	/************************************************/
	argv[6] = szOutputFile; /* fix arcvhivo de trazabilidad en importador 1 a 1 */
	/*************************************************************/
	iRetval = Procesar_Batch_Umov_PBF( 7 , argv ) ;
	/*************************************************************/
	if( iRetval < 0)
	{
		m_editTexto =  "Error de procesamiento del archivo = ";
		m_editTexto += itoa(iRetval, szRetval, 10);
		m_editTexto += "\n( -2 : No se pudo conectar a la base de datos)";
		m_editTexto += "\n( -1 : Argumentos incorrectos)";
		AfxMessageBox( m_editTexto );
	}
	// Retorno
	return iRetval;
};

std::string CFuegoDPSDlg::TransformarLeyenda(std::string sParaCambiar, std::string sTipoMovimiento)
{
	std::map<std::string, std::string>::iterator it;
	sParaCambiar.erase(std::find_if(sParaCambiar.rbegin(), sParaCambiar.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), sParaCambiar.end());
	it = mListadoLeyendas.find(sParaCambiar);
	if(it != mListadoLeyendas.end())
		return it->second;
	else
		return (sTipoMovimiento == "C") ? "CREDITOS VARIOS" : "DEBITOS INTERNOS";

}

void CFuegoDPSDlg::CargarLeyendas()
{
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRED $","CR CREDITO $             "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRED-$","CR CREDITO-$             "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRED.$","CR CREDITO $             "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRED90","CC CREDITO SUC 90        "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB CC VS","DEB CTA CTE VARIOS       "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("TRCE COM","TRANSFERENCIA EMISORA COM"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("TRCR COM","TRANSF RECEPTORA COM     "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("TRCE EXT","TRANSF EMISORA A EXTERNOS"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("TRCR EXT","TR RECEPTORA DE EXTERNOS "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("TRCE CPI","TRANSF EMIS CP A INTERNO "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("TRCE CCP","TRANSF EMISORA COM A CP  "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("TRCR CPE","TRANS REC C PROPIA EXTER "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("TRCE CPE","TRANS EMIS C PROP A EXTER"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IVA CR","PP IVA CREDITO FISCAL    "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IVA CRAJ","PP AJUSTE IVA CRED FISCAL"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IVA DEV","CR IVA A COBRAR          "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IVA V $","IVA VTOS $               "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IVA V DL","IVA VTOS DOLAR           "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IVA V EX","IVA VTOS EXTERIOR        "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IVA V PR","IVA VTO PRENDAS          "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IVA V$AN","IVA VTO $ ANSES          "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMP CR","IMP LEY 25413 CC CREDITOS"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMP CRDL","IMP LEY 25413 U$S        "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMP DB","IMP LEY 25413 CC DEBITOS "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMP DBDL","IMP LEY 25413 U$S        "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMP GCCA","IMP LEY 25413 CA TOTAL   "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMP PRON","IMP DB/CR PROPIAS NORMAL "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMP PROR","IMP DB/CR PROPIAS REDUCID"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMP.CR","IMP LEY 25413 T RED CREDS"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMP.CR.","IMP LEY 25413 CC T RED   "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMP.DB","IMPUESTO LEY 25413 CC RED"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMP.DB.","IMP LEY 25413 CC T REDU  "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB CCHCE","DEBITO COMIS CH CERTIF   "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB CHCAD","CC DEBITO CHEQUE CAMARA  "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB CHCAM","CC DEBITO CHEQUE CAMARA  "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB CHCER","CC DEBITO CH CERTIFICADO "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB COMI","TS DEBITO COMISIONES     "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB CRED","DEBITO CREDITOS          "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB CTA L","DEB TRANF LIBRE DISP     "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB CTABL","CC DEBITO CTA. BLOQUEADA "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB CTADL","TS DEB EN CUENTA DOLARES "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB CTALP","DEB TRANSF LIB DISP PROP "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB CTATE","TS DEB/CRED EN CUENTA    "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB ENVEX","CC COMISION ENVIO EXTRACT"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB EXPO","EX DEBITO EXPORTACIONES  "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB EXTDL","CC COMIS EMIS EXT U$S    "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB EXTE","EX DEBITO EXTERIOR       "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB EXTR","CC COMISION EMIS EXTRACTO"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB FACT","DEBITO FACTURA PREPAGA   "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB FCAJA","DEBITO POR FALLA DE CAJA "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB IMPOR","DEBITO IMPORTACIONES     "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DB MANCC","CC COMISION MANTEN CTA   "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR CBIOS","CREDITO CAMBIOS          "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR CC VS","CRED CTA CTE VARIOS      "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR CHCER","CC CREDITO CHEQUE CERTIF "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR CRED","CREDITO CREDITOS         "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR CREXT","CREDITO CREDITOS EXTERIOR"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR CTA L","CRED TRANSF LIBRE DISP   "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR CTALP","CRED TRAN LIB DISP PROPIA"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR DEP48","CRED CTA CTE DEP 48 HORAS"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR EXPOR","CREDITO EXPORTACIONES    "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR HAB01","ACREDIT HABER AG 01      "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR HAB02","ACREDIT HABER AG 02      "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR IMPOR","CREDITO IMPORTACIONES    "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR MEPIM","CRED MEP RECIBIDO C/IMPUE"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR MEPRE","CRED MEP RECIB MISMO TIT "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR MITRA","TRANSF. MAYOR. MINOR     "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR MNTRA","TRANSF. MAYOR/MINOR S/IMP"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CR VS TS","TS CREDITOS VS TESORERIA "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRI CP $","CRED INTERB $ X C PROPIAS"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRI CP D","CRD INTERB DOL X C PROPIA"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRI DJ $","CRED INTB DEP JUDICIALES$"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRI DJ D","CRD INTB DEP JUDICIALES D"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRI PR $","CRED INTERB PROVEEDORES $"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRI PR D","CRD INTERB PROVEEDORES DL"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRI SL $","CRED INTERB SUELDOS $    "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("CRI SL D","CRD INTERB SUELDOS DOL   "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DBI CP $","DEB INTERB $ X CTA PROPIA"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DBI CP D","DEB INTERB DOL X C PROPIA"));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IVA V $M","IVA VTOS MUTUALES        "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IVA V DM","IVA VTOS DL MUTUALES     "));

	mListadoLeyendas.insert(std::pair<std::string, std::string>("RETEN."  ,"RETENCIONES DE CTAS      "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IVA"     ,"IVA DE CTAS              "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("IMPUESTO","IMP LEY 25413            "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DBI PR $","DEB INTERB PROVEEDORES $ "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("ANT SUEL","ACRED ANTICIP SUELDOS    "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("DBI SL $","DEB INTERB SUELDOS $     "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("P FACIL" ,"COMISION PAGO FACIL      "));
	mListadoLeyendas.insert(std::pair<std::string, std::string>("L INT A2","LIQ INTERESES CTA CTE    "));
}