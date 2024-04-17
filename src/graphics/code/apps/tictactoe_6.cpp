
#include <glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>


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
	{0.0,1.0}, {1.0,0.0}, {1.0,1.0},
	{0.0,1.0}, {0.0,0.0}, {1.0,0.0},
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
	Shader(std::string const &&text) {
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





class GameState {

	// Textures needed for tic tac toe
	Texture x_tex; // For tiles marked 'x'
	Texture o_tex; // For tiles marked 'o'
	Texture blank_tex; // For blank tiles
	Texture x_wins_tex; // Message for when x wins
	Texture o_wins_tex; // Message for when o wins
	Texture draw_tex;   // Message for draws (aka ties)

	// The shaders/program used for rendering quads
	Shader<GL_VERTEX_SHADER> vertex;
	Shader<GL_FRAGMENT_SHADER> fragment;
	GPUProgram program;

	// Index of the "pos" attribute
	GLint pos_index;
	// Index of the "uv" attribute
	GLint uv_index;
	// Index of the "the_texture" uniform
	GLint tex_index;
	// Index of the "offset" uniform
	GLint offset_index;
	// Index of the "scale" uniform
	GLint scale_index;
	// Index of the "dim" uniform
	GLint dim_index;

	// Create Buffer Object for quad vertex positions
	Buffer<glm::vec3> pos;
	// Create Buffer Object for quad vertex uvs
	Buffer<glm::vec2> uv;
	// Vertex Array Object for quad mesh
	VAO vao;


	// 2D array of chars representing the board state
	char grid[3][3];

	// The x and y coordinates of the currently selected tile
	int x_select;
	int y_select;

	// Whether or not the user is actively attempting to place on an
	// already-filled tile
	bool bad;
	// Whether or not the game has ended
	bool endgame;
	// Which player has the current turn
	char turn;
	// The current winner, if endgame = true
	char winner;

public:

	// Initialize game state
	GameState();

	// Check if a given player has won
	bool won(char player);
	// Check if the board has been filled
	bool tie();
	// Select a tile on the grid for marking
	void select();
	// Deselect tile
	void deselect();
	// Move selection on the grid
	void move(int x_delta, int y_delta);
	// Render the game state to the screen
	void render();
};


GameState::GameState()
	: x_tex("assets/x.png",false)
	, o_tex("assets/o.png",false)
	, blank_tex("assets/blank.png",false)
	, x_wins_tex("assets/x_win.png",false)
	, o_wins_tex("assets/o_win.png",false)
	, draw_tex("assets/draw.png",false)
	, vertex(read_file_to_string("shaders/tictactoe/tictactoe4.vert"))
	, fragment(read_file_to_string("shaders/tictactoe/tictactoe4.frag"))
	, program(vertex,fragment)
	, pos_index(program.attribute_index("pos"))
	, uv_index(program.attribute_index("uv"))
	, tex_index(glGetUniformLocation(program,"the_texture"))
	, offset_index(glGetUniformLocation(program,"offset"))
	, scale_index(glGetUniformLocation(program,"scale"))
	, dim_index(glGetUniformLocation(program,"dim"))
{

	pos = quad_pos_cpu;
	uv = quad_uv_cpu;

	glBindVertexArray(vao);

	// Establish attribute binding and type for pos
	glEnableVertexAttribArray(pos_index);
	glBindBuffer(GL_ARRAY_BUFFER, pos);
	glVertexAttribPointer(pos_index, 3, GL_FLOAT, false, 0, nullptr);

	// Establish attribute binding and type for uv
	glEnableVertexAttribArray(uv_index);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glVertexAttribPointer(uv_index, 2, GL_FLOAT, false, 0, nullptr);


	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			grid[i][j] = ' ';
		}
	}
	x_select = 0;
	y_select = 0;
	bad = false;
	turn = 'x';
	endgame = false;
	winner = ' ';
}

bool GameState::won(char player) {
	bool won = false;
	// Check for winning columns
	for (int x = 0; x < 3; x++) {
		bool match = true;
		for (int y = 0; y < 3; y++) {
			if (grid[x][y] != player) {
				match = false;
			}
		}
		won = won || match;
	}
	// Check for winning rows
	for (int y = 0; y < 3; y++) {
		bool match = true;
		for (int x = 0; x < 3; x++) {
			if (grid[x][y] != player) {
				match = false;
			}
		}
		won = won || match;
	}
	// Check for TL-DR diagonal win
	bool match = true;
	for (int xy = 0; xy < 3; xy++) {
		if (grid[xy][xy] != player) {
			match = false;
		}
	}
	// Check for TR-DL diagonal win
	won = won || match;
	match = true;
	for (int xy = 0; xy < 3; xy++) {
		if (grid[xy][2 - xy] != player) {
			match = false;
		}
	}
	won = won || match;
	return won;
}

// Check if the board has been filled
bool GameState::tie() {
	bool filled = true;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			if (grid[x][y] == ' ') {
				filled = false;
			}
		}
	}
	return filled;
}

// Select a tile on the grid for marking
void GameState::select() {
	if (endgame) {
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {
				grid[x][y] = ' ';
			}
		}
		endgame = false;
		return;
	}
	if (grid[x_select][y_select] == ' ') {
		grid[x_select][y_select] = turn;
		if (won(turn)) {
			winner = turn;
			endgame = true;
		}
		else if (tie()) {
			endgame = true;
		}
		else if (turn == 'x') {
			turn = 'o';
		}
		else {
			turn = 'x';
		}
	}
	else {
		bad = true;
	}
}


// Deselect tile
void GameState::deselect() {
	bad = false;
}


// Move selection
void GameState::move(int x_delta, int y_delta) {
	x_select += x_delta;
	y_select += y_delta;
	if (x_select < 0) {
		x_select = 0;
	}
	else if (x_select > 2) {
		x_select = 2;
	}
	if (y_select < 0) {
		y_select = 0;
	}
	else if (y_select > 2) {
		y_select = 2;
	}
}










// Render the gamestate to the screen
void GameState::render() {
	// Make our GPU program the current one for the context
	glUseProgram(program);
	// Make sure things that are in front stay in front
	glEnable(GL_DEPTH_TEST);
	// Technically unnecessary, but the initial VAO binding may be
	// different in larger projects, when multiple VAOs are used
	glBindVertexArray(vao);
	// Also technically unnecessary, since this program always
	// uses texture unit zero, but its good to make sure anyway
	glUniform1i(tex_index, 0);
	glActiveTexture(GL_TEXTURE0);

	// If the game has ended, show the endgame message
	if (endgame) {
		// A message id to set based upon the winner
		GLuint message;
		if (winner == 'x') {
			message = x_wins_tex;
		}
		else if (winner == 'o') {
			message = o_wins_tex;
		}
		else {
			message = draw_tex;
		}
		// Bind the correct message to the texture for the quad
		glBindTexture(GL_TEXTURE_2D, message);
		// Place the message in the center of the window
		glm::vec2 offset(0,0);
		glm::vec2 scale (0.6,0.6);
		glUniform1i(tex_index, 0);
		glUniform2fv(offset_index, 1, (GLfloat*) & offset);
		glUniform2fv(scale_index,  1, (GLfloat*) & scale);
		// Endgame messages should never be dimmed
		glUniform1i (dim_index,1.0);
		// Draw the quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	else {
		glm::vec2 scale (1.f/5.f,1.f/5.f);
		glUniform2fv(scale_index,  1, (GLfloat*) & scale);
		// Iterate through the grid to draw each tile
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {
				std::cout << '[' << grid[x][y] << ']';
				// Select the correct texture for the tile
				GLint tile_image;
				if (grid[x][y] == 'o') {
					tile_image = o_tex;
				}
				else if (grid[x][y] == 'x') {
					tile_image = x_tex;
				}
				else {
					tile_image = blank_tex;
				}
				// Bind the correct texture
				glBindTexture(GL_TEXTURE_2D, tile_image);
				// Offset the quad to the correct grid position
				glm::vec2 offset((x-1)/2.f,(y-1)/2.f);
				glUniform2fv(offset_index, 1, (GLfloat*) & offset);
				// If the user is selecting an already filled tile,
				// that tile should be dimmed to show it as a recieved,
				// but invalid input.
				if ((x == x_select) && (y == y_select)) {
					if (bad) {
						glUniform1f(dim_index, 0.25f);
					}
					else {
						glUniform1f(dim_index, 0.5f);
					}
				}
				else {
					glUniform1f(dim_index, 1.0f);
				}
				// Draw the quad
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}


GameState* game_state;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_W:
			game_state->move(0, 1);
			break;
		case GLFW_KEY_A:
			game_state->move(-1, 0);
			break;
		case GLFW_KEY_S:
			game_state->move(0, -1);
			break;
		case GLFW_KEY_D:
			game_state->move(1, 0);
			break;
		case GLFW_KEY_SPACE:
			game_state->select();
		}
	}
	else if ((action == GLFW_RELEASE) && (key == GLFW_KEY_SPACE)) {
		game_state->deselect();
	}

}


int main() {

	// Setup Window
	std::vector<WindowHint> hints = {};
	GLFWwindow* window = setup(window_pos, window_dims, "Tic Tac Toe", hints);

	glfwSetKeyCallback(window,key_callback);

	game_state =  new GameState();

	// Keep going while the window shouldn't close
	while (!glfwWindowShouldClose(window)) {
		// Clear all color and depth information on the current framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		game_state->render();

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


