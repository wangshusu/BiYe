
#ifndef __LCD_H__
#define __LCD_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>

#define HEIGHT 480
#define WIDTH 800

/*
    打开屏幕函数
*/
void lcd_open(void);

/*
    关闭屏幕函数
*/
void lcd_close(void);

/*
    lcd_draw_point: 画点(给特定的像素点一个指定的颜色值)
    @x: 点的x轴坐标
    @y: 点的y轴坐标
    @color: 颜色值
    返回值：
        无。
*/
void lcd_draw_point(int x, int y, int color);

/*
    清屏:每一个像素点，显示相同的颜色
    @int color:颜色
*/
void lcd_clear_screen(int color);

/*
    lcd_draw_point_base: 基础 粗暴刷屏 1
    返回值：
        无。
*/
void lcd_draw_point_form0(int colorbuf[WIDTH][HEIGHT]);

/*
    lcd_draw_point_form1: 特效刷屏 百叶窗
    返回值：
        无。
*/
void lcd_draw_point_form1(int colorbuf[WIDTH][HEIGHT]);

/*
    lcd_draw_point_form2: 特效刷屏 飞来积木
    返回值：
        无。
*/
void lcd_draw_point_form2(int colorbuf[WIDTH][HEIGHT]);


void lcd_draw_rectangle(int x0, int y0,
                        int w,  int h,
                        int color);

void lcd_draw_circle(int x0, int y0,
                     double r, int color);
#endif