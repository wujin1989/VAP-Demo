_Pragma("once");

#include <stdio.h>
#include <stdint.h>
#include <stdalign.h>
#include "vap-queue.h"

typedef struct vap_fake_decoder_s {
	FILE* file;
	int width;
	int height;
} vap_fake_decoder_t;

typedef struct vap_frame_s {
	vap_queue_node_t node;
	int width;
	int height;
	int format;
	uint8_t data[];
} vap_frame_t;

extern void vap_decoder_create(vap_fake_decoder_t* decoder, int width, int height);
extern void vap_decoder_decode_frame(vap_fake_decoder_t* decoder, vap_frame_t* yuv, bool* eos);
extern void vap_decoder_destroy(vap_fake_decoder_t* decoder);