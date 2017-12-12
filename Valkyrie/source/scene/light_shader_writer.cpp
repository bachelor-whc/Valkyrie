#include <memory>
#include "valkyrie/scene/light_shader_writer.h"
#include "valkyrie/scene/object_manager.h"
#include "valkyrie/vulkan/physical_device.h"
using namespace Valkyrie;

struct ShPositionLight {
    glm::vec4 position;
    glm::vec4 color;
    glm::vec3 atten_factor;
    float intensity;
};

LightShaderWriter::LightShaderWriter() : 
    m_position_lights(),
    m_position_lights_buffer() {
    const auto position_light_buffer_size = Vulkan::PhysicalDevice::getAlignedMemorySize(sizeof(ShPositionLight), MAX_POSITION_LIGHT);
    m_position_lights_buffer.allocate({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT }, position_light_buffer_size);
    m_position_light_count_buffer.allocate({ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT }, sizeof(uint32_t));

    const auto step = Vulkan::PhysicalDevice::getAlignedMemoryStep(sizeof(ShPositionLight));
    for (int i = 0; i < MAX_POSITION_LIGHT; ++i) {
        m_position_light_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        m_position_light_write.descriptorCount = 1;
        m_position_light_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        m_position_light_buffer_info.buffer = m_position_lights_buffer.handle;
        m_position_light_buffer_info.range = position_light_buffer_size;
        m_position_light_write.pBufferInfo = &m_position_light_buffer_info;
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
    }
    else {
        return false;
    }
    return true;
}

void LightShaderWriter::removeLight(unsigned int ID) {
    std::remove(m_position_lights.begin(), m_position_lights.end(), ID);
}

void LightShaderWriter::updateLights() {
    const auto step = Vulkan::PhysicalDevice::getAlignedMemoryStep(sizeof(ShPositionLight));
    auto& object_manager = ObjectManager::instance();
    int size = m_position_lights.size();
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
        if (light_ptr->isNeedUpdateBuffer()) {
            ShPositionLight sh_positionlight = {};
            sh_positionlight.position = glm::vec4(light_ptr->getPosition(), 1.0f);
            sh_positionlight.color = glm::vec4(light_ptr->getColor(), 1.0f);
            sh_positionlight.atten_factor = glm::vec4(light_ptr->getAttenFactor(), 1.0f);
            sh_positionlight.intensity = light_ptr->getIntensity();
            m_position_lights_buffer.write(&sh_positionlight, step * i, sizeof(ShPositionLight));
        }
        light_ptr->resetDirty();
    }
    m_position_light_count_buffer.write(&size, 0, sizeof(int));
}

VkWriteDescriptorSet LightShaderWriter::getPositionLightWrite() {
    return m_position_light_write;
}

Vulkan::MemoryBuffer LightShaderWriter::getPositionLightCountBuffer() const {
    return m_position_light_count_buffer;
}
