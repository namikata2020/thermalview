
#include "thermalview.h"

double ktoc(double val,int radiometryLowGain) 
{
    if (radiometryLowGain) {
        return (val - 2731.5) / 10.0;   //Lowゲイン
    } else {
        return (val - 27315.0) / 100.0; //Hightゲイン　デフォルト
    }
}

static int win=-1;

void openwin(int x,int y)
{
	//gsetinitialattributes(BOTTOM_LEFT_ORIGIN,DISABLE);
    win = gopen(x,y);
    gsetnonblock(ENABLE);
}

void closewin(void)
{
	gclose(win);
}

static void makeTcolor(double temp,int *r,int *g,int *b)
{

	if(temp<0) temp=0; 
	if(temp>50) temp=50; 
	if(makecolor(IDL2_WAVES,0,50,temp,r,g,b)!=0) {
	//if(makecolor(DS9_RAINBOW,0,50,temp,r,g,b)!=0) {
		fprintf(stderr,"makecolor() over flow.\n");
		exit(1);
	} 
}

double convimage(uint16_t *imsrc,unsigned char *imdst,int wx,int wy)
{
	double maxtemp = ktoc(imsrc[0],0);
	for(int y=0;y<wy;y++) {
		for(int x=0;x<wx;x++) {
			int r,g,b;
			unsigned int temp = imsrc[wx*y+x ] ;
			double t = ktoc(temp,0);
			makeTcolor(t,&r,&g,&b);
			if(t > maxtemp) maxtemp = t;
			imdst[(wx*y+x)*3 + 0] = r;
			imdst[(wx*y+x)*3 + 1] = g;
			imdst[(wx*y+x)*3 + 2] = b;
		}
	}

	return maxtemp;
}

void drawimage(unsigned char *img,int wx,int wy,int magni)
{
	int imgxsz = wx * magni;
	int imgysz = wy * magni;
	unsigned char *imbuf = (unsigned char *)malloc(imgxsz*imgysz*4);
	for(int y = 0; y < imgysz; y++) {
		for(int x = 0; x < imgxsz; x++) {
			double ox = (double)x / (double)magni;
			int x0 = ox;
			double dx = ox - x0;
			int x1 = x0 + 1;
			if(x1 == wx) x1 = wx - 1;

			double oy = (double)y / (double)magni;
			int y0 = oy;
			double dy = oy - y0;
			int y1 = y0 + 1;
			if(y1 == wy) y1 = wy - 1;

			int r = img[3 * (x1 + y1 * wx) + 0] * dx * dy
			  + img[3 * (x1 + y0 * wx) + 0] * dx * (1.0 - dy)
			  + img[3 * (x0 + y1 * wx) + 0] * (1.0 - dx) * dy
			  + img[3 * (x0 + y0 * wx) + 0] * (1.0 - dx) * (1.0 - dy);
			int g = img[3 * (x1 + y1 * wx) + 1] * dx * dy
			  + img[3 * (x1 + y0 * wx) + 1] * dx * (1.0 - dy)
			  + img[3 * (x0 + y1 * wx) + 1] * (1.0 - dx) * dy
			  + img[3 * (x0 + y0 * wx) + 1] * (1.0 - dx) * (1.0 - dy);
			int b = img[3 * (x1 + y1 * wx) + 2] * dx * dy
			  + img[3 * (x1 + y0 * wx) + 2] * dx * (1.0 - dy)
			  + img[3 * (x0 + y1 * wx) + 2] * (1.0 - dx) * dy
			  + img[3 * (x0 + y0 * wx) + 2] * (1.0 - dx) * (1.0 - dy);

			imbuf[ (y*imgxsz+x)*4 + 0] = 0xff;
			imbuf[ (y*imgxsz+x)*4 + 1] = r;
			imbuf[ (y*imgxsz+x)*4 + 2] = g;
			imbuf[ (y*imgxsz+x)*4 + 3] = b;
		}
	}
	gputimage(win,0,0,imbuf,imgxsz,imgysz,0);
	free(imbuf);
}

void message(char *msg)
{
	newrgbcolor(win,255,0,0);
	drawstr(win,49,49,70,0,msg);
	drawstr(win,51,51,70,0,msg);
	drawstr(win,51,49,70,0,msg);
	drawstr(win,49,51,70,0,msg);
	newrgbcolor(win,255,255,255);
	drawstr(win,50,50,70,0,msg);
}

void drawbar(int xp,int yp)
{
	int r,g,b;
	const int bhi=15;
	char buf[256];
  /* カラーバーを表示 */
	for(int i=0 ; i<=50 ; i++ ){
		makeTcolor(i,&r,&g,&b);
		newrgbcolor(win,r,g,b) ;
		fillrect(win,xp,i*bhi+yp,50,bhi);
		if((i%5) == 0 ) {
			sprintf(buf,"%d C",i);
			newrgbcolor(win,255,255,255) ;
			drawstr(win,xp+55,i*bhi+yp,14,0,buf);
		}
	}
}

