///////////////////////////////////////////////////////////////////////////////////////////////
//                                                              
// Objeto que lee opciones

#ifndef _OPCIONES_H_
#define _OPCIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <sys/types.h>

#include <qusrinc/alias.h>
#include <qusrinc/typedefs.h>

#define _XML_BUFFER_SIZE_		   (1024*20)            /* MAX BUFFER SIZE */


class OPCIONES
{
	public :
		/* DEFAULT CONSTRUCTOR */
		EXPORT OPCIONES(char *file);

		/* DESTRUCTOR */
		EXPORT ~OPCIONES() ;
	
		/* GET TAG VALUE FROM INTERNAL MESSAGE */
		EXPORT char *GetTagValue(  char *ptrTAG, char *ptrVALUE, size_t nLength  );
		bool IsOpen;
		int errornumber;
	private:
		char      szXMLBuffer[ _XML_BUFFER_SIZE_ ];        // Internal XML buffer 
		char      *ptrLastTag; // internal ptr. to last parsed TAG		
		
		/* GET NEXT TAG VALUE FROM INTERNAL MESSAGE */
		char *GetNextTagValue(  char *ptrTAG, char *ptrVALUE, size_t nLength, 
									   short *pshError, BOOL bFromBeginning  );

	
};

#endif //


