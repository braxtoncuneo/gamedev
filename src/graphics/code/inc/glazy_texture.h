
#ifndef GLAZY_TEXTURE
#define GLAZY_TEXTURE

#include "glazy_common.h"

namespace glazy {

	class Texture {

		GLuint id;

	public:

		struct RGB8 {
			unsigned char r;
			unsigned char g;
			unsigned char b;
		};

		Texture(std::vector<RGB8> data, size_t width, size_t height, bool mipmap);
		Texture(Texture&) = delete;
		~Texture();
		operator GLuint();

	};

	class Sampler {

		GLuint id;

	public:

		Sampler();
		Sampler(Sampler&) = delete;
		~Sampler();

	};


}


#endif

