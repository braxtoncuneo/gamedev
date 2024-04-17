
#ifndef GLAZY_COMMON
#define GLAZY_COMMON

#include "glad.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include <fstream>
#include <string>



namespace glazy {


	namespace flags {
		extern bool debug;
	}

	namespace file {
		size_t file_size(std::ifstream& file);
		std::string read_file_to_string(std::string file_path);
	}

	namespace type {

		template<typename T>
		struct type_mapping;


		template<typename T, glm::length_t L, glm::qualifier Q>
		struct type_mapping<glm::vec<L, T, Q>> {
			static GLenum const value = type_mapping<T>::value;
		};

		template<typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
		struct type_mapping <glm::mat<C, R, T, Q>> {
			static GLenum const value = type_mapping<T>::value;
		};

		template<> struct type_mapping<GLbyte> { static GLenum const value = GL_BYTE; };
		template<> struct type_mapping<GLshort> { static GLenum const value = GL_SHORT; };
		template<> struct type_mapping<GLint> { static GLenum const value = GL_INT; };
		template<> struct type_mapping<GLubyte> { static GLenum const value = GL_UNSIGNED_BYTE; };
		template<> struct type_mapping<GLushort> { static GLenum const value = GL_UNSIGNED_SHORT; };
		template<> struct type_mapping<GLuint> { static GLenum const value = GL_UNSIGNED_INT; };
		template<> struct type_mapping<GLfloat> { static GLenum const value = GL_FLOAT; };
		template<> struct type_mapping<GLdouble> { static GLenum const value = GL_DOUBLE; };

		template<typename T>
		struct vec_length {
			static glm::length_t const value = 1;
		};

		template<typename T, glm::length_t L, glm::qualifier Q>
		struct vec_length<glm::vec<L, T, Q>> {
			static glm::length_t const value = L;
		};

	}


	namespace safety {
		void auto_throw(std::string context);
		void entry_guard(std::string fn_name);
		void exit_guard(std::string fn_name);
	}

	namespace context {
		struct WindowHint {
			int hint;
			int value;
		};
		void error_callback(int error_code, char const* desc);
		GLFWwindow* setup(glm::ivec2 position, glm::ivec2 dimensions, const char* title, std::vector<WindowHint> hints);
	}

}

#endif
