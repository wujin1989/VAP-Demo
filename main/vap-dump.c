#include "vap-dump.h"
#include <stdio.h>

FILE* outfile;
FILE* outfile2;

void vap_dump_enable(void) {
	if (!outfile) {
		outfile = fopen(OUTPUT_YUV, "ab+");
		if (!outfile) {
			return;
		}
	}
	if (!outfile2) {
		outfile2 = fopen(OUTPUT_YUV2, "ab+");
		if (!outfile2) {
			return;
		}
	}
}

void vap_dump_yuv420p(uint8_t* y, uint8_t* u, uint8_t* v, int width, int height) {
	fwrite(y, 1, width * height, outfile);
	fwrite(u, 1, width * height / 4, outfile);
	fwrite(v, 1, width * height / 4, outfile);
}

void vap_dump_yuv420p2(uint8_t* yuv, int width, int height) {
	fwrite(yuv, 1, width * height * 3 / 2, outfile2);
}

void vap_dump_disable(void) {
	fclose(outfile);
	outfile = NULL;
	fclose(outfile2);
	outfile2 = NULL;
}