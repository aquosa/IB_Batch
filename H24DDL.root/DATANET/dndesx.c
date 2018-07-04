#include <stdlib.h>
#include <string.h>

/*   Este archivo contiene las tablas usadas en el proceso de encryptado  */
/*   y desencryptado. Se toma como referencia las presentadas en :        */
/*   Federal Information Processing Standars Publication 46. 15-6-1977    */
/*   Specifications for the DATA ENCRYPTION STANDARD.                     */

/******         Permutaci¢n inicial IP                         ******/

char jip[64] =    { 57, 49, 41, 33, 25, 17,  9, 1,
		  59, 51, 43, 35, 27, 19, 11, 3,
		  61, 53, 45, 37, 29, 21, 13, 5,
		  63, 55, 47, 39, 31, 23, 15, 7,
		  56, 48, 40, 32, 24, 16,  8, 0,
		  58, 50, 42, 34, 26, 18, 10, 2,
		  60, 52, 44, 36, 28, 20, 12, 4,
		  62, 54, 46, 38, 30, 22, 14, 6 };

/******         Inversa de la permutaci¢n inicial IP_1         ******/

char jip_1[64] = {  39, 7, 47, 15, 55, 23, 63, 31,
		  38, 6, 46, 14, 54, 22, 62, 30,
		  37, 5, 45, 13, 53, 21, 61, 29,
		  36, 4, 44, 12, 52, 20, 60, 28,
		  35, 3, 43, 11, 51, 19, 59, 27,
		  34, 2, 42, 10, 50, 18, 58, 26,
		  33, 1, 41,  9, 49, 17, 57, 25,
		  32, 0, 40,  8, 48, 16, 56, 24 };

/******         E  BIT - SELECTION TABLE , usado por f(R,K)    ******/

char fe_bit[48] ={ 31,  0,  1,  2,  3,  4,
		   3,  4,  5,  6,  7,  8,
		   7,  8,  9, 10, 11, 12,
		  11, 12, 13, 14, 15, 16,
		  15, 16, 17, 18, 19, 20,
		  19, 20, 21, 22, 23, 24,
		  23, 24, 25, 26, 27, 28,
		  27, 28, 29, 30, 31,  0 };

/******         Funci¢n Permutaci¢n usado por f(R,K)           ******/

char fperm[32] = { 15,  6, 19, 20,
		  28, 11, 27, 16,
		   0, 14, 22, 25,
		   4, 17, 30,  9,
		   1,  7, 23, 13,
		  31, 26,  2,  8,
		  18, 12, 29,  5,
		  21, 10,  3, 24 };

/******    Funciones primitivas de selecci¢n S1, S2, ..., S8   ******/

char jfs[8][4][16] = {
		  14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	      0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	      4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	     15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,

	     15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0, 5,  10,
	      3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	      0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	     13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,

	     10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	     13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	     13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	      1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,

	      7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	     13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	     10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	      3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,

	      2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	     14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	      4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	     11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,

	     12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	     10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	      9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	      4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,

	      4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	     13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	      1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
	      6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,

	     13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	      1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	      7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	      2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 };

/******  Permuted Choice 1, PC_1 para obtener Co y Do del Key Schedule ******/

char pc_1[56] = {
		 56,  48,  40,  32,  24,  16,  8,
		  0,  57,  49,  41,  33,  25,  17,
		  9,   1,  58,  50,  42,  34,  26,
		 18,  10,   2,  59,  51,  43,  35,

		 62,  54,  46,  38,  30,  22,  14,
		  6,  61,  53,  45,  37,  29,  21,
		 13,   5,  60,  52,  44,  36,  28,
		 20,  12,   4,  27,  19,  11,   3 };

/******  Permuted Choice 2, PC_2 para obtener una subclave    ******/

char jpc_2[56]  = {
		 13,  16,  10,  23,   0,   4,
		  2,  27,  14,   5,  20,   9,
		 22,  18,  11,   3,  25,   7,
		 15,   6,  26,  19,  12,   1,
		 40,  51,  30,  36,  46,  54,
		 29,  39,  50,  44,  32,  47,
		 43,  48,  38,  55,  33,  52,
		 45,  41,  49,  35,  28,  31 } ;

/******  N£mero de left shifts, seg£n el n£mero de iteraciones  ******/

char lsh[16]   = {
		 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

#define CARNUL '\0'     /* Caracter Nulo */

#define CRIP_S   0      /* Encriptado sin extension */
#define CRIP_E   1      /* Encriptado con extension */

#define DESEN_S  2      /* Desencriptado sin compactacion */
#define DESEN_C  3      /* Desencriptado con compactacion */

#define XOR_S    4      /* Xor cipher/texto sin extension */
#define XOR_E    5      /* Xor cipher/texto con extension */

#define MAC_S    6      /* MAC sin extension */
#define MAC_E    7      /* MAC con extension */

#define SHIFTK   2      /* Shift sobre la clave secreta */

void keyE(char * key_in, char KEYi[][48],int shkey,int operac)
{
 int cont, contx, conty, contz ;
 char * mKEY;    /* secuencia que representa la clave del DES   */
 char * Ci;      /* Ci y Di, variables del KEY SCHEDULE         */
 char * Di;
 int bit1,bit2,bit3,bit4;

 mKEY = (char *) malloc(65);
 Ci   = (char *) malloc(29);
 Di   = (char *) malloc(29);

 if (shkey == SHIFTK) {
	  for (cont = 0 ; cont < 8; cont++)
			 key_in[cont] <<= 1;
	  }
 for (  cont = contz = 0; contz < 8; contz++ ) {
		  conty = (int) key_in[contz];
		  for ( contx = 128; contx >= 1; contx /= 2 ) {
				  mKEY[cont++] =  ( conty / contx);
				  conty %= contx;
				}
		  }

/* PERMUTED CHOICE 1, sobre mKEY y obtengo Co y Do, de 28 bits ambos */
	for ( cont = 0; cont < 28; cont++ ) {
	 Ci[cont] = mKEY[pc_1[cont]];
	 Di[cont] = mKEY[pc_1[cont+28]];
		 }

 /* Manejo de las subclaves : encrip /desencripto*/
	 if (operac == DESEN_S || operac == DESEN_C)
		 conty = 15;
	  else
		 conty = 0;

 /* Proceso Key Schedule, se obtiene los 16 Key ( Ki , i >0 ) */
	 for ( cont = 0; cont < 16; cont++ ) {
			/* Aplicar left shift circular, respecto de la tabla LSH a Ci y Di */
	  bit1 = Ci[0]; bit3 = Di[0];
	  if ( lsh[cont] == 2 ) {
					bit2   = Ci[1];   bit4 = Di[1];
					for ( contx =0; contx < 26; contx++ ) {
							Ci[contx] = Ci[contx+2];
							Di[contx] = Di[contx+2];
							}
					Ci[26] = bit1;  Di[26] = bit3;
					Ci[27] = bit2;  Di[27] = bit4;
					}
			 else  /* Solo queda que lsh[cont] = 1, ver la tabla LSH */
					{
					 for ( contx =0; contx < 27; contx++ ) {
							 Ci[contx] = Ci[contx+1];
							 Di[contx] = Di[contx+1];
							 }
					 Ci[27] = bit1;  Di[27] = bit3;
		}

			 /*  Permuted Choice 2 (  Ci+1 & Di+1 ) = Ki+1 */
			  for ( contx = 0; contx < 48; contx++ )
					  KEYi[conty][contx] = ( jpc_2[contx] < 28 ) ?
									Ci[jpc_2[contx]] : Di[jpc_2[contx]%28];

				 if (operac == DESEN_S || operac == DESEN_C)
					 conty--;
				  else
					 conty++;
			 }

  free(Di);
  free(Ci);
  free(mKEY);

}


void Edes (char * texto, char KEYi[][48], unsigned char * visible,
			 int operac)
{
 int cont, contx, conty, contz ;
 char * msge_bin;     /* Block a encryptar, esta en forma binaria    */
 char * Li;           /* Li y Ri, variables del en[des]cryptador     */
 char * Ri;
 char * ER;           /* guarda la expansi¢n de bits de R            */
 char * BS8;          /* es la concatenaci¢n que ofrecen S1,S2,..,S8 */
 unsigned int pos, fil, num4bits;
 int maux[32];
 long numl;

 msge_bin = (char *) malloc(65);
 Li       = (char *) malloc(33);
 Ri       = (char *) malloc(33);
 ER       = (char *) malloc(49);
 BS8      = (char *) malloc(33);

 if (operac == DESEN_C) {
	 for (contx=conty=0,cont=0; conty < 4 ; conty++, cont+=3)  {
			pos =  texto[cont];
			pos -= 32;
			fil =  texto[cont+1];
			pos += (fil-32)*48;
			fil =  texto[cont+2];
			pos += (fil-32)*2304;
			texto[contx++] = (unsigned char) (pos/256);
			texto[contx++] = (unsigned char) (pos%256);
		  }
	}

for ( contz=0,cont=0; cont < 8; cont++){
		conty = (int) texto[cont];
		for ( contx = 128; contx >= 1; contx /= 2 ) {
				msge_bin[contz++] =  ( conty / contx);
				conty %= contx;
				}
		}

/* Esta parte hace la permutacion inicial de los 64 bits (msge_bin) seg£n */
/* la TABLA  jip, se obtiene Lo y Ro ( ambos de 32 bits), respectivamente  */
	for ( contz = 0; contz < 32; contz++ ) {
	 Li[contz] = msge_bin[jip[contz]];
	 Ri[contz] = msge_bin[jip[contz+32]];
		 }

	/*****  ENCIPHERING COMPUTATION, se llega hasta el preouput    ***/
	for (cont = 0 ; cont < 16; cont++) {
		 for ( contx = 0; contx < 32; contx++ ) {
		  maux[contx] = Li[contx];
				 Li[contx] = Ri[contx];
				 }
		/*********  fRK  =  f ( Ri, KEYi[cant_proc] )  **********/
	  /* Expandir Ri de 32 a 48 bits, usando la tabla FE_BIT */
	  /* ER = Expandir(R) + KEYi[cant_proc] , con "+" como OR - EXCLUSIVO  */

		 for ( contx = 0; contx < 48; contx++ )
			ER[contx] = Ri[fe_bit[contx]] ^ (int)KEYi[cont][contx] ;

  /* Aplicamos los selectores S1, S2,...,S8 y en BS8 se obtienen los 32 bits */

		 conty = 0;   /* posici¢n dentro de BS8 */

		 for ( contz = 0, pos = 0; contz < 8; contz++, pos += 6 ) {

	/* Obtener 6 bits de ER, consecutivamente a partir de la posici¢n 0 */
				 fil = 2*ER[pos] + ER[pos+5];
				 contx= 8*ER[pos+1] + 4*ER[pos+2] + 2*ER[pos+3] + ER[pos+4];

	/* jfs [contz] [fil] [contx] da un valor de rango [0 - 15] => 4 bits */
				 num4bits = jfs[contz][fil][contx];

		  /* num4bits a binario almacen ndolo consecutivamente en BS8 */
				 for ( contx = 8; contx >= 1;  contx /=2 ) {
						 BS8[conty++] = ( num4bits / contx );
						 num4bits %= contx;
						 }
				 }
	/****  Ri =  fRk + maux , donde "+" denota el OR - EXCLUSIVO  ****/
		 for ( contx = 0; contx < 32; contx++ )
				Ri[contx] =  BS8[fperm[contx]] ^ maux[contx] ;
		 }

  /***  msge_bin = R16 & L16, & denota concat ***/
	 for ( contx = 0; contx < 32; contx++ ) {
			 msge_bin[contx] = Ri[contx];
			 msge_bin[contx + 32] = Li[contx];
			 }

  /** Todo el texto esta encryptado **/
	  contx = cont = 0;
	  while ( contx < 64 ) {
		  for ( contz=0, conty = 128; conty >= 1; conty /=2, contx++)
						 contz += conty*msge_bin[jip_1[contx]];
				 maux[cont++] = contz;
				 }

 /*  Se aplica Xor (texto,cipher) */
	  if (operac == XOR_S || operac == XOR_E) {
		  for (cont=0;cont<8;cont++)
				 maux[cont] ^= texto[cont];
		  }

/*  Parte visible del MAC, se extienden de 2 bytes a tres, de texto */
	 if (operac == CRIP_E || operac == XOR_E) {
		  for ( contx = conty = 0; contx < 8; contx += 2 ) {
				  numl = (long)((((long)(maux[contx]))*256) +
									(maux[contx+1]));
				  for ( contz = 0; contz < 3; contz++ ) {
						  visible[conty++] = (char) ((numl % 48 ) + 32 );
						  numl /= 48;
						  }
				  }
		  }
	  else
		 for ( contx = 0; contx < 8; contx ++)
			  visible[contx] = maux[contx];

  free(BS8);
  free(ER);
  free(Ri);
  free(Li);
  free(msge_bin);

 }

void mac_des (char * texto, char  KEYi[][48], unsigned char * salida, int operac,
				 int nroblq)
{
 int c,n;
 int modo=0;

 for (c=0; c<8; c++)
	  salida[c]=CARNUL;

 for (n=1; n <= nroblq; n++) {
	  if (n == nroblq)
		  modo = operac - MAC_S;
	  for (c=0; c<8; c++)
			texto[c] ^= salida[c];
	  Edes(texto,KEYi,salida,modo);
	  texto += 8;
	 }
}


char* DNDESX(char * texto, unsigned char * clave, int opera, int shkey, int numbl)

 {
 char * cipher;
 char KEYi[16][48];  /* Subclaves del proceso DES */
 static char memcip [13];

 cipher = (char *) memcip;

 cipher[12]=CARNUL;

 numbl = strlen(texto) / 8;

 keyE(clave,KEYi,shkey,opera);

 if (opera < MAC_S)
	 Edes(texto,KEYi,cipher,opera);
  else
	 if (opera <= MAC_E)
		 mac_des(texto,KEYi,cipher,opera,numbl);
	  else
		 cipher = "ERROR--PARAM";

  return (char *) cipher;

}

