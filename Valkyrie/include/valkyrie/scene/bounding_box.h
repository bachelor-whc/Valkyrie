#ifndef _BOOBS_ROUNDING_BOX_H_
#define _BOOBS_ROUNDING_BOX_H_
#include <glm/glm.hpp>

namespace Valkyrie {
	class RoundingBox {
	public:
		RoundingBox();
		RoundingBox(const glm::vec3& position, const glm::vec3& diagonal);
		virtual ~RoundingBox();

		bool overlap(const RoundingBox& rhs) const;

		glm::vec3 min;
		glm::vec3 max;
	};
}

Valkyrie::RoundingBox operator&(const Valkyrie::RoundingBox& lhs, const Valkyrie::RoundingBox& rhs);
Valkyrie::RoundingBox operator|(const Valkyrie::RoundingBox& lhs, const Valkyrie::RoundingBox& rhs);

#endif