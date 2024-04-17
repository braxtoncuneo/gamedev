#include <glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>

struct WindowHint {
	int hint;
	int value;
};

// The vertex positions of a two-triangle quad model
std::vector<glm::vec3> const quad_pos_cpu = {
	{-1.0,-1.0,0.0}, {1.0,1.0,0.0}, {1.0,-1.0,0.0},
	{-1.0,-1.0,0.0}, {-1.0,1.0,0.0}, {1.0,1.0,0.0},
};

// Window dimensions
glm::ivec2 const window_dims = { 400,400 };
glm::ivec2 const window_pos = { 100, 100 };

std::vector<glm::vec3> points;

GLFWwindow* setup(glm::ivec2 position, glm::ivec2 dimensions, const char* title, std::vector<WindowHint> hints);
std::string read_file_to_string(std::string file_path);
GLuint compile_shader(std::string text, GLenum shader_type);
GLuint link_program(GLuint vertex_shader, GLuint fragement_shader);

int main() {

	// Setup Window
	std::vector<WindowHint> hints = {};
	GLFWwindow* window = setup(window_pos, window_dims, "Tic Tac Toe", hints);


	// Load and compile vertex shader
	std::string vertex_shader_text = read_file_to_string("shaders/tictactoe/tictactoe1.vert");
	GLuint vertex_shader = compile_shader(vertex_shader_text, GL_VERTEX_SHADER);

	// Load and compile fragment shader
	std::string fragment_shader_text = read_file_to_string("shaders/tictactoe/tictactoe1.frag");
	GLuint fragment_shader = compile_shader(fragment_shader_text, GL_FRAGMENT_SHADER);

	// Link program from vertex and fragment shader
	GLuint program = link_program(vertex_shader, fragment_shader);


	// Create Vertex Array Object
	GLuint vao;
	glGenVertexArrays(1, &vao);

	// Create Buffer Object
	GLuint pos;
	glGenBuffers(1, &pos);

	// Set it as the current "GL_ARRAY_BUFFER"
	glBindBuffer(GL_ARRAY_BUFFER, pos);

	// Set the current "GL_ARRAY_BUFFER" with the contents
	// of quad_pos_cpu
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(glm::vec3)*quad_pos_cpu.size(),
		quad_pos_cpu.data(),
		GL_STATIC_DRAW
	);

	// Make vao the current vertex array
	glBindVertexArray(vao);

	// Find index of "pos" attribute
	GLuint pos_index = glGetAttribLocation(program, "pos");

	// Enable the attribute matching the index of the "pos" input
	// in the vertex shader
	glEnableVertexAttribArray(pos_index);

	// Set pos as the buffer for attribute at pos_index, with 3 elements
	// per component, a "float" component type, no normalization, no stride,
	// and no pointer
	glBindBuffer(GL_ARRAY_BUFFER, pos);
	glVertexAttribPointer(pos_index, 3, GL_FLOAT, false, 0, nullptr);

	// Make sure things that are in front stay in front
	glEnable(GL_DEPTH_TEST);

	// Make our GPU program the current one for the context
	glUseProgram(program);

	// Keep going while the window shouldn't close
	while (!glfwWindowShouldClose(window)) {
		// Clear all color and depth information on the current framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw the model
		glDrawArrays(GL_TRIANGLES, 0, quad_pos_cpu.size());

		// Make sure all work is complete
		glFlush();

		// Swap the framebuffer we are rendering to for the one that is being displayed
		glfwSwapBuffers(window);
		// Make sure all events are checked by GLFW
		glfwPollEvents();
	}

	// Cleaning up
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


// Loads a file from a path into a string and returns it
std::string read_file_to_string(std::string file_path) {
	// The file in question
	std::ifstream file(file_path);
	// Get size of file
	file.seekg(0, std::ios::end);
	size_t size = static_cast<size_t>(file.tellg());
	// Reset read/write positiion to the start of the file
	file.seekg(0, std::ios::beg);
	// Resize string to match file size
	std::string result;
	result.resize(size);
	// Copy file content into string
	result.assign((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	return result;
}

// Checks compilation status. Upon failure, throws compiler log info.
void check_compilation(GLuint id) {
	GLint status;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if (!status) {
		GLint log_length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_length);
		std::string log_text((size_t)log_length, '\0');
		GLsizei written;
		glGetShaderInfoLog(id, log_length, &written, reinterpret_cast<GLchar*>((char*)log_text.data()));
		std::string message = "Shader compilation failed. Error log:\n\n\"\"\"\n";
		message += log_text.c_str();
		message += "\n\"\"\"\n";
		throw std::runtime_error(message);
	}
}


// Checks compilation status. Upon failure, throws compiler log info.
void check_linking(GLuint id) {
	GLint status;
	glGetProgramiv(id, GL_LINK_STATUS, &status);
	if (!status) {
		GLint log_length;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_length);
		std::string log_text((size_t)log_length, '\0');
		GLsizei written;
		glGetProgramInfoLog(id, log_length, &written, reinterpret_cast<GLchar*>((char*)log_text.data()));
		std::string message = "GPU Program linking failed. Error log:\n\n\"\"\"\n";
		message += log_text.c_str();
		message += "\n\"\"\"\n";
		throw std::runtime_error(message);
	}
}



// Creates shader defined by the code "text" with shader type "shader_type",
// returning the shader's id.
GLuint compile_shader(std::string text, GLenum shader_type) {
	// Create the shader
	GLuint id = glCreateShader(shader_type);
	// Give it the code
	GLint const length = text.size();
	GLchar const* code = reinterpret_cast<GLchar const*>(text.c_str());
	glShaderSource(id, 1, &code, &length);
	// Compile
	glCompileShader(id);
	// Make sure shader is okay
	check_compilation(id);
	return id;
}


// Links a GPU program defined by a vertex/fragment shader with the given ids
GLuint link_program(GLuint vertex_shader, GLuint fragment_shader) {
	// Create the program
	GLuint id = glCreateProgram();
	// Add the shaders to the program
	glAttachShader(id,vertex_shader);
	glAttachShader(id,fragment_shader);
	// Link shaders together
	glLinkProgram(id);
	// Make sure shader is okay
	check_linking(id);
	return id;
}


// Our error callback function from before
void error_callback(int error_code, char const* desc) {
	throw std::runtime_error(desc);
}

// Our setup function from before
GLFWwindow* setup(glm::ivec2 position, glm::ivec2 dimensions, const char* title, std::vector<WindowHint> hints) {
	if (glfwInit() == GLFW_FALSE) {
		throw std::runtime_error("Failed to initialize GLFW.");
	}

	glfwSetErrorCallback(error_callback);

	std::vector<WindowHint> platform_hints;

	#ifdef __APPLE__
	platform_hints = {
		{GLFW_CONTEXT_VERSION_MAJOR, 4},
		{GLFW_CONTEXT_VERSION_MINOR, 1},
		{GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE},
		{GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE}
	};
	#else
	platform_hints = {
		{GLFW_CONTEXT_VERSION_MAJOR, 4},
		{GLFW_CONTEXT_VERSION_MINOR, 1},
		{GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE},
	};
	#endif

	for (auto&& hint : platform_hints) {
		glfwWindowHint(hint.hint, hint.value);
	}

	for (auto&& hint : hints) {
		glfwWindowHint(hint.hint, hint.value);
	}

	GLFWwindow* result = glfwCreateWindow(dimensions.x, dimensions.y, title, nullptr, nullptr);

	if (result == nullptr) {
		glfwTerminate();
		throw std::runtime_error("Failed to create window.");
	}

	glfwSetWindowPos(result, position.x, position.y);

	glfwMakeContextCurrent(result);
	glfwSwapInterval(1);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::runtime_error("Failed to initialize OpenGL context.");
	}
	return result;
}

