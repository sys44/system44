/* system44 - tty.c */
/* date: 29-09-25 */

#include "tty.h"
#include "vga.h"

static unsigned int l_mode = 0;

int l_init(tty_dev *td){
	td->l = 1;
	td->input_pos = 0;
	td->output_pos = 0;
	for(int i = 0; i < MAX; i++){
		td->input_buf[i] = 0;
		td->output_buf[i] = 0;
	}

	return 0;
}

int l_set_raw(){
	l_mode = 1;
	return 0;
}

int l_restore(){
	l_mode = 0;
	return 0;
}

int l_write(tty_dev *td, const char *c, size_t len){
	unsigned int rw = 0;
	while(rw < len && td->output_pos < MAX){
		char h = c[rw];
		td->output_buf[td->output_pos] = h;
		eputchar(h);
		td->output_pos++;
		rw++;
	}

	return rw;
}

int l_read(tty_dev *td){
	if(td->input_pos == 0) return 1;
	char dt = td->input_buf[0];
	for(int i = 0; i < td->input_pos - 1; i++){
		td->input_buf[i] = td->input_buf[i + 1];
	}

	td->input_pos--;

	return dt;
}

int l_wait(tty_dev *td){
	while(td->input_pos == 0){
		__asm__ volatile("hlt");
	}

	return 0;
}

void ob(unsigned short port, unsigned char l_val){
	__asm__ volatile("outb %0, %1" :: "a"(l_val), "d"(port));
}


void l_close(tty_dev *td){
	td->l = -1;
}

void l_clear(){
	for(int i = 0; i < 80 * 25; i++){
		VGA_BUF[i] = (0x07 << 8) | 0x20;
	}
}

void l_set_cur_p(int r, int d){
	unsigned short pos = r * 80 + d;
	ob(P_0, 0x0F);
	ob(P_1, pos & 0xFF);
	ob(P_0, 0x0E);
	ob(P_1, (pos >> 8) & 0xFF);
}

void l_hide_cur(){
	ob(P_0, 0x0A);
	ob(P_1, 0x20);
}

void l_show_cur(){
	ob(P_0, 0x0A);
	ob(P_1, 0x0E);
	ob(P_0, 0x0B);
	ob(P_1, 0x0F);
}
