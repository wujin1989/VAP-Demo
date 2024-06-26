_Pragma("once");

#include <stdint.h>

#define DEBUG 1
#define OUTPUT_YUV "out_yuv420p.yuv"
#define OUTPUT_YUV2 "out_yuv420p2.yuv"

extern void vap_dump_enable(void);
extern void vap_dump_yuv420p(uint8_t* y, uint8_t* u, uint8_t* v, int width, int height);
void vap_dump_yuv420p2(uint8_t* yuv, int width, int height);
extern void vap_dump_disable(void);