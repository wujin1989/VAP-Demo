_Pragma("once");

#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern GLFWwindow* vap_window_create(int width, int height);
extern void vap_window_destroy(void);