#include <glm/gtc/matrix_transform.hpp>
#include "valkyrie/scene/object.h"

const glm::vec3 Valkyrie::Scene::Object::s_x_axis(1.0f, 0.0f, 0.0f);
const glm::vec3 Valkyrie::Scene::Object::s_y_axis(0.0f, 1.0f, 0.0f);
const glm::vec3 Valkyrie::Scene::Object::s_z_axis(0.0f, 0.0f, 1.0f);

Valkyrie::Scene::Object::Object() :
	world(1.0f),
	position(),
	rotation(),
	scale() {

}

Valkyrie::Scene::Object::~Object() {

}

void Valkyrie::Scene::Object::start() {

}

void Valkyrie::Scene::Object::update() {
	world = glm::mat4(1.0f);
	world = glm::scale(world, scale);
	world = glm::rotate(world, rotation.x, s_x_axis);
	world = glm::rotate(world, rotation.y, s_y_axis);
	world = glm::rotate(world, rotation.z, s_z_axis);
	world = glm::translate(world, position);
}