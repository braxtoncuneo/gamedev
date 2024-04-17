// camera_demo.cpp using OpenGL shader architecture
// By Braxton Cuneo, Published under Creative Commons CC-BY

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glad.h>
#include <glfw3.h>

#include "glazy.h"
#include "shape.h"
#include <vector>



// Window dimensions
glm::ivec2 const window_dims = { 1000,1000 };
glm::ivec2 const window_pos = { 100, 100 };

// Tracks what coordinate system is shown
GLboolean show_model = false;
GLboolean show_world = false;
GLboolean show_view = false;

std::vector<glm::vec3> points;



GLFWwindow *setup();
void key_handler(GLFWwindow* window, int key, int scancode, int action, int mods);
void display(GLFWwindow*w,glazy::GPUProgram &prog);



int main() {

	std::vector<glazy::context::WindowHint> hints = {};
	GLFWwindow* window = setup(window_pos, window_dims, "Camera Demo", hints);
	glfwSetKeyCallback(window, key_handler);
	
	glazy::GPUProgram program(
		glazy::Shader<GL_VERTEX_SHADER>::from_file("./shaders/camera_demo/camera.vert"),
		{},
		{},
		{},
		glazy::Shader<GL_FRAGMENT_SHADER>::from_file("./shaders/camera_demo/camera.frag")
	);


	glazy::SharedVAO vao;
	glazy::SharedBuffer<glm::vec3> pos;
	points = glazy::shape::sphere(100, 100, 1);
	pos.set_data(points, GL_STATIC_DRAW);

	GLint pos_index = program.attribute_index("point");
	{
		glazy::VAO::BindGuard guard (vao);
		vao[pos_index].enable();
		vao[pos_index] = (glazy::Buffer<glm::vec3>&) pos;

		glEnable(GL_DEPTH_TEST);

		glUseProgram(program);

		while (!glfwWindowShouldClose(window)) {
			display(window, program);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}





void key_handler(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_M:
				show_model = !show_model;
				break;
			case GLFW_KEY_W:
				show_world = !show_world;
				break;
			case GLFW_KEY_V:
				show_view = !show_view;
				break;
		}
	}
}



void display(GLFWwindow* w, glazy::GPUProgram &program) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat time = (float) glfwGetTime();

	// Have FOV of 80 degrees, a 1:1 aspect ratio, a near plane of 0.1 and far plane of 1000.0
	glm::mat4 proj_transform = glm::perspective(80.0f, 1.0f, 0.1f, 1000.0f);

	// Moves model
	glm::mat4 modl_transform = glm::identity<glm::mat4>();
	modl_transform = glm::translate(modl_transform, glm::vec3{ cos(time * 0.1f) * 4, sin(time * 0.1f) * 4, 0 });
	modl_transform = glm::rotate(modl_transform,time, glm::vec3{0,1,0});

	// Moves camera
	glm::mat4 view_transform = glm::identity<glm::mat4>();
	view_transform = glm::translate(view_transform, glm::vec3{ 0, 0, -7 + cos(time*0.5f)*4});


	program[{"modl_transform"}] = modl_transform;
	program[{"view_transform"}] = view_transform;
	program[{"proj_transform"}] = proj_transform;
	program[{"show_model"}]     = show_model;
	program[{"show_world"}]     = show_world;
	program[{"show_view"}]      = show_view;

	glDrawArrays(GL_TRIANGLES, 0, points.size());

	glFlush();

	glfwSwapBuffers(w);
	glfwPollEvents();

}



