//
// IT24 SISTEMAS S.A.
// Proyecto Pago Automatico de Servicios (PAS)
// Transaction Control Server
// ISO-8583-Field header
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.07.31      mdc     Base
// (Beta)       1998.09.03      mdc     Se agrega tipo de campo variable de longitud 4
//

#ifndef FIELD_ISO8583_H
#define FIELD_ISO8583_H

// Tipos de Campos
#define FIELD_TYPE_FIX		0
#define FIELD_TYPE_VAR_MIN 	1
#define FIELD_TYPE_VAR_MAX	4

// Clase Descripcion de Campo dentro del Mensaje ISO-8583
class FIELD
	{	
	public:

		// Metodos Publicos
		FIELD(void);              // Constructor por default
		FIELD(WORD,WORD,WORD);    // Constructor opcional
		void Reset(void);         // Reset
		void Set(WORD,WORD,WORD); // Set
		
		// Variables Publicas	
		WORD wType;   // Tipo de Campo (0=Fijo,1,2,3,4=Variable)
		WORD wOffset; // Desplazamiento desde el inicio (en bytes)
		WORD wLength; // Longitud (en bytes)		
	};

#endif // FIELD_ISO8583_H