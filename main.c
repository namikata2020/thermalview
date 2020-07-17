#include <stdio.h>
#include <stdlib.h>
#include "thermalview.h"

#define MAG 7

unsigned char colorimg[CAMERA_X*CAMERA_Y*3];

int main(int argc, char **argv)
{
    int fd = -1;
    char *dev_name = "/dev/video0";

    fd = opencamera(dev_name);

    openwin(CAMERA_X*MAG+100,CAMERA_Y*MAG);
    drawbar(CAMERA_X*MAG,0);
    for (;;) {
        uint16_t *img = readcamera(fd);
        convimage(img,colorimg,CAMERA_X,CAMERA_Y);
        vidioc_qbuf(fd);
        drawimage(colorimg,CAMERA_X,CAMERA_Y,MAG);
        
        if(ggetch()>0) break;
    }
    closecamera(fd);
    closewin();
    return 0;
}
