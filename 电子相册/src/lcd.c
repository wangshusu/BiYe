
#include "lcd.h"



int fb_fd = -1; //屏幕文件的文件描述符
int* plcd = NULL;//指向内核帧缓冲区的首地址

//
void lcd_open(void)
{	
    int fd = open("/dev/fb0", O_RDWR);
	if (fd == -1)
	{
		printf("failed to open /dev/fb0\n");
		return ;
	}
    fb_fd = fd;

    plcd = mmap(
                NULL,
                800*480*4,
                PROT_WRITE,
                MAP_SHARED,
                fd,
                0    );
    if (plcd == MAP_FAILED)
    {
        printf("Sorry,failed to mmap\n");
        return ;
    }
}

void lcd_close(void)
{
    munmap(plcd, 800*480*4);
    close(fb_fd);
}

/*
    lcd_draw_point: 画点(给特定的像素点一个指定的颜色值)
    @x: 点的x轴坐标
    @y: 点的y轴坐标
    @color: 颜色值
    返回值：
        无。
*/
void lcd_draw_point(int x, int y, int color)
{
    if (x >= 0 && x < 800 && y >= 0 && y < 480)
    {
        *(plcd + y*800 +x) = color;
    }
}



/*
    lcd_draw_point_base: 基础 粗暴刷屏 1
    返回值：
        无。
*/
void lcd_draw_point_form0(int colorbuf[WIDTH][HEIGHT])
{
    for(int i = 0 ; i < WIDTH ; i++)
    {
        for(int j = 0 ; j < HEIGHT ; j++)
        {
            *(plcd + j*WIDTH + i) = colorbuf[i][j];
        }
    }
}

/*
    lcd_draw_point_form1: 特效刷屏 百叶窗
    返回值：
        无。
*/
void lcd_draw_point_form1(int colorbuf[WIDTH][HEIGHT])
{
    lcd_clear_screen(0xffffff);
    int store = 10;//有十个起始点
    int nStep = HEIGHT/store;//每个百叶窗的高度

    for(int j = 0 ; j < nStep ; j++)
    {
        for(int k = 0 ; k < store ; k++)
        {
            for(int i = 0 ; i < WIDTH ; i++)
            {
                *(plcd + (k*nStep+j)*WIDTH + i) = colorbuf[i][(k*nStep+j)];
            }
        }
        usleep(1500);
    }

}

/*
    lcd_draw_point_form2: 特效刷屏 飞来积木
    返回值：
        无。
*/
void lcd_draw_point_form2(int colorbuf[WIDTH][HEIGHT])
{

    lcd_clear_screen(0xffffff);
    int blocks_high = 3;//积木高度
    int blocks_width = 5;//积木宽度
    int x = WIDTH/blocks_width;
    int y = HEIGHT/blocks_high;
    int showbuf[160][160] = {0};
    int shownum = 0;//记录已经显示的个数
                                            //printf("%s %d\n",__FUNCTION__,__LINE__);
    srand((unsigned int)time(NULL)); //这里是拿根据时间改变的数

    while(shownum != x * y)
    {
        
        int random_x = (unsigned int)(rand() % x);//随机x轴坐标
        int random_y = (unsigned int)(rand() % y);//随机y轴坐标
        

        while(showbuf[random_x][random_y] == 1)//提高效率算法
        {
            if(random_x + 1 < x)//如果这一行还没结束
            {
                random_x += 1;
                while(showbuf[random_x][random_y] == 1)
                {
                    if(random_y + 1 < y)//如果这一列还没结束
                    {
                        random_y+= 1;
                    }
                    else//如果结束了
                    {
                        break;
                    }
                }
                if(showbuf[random_x][random_y] == 0)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }

        // if(random_x == 9)
        // {
        //     printf("x = %d , y = %d \n",random_x,random_y);
        // }
        if(showbuf[random_x][random_y] == 0)
        {
            int i = 0, j = 0;
            for(i = 0 ; i < blocks_high ; i++)
            {
                for(j = 0 ; j < blocks_width ; j++)
                {
                    //*(plcd + j*800 + i) = colorbuf[i][j];
                    *(plcd + (random_y *blocks_high + i)*800 + random_x *blocks_width + j) = colorbuf[random_x *blocks_width + j][random_y *blocks_high + i];
                }
            }
            showbuf[random_x][random_y] = 1;
            shownum++;
        }
        if(shownum % 100 == 0)
        {
            usleep(100);
        }
    }
    
    
}

//"清屏": 给屏幕上每一个像素点，显示相同的颜色
void lcd_clear_screen(int color)
{
    int x, y;

    for (y = 0 ; y < 480; y++)
    {
        for (x = 0; x < 800; x++)
        {
            lcd_draw_point(x,y, color);
        }
    }
}

void lcd_draw_rectangle(int x0, int y0,
                        int w,  int h,
                        int color)
{
    //(x,y)为该矩形内的任意一个点的坐标
    //  x >= x0 &&  x < x0 + w
    //  y >= y0 &&  y < y0 + h
    int x,y;

    for (y = y0; y < y0 + h; y++)
    {
        for (x = x0; x < x0 + w; x++)
        {
            lcd_draw_point(x,y, color);
        }

    }
}

void lcd_draw_circle(int x0, int y0,
                     double r, int color)
{
    //(x,y)为圆内的任意一个点坐标
    // (x-x0)*(x-x0) + (y-y0)*(y-y0) <= r*r
    int x,y;

    for (y = 0; y < 480; y++)
    {
        for (x = 0; x < 800; x++)
        {
            if ((x-x0)*(x-x0) + (y-y0)*(y-y0) <= r*r)
            {
                lcd_draw_point(x,y,color);
            }
        }
    }

}