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

std::vector<glm::vec3> pos_cpu;
std::vector<glm::vec2> uv_cpu;



GLFWwindow *setup();
void key_handler(GLFWwindow* window, int key, int scancode, int action, int mods);
GLuint create_vertex_buffer(std::vector<glm::vec3> pos_cpu);
void set_point_buffer(GLuint buffer);
void display(GLFWwindow*w,glazy::GPUProgram &prog, glazy::Texture &the_texture);



int main() {

	std::vector<glazy::context::WindowHint> hints = {};
	GLFWwindow* window = setup(window_pos, window_dims, "Camera Demo", hints);
	glfwSetKeyCallback(window, key_handler);
	
	glazy::GPUProgram program(
		glazy::Shader<GL_VERTEX_SHADER>::from_file("./shaders/texture/texture.vert"),
		{},
		{},
		{},
		glazy::Shader<GL_FRAGMENT_SHADER>::from_file("./shaders/texture/texture.frag")
	);

	std::vector<glazy::Texture::RGB8> texture_data;
	texture_data.resize(128 * 128);
	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			texture_data[y * 128 + x] = glazy::Texture::RGB8{
				static_cast<unsigned char>(y*2),
				static_cast<unsigned char>(x*2),
				255
			};
			if (((x % 10) == 0) || ((y%10) == 0)) {
				texture_data[y * 128 + x] = glazy::Texture::RGB8{ 0,0,0 };
			}
		}
	}

	glazy::Texture the_texture(texture_data, 128, 128, true);
	

	glazy::SharedVAO vao;
	
	glazy::SharedBuffer<glm::vec3> pos;
	pos_cpu = glazy::shape::sphere(100, 100, 1);
	pos.set_data(pos_cpu, GL_STATIC_DRAW);
	
	glazy::SharedBuffer<glm::vec2> uv;
	uv_cpu = glazy::shape::uv_grid(100, 100);
	uv.set_data(uv_cpu, GL_STATIC_DRAW);


	GLint pos_index = program.attribute_index("pos");
	GLint uv_index = program.attribute_index("uv");
	{
		glazy::VAO::BindGuard guard (vao);
		vao[pos_index].enable();
		vao[pos_index] = (glazy::Buffer<glm::vec3>&) pos;
		vao[uv_index].enable();
		vao[uv_index] = (glazy::Buffer<glm::vec2>&) uv;

		glEnable(GL_DEPTH_TEST);

		glUseProgram(program);
	
		glazy::safety::auto_throw("Before ActiveTexture");
		glActiveTexture(GL_TEXTURE0);
		glazy::safety::auto_throw("After ActiveTexture");
		glBindTexture(GL_TEXTURE_2D,(GLuint) the_texture);
		glazy::safety::auto_throw("After BindTexture");

		while (!glfwWindowShouldClose(window)) {
			display(window, program,the_texture);
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



void display(GLFWwindow* w, glazy::GPUProgram &program, glazy::Texture &the_texture) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat time = (float) glfwGetTime();

	// Have FOV of 80 degrees, a 1:1 aspect ratio, a near plane of 0.1 and far plane of 1000.0
	glm::mat4 proj_transform = glm::perspective(80.0f, 1.0f, 0.1f, 1000.0f);

	// Moves model
	float scale = sin(time*10) + 1.0;
	glm::mat4 modl_transform = glm::identity<glm::mat4>();
	modl_transform = glm::translate(modl_transform, glm::vec3{ cos(time * 0.1f) * 2, sin(time * 0.1f) * 2, 0 });
	modl_transform = glm::rotate(modl_transform,time, glm::vec3{0,1,0});
	//modl_transform = glm::scale(modl_transform, glm::vec3{scale,scale,scale});

	// Moves camera
	glm::mat4 view_transform = glm::identity<glm::mat4>();
	view_transform = glm::translate(view_transform, glm::vec3{ 0, 0, -4 + cos(time*0.5f)*2});


//	program[{"time"}] = time;
//	program[{"the_texture"}]    = (GLint) 0;
	program[{"modl_transform"}] = modl_transform;
	program[{"view_transform"}] = view_transform;
	program[{"proj_transform"}] = proj_transform;

	glDrawArrays(GL_TRIANGLES, 0, pos_cpu.size());

	glFlush();

	glfwSwapBuffers(w);
	glfwPollEvents();

}



