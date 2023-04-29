#include "bmp.h"
#include "lcd.h"

/*
    显示bmp图片函数
    @char *bmp_file:路径
    @int x0, int y0 : 在哪个点显示图片
    无返回值
*/
void display_bmp(char *bmp_file, int x0, int y0, int cmd)
{
    int fd = open(bmp_file, O_RDONLY);
    if (fd == -1)
    {
        printf("failed to open %s\n", bmp_file);
        return;
    }

    int width, height;
    short depth;

    lseek(fd, 0x12, SEEK_SET);
    read(fd, &width, 4);

    lseek(fd, 0x16, SEEK_SET);
    read(fd, &height, 4);

    printf("%d X %d\n", width, height);

    lseek(fd, 0x1c, SEEK_SET);
    read(fd, &depth, 2);
    
    if (  !(depth == 24 || depth == 32) )
    {
        printf("Sorry, Not Supported\n");
        close(fd);
        return ;
    }

    //像素数组保存了每个像素点的颜色值
    //  abs(width) * abs(height) * (depth/8)

    // 一行实际字节数:  width * (depth/8)
    // 一行的末尾可能填充几个 “赖子 laizi”
    int laizi = 0; //一行末尾填充的“赖子”数
    int line_valid_bytes = abs(width) * (depth/8); //一行有效的字节数

    if ( line_valid_bytes % 4 )
    {
        laizi =  4 - line_valid_bytes % 4;
    }

    int line_total_bytes; //一行总共字节数
    line_total_bytes = line_valid_bytes + laizi;

    int total_bytes; //整个像素数组的字节数
    total_bytes = line_total_bytes * abs(height);

    char *pixel = malloc( total_bytes );
    lseek(fd, 54, SEEK_SET);
    read(fd, pixel, total_bytes);

    unsigned char b,g,r,a;
    int color;
    int i = 0;
    int x,y;

    int colorbuf[800][480] = {0};

    for (y = 0; y < abs(height); y++)
    {
        for (x = 0; x < abs(width); x++)
        {
            b = pixel[i++];
            g = pixel[i++];
            r = pixel[i++];
            if (depth == 32)
            {
                a = pixel[i++];
            }
            else
            {
                a = 0;
            }
            color = (a << 24) | (r << 16) | (g << 8) | (b);
            if((x + x0) < 800 && (y0 + abs(height) -1 - y) < 480)//防止越界
            {
                colorbuf[x + x0][y0 + abs(height) -1 - y] = color;
                //lcd_draw_point(x0 + x, y0 + abs(height) -1 - y ,  color);
            } 
        }
        i += laizi;
    }

    switch (cmd)
    {
    case 0:
        lcd_draw_point_form0(colorbuf);
        break;
    case 1:
        lcd_draw_point_form1(colorbuf);
        break;
    case 2:
        lcd_draw_point_form2(colorbuf);
        break;
    default:
        break;
    }
    
    free(pixel);
}