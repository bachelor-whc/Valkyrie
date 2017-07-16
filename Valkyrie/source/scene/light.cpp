#include "valkyrie/scene/light.h"
using namespace Valkyrie::Scene;

Light::Light(const Type type) : m_type(type) {

}

Light::~Light() {

}

void Light::setColor(const glm::vec3& color) {
	m_color = color;
}

glm::vec3 Light::getColor() {
	return m_color;
}

Light::Type Light::getType() {
	return m_type;
}