# Writing a Basic Graphics Application

To start off, here is our initial "hello world" for graphics.

```cpp
{{ #include ./code/apps/tictactoe_1.cpp }}
```

This is a lot, so let's break it down.

## The Startup Code

```cpp
void error_callback(int error_code, char const* desc) {
	throw std::runtime_error(desc);
}

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
```

This code is the same as shown in the previous sub-chapter, and works as previously described.
While many parts of this program will be refactored in this sub-chapter, the startup code will remain the same throughout.

## Setting up Geometry

This portion defines a sequence of six points which form a pair of triplets, with each triplet describing a triangle.
Together, this pair of triangles covers all x/y positions in the screenspace.

```cpp
std::vector<glm::vec3> const quad_pos_cpu = {
	{-1.0,-1.0,0.0}, {1.0,1.0,0.0}, {1.0,-1.0,0.0},
	{-1.0,-1.0,0.0}, {-1.0,1.0,0.0}, {1.0,1.0,0.0},
};
```

## Getting the Geometry on the GPU

Getting the geometry on the GPU requires a **buffer** to store said geometry.
Below, the code:
- allocates a buffer, storing its id into `quad_pos`
- binds the buffer as `GL_ARRAY_BUFFER`
- sets the data for the buffer bound to `GL_ARRAY_BUFFER` (the buffer that was just allocated)
- allocates a VAO, storing its id into `vao`
- binds the newly created VAO as the current VAO

```cpp
	// Load quad point positions into GPU
	GLuint quad_pos;
	glGenBuffers(1, &quad_pos);
	glBindBuffer(GL_ARRAY_BUFFER, quad_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * quad_pos_cpu.size(), quad_pos_cpu.data(), GL_STATIC_DRAW);


	// Setup VAO
	GLuint vao;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
```


## Setting up a Program

In main, the following code sets up the GPU program for this program:

```cpp
	// Load and compile vertex shader
	std::string vertex_shader_text = read_file_to_string("shaders/tictactoe/tictactoe.vert");
	GLuint vertex_shader = compile_shader(vertex_shader_text, GL_VERTEX_SHADER);

	// Load and compile fragment shader
	std::string fragment_shader_text = read_file_to_string("shaders/tictactoe/tictactoe.frag");
	GLuint fragment_shader = compile_shader(fragment_shader_text, GL_FRAGMENT_SHADER);

	// Link program from vertex and fragment shader
	GLuint program = link_program(vertex_shader, fragment_shader);
```

This code calls into functions defined elsewhere, loading the code text for a vertex shader and a fragment shader, compiles it, then links it into a GPU program.

### `read_file_to_string`

This is our file reading function.
It determines the size of a file by finding the difference in position between the start and end of the file, resizes a string to match the file size, then loads the content of the file into the result string.

```cpp
std::string read_file_to_string(std::string file_path) {
	std::ifstream file(file_path);
	file.seekg(0, std::ios::end);
	size_t size = static_cast<size_t>(file.tellg());
	file.seekg(0, std::ios::beg);
	std::string result;
	result.resize(size);
	result.assign((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	return result;
}
```

## `compile_shader`

The shader compilation function takes the code for a shader, and creates an object representing a binary compiled from the code provided.
The compiler and context need to distinguish between the different types of possible shader, so an enumeration representing the shader type must also be provided.

As with buffers and VAOs, shaders are represented through an id.
Once an id has been allocated, the code defining the shader can be supplied to it via `glShaderSource`.
This function accepts an array of pointers to c strings, which is meant to let you mix and match different chunks of shader code.
For our purposes, we'll just be supplying a single string, representing a complete shader definition.

Once the code has been given to the shader, one can attempt to compile it through `glCompileShader`.

```cpp
GLuint compile_shader(std::string text, GLenum shader_type) {
	GLuint id = glCreateShader(shader_type);
	GLint const length = text.size();
	GLchar const* code = reinterpret_cast<GLchar const*>(text.c_str());
	glShaderSource(id, 1, &code, &length);
	glCompileShader(id);
	return id;
}
```


## `check_compilation`

To see if the shader actually compiled, we need to read to check the `GL_COMPILE_STATUS` field of the shader.
The value of this status, as well as the values of other shader-associated information, can be retrieved through the `glGetShader*` family of functions.

If the status is false, then compilation failed.
Unless we wish to stumble blindly when debugging our shaders, we will need to read the log of the compiler to determine *why* compilation failed.
The log is just a string, but we need to allocate a buffer to retrieve that string from the GPU driver.
Like the compilation status, the length of this log can be queried through `glGetShaderiv`.
Once the log length is found, the log can be read into a buffer of characters through `glGetShaderInfoLog`.
Since the size of the buffer may not match the size of the log, the length of the buffer is supplied as input and a pointer to another length is supplied, with the pointed length set to indicate how many characters were actually written to the buffer.


```cpp
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

```

## `link_program`

A GPU program is made up of a set of shaders.
To "glue" these shaders together into a program, the set of corresponding shaders must first be **attached** to the program via `glAttachShader`, then **linked** into an executable object via `glLinkProgram`.

As with shaders, GPU programs are handled through an integer id that must be allocated.
Also, like shader compilation, program linking can fail, and the failure status must be checked.

```cpp
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
```



## `check_linking`

The process of checking the result of linking is almost identical to the process for shader compilation.

```cpp
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
```


## Setting up Program Inputs

As mentioned in a previous sub-chapter:

- GPU programs recieve a set of primitives (made up of vertexes) as input
- The collection of vertexes is described through a VAO through a set of buffers
- Buffers are used to store the attribute data of vertexes

### Setting up a buffer

Below, the program allocates a buffer id, then binds it to the bindpoint `GL_ARRAY_BUFFER`.
Unless you have access to newer versions of OpenGL with `glNamedBufferData`, the data of a buffer can only be set through `glBufferData` or setting up GPU/CPU memory mapping through `glMapBuffer`, both which require binding the destination buffer to a bind point.

In this case, we are using `glBufferData`, supplying the binding through which buffer data will be set, the new size of the GPU-side buffer (in bytes), a pointer to the CPU-side buffer (the data array of our vector), and a usage hint. Check out the OpenGL docs for more information about usage hints. If you don't plan on writing to this buffer on the GPU-side and don't plan on reading it from the CPU-side, `GL_STATIC_DRAW` is usually what you want to use.

```cpp

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
```

### Setting up a VAO

Like buffers, VAOs must have an allocated integer id.
Unlike buffers, there is only one VAO bind point, meaning that there is only one active VAO at a time (the one most recently bound).

Each attribute attached to a VAO is associated with an index.
Upon compilation, GPU programs assign an index to each of the attributes associated with input vertexes.
To determine which index corresponds with which attribute, the index must either be pre-determined in the shader code (not the case for this program), or the index must be queried through `glGetAttribLocation`.

Assuming the query succeeds (does not return a negative number), the returned integer can be used to reference the corresponding attribute.

By default, attributes are disabled and aren't associated with any data.
To fix that, an attribute must be enabled through `glEnableVertexAttribArray` and then set through `glVertexAttribPointer`.

Note that no bind point is given to `glVertexAttribPointer`.
This function implicitly works on the buffer that was most recently bound to `GL_ARRAY_BUFFER`, so it is vital to bind the appropriate buffer before calling this function.
The inputs to `glVertexAttribPointer` (aside from the index itself) represent the type information of the supplied data.
In this case (from left to right) this call is stating"
- each element of this buffer has **3** components
- the components of each element are **float** values
- these elements should **not** be normalized
- these elements start at offset **0**
- (we won't be getting into the final, pointer argument for now)

```cpp
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
```

### Setting up the Display Loop

Finally, just before the display loop, we tell the OpenGL context that things that are closer to the camera should always overwrite things that are farther from the camera.

```cpp
	// Make sure things that are in front stay in front
	glEnable(GL_DEPTH_TEST);

	// Make our GPU program the current one for the context
	glUseProgram(program);
```

## The Display Loop

In the display loop, we continue to render the quad to the screen while the window should not close (`glfwWindowShouldClose`).
To make sure the frame buffer starts off clean before we draw anything, we clear it with `glClear`, indicating both depth and color information should be reset.

After this, the current VAO (our only VAO) is fed into the current program (our only program), rendering the quad to the framebuffer (`glDrawArrays`).
To make sure this rendering occurs fully, we flush the rendering pipeline (`glFlush`).
Finally, we swap the current render framebuffer with the one we are displaying, showing the just-rendered frame.

```cpp
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
```

## The Shaders

### Vertex Shader

This is our vertex shader.
An instance of a vertex shader is run for each vertex that is provided as input, with each vertex attribute declared globally as **in**.
Vertex shaders can do a wide variety of computations upon its inputs, but - for this first shader - the vertex position will simply be passed on to the fragment shader through the built-in **out** variable, `gl_Position`.

```cpp
{{ #include ./code/shaders/tictactoe/tictactoe1.vert }}
```

### Fragment Shader

The fragment shader simply writes a fully opaque yellow color out to the frame buffer.

```cpp
{{ #include ./code/shaders/tictactoe/tictactoe1.frag }}
```











