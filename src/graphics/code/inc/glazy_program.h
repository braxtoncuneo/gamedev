
#ifndef GLAZY_PROGRAM
#define GLAZY_PROGRAM

#include "glazy_common.h"


namespace glazy {

	template <GLenum KIND>
	class Shader {

	private:
		bool   empty;
		GLuint id;

		void check_compilation() {
			safety::entry_guard("Shader::check_compilation");
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
			safety::exit_guard("Shader::check_compilation");
		}

	public:

		Shader() : id(0), empty(true) {}

		Shader(std::string text) :
			id(glCreateShader(KIND)),
			empty(false)
		{
			safety::entry_guard("Shader::Shader(std::string)");
			if (id == 0) {
				throw std::runtime_error("Failed to allocate id for shader.");
			}
			GLint const length = text.size();
			GLchar const* code = reinterpret_cast<GLchar const*>(text.c_str());
			glShaderSource(id, 1, &code, &length);
			glCompileShader(id);
			check_compilation();
			safety::exit_guard("Shader::Shader(std::string)");
		}

		static Shader from_file(std::string path) {
			return Shader(file::read_file_to_string(path));
		}

		~Shader() {
			glDeleteShader(id);
		}

		bool is_empty() {
			return empty;
		}

		operator GLuint() {
			return id;
		}

	};

	namespace uniform {

		template<typename T> void SetUniformV(GLint loc, size_t c, T* ptr) {
			throw std::runtime_error("Templated type deduction not supported for this input type.");
		}

		template<> void SetUniformV(GLint loc, size_t c, GLfloat* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::vec2* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::vec3* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::vec4* ptr);
		template<> void SetUniformV(GLint loc, size_t c, GLint* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::ivec2* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::ivec3* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::ivec4* ptr);
		template<> void SetUniformV(GLint loc, size_t c, GLuint* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::uvec2* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::uvec3* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::uvec4* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::mat2* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::mat3* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::mat4* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::mat2x3* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::mat3x2* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::mat2x4* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::mat4x2* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::mat3x4* ptr);
		template<> void SetUniformV(GLint loc, size_t c, glm::mat4x3* ptr);

		template<typename T>
		struct UniformSetter {
			static void set(GLint loc, T& inp) {
				SetUniformV(loc, 1, &inp);
			}
		};
		
		template<>
		struct UniformSetter<GLboolean> {
			static void set(GLint loc, GLboolean& inp) {
				GLint val = inp;
				SetUniformV(loc, 1, &val);
			}
		};
		

		template<typename T>
		struct UniformSetter <std::vector<T>> {
			static void set(GLint loc, std::vector<T>& inp) {
				SetUniformV(loc, inp.size(), inp.data());
			}
		};
	}

	template<typename T>
	void SetUniform(GLint loc, T& inp) {
		uniform::UniformSetter<T>::set(loc, inp);
	}


	class GPUProgram;

	class GPUAccessor {
	private:
		GPUProgram& prog;
		std::string name;
	public:
		GPUAccessor(GPUProgram& prog, std::string name);

		template<typename T>
		void operator=(T other);

	};


	class GPUProgram {
	private:
		GLuint id;
		void check_linking();
	public:

		void attach(GLuint shader_id);
		GPUProgram(
			Shader<GL_VERTEX_SHADER>          vertex,
			Shader<GL_TESS_CONTROL_SHADER>    tess_cont,
			Shader<GL_TESS_EVALUATION_SHADER> tess_eval,
			Shader<GL_GEOMETRY_SHADER>        geometry,
			Shader<GL_FRAGMENT_SHADER>        fragment
		);
		operator GLuint();
		GPUAccessor operator[](std::string name);

		class BindGuard {

			static std::stack<GLuint> bind_stack;

			GPUProgram& program;

		public:
			BindGuard(GPUProgram& program);
			~BindGuard();
		};


		GLint attribute_index(std::string name);

	};

	template<typename T>
	void GPUAccessor::operator=(T other) {
		safety::entry_guard("GPUProgram::GPUAccessor::operator=");
		{
			GPUProgram::BindGuard guard(prog);
			GLint loc = glGetUniformLocation(prog, reinterpret_cast<GLchar const*>(name.c_str()));
			if (loc < 0) {
				throw std::runtime_error("Invalid uniform name '" + name + "'");
			}
			else {
				SetUniform(loc, other);
			}
		}
		safety::exit_guard("GPUProgram::GPUAccessor::operator=");
	}



}


#endif
