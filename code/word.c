#include "lcd.h"
#include <sys/types.h>      
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//定义三维数组表示0-9，第一维指向单个数字的字符点阵
char num[10][46][3] = {
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFE,
0x00,0x07,0xFF,0x00,0x0F,0xFF,0x80,0x1F,0x9F,0xC0,0x1F,0x07,0xC0,0x3E,0x07,0xE0,
0x3E,0x03,0xE0,0x3C,0x03,0xE0,0x7C,0x03,0xE0,0x7C,0x01,0xF0,0x7C,0x01,0xF0,0x7C,
0x01,0xF0,0x7C,0x01,0xF0,0x7C,0x01,0xF0,0x7C,0x01,0xF0,0x7C,0x01,0xF0,0x7C,0x01,
0xF0,0x7C,0x01,0xF0,0x7C,0x01,0xE0,0x7C,0x03,0xE0,0x7C,0x03,0xE0,0x3E,0x03,0xE0,
0x3E,0x07,0xC0,0x1F,0x0F,0xC0,0x1F,0xFF,0x80,0x0F,0xFF,0x00,0x07,0xFE,0x00,0x00,
0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,
0x00,0x00,0x7C,0x00,0x01,0xFC,0x00,0x0F,0xFC,0x00,0x1F,0xFC,0x00,0x1F,0xFC,0x00,
0x1E,0x7C,0x00,0x18,0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,
0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,
0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,
0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFC,
0x00,0x0F,0xFF,0x00,0x1F,0xFF,0x80,0x3F,0x1F,0xC0,0x3C,0x07,0xC0,0x38,0x07,0xC0,
0x00,0x03,0xE0,0x00,0x03,0xE0,0x00,0x03,0xE0,0x00,0x03,0xC0,0x00,0x07,0xC0,0x00,
0x0F,0xC0,0x00,0x0F,0x80,0x00,0x3F,0x00,0x00,0x7E,0x00,0x00,0xFC,0x00,0x03,0xF8,
0x00,0x07,0xF0,0x00,0x0F,0xC0,0x00,0x1F,0x80,0x00,0x1F,0x00,0x00,0x3E,0x00,0x00,
0x3C,0x00,0x00,0x7C,0x00,0x00,0x7F,0xFF,0xE0,0x7F,0xFF,0xE0,0x7F,0xFF,0xE0,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFC,
0x00,0x1F,0xFE,0x00,0x1F,0xFF,0x00,0x1E,0x1F,0x80,0x18,0x0F,0x80,0x00,0x07,0xC0,
0x00,0x07,0xC0,0x00,0x07,0xC0,0x00,0x07,0xC0,0x00,0x0F,0x80,0x00,0x1F,0x80,0x00,
0xFF,0x00,0x0F,0xFE,0x00,0x0F,0xFE,0x00,0x0F,0xFF,0x00,0x00,0x1F,0x80,0x00,0x0F,
0xC0,0x00,0x07,0xC0,0x00,0x03,0xE0,0x00,0x03,0xE0,0x00,0x03,0xE0,0x00,0x07,0xC0,
0x00,0x07,0xC0,0x38,0x0F,0xC0,0x3F,0xFF,0x80,0x3F,0xFF,0x00,0x3F,0xFE,0x00,0x07,
0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
0x00,0x00,0x1F,0x80,0x00,0x1F,0x80,0x00,0x3F,0x80,0x00,0x7F,0x80,0x00,0x7F,0x80,
0x00,0xFF,0x80,0x00,0xFF,0x80,0x01,0xFF,0x80,0x03,0xEF,0x80,0x03,0xCF,0x80,0x07,
0xCF,0x80,0x0F,0x8F,0x80,0x1F,0x0F,0x80,0x1F,0x0F,0x80,0x3E,0x0F,0x80,0x7C,0x0F,
0x80,0xF8,0x0F,0x80,0xFF,0xFF,0xF8,0xFF,0xFF,0xF8,0xFF,0xFF,0xF8,0x00,0x0F,0x80,
0x00,0x0F,0x80,0x00,0x0F,0x80,0x00,0x0F,0x80,0x00,0x0F,0x80,0x00,0x0F,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,
0x80,0x0F,0xFF,0x80,0x1F,0xFF,0x80,0x1F,0xFF,0x80,0x1F,0x00,0x00,0x1F,0x00,0x00,
0x1F,0x00,0x00,0x1F,0x00,0x00,0x1F,0x00,0x00,0x1E,0x00,0x00,0x1E,0xC0,0x00,0x1F,
0xFC,0x00,0x1F,0xFF,0x00,0x1F,0xFF,0x80,0x00,0x1F,0xC0,0x00,0x0F,0xC0,0x00,0x07,
0xC0,0x00,0x03,0xE0,0x00,0x03,0xE0,0x00,0x03,0xE0,0x00,0x03,0xE0,0x00,0x07,0xC0,
0x00,0x07,0xC0,0x38,0x1F,0xC0,0x3F,0xFF,0x80,0x3F,0xFF,0x00,0x3F,0xFE,0x00,0x07,
0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,
0xC0,0x01,0xFF,0xC0,0x03,0xFF,0xC0,0x07,0xE1,0xC0,0x0F,0xC0,0x00,0x1F,0x80,0x00,
0x1F,0x00,0x00,0x1E,0x00,0x00,0x3E,0x00,0x00,0x3E,0x00,0x00,0x3C,0x7E,0x00,0x3D,
0xFF,0x80,0x7F,0xFF,0xC0,0x7F,0xFF,0xC0,0x7F,0x07,0xE0,0x7E,0x03,0xE0,0x7E,0x01,
0xF0,0x7C,0x01,0xF0,0x7C,0x01,0xF0,0x3C,0x01,0xF0,0x3E,0x01,0xF0,0x3E,0x03,0xE0,
0x1F,0x03,0xE0,0x1F,0x87,0xE0,0x0F,0xFF,0xC0,0x07,0xFF,0x80,0x03,0xFF,0x00,0x00,
0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,
0xE0,0x7F,0xFF,0xE0,0x7F,0xFF,0xE0,0x7F,0xFF,0xE0,0x00,0x03,0xC0,0x00,0x07,0xC0,
0x00,0x07,0x80,0x00,0x0F,0x80,0x00,0x0F,0x00,0x00,0x1F,0x00,0x00,0x1E,0x00,0x00,
0x3E,0x00,0x00,0x3C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0xF8,0x00,0x00,0xF8,
0x00,0x00,0xF0,0x00,0x01,0xF0,0x00,0x01,0xF0,0x00,0x03,0xE0,0x00,0x03,0xE0,0x00,
0x03,0xE0,0x00,0x03,0xE0,0x00,0x07,0xC0,0x00,0x07,0xC0,0x00,0x07,0xC0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFC,
0x00,0x07,0xFF,0x00,0x0F,0xFF,0x80,0x1F,0x8F,0xC0,0x3F,0x07,0xC0,0x3E,0x03,0xC0,
0x3E,0x03,0xC0,0x3E,0x03,0xC0,0x3E,0x03,0xC0,0x1E,0x07,0xC0,0x1F,0x8F,0x80,0x0F,
0xFF,0x00,0x07,0xFE,0x00,0x0F,0xFF,0x80,0x1F,0xFF,0xC0,0x3F,0x07,0xC0,0x3E,0x03,
0xE0,0x7C,0x01,0xE0,0x7C,0x01,0xF0,0x7C,0x01,0xF0,0x7C,0x01,0xF0,0x7C,0x01,0xE0,
0x7E,0x03,0xE0,0x3F,0x07,0xE0,0x1F,0xFF,0xC0,0x0F,0xFF,0x80,0x07,0xFF,0x00,0x00,
0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFC,
0x00,0x0F,0xFF,0x00,0x1F,0xFF,0x80,0x1F,0x9F,0x80,0x3E,0x07,0xC0,0x3E,0x07,0xC0,
0x7C,0x03,0xE0,0x7C,0x03,0xE0,0x7C,0x03,0xE0,0x7C,0x03,0xE0,0x7C,0x03,0xE0,0x7C,
0x03,0xE0,0x3E,0x07,0xE0,0x3F,0x0F,0xE0,0x1F,0xFF,0xE0,0x0F,0xFF,0xE0,0x07,0xFF,
0xE0,0x00,0xE3,0xE0,0x00,0x03,0xE0,0x00,0x03,0xE0,0x00,0x07,0xC0,0x00,0x07,0xC0,
0x00,0x0F,0x80,0x30,0x1F,0x80,0x3F,0xFF,0x00,0x3F,0xFE,0x00,0x3F,0xF8,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

//显示单个字符（取模工具提供的方法）
//屏幕的长对应 x ，宽对应 y ，下同
void show_word(int x, int y, int w, int h, char word[][w/8], int color)
{
    int i, j, k;
    for( i=0; i<h; i++ ){
        for( j=0; j<w/8; j++ ){
            //解析 word[i][j] 的8个bits位，为1就显示颜色，为0就不显示 
            for( k=7; k>=0; k-- ){
                if( word[i][j] & (1<<k) ){
                    display_point( i+y, j*8+(7-k)+x, color );
                }
            }
        }
    }
}

//倒计时
void count_number(int x, int y, int number)
{
    while(number>=-1){
        int n = number;
        int offset = 0; //偏移量，每一位数字的间距
        //从低位到高位显示每一位的数字
        while(n){
            show_word(x-offset, y, 24, 46, num[n%10], 0x000000);
            offset += 20;
            n /= 10;
        }
        number--;
        sleep(1);
        show_picture(0xFFFFFF);
    }
}

//显示数字文本
void show_number(int x, int y, int number)
{
    int offset = 0;
    while(number>0){
        show_word(x-offset, y, 24, 46, num[number%10], 0xFF34D3);
        offset += 20;
        number /= 10;
    }
}