#include "fft512.h"
#include "math.h"

/*HOW to compute DFT with FFT
Usually, the ADC chip is only 12 bits or 16 bits. 32 bit unsigned int is large
enough for 128 bit computation.

1. How to tackle with WN?    WN=exp(-j*2*pi/N)
The product of WN is a vector moving on the unit circle. For 128 points FFT, WN
will have 128 points on the circle. But we have to only store the value of half
of these points due to symmetry.
NOTICE:
In this way, we make multiplication of WN into a index finding.

2. How many point is enough for power grid?
A typical ADC chip for power grid is AD73360, which has a bandwidth of 4kHz. 
For 50Hz power system, it means that, in one period 80 points are sampled.
So we could have a 128-point FFT.

3. Accuracy
16-bit ADC is accurate enough for smart grid.
2^16=65536
2^15=32768
2^14=16384
Significant figures
*/

/*WN=exp(-j*2*pi/N)*/
static complex_t W256[128]={
{32767,    0},{32758,  804},{32729, 1608},{32679, 2411},
{32610, 3212},{32522, 4011},{32413, 4808},{32286, 5602},
{32138, 6393},{31972, 7180},{31786, 7962},{31581, 8740},
{31357, 9512},{31114,10279},{30853,11039},{30572,11793},
{30274,12540},{29957,13279},{29622,14010},{29269,14733},
{28899,15447},{28511,16151},{28106,16846},{27684,17531},
{27246,18205},{26791,18868},{26320,19520},{25833,20160},
{25330,20788},{24812,21403},{24279,22006},{23732,22595},
{23170,23170},{22595,23732},{22006,24279},{21403,24812},
{20788,25330},{20160,25833},{19520,26320},{18868,26791},
{18205,27246},{17531,27684},{16846,28106},{16151,28511},
{15447,28899},{14733,29269},{14010,29622},{13279,29957},
{12540,30274},{11793,30572},{11039,30853},{10279,31114},
{ 9512,31357},{ 8740,31581},{ 7962,31786},{ 7180,31972},
{ 6393,32138},{ 5602,32286},{ 4808,32413},{ 4011,32522},
{ 3212,32610},{ 2411,32679},{ 1608,32729},{  804,32758},
{    0,32767},{ -804,32758},{-1608,32729},{-2411,32679},
{-3212,32610},{-4011,32522},{-4808,32413},{-5602,32286},
{-6393,32138},{-7180,31972},{-7962,31786},{-8740,31581},
{ -9512,31357},{-10279,31114},{-11039,30853},{-11793,30572},
{-12540,30274},{-13279,29957},{-14010,29622},{-14733,29269},
{-15447,28899},{-16151,28511},{-16846,28106},{-17531,27684},
{-18205,27246},{-18868,26791},{-19520,26320},{-20160,25833},
{-20788,25330},{-21403,24812},{-22006,24279},{-22595,23732},
{-23170,23170},{-23732,22595},{-24279,22006},{-24812,21403},
{-25330,20788},{-25833,20160},{-26320,19520},{-26791,18868},
{-27246,18205},{-27684,17531},{-28106,16846},{-28511,16151},
{-28899,15447},{-29269,14733},{-29622,14010},{-29957,13279},
{-30274,12540},{-30572,11793},{-30853,11039},{-31114,10279},
{-31357,9512},{-31581,8740},{-31786,7962},{-31972,7180},
{-32138,6393},{-32286,5602},{-32413,4808},{-32522,4011},
{-32610,3212},{-32679,2411},{-32729,1608},{-32758,804}
};

static complex_t W512[256]={
{32767,0},{32766,402},{32758,804},{32746,1206},
{32729,1608},{32706,2009},{32679,2411},{32647,2811},
{32610,3212},{32568,3612},{32522,4011},{32470,4410},
{32413,4808},{32352,5205},{32286,5602},{32214,5998},
{32138,6393},{32058,6787},{31972,7180},{31881,7571},
{31786,7962},{31686,8351},{31581,8740},{31471,9127},
{31357,9512},{31238,9896},{31114,10279},{30986,10660},
{30853,11039},{30715,11417},{30572,11793},{30425,12167},
{30274,12540},{30118,12910},{29957,13279},{29792,13646},
{29622,14010},{29448,14373},{29269,14733},{29086,15091},
{28899,15447},{28707,15800},{28511,16151},{28311,16500},
{28106,16846},{27897,17190},{27684,17531},{27467,17869},
{27246,18205},{27020,18538},{26791,18868},{26557,19195},
{26320,19520},{26078,19841},{25833,20160},{25583,20475},
{25330,20788},{25073,21097},{24812,21403},{24548,21706},
{24279,22006},{24008,22302},{23732,22595},{23453,22884},
{23170,23170},{22884,23453},{22595,23732},{22302,24008},
{22006,24279},{21706,24548},{21403,24812},{21097,25073},
{20788,25330},{20475,25583},{20160,25833},{19841,26078},
{19520,26320},{19195,26557},{18868,26791},{18538,27020},
{18205,27246},{17869,27467},{17531,27684},{17190,27897},
{16846,28106},{16500,28311},{16151,28511},{15800,28707},
{15447,28899},{15091,29086},{14733,29269},{14373,29448},
{14010,29622},{13646,29792},{13279,29957},{12910,30118},
{12540,30274},{12167,30425},{11793,30572},{11417,30715},
{11039,30853},{10660,30986},{10279,31114},{9896,31238},
{9512,31357},{9127,31471},{8740,31581},{8351,31686},
{7962,31786},{7571,31881},{7180,31972},{6787,32058},
{6393,32138},{5998,32214},{5602,32286},{5205,32352},
{4808,32413},{4410,32470},{4011,32522},{3612,32568},
{3212,32610},{2811,32647},{2411,32679},{2009,32706},
{1608,32729},{1206,32746},{804,32758},{402,32766},
{0,32767},{-402,32766},{-804,32758},{-1206,32746},
{-1608,32729},{-2009,32706},{-2411,32679},{-2811,32647},
{-3212,32610},{-3612,32568},{-4011,32522},{-4410,32470},
{-4808,32413},{-5205,32352},{-5602,32286},{-5998,32214},
{-6393,32138},{-6787,32058},{-7180,31972},{-7571,31881},
{-7962,31786},{-8351,31686},{-8740,31581},{-9127,31471},
{-9512,31357},{-9896,31238},{-10279,31114},{-10660,30986},
{-11039,30853},{-11417,30715},{-11793,30572},{-12167,30425},
{-12540,30274},{-12910,30118},{-13279,29957},{-13646,29792},
{-14010,29622},{-14373,29448},{-14733,29269},{-15091,29086},
{-15447,28899},{-15800,28707},{-16151,28511},{-16500,28311},
{-16846,28106},{-17190,27897},{-17531,27684},{-17869,27467},
{-18205,27246},{-18538,27020},{-18868,26791},{-19195,26557},
{-19520,26320},{-19841,26078},{-20160,25833},{-20475,25583},
{-20788,25330},{-21097,25073},{-21403,24812},{-21706,24548},
{-22006,24279},{-22302,24008},{-22595,23732},{-22884,23453},
{-23170,23170},{-23453,22884},{-23732,22595},{-24008,22302},
{-24279,22006},{-24548,21706},{-24812,21403},{-25073,21097},
{-25330,20788},{-25583,20475},{-25833,20160},{-26078,19841},
{-26320,19520},{-26557,19195},{-26791,18868},{-27020,18538},
{-27246,18205},{-27467,17869},{-27684,17531},{-27897,17190},
{-28106,16846},{-28311,16500},{-28511,16151},{-28707,15800},
{-28899,15447},{-29086,15091},{-29269,14733},{-29448,14373},
{-29622,14010},{-29792,13646},{-29957,13279},{-30118,12910},
{-30274,12540},{-30425,12167},{-30572,11793},{-30715,11417},
{-30853,11039},{-30986,10660},{-31114,10279},{-31238,9896},
{-31357,9512},{-31471,9127},{-31581,8740},{-31686,8351},
{-31786,7962},{-31881,7571},{-31972,7180},{-32058,6787},
{-32138,6393},{-32214,5998},{-32286,5602},{-32352,5205},
{-32413,4808},{-32470,4410},{-32522,4011},{-32568,3612},
{-32610,3212},{-32647,2811},{-32679,2411},{-32706,2009},
{-32729,1608},{-32746,1206},{-32758,804},{-32766,402}};

/*This function is the core function of FFT */
/*This is a FFT function for a sequence less than 512 points
For sequence whose length is not the power of 2, the function will add 0 at the
end of the sequence before radix-2 FFT.
To avoid overflow, the result is 1/32, 1/64, 1/128, 1/256 or 1/512 of the final
FFT result.
*/
int fft512p(int32_t x[],complex_t X[],int32_t sequence_length)
{
	complex_t product,adder;
	uint32_t i,j,k;								// count the time of loops
	uint32_t m;	
	uint32_t d,d2;								// butterfly spacing d, d2 is double d
	uint32_t wd;									// dif of WN


	uint32_t N,log2base_N;
	/*0. points of FFT
	For radix-2 FFT we have 64 points, 128 points, 256 points and 512 points.
	The number of FFT points depends on the length of the sequence.
	N represents the number of FFT points, and log2base_N is logarithms of N.*/
	if (sequence_length >512)
	{
		return 1;//error info
	}
	else if (sequence_length >256)
	{
		N          = 512;
		log2base_N =   9;
	}
	else if (sequence_length >128)
	{
		N          = 256;
		log2base_N =   8;
	}
	else if (sequence_length >64)
	{
		N          = 128;
		log2base_N =   7;
	}
	else if (sequence_length >32)
	{
		N          =  64;
		log2base_N =   6;
	}
		
	
	/*1.bit reverse and copy data
	For sequences whose length is not 64, 128, 256 or 512, add 0 at the end of
	the sequence*/
	for (i=0;i< N ;i++)
	{
		// compute index
		j=0;
		for (k=0;k<log2base_N;k++)
		{	 
			j=j<<1;
			j=j+((i>>k)&0x00000001);
		}
		if (j<sequence_length)
		{
		//copy sampled data to complex structure
			(X+i)->real=x[j];
		}
		else
		{
			(X+i)->real=0;/*zero*/
		}
		(X+i)->imag=0;
	}
																	

	/*2. compute*/
	for (m=0;m<log2base_N;m++)						// 2^7=128;m=0,1,2,3,4,5,6
	{
		d2=1<<(m+1); 						// d2=2,4,8,16,32,64,128
		d=1<<m;								// d=1,2,4,8,16,32,64
		wd=512>>(m+1);							// wd=64,32,16,8,4,2,1
		
		// compute each butterfly
		for (i=0;i<N;i=i+d2)				//	ergodic group
		{									
		/*the number of group descend 64,32,16, 8, 4, 2,  1*/	
		/*	as the d2 					 2, 4, 8,16,32,64,128
		d=1, i=0,2,4,6,8,10,...,126				
		d=2, i=0,4,8,12,...124					  
		d=4, i=0,8,16,24,...120
		d=8, i=0,16,32,48,...112
		d=16,i=0,32,64,96						  16
		d=32,i=0,64
		d=64,i=0
			*/
			k=i;
			for (j=0;j<(512>>1);j=j+wd)			 //	ergodic each butterfly in group 
			{								 //	in each group, the number of butterflies 

				// compute the butterfly
				product.real=(X+k+d)->real*W512[j].real-(X+k+d)->imag*W512[j].imag;
				product.imag=(X+k+d)->real*W512[j].imag+(X+k+d)->imag*W512[j].real;
				product.real=(product.real+16384)>>15;//
				product.imag=(product.imag+16384)>>15;

				adder.real=(X+k)->real;
				adder.imag=(X+k)->imag;

				//Com_add(adder,product,X[k]);
				(X+k)->real=(X+k)->real+product.real;
				(X+k)->imag=(X+k)->imag+product.imag;
				
				//Com_sub(adder,product,X[k+d]);
				(X+k+d)->real=adder.real-product.real;
				(X+k+d)->imag=adder.imag-product.imag;
				k=k+1;
			}
		}
		/*This part will be excuted 7 times, namely X[i] is 1/128*/
		for (i=0;i<N;i++)
		{
			(X+i)->real=((X+i)->real+1)>>1;	
			(X+i)->imag=((X+i)->imag+1)>>1;
		}
	}
	return 0;
}


int fft256p(int32_t x[],complex_t X[],int32_t sequence_length)
{
	complex_t product,adder;
	uint32_t i,j,k;								// count the time of loops
	uint32_t m;	
	uint32_t d,d2;								// butterfly spacing d, d2 is double d
	uint32_t wd;									// dif of WN


	uint32_t N,log2base_N;
	if (sequence_length >256)
	{
		return 1;//
	}
	else if (sequence_length >128)
	{
		N          = 256;
		log2base_N =   8;
	}
	else if (sequence_length >64)
	{
		N          = 128;
		log2base_N =   7;
	}
	else if (sequence_length >32)
	{
		N          =  64;
		log2base_N =   6;
	}
		
	
	/*1.bit reverse and copy data*/
	for (i=0;i< N ;i++)
	{
		// compute index
		j=0;
		for (k=0;k<log2base_N;k++)
		{	 
			j=j<<1;
			j=j+((i>>k)&0x00000001);
		}
		if (j<sequence_length)
		{
		//copy sampled data to complex structure
			(X+i)->real=x[j];
		}
		else
		{
			(X+i)->real=0;
		}
		(X+i)->imag=0;
	}
																	

	/*2. compute*/
	for (m=0;m<log2base_N;m++)						// 2^7=128;m=0,1,2,3,4,5,6
	{
		d2=1<<(m+1); 						// d2=2,4,8,16,32,64,128
		d=1<<m;								// d=1,2,4,8,16,32,64
		wd=256>>(m+1);							// wd=64,32,16,8,4,2,1
		
		// compute each butterfly
		for (i=0;i<N;i=i+d2)				//	ergodic group
		{									
		/*the number of group descend 64,32,16, 8, 4, 2,  1*/	
		/*	as the d2 					 2, 4, 8,16,32,64,128
		d=1, i=0,2,4,6,8,10,...,126				
		d=2, i=0,4,8,12,...124					  
		d=4, i=0,8,16,24,...120
		d=8, i=0,16,32,48,...112
		d=16,i=0,32,64,96						  16
		d=32,i=0,64
		d=64,i=0
			*/
			k=i;
			for (j=0;j<(256>>1);j=j+wd)			 //	ergodic each butterfly in group 
			{								 //	in each group, the number of butterflies 

				// compute the butterfly
				product.real=(X+k+d)->real*W256[j].real-(X+k+d)->imag*W256[j].imag;
				product.imag=(X+k+d)->real*W256[j].imag+(X+k+d)->imag*W256[j].real;
				product.real=(product.real+16384)>>15;//
				product.imag=(product.imag+16384)>>15;

				adder.real=(X+k)->real;
				adder.imag=(X+k)->imag;

				//Com_add(adder,product,X[k]);
				(X+k)->real=(X+k)->real+product.real;
				(X+k)->imag=(X+k)->imag+product.imag;
				
				//Com_sub(adder,product,X[k+d]);
				(X+k+d)->real=adder.real-product.real;
				(X+k+d)->imag=adder.imag-product.imag;
				k=k+1;
			}
		}
		/*This part will be excuted 7 times, namely X[i] is 1/128*/
		for (i=0;i<N;i++)
		{
			(X+i)->real=((X+i)->real+1)>>1;	
			(X+i)->imag=((X+i)->imag+1)>>1;
		}
	}
	return 0;
}