// Forecasting COVID-19
// 2020-03-29 by Matjaz Perc (http://www.matjazperc.com/)

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// files and counters
FILE *in,*out;		
char filename[1000];
long i,j,k,q,p,s;

// data, growth rates, forecasting
long data[1000],NOF;	
double growrate[1000],avggrowrate,mingrowrate,maxgrowrate,predict;
long lastxdays,daystoheal,daystodie;
double deathrate,healed,died,decgrowrate;

// calendar
char date[36890][11];
long day[36890],month[36890],year[36890];
long y,m,d,n;

// main program
int main(void)
{
	k=0; // make the calendar
	for (y=2020;y<2121;y++)
	{
		if ((y % 4)!=0) n=0; else if ((y % 100)!=0) n=1; else if ((y % 400)!=0) n=0; else n=1;
		for (m=1;m<=12;m++)
		{
			for (d=1;d<=31;d++)
			{
				if (m==2 && n==0 && d>28) break;
				if (m==2 && n==1 && d>29) break;
				if ((m==4 || m==6 || m==9 || m== 11) && d>30) break;
				month[k]=m; day[k]=d; year[k]=y;
				sprintf(date[k],"%ld/%ld%/%ld",m,d,y);
				k++;
			}
		}
	}

	printf("year the first entry in the file was recorded (e.g. 2020) and press enter: ");scanf("%ld",&y);
	printf("month when the first entry in the file was recorded (e.g. 2) and press enter: ");scanf("%ld",&m);
	printf("day when the first entry in the file was recorded (e.g. 25) and press enter: ");scanf("%ld",&d);printf("\n");
	printf("how many days to use (e.g. 14) and press enter: ");scanf("%ld",&lastxdays);printf("\n");
	sprintf(filename,"data.txt");

	// change these values if considering different death rates or #days to get well or to die
	deathrate=0.04; 
	daystoheal=14;
	daystodie=21;

	for (i=0;i<k;i++) { // find the index of the starting date and remember as n
		if (day[i]==d && month[i]==m && year[i]==y) break;}
	n=i;

	i=0; // read the data from file
	in=fopen(filename,"r");
	out=fopen("actual.txt","w");
	while (!(feof(in)))
	{
		fscanf(in,"%ld\n",&data[i]);
		if (i>0) growrate[i]=((double)data[i]/(double)data[i-1])-1.0;

		healed=0.0;died=0.0;
		if ((i-daystoheal)>=0) healed=data[i-daystoheal]*(1-deathrate);
		if ((i-daystodie)>=0) died=data[i-daystodie]*deathrate;

		fprintf(out,"%s %.0lf %.4lf\n",date[i+n],(double)(data[i]-healed-died),growrate[i]);

		i++;
	}
	NOF=i;
	fclose(in); fclose(out);

	// determine minimal, maximal, and average growth rate during the past lastxdays (14 default, but check data to adjust)
	mingrowrate=1e9; maxgrowrate=0.0; avggrowrate=0.0;
	for (i=NOF-lastxdays;i<NOF;i++)
	{
		avggrowrate=avggrowrate+growrate[i];
		if (growrate[i]<mingrowrate) mingrowrate=growrate[i];
		if (growrate[i]>maxgrowrate) maxgrowrate=growrate[i];
	}
	avggrowrate=avggrowrate/(double)lastxdays;
	
	// print min., max., avg. growth rates to screen
	printf("using last %ld days\n",lastxdays);
	printf("minimal growth rate is %.4lf\n",mingrowrate);
	printf("maximal growth rate is %.4lf\n",maxgrowrate);
	printf("average growth rate is %.4lf\n",avggrowrate);

	// forecasting
	out=fopen("forecast.txt","w");
	maxgrowrate=maxgrowrate*1.2;
	decgrowrate=maxgrowrate/30.0;

	for (q=0;q<=30;q++)
	{
		predict=data[NOF-1];
		for (i=NOF;i<NOF+daystoheal;i++)
		{
			predict=predict*(1+maxgrowrate-q*decgrowrate); 
			healed=0.0;died=0.0;
			if ((i-daystoheal)>=0) healed=data[i-daystoheal]*(1-deathrate);
			if ((i-daystodie)>=0) died=data[i-daystodie]*deathrate;
			fprintf(out,"%s %.0lf %.4lf\n",date[i+n],predict-healed-died,maxgrowrate-q*decgrowrate);
		}
		fprintf(out,"\n");
	}
	fclose(out);

	printf("\npress a number and enter to close: ");scanf("%ld",&i);	
	return(1);
}
