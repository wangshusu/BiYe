
#include "showphoto.h"

void showphoto(char *photopath,int x,int y,int cmd)
{
	lcd_open();

	display_bmp(photopath,x,y,cmd);

	lcd_close();
}


