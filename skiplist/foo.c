#include <stdio.h>

int main(int argc, char *argv[]) {
	int h, i;

	for(i = 0; i < 100; i++) {
		char buf[256];
		strcpy(buf, "./tmp-file-XXXXXX");
		h = mkstemp(buf);
		sync();
		if(h > 0) {
			write(h, &h, 1);
			sync();
			close(h);
			sync();
		}
	}
}

