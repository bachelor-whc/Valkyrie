#include <memory>
#include "valkyrie/scene/light_shader_writer.h"
#include "valkyrie/scene/object_manager.h"
using namespace Valkyrie;

struct ShPositionLight {
    glm::vec4 position;
    glm::vec4 color;
    glm::vec3 atten_factor;
    float intensity;
};

LightShaderWriter::LightShaderWriter() : 
    m_position_lights(),
    m_position_light_buffer(),
    m_position_light_writes(),
    m_dummy_position_light_writes(MAX_POSITION_LIGHT) {
    m_dummy_position_light_buffer.allocate({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT }, sizeof(ShPositionLight));
    m_position_light_count_buffer.allocate({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT }, sizeof(uint32_t));
    for (int i = 0; i < MAX_POSITION_LIGHT; ++i) {
        m_dummy_position_light_writes[i] = m_dummy_position_light_buffer.getWriteSet();
        m_dummy_position_light_writes[i].dstArrayElement = i;
    }
}

LightShaderWriter::~LightShaderWriter() {
    Vulkan::DestroyMemoryBuffer(m_position_light_count_buffer);
}

bool LightShaderWriter::addLight(unsigned int ID) {
    if (std::find(m_position_lights.begin(), m_position_lights.end(), ID) != m_position_lights.end()) {
        return true;
    }
    auto& object_manager = ObjectManager::instance();
    Scene::ObjectPtr object_ptr = object_manager.getObjectPtr(ID);
    if (object_ptr == nullptr) {
        return false;
    }
    std::shared_ptr<Scene::PositionLight> light_ptr = nullptr;
    light_ptr = std::dynamic_pointer_cast<Scene::PositionLight>(object_ptr);
    if (light_ptr != nullptr && m_position_lights.size() < MAX_POSITION_LIGHT) {
        m_position_lights.push_back(ID);
        auto& memory_buffer = m_position_light_buffer[ID];
        memory_buffer.allocate({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT }, sizeof(ShPositionLight));
    }
    else {
        return false;
    }
    return true;
}

void LightShaderWriter::removeLight(unsigned int ID) {
    std::remove(m_position_lights.begin(), m_position_lights.end(), ID);
    if (m_position_light_buffer.count(ID) > 0) {
        Vulkan::DestroyMemoryBuffer(m_position_light_buffer[ID]);
        m_position_light_buffer.erase(ID);
    }
}

void LightShaderWriter::updateLights() {
    auto& object_manager = ObjectManager::instance();
    int size = m_position_lights.size();
    m_position_light_writes.clear();
    for (int i = 0; i < size; ++i) {
        auto object_ptr = object_manager.getObjectPtr(m_position_lights[i]);
        if (object_ptr == nullptr) {
            assert(false);
        }
        std::shared_ptr<Scene::PositionLight> light_ptr = nullptr;
        light_ptr = std::dynamic_pointer_cast<Scene::PositionLight>(object_ptr);
        if (light_ptr == nullptr) {
            continue;
        }
        auto& memory_buffer = m_position_light_buffer[light_ptr->getID()];
        if (light_ptr->isNeedUpdateBuffer()) {
            ShPositionLight sh_positionlight = {};
            sh_positionlight.position = glm::vec4(light_ptr->getPosition(), 1.0f);
            sh_positionlight.color = glm::vec4(light_ptr->getColor(), 1.0f);
            sh_positionlight.atten_factor = glm::vec4(light_ptr->getAttenFactor(), 1.0f);
            sh_positionlight.intensity = light_ptr->getIntensity();
            memory_buffer.write(&sh_positionlight, 0, sizeof(ShPositionLight));
        }
        m_position_light_writes.push_back(memory_buffer.getWriteSet());
        m_position_light_writes[i].dstArrayElement = i;
    }
    m_position_light_count_buffer.write(&size, 0, sizeof(int));
}

VkWriteDescriptorSet* LightShaderWriter::getPositionLightWrites() {
    return m_position_light_writes.data();
}

uint32_t LightShaderWriter::getPositionLightWritesCount() const {
    return m_position_light_writes.size();
}



Vulkan::MemoryBuffer LightShaderWriter::getPositionLightCountBuffer() const {
    return m_position_light_count_buffer;
}

VkWriteDescriptorSet * Valkyrie::LightShaderWriter::getDummyPositionLightWrites() {
    return m_dummy_position_light_writes.data() + m_position_light_writes.size();
}

uint32_t Valkyrie::LightShaderWriter::getDummyPositionLightWritesCount() const {
    return MAX_POSITION_LIGHT - m_position_light_writes.size();
}

