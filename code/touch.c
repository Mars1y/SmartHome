#include "lcd.h"
#include "touch.h"
#include "word.h"
#include "bmp.h"
#include <sys/types.h>      
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

typedef struct input_event input_event;

//触摸屏的长对应 x ，宽对应 y ，下同
pear get_touch()
{
    int x, y;
    input_event ev;
    int fd = open("/dev/input/event0", O_RDWR);
    if(fd==-1){
        perror("/dev/input/event0 open error");
        exit(-1);
    }
    while(1){
        if(read(fd, &ev, sizeof(input_event))==-1){
            perror("input_event read error");
            exit(-1);
        }
        if(ev.type==EV_ABS){
            if(ev.code==ABS_X){
                x = ev.value;
            }
            else if(ev.code==ABS_Y){
                y = ev.value;
            }
            //退出循环，即没有点按的情况有以下两种
            else if(ev.code==ABS_PRESSURE&&ev.value==0){
                break;
            }
        }
        else if(ev.type==EV_KEY&&ev.code==BTN_TOUCH&&ev.value==0){
            break;
        }
    }
    if(close(fd)==-1){
        perror("/dev/input/event0 close error");
    }
    pear posit;
    posit.x = x;
    posit.y = y;
    return posit;
}

int get_direction()
{
    input_event ev;
    int fd = open("/dev/input/event0", O_RDWR);
    if(fd==-1){
        perror("/dev/input/event0 open error");
        exit(-1);
    }
    int x0, y0, x1, y1;  
    x0 = x1 = y0 = y1 = -1;
    while(1){
        if(read(fd, &ev, sizeof(input_event))==-1){
            perror("input_event read error");
            exit(-1);
        }
        if(ev.type==EV_ABS){
            if(ev.code==ABS_X){
                if(x0==-1){   //滑动起点x坐标赋值
                    x0 = ev.value;
                }
                else{
                    x1 = ev.value;
                }
            }
            else if(ev.code==ABS_Y){    //滑动起点y坐标赋值
                if(y0==-1){
                    y0 = ev.value;
                }
                else{
                    y1 = ev.value;
                }
            }
            //退出循环，即没有点按的情况有以下两种
            else if(ev.code==ABS_PRESSURE&&ev.value==0){
                break;
            }
        }
        else if(ev.type==EV_KEY&&ev.code==BTN_TOUCH&&ev.value==0){
            break;
        }
    }
    if(close(fd)==-1){
        perror("/dev/input/event0 close error");
    }
    int dx, dy;
    dx = x1-x0;
    dy = y1-y0;
    if(abs(dx)>abs(dy)){
        if(dx>0)
            return 3;
        else if(dx<0)
            return 4;
    }
    else if(abs(dx)<abs(dy)){
        if(dy>0)
            return 1;
        else if(dy<0)
            return 2;
    }
}

void electric_album()
{
    char picture[5][32] = {"../bmp/c1.bmp", "../bmp/c2.bmp", "../bmp/c3.bmp", "../bmp/c4.bmp", "../bmp/c5.bmp"};
    int i = 0;
    int dir = -1;
    draw_picture(0, 0, picture[0]);
    while(1){
        dir = get_direction();
        if(dir==LEFT){
            if(i-1<0){
                continue;
            }
            draw_picture(0, 0, picture[--i]);
        }
        else if(dir==RIGHT){
            if(i+1>4){
                continue;
            }
            draw_picture(0, 0, picture[++i]);
        }
    }
}