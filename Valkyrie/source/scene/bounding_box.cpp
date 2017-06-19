#include "valkyrie/scene/bounding_box.h"
using namespace Valkyrie;

Scene::RoundingBox::RoundingBox() :
	min(),
	max(INFINITY, INFINITY, INFINITY) {

}

Scene::RoundingBox::RoundingBox(const glm::vec3& position, const glm::vec3& diagonal) {
	auto&& min_temp = position;
	auto&& max_temp = diagonal;
	min = glm::min(min_temp, max_temp);
	max = glm::max(min_temp, max_temp);
}

Scene::RoundingBox::~RoundingBox() {

}

bool Scene::RoundingBox::overlap(const RoundingBox& rhs) const {
	bool x = (max.x >= rhs.min.x) && (min.x <= rhs.max.x);
	bool y = (max.y >= rhs.min.y) && (min.y <= rhs.max.y);
	bool z = (max.z >= rhs.min.z) && (min.z <= rhs.max.z);
	return (x && y && z);
}

Valkyrie::Scene::RoundingBox operator&(const Valkyrie::Scene::RoundingBox& lhs, const Valkyrie::Scene::RoundingBox& rhs) {
	if (!lhs.overlap(rhs)) {
		return Scene::RoundingBox();
	}
	return Scene::RoundingBox(
		glm::min(lhs.max, rhs.max),
		glm::max(lhs.min, rhs.min)
	);
}

Valkyrie::Scene::RoundingBox operator|(const Valkyrie::Scene::RoundingBox& lhs, const Valkyrie::Scene::RoundingBox& rhs) {
	return Scene::RoundingBox(
		glm::min(lhs.min, rhs.min),
		glm::max(lhs.max, rhs.max)
	);
}
