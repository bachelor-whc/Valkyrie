#include "valkyrie/scene/light_position.h"
#include "valkyrie/vulkan/command_buffer.h"
using namespace Valkyrie::Scene;

PositionLight::PositionLight() : 
    Light(Type::POSITION),
    m_dirty(){

}

PositionLight::~PositionLight() {

}

void PositionLight::setColor(const glm::vec3& color) {
    Light::setColor(color);
    m_dirty.enableDirtyFlag(COLOR);
}

void PositionLight::setPosition(const glm::vec3& position) {
    m_position = position;
    m_dirty.enableDirtyFlag(POSITION);
}

glm::vec3 PositionLight::getPosition() const {
    return m_position;
}

void PositionLight::setAttenFactor(const glm::vec3 & atten_factor) {
    m_atten_factor = atten_factor;
    m_dirty.enableDirtyFlag(ATTEN_FACTOR);
}

glm::vec3 PositionLight::getAttenFactor() const
{
    return m_atten_factor;
}

void PositionLight::setIntensity(const float intensity) {
    m_intensity = intensity;
    m_dirty.enableDirtyFlag(INTENSITY);
}

float PositionLight::getIntensity() const {
    return m_intensity;
}

bool PositionLight::_isNeedUpdateBuffer() const {
    return m_dirty.dirty();
}

void PositionLight::_resetDirty() {
    m_dirty.reset();
}


