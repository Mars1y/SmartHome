#include "lcd.h"
#include "bmp.h"
#include "word.h"
#include "touch.h"
#include "uart.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

//LCD屏幕像素与触摸屏坐标的换算比例
//x 对应屏幕的长，y 对应屏幕的宽
const double dx = 1.28;
const double dy = 1.25;

int main()
{
    //打开屏幕
    lcd_init();
    //显示主页面
    draw_picture(0, 0, "../bmp/mind.bmp");
    draw_picture(30, 675, "../bmp/off.bmp");
    draw_picture(110, 675, "../bmp/off.bmp");
    draw_picture(190, 675, "../bmp/off.bmp");
    //创建线程，获取GY39传感器数据并显示 
    pthread_t tid = 1;
    if(pthread_create( &tid, NULL, get_gy39, NULL )!=0){
        perror("thread create error:");
        exit(-1);
    }
    //加入手动控制家居功能
    int dir = -1;
    pear posit;
    while(1){
        posit = get_touch();
        if(posit.x>675*dx&&posit.x<775*dx&&posit.y>30*dy&&posit.y<80*dy){
            //led灯
            led_beep_ctrl('l');  
        }
        else if(posit.x>675*dx&&posit.x<775*dx&&posit.y>110*dy&&posit.y<160*dy){
            //蜂鸣器
            led_beep_ctrl('c');
        }
        else if(posit.x>675*dx&&posit.x<775*dx&&posit.y>190*dy&&posit.y<240*dy){
            mp3_ctl("../mp3/pgydyd.mp3");
        }
    }

    return 0;
}

