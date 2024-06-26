#include "vap-renderer.h"
#include "vap-decoder.h"
#include "vap-dump.h"
#include <stdlib.h>

const char* VERTEX_SHADER = "						 \
	attribute vec2 v_Position;                       \
	attribute vec2 vTexCoordinateAlpha;              \
	attribute vec2 vTexCoordinateRgb;                \
	varying vec2 v_TexCoordinateAlpha;               \
	varying vec2 v_TexCoordinateRgb;                 \
													 \
	void main() {                                    \
		v_TexCoordinateAlpha = vTexCoordinateAlpha;  \
		v_TexCoordinateRgb = vTexCoordinateRgb;      \
		gl_Position = vec4(v_Position, 0.0, 1.0);    \
	}                                                \
	";


const char* FRAGMENT_SHADER = "                                             \
	precision mediump float;                                                \
	uniform sampler2D sampler_y;                                            \
	uniform sampler2D sampler_u;                                            \
	uniform sampler2D sampler_v;                                            \
	varying vec2 v_TexCoordinateAlpha;                                      \
	varying vec2 v_TexCoordinateRgb;                                        \
	uniform mat3 convertMatrix;                                             \
	uniform vec3 offset;                                                    \
                                                                            \
	void main() {                                                           \
		highp vec3 yuvColorAlpha;                                           \
		highp vec3 yuvColorRGB;                                             \
		highp vec3 rgbColorAlpha;                                           \
		highp vec3 rgbColorRGB;                                             \
		yuvColorAlpha.x = texture2D(sampler_y,v_TexCoordinateAlpha).r;      \
		yuvColorRGB.x = texture2D(sampler_y,v_TexCoordinateRgb).r;          \
		yuvColorAlpha.y = texture2D(sampler_u,v_TexCoordinateAlpha).r;      \
		yuvColorAlpha.z = texture2D(sampler_v,v_TexCoordinateAlpha).r;      \
		yuvColorRGB.y = texture2D(sampler_u,v_TexCoordinateRgb).r;          \
		yuvColorRGB.z = texture2D(sampler_v,v_TexCoordinateRgb).r;          \
		yuvColorAlpha += offset;                                            \
		yuvColorRGB += offset;                                              \
		rgbColorAlpha = convertMatrix * yuvColorAlpha;                      \
		rgbColorRGB = convertMatrix * yuvColorRGB;                          \
		gl_FragColor=vec4(rgbColorRGB, rgbColorAlpha.r);                    \
	}                                                                       \
	";

static unsigned int vap_renderer_create_program(void) {
	int  success;
	char info[512];

	unsigned int vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &VERTEX_SHADER, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
	}
	unsigned int fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &FRAGMENT_SHADER, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
	}
	unsigned int shader_program;
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info);
		printf("ERROR::SHADER::PROGRAM::LINK_FAILED\n");
	}
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

static float vap_renderer_switch_x(float x) {
	return x * 2.0F - 1.0F;
}

static float vap_renderer_switch_y(float y) {
	return ((y * 2.0F - 2.0F) * -1.0F) - 1.0F;
}

static void vap_renderer_vertex_convert(int width, int height, vap_rect_t* rect, float* array) {
	array[0] = vap_renderer_switch_x((float)(rect->x) / width);
	array[1] = vap_renderer_switch_y((float)(rect->y) / height);

	array[2] = vap_renderer_switch_x((float)(rect->x) / width);
	array[3] = vap_renderer_switch_y(((float)(rect->y) + rect->height) / height);

	array[4] = vap_renderer_switch_x(((float)(rect->x) + rect->width) / width);
	array[5] = vap_renderer_switch_y((float)(rect->y) / height);

	array[6] = vap_renderer_switch_x(((float)(rect->x) + rect->width) / width);
	array[7] = vap_renderer_switch_y(((float)(rect->y) + rect->height) / height);
}

static void vap_renderer_texcoords_convert(int width, int height, vap_rect_t* rect, float* array) {
	array[0] = (float)rect->x / width;
	array[1] = (float)rect->y / height;

	array[2] = (float)rect->x / width;
	array[3] = ((float)rect->y + rect->height) / height;

	array[4] = ((float)rect->x + rect->width) / width;
	array[5] = (float)rect->y / height;

	array[6] = ((float)rect->x + rect->width) / width;
	array[7] = ((float)rect->y + rect->height) / height;
}

void vap_renderer_create(vap_renderer_t* renderer, vap_anim_cfg_t* cfg) {
	renderer->shader_program = vap_renderer_create_program();
	renderer->av_position = glGetAttribLocation(renderer->shader_program, "v_Position");
	renderer->rgb_position = glGetAttribLocation(renderer->shader_program, "vTexCoordinateRgb");
	renderer->alpha_position = glGetAttribLocation(renderer->shader_program, "vTexCoordinateAlpha");
	renderer->sampler_y = glGetUniformLocation(renderer->shader_program, "sampler_y");
	renderer->sampler_u = glGetUniformLocation(renderer->shader_program, "sampler_u");
	renderer->sampler_v = glGetUniformLocation(renderer->shader_program, "sampler_v");
	renderer->convert_mat_unifrom = glGetUniformLocation(renderer->shader_program, "convertMatrix");
	renderer->convert_offset_uniform = glGetUniformLocation(renderer->shader_program, "offset");

	renderer->YUV_OFFSET[0] = 0.0F;
	renderer->YUV_OFFSET[1] = -0.501960814F;
	renderer->YUV_OFFSET[2] = -0.501960814F;

	renderer->YUV_MATRIX[0] = 1.0F;
	renderer->YUV_MATRIX[1] = 1.0F;
	renderer->YUV_MATRIX[2] = 1.0F;
	renderer->YUV_MATRIX[3] = 0.0F;
	renderer->YUV_MATRIX[4] = -0.3441F;
	renderer->YUV_MATRIX[5] = 1.772F;
	renderer->YUV_MATRIX[6] = 1.402F;
	renderer->YUV_MATRIX[7] = -0.7141F;
	renderer->YUV_MATRIX[8] = 0.0F;

	float VERTICES[24] = {
		-1.0, 1.0,		0.0, 0.496,		0.0, 0.0,                //左上
		-1.0, -1.0,     0.0, 0.989,		0.0, 0.493,				 //左下
		1.0, 1.0,       1.0, 0.496,		1.0, 0.0,				 //右上
		1.0, -1.0,      1.0, 0.989,		1.0, 0.493,				 //右下
	};
	memcpy(renderer->vertices, VERTICES, sizeof(VERTICES));

	glGenVertexArrays(1, &renderer->VAO);
	glGenBuffers(1, &renderer->VBO);

	glBindVertexArray(renderer->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(renderer->vertices), renderer->vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(renderer->av_position, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(renderer->av_position);

	glVertexAttribPointer(renderer->alpha_position, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(renderer->alpha_position);

	glVertexAttribPointer(renderer->rgb_position, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(renderer->rgb_position);

	glGenTextures(sizeof(renderer->texture_ids) / sizeof(renderer->texture_ids[0]), renderer->texture_ids);

	for (int i = 0; i < (sizeof(renderer->texture_ids) / sizeof(renderer->texture_ids[0])); i++) {
		glBindTexture(GL_TEXTURE_2D, renderer->texture_ids[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

void vap_renderer_render_frame(vap_renderer_t* renderer, vap_frame_t* yuv) {
	glClearColor(0.90f, 0.85f, 0.70f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	renderer->y = (uint8_t*)malloc(yuv->width * yuv->height);
	renderer->u = (uint8_t*)malloc(yuv->width * yuv->height / 4);
	renderer->v = (uint8_t*)malloc(yuv->width * yuv->height / 4);

	if (!renderer->y || !renderer->u || !renderer->v) {
		return;
	}
	for (int i = 0; i < yuv->height; i++) {
		for (int j = 0; j < yuv->width; j++) {
			renderer->y[i * yuv->width + j] = yuv->data[i * yuv->width + j];
		}
	}
	for (int i = 0; i < yuv->height / 2; i++) {
		for (int j = 0; j < yuv->width / 2; j++) {
			renderer->u[i * yuv->width / 2 + j] = yuv->data[yuv->width * yuv->height + i * yuv->width / 2 + j];
			renderer->v[i * yuv->width / 2 + j] = yuv->data[yuv->width * yuv->height * 5 / 4 + i * yuv->width / 2 + j];
		}
	}
#ifdef DEBUG
	vap_dump_yuv420p(renderer->y, renderer->u, renderer->v, yuv->width, yuv->height);
#endif
	if (yuv->width > 0 && yuv->height > 0 && renderer->y != NULL && renderer->u != NULL && renderer->v != NULL) {
		glUseProgram(renderer->shader_program);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderer->texture_ids[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, yuv->width, yuv->height, 0, GL_RED, GL_UNSIGNED_BYTE, renderer->y);
		glUniform1i(renderer->sampler_y, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderer->texture_ids[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, yuv->width / 2, yuv->height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, renderer->u);
		glUniform1i(renderer->sampler_u, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, renderer->texture_ids[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, yuv->width / 2, yuv->height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, renderer->v);
		glUniform1i(renderer->sampler_v, 2);

		glUniform3fv(renderer->convert_offset_uniform, 1, renderer->YUV_OFFSET);
		glUniformMatrix3fv(renderer->convert_mat_unifrom, 1, false, renderer->YUV_MATRIX);

		glBindVertexArray(renderer->VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	glDisable(GL_BLEND);

	free(renderer->y); renderer->y = NULL;
	free(renderer->u); renderer->u = NULL;
	free(renderer->v); renderer->v = NULL;
}

void vap_renderer_destroy(vap_renderer_t* renderer) {
	glDeleteTextures(sizeof(renderer->texture_ids), renderer->texture_ids);

	glDeleteVertexArrays(1, &renderer->VAO);
	glDeleteBuffers(1, &renderer->VBO);
}