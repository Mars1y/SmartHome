#include "lcd.h"
#include <sys/types.h>      
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

int fd = -1;
int *plcd = NULL;
const int row = 480;
const int col = 800;

void lcd_init()
{
    fd = open("/dev/fb0", O_RDWR);
    if(fd==-1){
        perror("/dev/fb0 open error");
        exit(-1);
    }
    plcd = mmap(NULL, row*col*4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(plcd==MAP_FAILED){
        perror("mmap function error");
        exit(-1);
    }
}

//屏幕的宽对应 x ，长对应 y ，下同
void display_point(int x, int y, int color)
{
    if(x>=0&&x<row && y>=0&&y<col){
        *(plcd+x*col+y) = color;
    }
}

void display_rectangle(int x1, int x2, int y1, int y2, int color)
{
    for(int i=x1; i<=x2; i++){
        for(int j=y1; j<=y2; j++){
            display_point(i, j, color);
        }
    }
}

void draw_circle(int a, int b, int r, int color)
{
    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            if(pow((i-a), 2)+pow((j-b), 2)<=pow(r, 2)){
                display_point(i, j, color);
            }
        }
    }
}

void show_picture(int color)
{
    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            display_point(i, j, color);
        }
    }
}

void lcd_close()
{
    if(munmap(plcd, row*col*4)==-1){
        perror("munmap function error");
        exit(-1);
    }
    if(close(fd)==-1){
        perror("close /dev/fb0 error");
        exit(-1);
    }
}