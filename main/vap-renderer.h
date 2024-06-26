_Pragma("once");

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "vap-decoder.h"

typedef struct vap_rect_s {
    int x;
    int y;
    int width;
    int height;
}vap_rect_t;

typedef struct vap_anim_cfg_s {
    int actual_width;
    int actual_height;
    int desired_width;
    int desired_height;
    int framerate;
    vap_rect_t alpha_rect;
    vap_rect_t rgb_rect;
} vap_anim_cfg_t;

typedef struct vap_renderer_s {
	unsigned int shader_program;
    int av_position;
    int rgb_position;
    int alpha_position;
    int sampler_y;
    int sampler_u;
    int sampler_v;
    int convert_mat_unifrom;
    int convert_offset_uniform;
    float YUV_OFFSET[3];
    float YUV_MATRIX[9];
    unsigned int texture_ids[3];
    uint8_t* y;
    uint8_t* u;
    uint8_t* v;
    float vertices[24];
    unsigned int VAO, VBO;
} vap_renderer_t;

extern void vap_renderer_create(vap_renderer_t* renderer, vap_anim_cfg_t* cfg);
extern void vap_renderer_render_frame(vap_renderer_t* renderer, vap_frame_t* yuv);
extern void vap_renderer_destroy(vap_renderer_t* renderer);