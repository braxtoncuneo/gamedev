# Setting up an OpenGL + GLFW Program


Both OpenGL and GLFW work through a **context**, a global state which is used to track/manage systems.
Both states need to be set up.
Below is a snippet that should work for most people:

```cpp
struct WindowHint {
	int hint;
	int value;
};

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

Let's break down what's happening.


## GLFW Initialization

Here, the code attempts to start up a GLFW context, throwing an error on failure.
```cpp
if (glfwInit() == GLFW_FALSE) {
    throw std::runtime_error("Failed to initialize GLFW.");
}
```

Assuming everything is initialized fine, the first order of business is setting up an error callback.
Whenever GLFW encounters an error, it calls the function registered as the error callback.
When first learning any system, its better to fail fast and obviously, rather than silently, so the registered function in this snippet will immediately throw the error's description if it is called.


## Setup for Platforms

The properties of a window managed by GLFW are controlled through **window hints**.
This includes the properties of the OpenGL context that will be used to render graphics for the window.

Different platforms support different features.
Apple, notably, doesn't have the best support for more recent versions of OpenGL.
To account for this, a preprocessor switch will provide window hints for the corresponding platform.

To keep things cross-platform compatible, we'll try to limit features to OpenGL 4.1.

```cpp
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
```

You may override any of these hints via the `hints` parameter of the `setup` function.

```cpp
for (auto&& hint : hints) {
	glfwWindowHint(hint.hint, hint.value);
}
```

All applied hints will affect the next window creation, performed here.
If the window fails to be created, it's best to throw.

```cpp
GLFWwindow* result = glfwCreateWindow(dimensions.x, dimensions.y, title, nullptr, nullptr);

if (result == nullptr) {
	glfwTerminate();
	throw std::runtime_error("Failed to create window.");
}
```

Assuming the window was created, the position on the screen is set, the window is made the current context for the thread (meaning the OpenGL operations done will affect the window), and the window is set to swap the frame buffer at the base refresh rate.

```cpp
glfwSetWindowPos(result, position.x, position.y);

glfwMakeContextCurrent(result);
glfwSwapInterval(1);
```

The code for this textbook loads OpenGL through glad.
You may get your own glad headers [through this website](https://glad.dav1d.de/).
Assuming it doesn't fail, this should set up a valid OpenGL context for the newly spawned window.

```cpp
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	throw std::runtime_error("Failed to initialize OpenGL context.");
}
return result;
```

