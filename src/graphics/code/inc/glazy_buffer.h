

#ifndef GLAZY_BUFFER
#define GLAZY_BUFFER

#include "glazy_common.h"


namespace glazy {

	namespace compat {

		// macOS does not support OpenGL 4.5, and so does not have compat::named_buffer_data.
		// To fix this, we have the janky solution of rapidly swapping the original
		// GL_ARRAY_BUFFER out, binding our buffer briefly to perform the operation,
		// and then swapping the original back in. This is less inefficient, but it
		// is the price that must be paid for ergonomic data movement without
		// introducing nasty, hard-to-debug side effects.

		void named_buffer_data(GLuint id, size_t size, void* data, GLenum usage);
		void* map_named_buffer(GLuint id, GLenum access);
		void unmap_named_buffer(GLuint id);
	}


	template<typename T>
	class SharedBuffer;

	template<typename T>
	class ArrayBindGuard;

	template <typename T>
	class Buffer {

		friend class ArrayBindGuard<T>;

		GLuint  id;
		void* mapping;

	public:



		Buffer()
			: mapping(nullptr)
		{
			safety::entry_guard("Buffer::Buffer()");
			id = 0;
			glGenBuffers(1, &id);
			if (id == 0) {
				throw std::runtime_error("Failed to allocate id for buffer.");
			}
			safety::exit_guard("Buffer::Buffer()");
		}


		Buffer(Buffer&& other)
			: id(other.id)
			, mapping(other.mapping)
		{
			safety::entry_guard("Buffer::Buffer(Buffer&&)");
			other.id = 0;
			other.mapping = nullptr;
			safety::exit_guard("Buffer::Buffer(Buffer&&)");
		}

		~Buffer() {
			if (glIsBuffer(id)) {
				glDeleteBuffers(1, &id);
			}
		}

		operator GLuint() const {
			return id;
		}

		void bind(GLenum target) {
			safety::entry_guard("Buffer::bind");
			glBindBuffer(target, id);
			safety::exit_guard("Buffer::bind");
		}



		void set_data(T& data, GLenum usage) {
			safety::entry_guard("Buffer::set_data");
			compat::named_buffer_data(id, sizeof(T), &data, usage);
			safety::exit_guard("Buffer::set_data");
		}
		void set_data(T&& data, GLenum usage) {
			set_data(data, usage);
		}

		void set_data(std::vector<T>& data, GLenum usage) {
			safety::entry_guard("Buffer::set_data");
			compat::named_buffer_data(id, sizeof(T) * data.size(), data.data(), usage);
			safety::exit_guard("Buffer::set_data");
		}
		void set_data(std::vector<T>&& data, GLenum usage) {
			set_data(data, usage);
		}

		void map(GLenum access) {
			safety::entry_guard("Buffer::map");
			void* new_mapping = compat::map_named_buffer(id, access);
			if (new_mapping != nullptr) {
				mapping = new_mapping;
			}
			safety::exit_guard("Buffer::map");
		}

		void unmap() {
			safety::entry_guard("Buffer::map");
			compat::unmap_named_buffer(id);
			mapping = 0;
			safety::exit_guard("Buffer::map");
		}

		T& operator[](size_t index) {
			safety::entry_guard("Buffer::operator[]");
			if (mapping == nullptr) {
				throw std::runtime_error("Attempted to index Buffer without an active mapping.");
			}
			safety::exit_guard("Buffer::operator[]");
			return (reinterpret_cast<T*>(mapping))[index];
		}


	};

	template<typename T>
	class ArrayBindGuard {

		static std::stack<GLuint> bind_stack;

		Buffer<T>& buffer;

	public:
		ArrayBindGuard(Buffer<T>& buffer)
			: buffer(buffer)
		{
			safety::entry_guard("Buffer::BindGuard::BindGuard");
			if (bind_stack.empty() || (bind_stack.top() != buffer.id)) {
				glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
			}
			bind_stack.push(buffer.id);
			safety::exit_guard("Buffer::BindGuard::BindGuard");
		}

		ArrayBindGuard(SharedBuffer<T>& sbuf)
			: buffer(sbuf)
		{
			safety::entry_guard("Buffer::BindGuard::BindGuard");
			if (bind_stack.empty() || (bind_stack.top() != buffer.id)) {
				glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
			}
			bind_stack.push(buffer.id);
			safety::exit_guard("Buffer::BindGuard::BindGuard");
		}

		~ArrayBindGuard() {
			if (bind_stack.empty()) {
				std::cerr << "ERROR: BindGuard has run more destructors than constructors! "
					"Do not manually invoke the destructor of a BindGuard!\n";
				return;
			}
			bind_stack.pop();
			if ((!bind_stack.empty()) && (bind_stack.top() != buffer.id)) {
				glBindBuffer(GL_ARRAY_BUFFER, bind_stack.top());
			}
		}
	};

	template<typename T>
	std::stack<GLuint> ArrayBindGuard<T>::bind_stack;


	template <typename T>
	class SharedBuffer {

		std::shared_ptr<Buffer<T>> buffer;

	public:

		// Allocates a buffer ID by default
		SharedBuffer() : buffer(new Buffer<T>()) {}

		// The default copy constructor, assignment operator,
		// and destructor should be fine, since garbage collection
		// is managed by the shared pointer

		operator GLuint() const {
			return (*buffer);
		}

		operator Buffer<T>& () const {
			return (*buffer);
		}

		void bind(GLenum target) {
			buffer->bind(target);
		}

		void set_data(T& data, GLenum usage) {
			buffer->set_data(data, usage);
		}
		void set_data(T&& data, GLenum usage) {
			buffer->set_data(data, usage);
		}

		void set_data(std::vector<T>& data, GLenum usage) {
			buffer->set_data(data, usage);
		}
		void set_data(std::vector<T>&& data, GLenum usage) {
			buffer->set_data(data, usage);
		}

		void map(GLenum access) {
			buffer->map(access);
		}

		void unmap() {
			buffer->unmap();
		}

		T& operator[](size_t index) {
			return (*buffer)[index];
		}

	};

}

#endif

