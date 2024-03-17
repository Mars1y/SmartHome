#include <sys/types.h>
#include <termios.h> 
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include "lcd.h"
#include "touch.h"
#include "word.h"
#include "bmp.h"
#include "uart.h"
#include <sys/types.h>      
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>

#define COM2 "/dev/ttySAC1"
#define COM3 "/dev/ttySAC2"
#define COM4 "/dev/ttySAC3"

int state_mp3 = 0;
int state_led = 0;
int state_curtain = 0;

int init_serial(const char *file, int baudrate)
{ 
    int fd;

    fd = open(file, O_RDWR);
    if (fd == -1)
    {
        perror("open device error:");
        exit(-1);
    }

    struct termios myserial;
    //清空结构体
    memset(&myserial, 0, sizeof (myserial));
    //O_RDWR               
    myserial.c_cflag |= (CLOCAL | CREAD);
    //设置控制模式状态，本地连接，接受使能
    //设置 数据位
    myserial.c_cflag &= ~CSIZE;   //清空数据位
    myserial.c_cflag &= ~CRTSCTS; //无硬件流控制
    myserial.c_cflag |= CS8;      //数据位:8

    myserial.c_cflag &= ~CSTOPB;//   //1位停止位
    myserial.c_cflag &= ~PARENB;  //不要校验
    //myserial.c_iflag |= IGNPAR;   //不要校验
    //myserial.c_oflag = 0;  //输入模式
    //myserial.c_lflag = 0;  //不激活终端模式

    switch (baudrate)
    {
        case 9600:
            cfsetospeed(&myserial, B9600);  //设置波特率
            cfsetispeed(&myserial, B9600);
            break;
        case 115200:
            cfsetospeed(&myserial, B115200);  //设置波特率
            cfsetispeed(&myserial, B115200);
            break;
        case 19200:
            cfsetospeed(&myserial, B19200);  //设置波特率
            cfsetispeed(&myserial, B19200);
            break;
    }

    /* 刷新输出队列,清除正接受的数据 */
    tcflush(fd, TCIFLUSH);

    /* 改变配置 */
    tcsetattr(fd, TCSANOW, &myserial);

    return fd;
}

//解析数据
void parse_date(unsigned char buf[], int n)
{
    int t, p, hum, h, lux;
    if(buf[2]==0x15&&n==9){
        lux = (buf[4]<<24)|(buf[5]<<16)|(buf[6]<<8)|buf[7];
        lux /= 100;
        //printf("lux: %d\n", lux);
        //画白色矩形，擦除上一次获取的数据
        display_rectangle(390, 450, 660, 720, 0xFFFFFF); //光强
        show_number(700, 400, lux);
        if(lux<50){
            state_led = 0;
            led_beep_ctrl('l');
            //提示已开灯
            draw_picture(135, 260, "../bmp/window.bmp");
        }
        else if(lux>=50){
            state_led = 1;
            led_beep_ctrl('l');
            //提示已关灯
            draw_picture(135, 260, "../bmp/window2.bmp");
        }
    }
    else if(buf[2]==0x45&&n==15){
        t = (buf[4]<<8)|buf[5];
        t /= 100;
        //printf("t: %d\n", t);
        p = (buf[6]<<24) | (buf[7]<<16) | (buf[8]<<8) | buf[9];
        p /= 100000;
        //printf("p: %d\n", p);
        hum = (buf[10]<<8)|buf[11];
        hum /= 100;
        //printf("hum: %d\n", hum);
        h = (buf[12]<<8)|buf[13];
        //printf("h: %d\n", h);
        //画白色矩形，擦除上一次获取的数据
        display_rectangle(390, 450, 140, 222, 0xFFFFFF); //气压
        display_rectangle(280, 355, 155, 222, 0xFFFFFF); //温度
        display_rectangle(280, 355, 420, 480, 0xFFFFFF); //湿度
        display_rectangle(390, 450, 420, 480, 0xFFFFFF); //海拔
        show_number(200, 310, t);
        show_number(200, 400, p);
        show_number(455, 310, hum);
        show_number(455, 400, h);
        if(t>30){
            state_curtain = 0;
            led_beep_ctrl('c');
        }
        else if(t<=30){
            state_curtain = 1;
            led_beep_ctrl('c');
        }
    }
    sleep(1);  //每10秒读一次
}

//获取GY39传感器数据 
void *get_gy39(void *arg)
{
    //1.打开串口并初始化 
    int fd = init_serial( COM2, 9600 );
    if( fd == -1 )
    {
        perror("open device error:");
        exit(-1);
    }

    //2.发送指令 
    unsigned char cmd[3] = { 0xA5, 0x83, 0x28 };
    write( fd, cmd, 3 );

    unsigned char buf[15] = {0};
    unsigned char ch;

    while( 1 )
    {
        //3.接收数据
        //先读取第一个0x5A 
        do 
        {
            read( fd, &ch, 1 );
        }while( ch != 0x5A );

        //读取第二个0x5A 
        read( fd, &ch, 1 );
        if( ch != 0x5A )
        {
            continue;
        }

        int i = 0;
        buf[0] = 0x5A;
        buf[1] = 0x5A;

        //读取 “数据类型”
        read( fd, &buf[2], 1 );

        //读取 “数据长度”
        read( fd, &buf[3], 1 );

        //读取 传感器数据内容
        int len = buf[3];
        for( i=0; i<len; i++ )
        {
            read( fd, &buf[4+i], 1 );
        }

        //读取 校验和 
        read( fd, &buf[ 4+len ], 1 );

        //4.解析数据，并显示 
        //根据手册解析buf数组里面的数据 
        parse_date(buf, 4+len+1);
    }

    //5.关闭串口 
    if(close(fd)==-1){
        perror("close device error:");
        exit(-1);
    }
}

void led_beep_ctrl(char device)
{
    int fd = -1;
    if(device=='l'){
        state_led++;
        fd = open("/sys/kernel/gec_ctrl/led_all", O_RDWR);
        if(fd==-1){
            perror("led_all open error");
            exit(-1);
        }
        state_led %= 2;
        write(fd, &state_led, sizeof(state_led));
        if(state_led==1){
            draw_picture(30, 675, "../bmp/on.bmp");
        }
        else if(state_led==0){
            draw_picture(30, 675, "../bmp/off.bmp");
        }
        printf("led:%d\n", state_led);
    }
    else if(device=='c'){
        state_curtain++;
        fd = open("/sys/kernel/gec_ctrl/beep", O_RDWR);
        if(fd==-1){
            perror("beep open error");
            exit(-1);
        }
        state_curtain %= 2;
        write(fd, &state_curtain, sizeof(state_curtain));
        if(state_curtain==1){
            draw_picture(110, 675, "../bmp/on.bmp");
        }
        else if(state_curtain==0){
            draw_picture(110, 675, "../bmp/off.bmp");
        }
        printf("beep:%d\n", state_curtain);
    }
    if(close(fd)==-1){
        perror("close device error");
        exit(-1);
    }
}

void mp3_ctl(char * filename) 
{
	char command[1024] = {0};
	if (state_mp3==0){
		sprintf(command, "madplay -Q %s &", filename);
        draw_picture(190, 675, "../bmp/on.bmp");
		state_mp3 = 1;
	} 
    else if(state_mp3==1){
        printf("close audio\n");
		sprintf(command, "killall madplay", filename);
        draw_picture(190, 675, "../bmp/off.bmp");
		state_mp3 = 0;
	} 
	system(command); 
}