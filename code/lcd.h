#ifndef __LCD_H__
#define __LCD_H__

//打开LCD
void lcd_init();
//关闭LCD
void lcd_close();
//在屏幕上画一个点
void display_point(int x, int y, int color);
//在屏幕上铺满纯色
void show_picture(int color);
//在屏幕上画一个纯色矩形
void display_rectangle(int x1, int x2, int y1, int y2, int color);
//在屏幕上画一个纯色圆形
void draw_circle(int a, int b, int r, int color);

#endif