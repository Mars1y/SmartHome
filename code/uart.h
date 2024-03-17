#ifndef __UART_H__
#define __UART_H__

//串口初始化
int init_serial(const char *file, int baudrate);
//获取传感器数据
//测试用的函数名
//void get_gy39(); 
//由于需要在主函数创建线程，实现在后台实时获取传感器数据，因此改为适配线程函数的格式
void *get_gy39(void *arg);
//解析传感器数据
void parse_date(unsigned char buf[], int n);
//控制家居：温度报警 智能亮灯
void led_beep_ctrl(char device);
//手动开关BGM
void mp3_ctl(char * filename);

#endif