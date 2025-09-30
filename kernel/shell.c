/* system44 - shell.c */
/* date: 29-09-25 */

#include "../drivers/vga.h"
#include "../drivers/tty.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"
#include "../lib/memory.h"

void sh(tty_dev *td){
	char c[128];
	int i = 0;
	for(;;){
		l_write(td, "# ", 2);
		memset(c, 0, 128);
		for(;;){
			char cm = get_key();
			c[i++] = cm;
			l_write(td, &cm, 1);
			if(cm == '\n' || i >= 127) break;
		}
	}
}
