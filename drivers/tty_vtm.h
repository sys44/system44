#ifndef TTY_H
#define TTY_H

#define BUF 256
#define MAX 128

#define VGA_BUF ((unsigned short *)0xb8000) /* mm io addr */

#define P_0 0x3D4 /* vga c-r */
#define P_1 0x3D5 /* vga d-r */

typedef unsigned int size_t;

typedef struct{
	int l;
	char input_buf[MAX];
	char output_buf[MAX];
	int input_pos;
	int output_pos;
} tty_dev;

int l_init(tty_dev *td);
int l_set_raw();
int l_restore();
int l_write(tty_dev *td, const char *c, size_t len);
int l_read(tty_dev *td);
int l_wait(tty_dev *td);

void ob(unsigned short port, unsigned char l_val);
void l_close(tty_dev *td);
void l_clear();
void l_set_cur_p(int r, int d);
void l_hide_cur();
void l_show_cur();

#endif
