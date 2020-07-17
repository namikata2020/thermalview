#
# thermalview make file
#

CC    = gcc
CFLAGS = -O3 -g -Wall $(INCLUDE)
INCLUDE = -I/usr/local/include
LDFLAGS = 
LIBS = -L/usr/local/lib -leggx -lX11 -lm -lv4l2

TARGET = thermalview
SRCS = main.c camera.c image.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET) 

$(TARGET): $(OBJS) 
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS) $(INCLUDE) $(LDFLAGS) $(LIBS) 


clean:
	rm -f *.o *~  $(TARGET)

