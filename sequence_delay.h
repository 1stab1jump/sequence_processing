/*  Function of this module:
This module provides a function to compute the delay of two sequences x[n] and
y[n] which are 'similar'. That is, x[n] and y[n] have the same sample rate and
spectrum. So y[n] can be regarded as a delayed x[n] with noise signal, namely,
y[n]=x[n-tao]+n[t].
As x[n] and y[n] are both discrete signal, we search TAO in a positive interger
range, from 1 to L. In this range we compute and compare the cross-correlation 
function of y[n] and x[n-tao], we can find a TAO where y[n] and x[n-TAO] attain
the maximum cross-correlation function. And this is the delay of y[n] to x[n].

Important parameters:
W : window length of the sequences. And to boost computation, W is a power of 2
 so we have a coresponding W_BIT. 
L : the length of search range.
*/

/*  Instructions:
Include basic hardware drvier files.
*/
#ifndef __SEQUENCE_DELAY_H__
#define __SEQUENCE_DELAY_H__
/*Cases:
This module was firstly used in flow velocity computation in 2017. In tube two
probes were installed to sample the signal of flow, and from this two probes
two sampled sequences x[n] and y[n] were obtained, by computation the delay of
x[n] and y[n], we could find how long it takes for signal from probe x[n] to
probe y[n]. And with the distance of the two probes, we could find the velocity
of flow.
*/

//Wide of the window. window of X and Y should be the same length
#define W (4096)
#define W_BIT (12)

//Length of search
#define L (2048)

//L+W=6144

extern double cov_x_y(void);
extern uint32_t num_of_max_cov(uint16_t channel1[],uint16_t channel2[]);

#endif 
