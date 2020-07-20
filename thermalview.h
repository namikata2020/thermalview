/*
 * thermalview ヘッダファイル
 */
#include <stdio.h>
#include <stdlib.h>
#include <eggx.h>
#include <stdint.h>
#include <stddef.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define CAMERA_X 160
#define CAMERA_Y 120

#define COLOR_SW_LEVEL 37.0 

struct buffer {
    void   *start;
    size_t length;
};

double ktoc(double val,int radiometryLowGain);
int opencamera(char dev_name[]);
uint16_t *readcamera(int fd);
void vidioc_qbuf(int fd);
void closecamera(int fd);

void openwin(int x,int y);
void closewin(void);
double convimage(uint16_t *imsrc,unsigned char *imdst,int wx,int wy);
void drawimage(unsigned char *img,int wx,int wy,int magni);
void message(char *msg);
void drawbar(int xp,int yp);

void setfonts(char fontsfile[]);
void drawchar(int win,int draw_x,int draw_y,int fontssize,double angle,char text[],int r,int g,int b);
