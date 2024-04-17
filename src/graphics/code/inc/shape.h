
#ifndef GLAZY_SHAPE
#define GLAZY_SHAPE

#include <glm/glm.hpp>
#include <vector>

namespace glazy {

	namespace shape {

		std::vector<glm::vec2> uv_grid(size_t w, size_t h);
		std::vector<glm::vec3> quad();
		std::vector<glm::vec3> box(float w, float h, float d);
		std::vector<glm::vec3> sphere(size_t wedges, size_t layers, float radius);

	}

}

#endif GLAZY_SHAPE

