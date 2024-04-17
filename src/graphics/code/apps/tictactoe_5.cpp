#include <glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image.h>

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

// The texture positions of the quad's vertexes
std::vector<glm::vec2> const quad_uv_cpu = {
	{0.0,0.0}, {1.0,1.0}, {1.0,0.0},
	{0.0,0.0}, {0.0,1.0}, {1.0,1.0},
};

// Window dimensions
glm::ivec2 const window_dims = { 400,400 };
glm::ivec2 const window_pos = { 100, 100 };

std::vector<glm::vec3> points;

GLFWwindow* setup(glm::ivec2 position, glm::ivec2 dimensions, const char* title, std::vector<WindowHint> hints);
std::string read_file_to_string(std::string file_path);


class Texture {
	GLuint id;
public:

	Texture(std::string file_path, bool mipmap) {
		// Allocate id
		id = 0;
		glGenTextures(1, &id);
		if (id == 0) {
			throw std::runtime_error("Failed to allocate texture id.");
		}
		// Load image data, width, and height
		int width, height, n;
		unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &n, 3);
		
		// Store image data to texture
		glBindTexture(GL_TEXTURE_2D, id);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		
		// Set up mipmaps, if necessary. We'll assume you want GL_NEAREST if you don't want mipmaps
		if (mipmap) {
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		// Unbind the texture, to avoid foot rakes
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// Free id at end of lifetime
	~Texture() {
		glDeleteTextures(1, &id);
	}
	// No copies, to avoid double-deletes
	Texture(Texture&) = delete;
	// Allow class to automatically cast into its id
	operator GLuint() {
		return id;
	}
};




template<typename T>
class Buffer {
	GLuint id;
public:
	// Allocate id at start of lifetime
	Buffer() {
		glGenBuffers(1, &id);
		if (id == 0) {
			throw std::runtime_error("Failed to allocate program id.");
		}
	}
	// Free id at end of lifetime
	~Buffer() {
		glDeleteBuffers(1, &id);
	}
	// No copies, to avoid double-deletes
	Buffer(Buffer&) = delete;
	// Allow class to automatically cast into its id
	operator GLuint() {
		return id;
	}

	void operator=(T const& data) {
		// Set it as the current "GL_ARRAY_BUFFER"
		glBindBuffer(GL_ARRAY_BUFFER, id);
		// Set the current "GL_ARRAY_BUFFER" with the content of data
		glBufferData(GL_ARRAY_BUFFER, sizeof(T), &data, GL_STATIC_DRAW);
	}

	void operator=(std::vector<T> const& data) {
		// Set it as the current "GL_ARRAY_BUFFER"
		glBindBuffer(GL_ARRAY_BUFFER, id);
		// Set the current "GL_ARRAY_BUFFER" with the content of data
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW);
	}

};

class VAO {
	GLuint id;
public:
	// Allocate id at start of lifetime
	VAO() {
		glGenVertexArrays(1, &id);
		if (id == 0) {
			throw std::runtime_error("Failed to allocate VAO id.");
		}
	}
	// Free id at end of lifetime
	~VAO() {
		glDeleteVertexArrays(1, &id);
	}
	// No copies, to avoid double-deletes
	VAO(VAO&) = delete;
	// Allow class to automatically cast into its id
	operator GLuint() {
		return id;
	}
};

// Make the shader type a parameter, so we can tell the
// different types apart at compile time and have the compiler
// catch any mismatched shader types.
template<GLenum TYPE>
class Shader {
	GLuint id;

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

public:
	// Allocate id and compile shader at start of lifetime
	Shader(std::string& text) {
		// Create the shader
		id = glCreateShader(TYPE);
		if (id == 0) {
			throw std::runtime_error("Failed to allocate shader id.");
		}
		// Give it the code
		GLint const length = text.size();
		GLchar const* code = reinterpret_cast<GLchar const*>(text.c_str());
		glShaderSource(id, 1, &code, &length);
		// Compile
		glCompileShader(id);
		// Make sure shader is okay
		check_compilation(id);
	}
	// Free id at end of lifetime
	~Shader() {
		glDeleteShader(id);
	}
	// No copies, to avoid double-deletes
	Shader(Shader&) = delete;
	// Allow class to automatically cast into its id
	operator GLuint() {
		return id;
	}

};


class GPUProgram {
	GLuint id;

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

public:
	// Allocate id and link program at start of lifetime
	GPUProgram(Shader<GL_VERTEX_SHADER>& vertex, Shader<GL_FRAGMENT_SHADER>& fragment) {
		// Create the program
		id = glCreateProgram();
		if (id == 0) {
			throw std::runtime_error("Failed to allocate program id.");
		}
		// Add the shaders to the program
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		// Link shaders together
		glLinkProgram(id);
		// Make sure shader is okay
		check_linking(id);
	}
	// Free id at end of lifetime
	~GPUProgram() {
		glDeleteProgram(id);
	}
	// No copies, to avoid double-deletes
	GPUProgram(GPUProgram&) = delete;
	// Allow class to automatically cast into its id
	operator GLuint() {
		return id;
	}

	GLint attribute_index(std::string name) {
		GLint index = glGetAttribLocation(id, name.c_str());
		if (index < 0) {
			std::string message = "Attribute '";
			message += name + "' does not exist.";
			throw std::runtime_error(message);
		}
	}

};



int main() {

	// Setup Window
	std::vector<WindowHint> hints = {};
	GLFWwindow* window = setup(window_pos, window_dims, "Tic Tac Toe", hints);


	// Load and compile vertex shader
	std::string vertex_shader_text = read_file_to_string("shaders/tictactoe/tictactoe3.vert");
	Shader<GL_VERTEX_SHADER> vertex_shader(vertex_shader_text);

	// Load and compile fragment shader
	std::string fragment_shader_text = read_file_to_string("shaders/tictactoe/tictactoe3.frag");
	Shader<GL_FRAGMENT_SHADER> fragment_shader(fragment_shader_text);

	// Link program from vertex and fragment shader
	GPUProgram program(vertex_shader, fragment_shader);

	// Find index of "pos" attribute
	GLuint pos_index = program.attribute_index("pos");

	// Find index of "uv" attribute
	GLuint uv_index = program.attribute_index("uv");


	// Create Vertex Array Object
	VAO vao;

	// Create Buffer Object for positions
	Buffer<glm::vec3> pos;
	pos = quad_pos_cpu;

	// Create Buffer Object for uvs
	Buffer<glm::vec2> uv;
	uv = quad_uv_cpu;

	// Create texture
	Texture tex("assets/x.png",true);

	// Make vao the current vertex array
	glBindVertexArray(vao);

	// Establish attribute binding and type for pos
	glEnableVertexAttribArray(pos_index);
	glBindBuffer(GL_ARRAY_BUFFER, pos);
	glVertexAttribPointer(pos_index, 3, GL_FLOAT, false, 0, nullptr);

	// Establish attribute binding and type for uv
	glEnableVertexAttribArray(uv_index);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glVertexAttribPointer(uv_index, 2, GL_FLOAT, false, 0, nullptr);

	// Make sure things that are in front stay in front
	glEnable(GL_DEPTH_TEST);

	// Make our GPU program the current one for the context
	glUseProgram(program);
		
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,(GLuint) tex);
	GLint tex_index = glGetUniformLocation(program, "the_texture");
	glUniform1i(tex_index, 0);

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
	// Hints derived from GLFW documentation: https://www.glfw.org/faq.html#41__how_do_i_create_an_opengl_30_context
	// Versions updated to 4.1, since that seemed to work for students in previous terms
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

