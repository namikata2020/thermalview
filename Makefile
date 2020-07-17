#
# thermalview make file
#

CC    = gcc
CFLAGS = -O3 -g -Wall $(INCLUDE)
INCLUDE = -I/usr/local/include -I/usr/include/freetype2
LDFLAGS = 
LIBS = -L/usr/local/lib -leggx -lX11 -lm -lv4l2 -lfreetype

TARGET = thermalview
SRCS = main.c camera.c image.c drawchar.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET) 

$(TARGET): $(OBJS) 
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS) $(INCLUDE) $(LDFLAGS) $(LIBS) 


clean:
	rm -f *.o *~  $(TARGET)

