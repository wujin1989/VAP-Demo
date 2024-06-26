#include "vap-decoder.h"
#include <string.h>
#include <stdlib.h>

#define VIDEO_PATH "video.yuv"

void vap_decoder_create(vap_fake_decoder_t* decoder, int width, int height) {
	decoder->file   = fopen(VIDEO_PATH, "rb");
	decoder->width  = width;
	decoder->height = height;
}

void vap_decoder_decode_frame(vap_fake_decoder_t* decoder, vap_frame_t* yuv, bool* eos) {
	yuv->format = 1;
	yuv->width  = decoder->width;
	yuv->height = decoder->height;
	
	memset(yuv->data, 0, (yuv->width * yuv->height * 3) / 2);
	size_t bytes_read = fread(yuv->data, 1, (yuv->width * yuv->height * 3) / 2, decoder->file);
	if (feof(decoder->file)) {
		*eos = true;
	}
}

void vap_decoder_destroy(vap_fake_decoder_t* decoder) {
	if (!decoder->file) {
		return;
	}
	fclose(decoder->file);
	decoder->width = 0;
	decoder->height = 0;
}