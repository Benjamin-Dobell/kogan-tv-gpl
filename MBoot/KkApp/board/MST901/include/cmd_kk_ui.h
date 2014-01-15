#ifndef _CMD_KK_UI_H
#define _CMD_KK_UI_H

#define KK_UI_DBG(x)             x


int kk_ui_create_osdWin(void);
int kk_ui_draw_rect(unsigned short dispX, unsigned short dispY, unsigned short dispWidth, unsigned short dispHeiht, unsigned int color);
int kk_ui_draw_progress(unsigned short dispX, unsigned short dispY, unsigned short per, unsigned int color);
int kk_ui_draw_string(unsigned short dispX, unsigned short dispY, unsigned int color, unsigned short attrib, char* string);
int kk_ui_draw_jpg(unsigned int  jpgOffset, unsigned int  jpgSize, char* jpgCrc, unsigned short dispX, unsigned short dispY, unsigned short dispWidth,unsigned short  dispHeiht,unsigned char  bHorStretch, unsigned char  bVerStretch) ; //
int kk_ui_flush_osd(void);
int kk_ui_destory_osdWin(void);
#endif

