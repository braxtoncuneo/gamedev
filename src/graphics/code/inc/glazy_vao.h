

#ifndef GLAZY_VAO
#define GLAZY_VAO

#include "glazy_buffer.h"

namespace glazy {

	class SharedVAO;

	class VAO {
		GLuint id;
	public:

		VAO();
		VAO(VAO&& other);
		~VAO();
		operator GLuint() const;

		class BindGuard {
			static std::stack<GLuint> bind_stack;
			VAO& vao;
		public:
			BindGuard(VAO& vao);
			BindGuard(SharedVAO& svao);
			~BindGuard();
		};

		void bind();


		class Attribute {
			VAO& vao;
			size_t index;
		public:

			Attribute(VAO& vao, size_t index);
			Attribute& enable();
			Attribute& disable();

			template<typename T>
			Attribute& operator=(Buffer<T>& other) {
				safety::entry_guard("AttributeAccessor::operator=");
				{
					BindGuard bind_guard(vao);
					ArrayBindGuard<T> array_guard(other);
					GLenum typing = type::type_mapping<T>::value;
					std::string error_message = "Invalid attribute type ";
					switch (typing) {
					case GL_BYTE: case GL_UNSIGNED_BYTE: case GL_SHORT: case GL_UNSIGNED_SHORT: case GL_INT: case GL_UNSIGNED_INT:
						glVertexAttribIPointer(index, type::vec_length<T>::value, typing, 0, nullptr);
						break;
					case GL_HALF_FLOAT: case GL_FLOAT: case GL_DOUBLE:
						glVertexAttribPointer(index, type::vec_length<T>::value, typing, false, 0, nullptr);
						break;
					default:
						throw std::runtime_error(error_message + std::to_string(typing));
					}
				}
				safety::exit_guard("AttributeAccessor::operator=");
				return *this;
			}


		};

		Attribute operator[] (size_t index);


	};


	class SharedVAO {

		std::shared_ptr<VAO> vao;

	public:

		SharedVAO();

		operator GLuint();
		operator VAO& ();
		VAO::Attribute operator[] (size_t index);

	};


}

#endif
