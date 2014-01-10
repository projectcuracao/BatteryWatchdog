/*************************************************************
 * Useful averaging routines.
 * (c) 2012 Majenki Technologies
 * http://www.majenko.co.uk
 *************************************************************/

/* Note - this is currently a work in progress.  There are many
   more routines to be added. */

#ifndef _AVERAGE_H
#define _AVERAGE_H

/******
 * Rolling average.
 *
 * These routines all take three arguements, and are overloaded for the
 * different data types.
 * 
 * The first arguement is an array of the required data type to store
 * the rolling average data in.  The second arguement is the number of
 * slices in this array.  The third arguement is the new reading to
 * add to the array.  The current rolling average is returned.
 ******/

extern int rollingAverage(int *, int, int);
extern char rollingAverage(char *, int, char);
extern long rollingAverage(long *, int, long);
extern float rollingAverage(float *, int, float);
extern unsigned int rollingAverage(unsigned int *, int, unsigned int);
extern unsigned long rollingAverage(unsigned long *, int, unsigned long);
extern unsigned char rollingAverage(unsigned char *, int, unsigned char);

/******
 * Mean routines.
 * Pass an array, and the number of slices in the array, and it returns
 * the mean average.
 ******/

extern int mean(int *, int);
extern unsigned int mean(unsigned int *, int);
extern char mean(char *, int);
extern unsigned char mean(unsigned char *, int);
extern long mean(long *, int);
extern unsigned long mean(unsigned long *, int);
extern float mean(float *, int);

/******
 * Mode.
 * Pass it an array, and the number of slices in the array, and it returns
 * the value of the most common number in that array.
 ******/

extern int mode(int *, int);
extern unsigned int mode(unsigned int *, int);
extern char mode(char *, int);
extern unsigned char mode(unsigned char *, int);
extern long mode(long *, int);
extern unsigned long mode(unsigned long *, int);
extern float mode(float *, int);

/******
 * Minimum.
 * Pass an array and slice count, and it returns the minimum value from the
 * array.
 ******/

extern int minimum(int *, int);
extern unsigned int minimum(unsigned int *, int);
extern char minimum(char *, int);
extern unsigned char minimum(unsigned char *, int);
extern long minimum(long *, int);
extern unsigned long minimum(unsigned long *, int);
extern float minimum(float *, int);

/******
 * Maximum.
 * Pass an array and slice count, and it returns the maximum value from the
 * array.
 ******/

extern int maximum(int *, int);
extern unsigned int maximum(unsigned int *, int);
extern char maximum(char *, int);
extern unsigned char maximum(unsigned char *, int);
extern long maximum(long *, int);
extern unsigned long maximum(unsigned long *, int);
extern float maximum(float *, int);

/******
 * Standard Deviation
 * This is the tricky one.  Pass it, again, an array and a slice count, and
 * it will return the standard deviation of the contents of the array.  This
 * is the square root of the mean of the squares of the difference of the
 * values to the array mean.  Got it?
 ******/

float stddev(int *, int);
float stddev(unsigned int *, int);
float stddev(char *, int);
float stddev(unsigned char *, int);
float stddev(long *, int);
float stddev(unsigned long *, int);
float stddev(float *, int);

#endif



// Rolling average routines

int rollingAverage(int *store, int size, int entry)
{
	int l;
	long total = 0;
	int result;

	for(l=0; l<size-1; l++)
	{
		store[l] = store[l+1];
		total += store[l];
	}
	store[size-1] = entry;
	total += entry;
	result = total / size;

	return result;
}

char rollingAverage(char *store, int size, char entry)
{
	int l;
	long total = 0;
	char result;

	for(l=0; l<size-1; l++)
	{
		store[l] = store[l+1];
		total += store[l];
	}
	store[size-1] = entry;
	total += entry;
	result = total / size;

	return result;
}

float rollingAverage(float *store, int size, float entry)
{
	int l;
	float total = 0;
	float result;

	for(l=0; l<size-1; l++)
	{
		store[l] = store[l+1];
		total += store[l];
	}
	store[size-1] = entry;
	total += entry;
	result = total / (float)size;

	return result;
}

unsigned int rollingAverage(unsigned int *store, int size, unsigned int entry)
{
	int l;
	unsigned long total = 0;
	unsigned int result;

	for(l=0; l<size-1; l++)
	{
		store[l] = store[l+1];
		total += store[l];
	}
	store[size-1] = entry;
	total += entry;
	result = total / size;

	return result;
}

unsigned long rollingAverage(unsigned long *store, int size, unsigned long entry)
{
	int l;
	unsigned long total = 0;
	unsigned long result;

	for(l=0; l<size-1; l++)
	{
		store[l] = store[l+1];
		total += store[l];
	}
	store[size-1] = entry;
	total += entry;
	result = total / size;

	return result;
}

unsigned char rollingAverage(unsigned char *store, int size, unsigned char entry)
{
	int l;
	unsigned long total = 0;
	unsigned char result;

	for(l=0; l<size-1; l++)
	{
		store[l] = store[l+1];
		total += store[l];
	}
	store[size-1] = entry;
	total += entry;
	result = total / size;

	return result;
}

// Mean average routines

int mean(int *data, int count)
{
	int i;
	long total;
	int result;

	total = 0;
	for(i=0; i<count; i++)
	{
		total = total + data[i];
	}
	result = total / count;
	return result;
}

unsigned int mean(unsigned int *data, int count)
{
	int i;
	unsigned long total;
	unsigned int result;

	total = 0;
	for(i=0; i<count; i++)
	{
		total = total + data[i];
	}
	result = total / count;
	return result;
}

char mean(char *data, int count)
{
	int i;
	long total;
	char result;

	total = 0;
	for(i=0; i<count; i++)
	{
		total = total + data[i];
	}
	result = total / count;
	return result;
}

unsigned char mean(unsigned char *data, int count)
{
	int i;
	unsigned long total;
	unsigned char result;

	total = 0;
	for(i=0; i<count; i++)
	{
		total = total + data[i];
	}
	result = total / count;
	return result;
}

long mean(long *data, int count)
{
	int i;
	long total;
	long result;

	total = 0;
	for(i=0; i<count; i++)
	{
		total = total + data[i];
	}
	result = total / count;
	return result;
}

unsigned long mean(unsigned long *data, int count)
{
	int i;
	unsigned long total;
	unsigned long result;

	total = 0;
	for(i=0; i<count; i++)
	{
		total = total + data[i];
	}
	result = total / count;
	return result;
}

float mean(float *data, int count)
{
	int i;
	float total;
	float result;

	total = 0;
	for(i=0; i<count; i++)
	{
		total = total + data[i];
	}
	result = total / (float)count;
	return result;
}


/* Mode */

int mode(int *data, int count)
{
	int pos;
	int inner;
	int most;
	int mostcount;
	int current;
	int currentcount;

	most = 0;
	mostcount = 0;
	for(pos=0; pos<count; pos++)
	{
		current = data[pos];
		currentcount = 0;
		for(inner=pos+1; inner<count; inner++)
		{
			if(data[inner]==current)
			{
				currentcount++;
			}
		}
		if(currentcount > mostcount)
		{
			most = current;
			mostcount = currentcount;
		}
		// If we have less array slices left than the current
		// maximum count, then there is no room left to find
		// a bigger count.  We have finished early and we can
		// go home.
		if(count-pos < mostcount)
		{
			break;
		}
	}
	return most;
}

unsigned int mode(unsigned int *data, int count)
{
	int pos;
	int inner;
	unsigned int most;
	int mostcount;
	unsigned int current;
	int currentcount;

	most = 0;
	mostcount = 0;
	for(pos=0; pos<count; pos++)
	{
		current = data[pos];
		currentcount = 0;
		for(inner=pos+1; inner<count; inner++)
		{
			if(data[inner]==current)
			{
				currentcount++;
			}
		}
		if(currentcount > mostcount)
		{
			most = current;
			mostcount = currentcount;
		}
		// If we have less array slices left than the current
		// maximum count, then there is no room left to find
		// a bigger count.  We have finished early and we can
		// go home.
		if(count-pos < mostcount)
		{
			break;
		}
	}
	return most;
}

char mode(char *data, int count)
{
	int pos;
	int inner;
	char most;
	int mostcount;
	char current;
	int currentcount;

	most = 0;
	mostcount = 0;
	for(pos=0; pos<count; pos++)
	{
		current = data[pos];
		currentcount = 0;
		for(inner=pos+1; inner<count; inner++)
		{
			if(data[inner]==current)
			{
				currentcount++;
			}
		}
		if(currentcount > mostcount)
		{
			most = current;
			mostcount = currentcount;
		}
		// If we have less array slices left than the current
		// maximum count, then there is no room left to find
		// a bigger count.  We have finished early and we can
		// go home.
		if(count-pos < mostcount)
		{
			break;
		}
	}
	return most;
}

unsigned char mode(unsigned char *data, int count)
{
	int pos;
	int inner;
	unsigned char most;
	int mostcount;
	unsigned char current;
	int currentcount;

	most = 0;
	mostcount = 0;
	for(pos=0; pos<count; pos++)
	{
		current = data[pos];
		currentcount = 0;
		for(inner=pos+1; inner<count; inner++)
		{
			if(data[inner]==current)
			{
				currentcount++;
			}
		}
		if(currentcount > mostcount)
		{
			most = current;
			mostcount = currentcount;
		}
		// If we have less array slices left than the current
		// maximum count, then there is no room left to find
		// a bigger count.  We have finished early and we can
		// go home.
		if(count-pos < mostcount)
		{
			break;
		}
	}
	return most;
}

long mode(long *data, int count)
{
	int pos;
	int inner;
	long most;
	int mostcount;
	long current;
	int currentcount;

	most = 0;
	mostcount = 0;
	for(pos=0; pos<count; pos++)
	{
		current = data[pos];
		currentcount = 0;
		for(inner=pos+1; inner<count; inner++)
		{
			if(data[inner]==current)
			{
				currentcount++;
			}
		}
		if(currentcount > mostcount)
		{
			most = current;
			mostcount = currentcount;
		}
		// If we have less array slices left than the current
		// maximum count, then there is no room left to find
		// a bigger count.  We have finished early and we can
		// go home.
		if(count-pos < mostcount)
		{
			break;
		}
	}
	return most;
}

unsigned long mode(unsigned long *data, int count)
{
	int pos;
	int inner;
	unsigned long most;
	int mostcount;
	unsigned long current;
	int currentcount;

	most = 0;
	mostcount = 0;
	for(pos=0; pos<count; pos++)
	{
		current = data[pos];
		currentcount = 0;
		for(inner=pos+1; inner<count; inner++)
		{
			if(data[inner]==current)
			{
				currentcount++;
			}
		}
		if(currentcount > mostcount)
		{
			most = current;
			mostcount = currentcount;
		}
		// If we have less array slices left than the current
		// maximum count, then there is no room left to find
		// a bigger count.  We have finished early and we can
		// go home.
		if(count-pos < mostcount)
		{
			break;
		}
	}
	return most;
}

float mode(float *data, int count)
{
	int pos;
	int inner;
	float most;
	int mostcount;
	float current;
	int currentcount;

	most = 0;
	mostcount = 0;
	for(pos=0; pos<count; pos++)
	{
		current = data[pos];
		currentcount = 0;
		for(inner=pos+1; inner<count; inner++)
		{
			if(data[inner]==current)
			{
				currentcount++;
			}
		}
		if(currentcount > mostcount)
		{
			most = current;
			mostcount = currentcount;
		}
		// If we have less array slices left than the current
		// maximum count, then there is no room left to find
		// a bigger count.  We have finished early and we can
		// go home.
		if(count-pos < mostcount)
		{
			break;
		}
	}
	return most;
}

/* Minimum - find the minimum value of an array */

int minimum(int *data,int count)
{
	int l;
	int minval;

	minval = 32767;

	for(l=0; l<count; l++)
	{
		if(data[l] < minval)
		{
			minval = data[l];
		}
	}
	return minval;
}

unsigned int minimum(unsigned int *data,int count)
{
	int l;
	unsigned int minval;

	minval = 65535;

	for(l=0; l<count; l++)
	{
		if(data[l] < minval)
		{
			minval = data[l];
		}
	}
	return minval;
}

char minimum(char *data,int count)
{
	int l;
	char minval;

	minval = 127;

	for(l=0; l<count; l++)
	{
		if(data[l] < minval)
		{
			minval = data[l];
		}
	}
	return minval;
}

unsigned char minimum(unsigned char *data,int count)
{
	int l;
	unsigned char minval;

	minval = 255;

	for(l=0; l<count; l++)
	{
		if(data[l] < minval)
		{
			minval = data[l];
		}
	}
	return minval;
}

long minimum(long *data,int count)
{
	int l;
	long minval;

	minval = 2147483647L;

	for(l=0; l<count; l++)
	{
		if(data[l] < minval)
		{
			minval = data[l];
		}
	}
	return minval;
}

unsigned long minimum(unsigned long *data,int count)
{
	int l;
	unsigned long minval;

	minval = 4294967295UL;

	for(l=0; l<count; l++)
	{
		if(data[l] < minval)
		{
			minval = data[l];
		}
	}
	return minval;
}

float minimum(float *data,int count)
{
	int l;
	float minval;

	minval = 3.402823466e+38f;

	for(l=0; l<count; l++)
	{
		if(data[l] < minval)
		{
			minval = data[l];
		}
	}
	return minval;
}

/* Maximum - find the biggest value in an array */

int maximum(int *data,int count)
{
	int l;
	int maxval;

	maxval = -32768;

	for(l=0; l<count; l++)
	{
		if(data[l] > maxval)
		{
			maxval = data[l];
		}
	}
	return maxval;
}

unsigned int maximum(unsigned int *data,int count)
{
	int l;
	unsigned int maxval;

	maxval = 0;

	for(l=0; l<count; l++)
	{
		if(data[l] > maxval)
		{
			maxval = data[l];
		}
	}
	return maxval;
}

char maximum(char *data,int count)
{
	int l;
	char maxval;

	maxval = -128;

	for(l=0; l<count; l++)
	{
		if(data[l] > maxval)
		{
			maxval = data[l];
		}
	}
	return maxval;
}

unsigned char maximum(unsigned char *data,int count)
{
	int l;
	unsigned char maxval;

	maxval = 0;

	for(l=0; l<count; l++)
	{
		if(data[l] > maxval)
		{
			maxval = data[l];
		}
	}
	return maxval;
}

long maximum(long *data,int count)
{
	int l;
	long maxval;

	maxval = -2147483648L;

	for(l=0; l<count; l++)
	{
		if(data[l] > maxval)
		{
			maxval = data[l];
		}
	}
	return maxval;
}

unsigned long maximum(unsigned long *data,int count)
{
	int l;
	unsigned long maxval;

	maxval = 0;

	for(l=0; l<count; l++)
	{
		if(data[l] > maxval)
		{
			maxval = data[l];
		}
	}
	return maxval;
}

float maximum(float *data,int count)
{
	int l;
	float maxval;

	maxval = 1.175494351e-38f;

	for(l=0; l<count; l++)
	{
		if(data[l] > maxval)
		{
			maxval = data[l];
		}
	}
	return maxval;
}

/* Standard deviation. */

float stddev(int *data, int count)
{
	float square;
	float sum;
	int mu;
	float theta;
	int i;

	mu = mean(data,count);

	sum = 0;
	for(i=0; i<count; i++)
	{
		theta = (float)mu - (float)data[i];
		square = theta * theta;
		sum += square;
	}
	return sqrt(sum/(float)count);
}
 
float stddev(unsigned int *data, int count)
{
	float square;
	float sum;
	unsigned int mu;
	float theta;
	int i;

	mu = mean(data,count);

	sum = 0;
	for(i=0; i<count; i++)
	{
		theta = (float)mu - (float)data[i];
		square = theta * theta;
		sum += square;
	}
	return sqrt(sum/(float)count);
}
 
float stddev(char *data, int count)
{
	float square;
	float sum;
	char mu;
	float theta;
	int i;

	mu = mean(data,count);

	sum = 0;
	for(i=0; i<count; i++)
	{
		theta = (float)mu - (float)data[i];
		square = theta * theta;
		sum += square;
	}
	return sqrt(sum/(float)count);
}
 
float stddev(unsigned char *data, int count)
{
	float square;
	float sum;
	unsigned char mu;
	float theta;
	int i;

	mu = mean(data,count);

	sum = 0;
	for(i=0; i<count; i++)
	{
		theta = (float)mu - (float)data[i];
		square = theta * theta;
		sum += square;
	}
	return sqrt(sum/(float)count);
}
 
float stddev(long *data, int count)
{
	float square;
	float sum;
	long mu;
	float theta;
	int i;

	mu = mean(data,count);

	sum = 0;
	for(i=0; i<count; i++)
	{
		theta = (float)mu - (float)data[i];
		square = theta * theta;
		sum += square;
	}
	return sqrt(sum/(float)count);
}
 
float stddev(unsigned long *data, int count)
{
	float square;
	float sum;
	unsigned long mu;
	float theta;
	int i;

	mu = mean(data,count);

	sum = 0;
	for(i=0; i<count; i++)
	{
		theta = (float)mu - (float)data[i];
		square = theta * theta;
		sum += square;
	}
	return sqrt(sum/(float)count);
}
 
float stddev(float *data, int count)
{
	float square;
	float sum;
	float mu;
	float theta;
	int i;

	mu = mean(data,count);

	sum = 0;
	for(i=0; i<count; i++)
	{
		theta = (float)mu - (float)data[i];
		square = theta * theta;
		sum += square;
	}
	return sqrt(sum/(float)count);
}
 

