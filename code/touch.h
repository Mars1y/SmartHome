#ifndef __TOUCH_H__
#define __TOUCH_H__

//点按坐标结构体
typedef struct pear
{
    int x;
    int y;
} pear;

//获取点按坐标
pear get_touch();
//获取手指滑动的方向
int get_direction();
//实现模拟手机相册滑动切图的功能
void electric_album();

#endif