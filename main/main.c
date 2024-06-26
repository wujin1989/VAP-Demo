#include "vap-window.h"
#include "vap-decoder.h"
#include "vap-renderer.h"
#include "vap-queue.h"
#include "vap-dump.h"
#include <threads.h>
#include <string.h>
#include <stdlib.h>

vap_anim_cfg_t anim_cfg = { 736, 1168, 736, 576, 25, {0,580,736,576}, {0,0,736,576} };

vap_queue_t dataqueue;
mtx_t dq_mtx;
cnd_t dq_cnd;

static int decode_routine(void* param) {
	vap_fake_decoder_t decoder;
	vap_decoder_create(&decoder, anim_cfg.actual_width, anim_cfg.actual_height);
	int framecnt = 0;
	bool eos = false;
	while (true) {
		vap_frame_t* p_yuv = malloc(sizeof(vap_frame_t) + decoder.width * decoder.height * 3 / 2);
		if (!p_yuv) {
			abort();
		}
		vap_decoder_decode_frame(&decoder, p_yuv, &eos);
		if (eos) {
			free(p_yuv);
			p_yuv = NULL;
			break;
		}
		framecnt++;
#ifdef DEBUG
		vap_dump_yuv420p2(p_yuv->data, p_yuv->width, p_yuv->height);
#endif
		mtx_lock(&dq_mtx);
		vap_queue_enqueue(&dataqueue, &p_yuv->node);
		cnd_signal(&dq_cnd);
		mtx_unlock(&dq_mtx);
	}
	printf("decode %d frames\n", framecnt);
	vap_decoder_destroy(&decoder);
	return 0;
}

static void vap_decode_thread_create(void) {
	thrd_t tid;
	thrd_create(&tid, decode_routine, NULL);
	thrd_detach(tid);
}

int main(void) {
	GLFWwindow* window = vap_window_create(anim_cfg.desired_width, anim_cfg.desired_height);

	vap_queue_init(&dataqueue);
	mtx_init(&dq_mtx, mtx_plain);
	cnd_init(&dq_cnd);
#ifdef DEBUG
	vap_dump_enable();
#endif
	vap_renderer_t renderer;
	vap_renderer_create(&renderer, &anim_cfg);

	vap_decode_thread_create();

	while (!glfwWindowShouldClose(window)) {
		mtx_lock(&dq_mtx);
		while (vap_queue_empty(&dataqueue)) {
			cnd_wait(&dq_cnd, &dq_mtx);
		}
		vap_queue_node_t* n = vap_queue_dequeue(&dataqueue);
		if (n) {
			vap_frame_t* yuvframe = vap_queue_data(n, vap_frame_t, node);
			vap_renderer_render_frame(&renderer, yuvframe);
			free(yuvframe); 
			yuvframe = NULL;
		} else {
			mtx_unlock(&dq_mtx);
			break;
		}
		mtx_unlock(&dq_mtx);

		glfwSwapBuffers(window);
		glfwPollEvents();

		thrd_sleep(&(struct timespec) { .tv_sec = 0, .tv_nsec = 40000000 }, NULL);
	}
	vap_window_destroy();

	vap_renderer_destroy(&renderer);
	mtx_destroy(&dq_mtx);
	cnd_destroy(&dq_cnd);
#ifdef DEBUG
	vap_dump_disable();
#endif
	return 0;
}