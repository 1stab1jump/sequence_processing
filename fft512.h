//*****************************************************************************
//FFT512.h
//*****************************************************************************

#include "stm32f7xx_hal.h"

#ifndef __FFT512_H__
#define __FFT512_H__



//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************

typedef struct
{	
	int32_t real;								//int 32bits 
	int32_t imag;								//int 32bits
}complex_t;

//*****************************************************************************
// data
//*****************************************************************************
	
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Prototypes for the ADCSET functions.
//
//*****************************************************************************
extern int fft256p(int32_t x[],complex_t X[],int32_t sequence_length);
extern int fft512p(int32_t x[],complex_t X[],int32_t sequence_length);

//*****************************************************************************
//
// Declaration for the ADCSET variables.
//
//*****************************************************************************

#ifdef __cplusplus
}
#endif
#endif // __FFT128_H__
