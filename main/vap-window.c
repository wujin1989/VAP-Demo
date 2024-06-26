#include "vap-window.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	printf("window size changed, width: %d, height: %d\n", width, height);
}

GLFWwindow* vap_window_create(int width, int height) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "VAP-Player", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		abort();
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD\n");
		abort();
	}
	return window;
}

void vap_window_destroy(void) {
	glfwTerminate();
}