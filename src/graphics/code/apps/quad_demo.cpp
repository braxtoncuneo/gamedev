// quad.cpp using OpenGL shader architecture
// By Braxton Cuneo, Published under Creative Commons CC-BY

#include <glad.h>
#include <glfw3.h>

#include "glazy.h"
#include "shape.h"
#include <vector>


// Window dimensions
glm::ivec2 const window_dims = { 400,400 };
glm::ivec2 const window_pos = { 100, 100 };

// Tracks what coordinate system is shown
GLboolean show_model = false;
GLboolean show_world = false;
GLboolean show_view = false;

std::vector<glm::vec3> points;


int main() {

	std::vector<glazy::context::WindowHint> hints = {};
	GLFWwindow* window = setup(window_pos, window_dims, "Quad Demo", hints);


	glazy::GPUProgram program(
		glazy::Shader<GL_VERTEX_SHADER>::from_file("./shaders/quad/quad.vert"),
		{},
		{},
		{},
		glazy::Shader<GL_FRAGMENT_SHADER>::from_file("./shaders/quad/quad.frag")
	);


	glazy::SharedVAO vao;
	glazy::SharedBuffer<glm::vec3> pos;
	points = glazy::shape::quad();
	pos.set_data(points, GL_STATIC_DRAW);

	GLint pos_index = program.attribute_index("point");
	{
		glazy::VAO::BindGuard guard (vao);
		vao[pos_index].enable();
		vao[pos_index] = (glazy::Buffer<glm::vec3>&) pos;


		glEnable(GL_DEPTH_TEST);
		glUseProgram(program);

		while (!glfwWindowShouldClose(window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			float time = (float) glfwGetTime();

			glDrawArrays(GL_TRIANGLES, 0, points.size());

			glFlush();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


