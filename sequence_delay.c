#include "sequence_delay.h"
#include "math.h"
/*original data*/
extern double xy_cov_seq[L];
/*for compute*/

//
uint32_t sum_x,sum_y;
uint64_t sum_xx,sum_yy,sum_xy;//


/*original value of sum
channel1 and channel2 are 16-bit unsigned integer
*/
int start_compute(uint16_t channel1[],uint16_t channel2[])
{
	int i;
	uint32_t x_i,y_i;
	uint32_t xx_i,yy_i,xy_i;

	uint16_t *px,*py;/*the start of the two sequences*/

	
	px=channel1;
	py=channel2;
	
	sum_x=0;
	sum_xx=0;
	
	sum_y=0;
	sum_yy=0;
	
	sum_xy=0;
	
	/*two sequences {x[W]}and {y[W]},and  {x^2[W]},{y^2[W]}, {x*y[W]} */
	/*sum up*/
	for (i=0;i<W;i++)
	{
		x_i=*(px+i);				//16-bit integer
		y_i=*(py+i);				//16-bit integer
		sum_x		+=	x_i;
		xx_i	=	x_i*x_i;			//32-bit interger

		sum_xx	+=	(uint64_t)xx_i;
		
		sum_y		+=	y_i;
		yy_i	=	y_i*y_i;			//32-bit integer
		sum_yy	+=	(uint64_t)yy_i;
		
		xy_i	=	x_i*y_i;
		sum_xy	+=	(uint64_t)xy_i;
	}
	return 0;
}
	
/*compute new y related sum:xy,y,yy*/
uint32_t new_y_compute(uint32_t start,uint16_t channel1[],uint16_t channel2[])
{
	int i=0;
	uint32_t x_i,y_i;
	uint32_t yy_i,xy_i;

	uint16_t *px,*py;/*the start of the two sequences*/
	
	px=channel1;
	py=channel2+start;

	sum_y=0;
	sum_yy=0;
	sum_xy=0;
	
	for (i=0;i<W;i++)
	{
		x_i=*(px+i);
		y_i=*(py+i);
		sum_y		+=	y_i;
		yy_i	=	y_i*y_i;
		sum_yy	+=	(uint64_t)yy_i;
		xy_i	=	x_i*y_i;
		sum_xy	+=	(uint64_t)xy_i;
	}
	return 0;
}


/*Compute the cov of x[] and y[]
ADC: 12 bit uint 
256 point
channel 1:x 12bit;xx 24bit;sum_xx 32bit
channel 2:y 12bit;yy 24bit;sum_yy 32bit
xy 24bit; sum_xy 32bit
*/
double cov_x_y(void)
{
//	double frac_num,frac_den;
	uint64_t ave_xx,ave_yy,ave_xy,ave_x,ave_y;
	double frac_num,frac_den;
	
	ave_xx	=	sum_xx>>W_BIT;
	ave_yy	=	sum_yy>>W_BIT;
	ave_xy	=	sum_xy>>W_BIT;
	
	ave_x		=	sum_x>>W_BIT;
	ave_y		=	sum_y>>W_BIT;
	
	
	frac_num	=		(double)(ave_xy)-(double)(ave_x*ave_y)	;
	frac_den=sqrt(  
													((double)ave_xx-(double)((ave_x*ave_x)))
											*		((double)ave_yy-(double)((ave_y*ave_y)))
								);
	if (frac_den>0)
	{
		return (frac_num/frac_den);
	}
	else 
	{
		return 0;
	}
}

uint32_t num_of_max_cov(uint16_t channel1[],uint16_t channel2[])
{
//	double xy_cov_seq[L];
	double max_cov;
	int num;
	int i;
	start_compute(channel1,channel2);
	
	xy_cov_seq[0]=cov_x_y();
	max_cov=xy_cov_seq[0];

	//
	for (i=1;i<L;i++)
	{
		new_y_compute(i,channel1,channel2);
		xy_cov_seq[i]=cov_x_y();
		if (xy_cov_seq[i]>max_cov)
		{
			max_cov=xy_cov_seq[i];
			num=i;
		}
	}
	return num;
}
