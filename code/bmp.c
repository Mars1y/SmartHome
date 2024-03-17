#include "bmp.h"
#include "lcd.h"
#include <sys/types.h>      
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//屏幕的宽对应 x ，长对应 y
void draw_picture(int x, int y, char *filename)
{
    /*打开bmp文件*/
    int fd = -1;
    fd = open(filename, O_RDWR);
    if(fd==-1){
        perror("bmp file open error");
        exit(-1);
    }
    /*获取bmp图片的数据*/
    int width, height;
    short depth;
    if(lseek(fd, 0x12, SEEK_SET)==-1){
        perror("lseek error");
        exit(-1);
    }
    if(read(fd, &width, 4)==-1){
        perror("read function error");
        exit(-1);
    }
    if(lseek(fd, 0x16, SEEK_SET)==-1){
        perror("lseek error");
        exit(-1);
    }
    if(read(fd, &height, 4)==-1){
        perror("read function error");
        exit(-1);
    }
    if(lseek(fd, 0x1C, SEEK_SET)==-1){
        perror("lseek error");
        exit(-1);
    }
    if(read(fd, &depth, 2)==-1){
        perror("read function error");
        exit(-1);
    }
    int laizi = 0;  //赖子
    int line_size = 0;  //每行实际字节数
    laizi = 4-(abs(width)*(depth/8))%4;
    if(laizi==4){
        laizi = 0;
    }
    //每行实际的字节数 = 有效字节数 + 赖子 
    line_size = abs(width)*(depth/8)+laizi;
    char buf[abs(height)*line_size];   
    if(lseek(fd, 0x36, SEEK_SET)==-1){
        perror("lseek error");
        exit(-1);
    }
    if(read(fd, buf, abs(height)*line_size)==-1){
        perror("read function error");
        exit(-1);
    }
    /*在屏幕上显示图片*/
    int color;
    int b, g, r;
    short a = 0;
    int i, j;
    int num = 0;
    for(i=0; i<abs(height); i++){
        for(j=0; j<abs(width); j++){
            //合成颜色数据
            b = buf[num++];
            g = buf[num++];
            r = buf[num++];
            if(depth==32){
                a = buf[num++];
            }
            color = (a<<24)|(r<<16)|(g<<8)|b;
            display_point(height>0 ? x+abs(height)-1-i : x+i, width>0 ? y+j : y+abs(width)-1-j, color);
        }
        num = num+laizi;  //跳过无效的数据
    }
    /*关闭bmp文件*/
    if(close(fd)==-1){
        perror("close bmp file error");
        exit(-1);
    }
}