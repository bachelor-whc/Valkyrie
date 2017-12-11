#include "valkyrie/scene/light_shader_writer.h"
#include "valkyrie/scene/object_manager.h"

Valkyrie::LightShaderWriter::LightShaderWriter() : 
    m_position_lights(MAX_POSITION_LIGHT)
{

}

Valkyrie::LightShaderWriter::~LightShaderWriter() {

}

void Valkyrie::LightShaderWriter::addLight(unsigned int ID) {
    if (std::find(m_position_lights.begin(), m_position_lights.end(), ID) != m_position_lights.end()) {
        return;
    }
    auto& object_manager = ObjectManager::instance();
    auto object_ptr = object_manager.getObjectPtr(ID);
    if (object_ptr != nullptr) {
        m_position_lights.push_back(ID);
    }
}

void Valkyrie::LightShaderWriter::removeLight(unsigned int ID) {
    std::remove(m_position_lights.begin(), m_position_lights.end(), ID);
}

void Valkyrie::LightShaderWriter::updateLights() {

}
