//
// IT24 Sistemas S.A.
// BitMap String Library for ISO8583 class
//
//   Libreria de Cadenas BitMaps
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.07.31      mdc     Base
// (Alfa)       1998.08.05      mdc     bitmapstring_t
// (Alfa)       1999.01.06      mdc     renombrado del header a BITMAPL
// (Beta)       2003.01.21      mdc     BOOL ::GetAt(PSTR,INT=BITMAP_STR_LEN,INT);
//

#ifndef _BITMAPL_H_
#define _BITMAPL_H_

// Longitud de bitmap segun ISO 
#ifndef _BITMAP_STR_LEN_	
#define BITMAP_STR_LEN  (16)
#endif // _BITMAP_STR_LEN_	
// Longitud maxima de bitmaps soportados simultaneamente
#ifndef _MAX_BITMAPS_
#define MAX_BITMAPS		(30)
#endif // _MAX_BITMAPS_
// Campos ISO
#ifndef _CISOFIELDS_
#define CISOFIELDS		(128)
#endif //


// Tipo Cadena Bitmap ANSI
typedef struct
	{
	unsigned char byte[BITMAP_STR_LEN];	 // Bitmap string
	} bitmapstring_t[MAX_BITMAPS];


// Clase Lista de Mapa de Bits
class BITMAPLIST
{
public:
	
	// Metodos publicos de la clase
	EXPORT BITMAPLIST(void);					 // Constructor por default
	EXPORT BITMAPLIST(PSTR,INT=BITMAP_STR_LEN); // Constructor opcional	
	EXPORT ~BITMAPLIST();						 // Destructor

	EXPORT BOOL Insert(PSTR,INT=BITMAP_STR_LEN); // Insercion de string Bitmap		
	EXPORT BOOL Find(PSTR,INT=BITMAP_STR_LEN);	  // Buscador de string Bitmap	
	
	///////////////////////////////////////////////////////////////////////////////////
	EXPORT BOOL GetAt(PSTR,INT=BITMAP_STR_LEN,INT=0);// Recuperador de string Bitmap en X posicion
	EXPORT INT	GetCount();					      // Contador
	EXPORT BOOL MakeValidField(INT iPosition, WORD wField);   // Habilitar campo X
	EXPORT BOOL MakeInvalidField(INT iPosition, WORD wField); // Invalidar campo X
	EXPORT BOOL IsValidField(INT iPosition, WORD wField);     // Es Campo valido?		
	///////////////////////////////////////////////////////////////////////////////////

private:

	// Lista privada para mantener los string de bitmaps	
	bitmapstring_t	strlPrimary;         // Lista de strings Bitmaps
	bitmapstring_t	strlSecondary;       // Lista de strings Bitmaps
	INT				iCounter;			 // Contador de bitmaps cargados

};

#endif // BITMAPL_H

