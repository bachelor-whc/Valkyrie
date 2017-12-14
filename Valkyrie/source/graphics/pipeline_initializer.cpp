#include <regex>
#include <queue>
#include <valkyrie/graphics/pipeline_initializer.h>
using namespace Valkyrie;
using namespace Graphics;

void PipelineShadersInitializer::initializeShaders(const std::string& filepath, Graphics::Pipeline& pipeline) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::string ex_message = "File " + filepath + " not open.";
        throw std::exception(ex_message.c_str());
    }
    file >> m_json;
    file.close();

    if (m_json.count("spvs") == 0) {
        throw std::exception("illegal descriptor, lack of spv file informations.");
    }

    for (auto& kv : m_json["spvs"].get<JSON::object_t>()) {
        auto& filename = kv.first;
        auto stage = kv.second.get<uint32_t>();
        Pipeline::ShaderStage val_stage;
        switch (stage)
        {
        case VK_SHADER_STAGE_VERTEX_BIT:
            val_stage = Pipeline::ShaderStage::VERTEX;
            break;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            val_stage = Pipeline::ShaderStage::FRAGMENT;
            break;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        case VK_SHADER_STAGE_GEOMETRY_BIT:
        case VK_SHADER_STAGE_COMPUTE_BIT:
        default:
            assert(false);
            break;
        }
        auto code = Vulkan::Shader::LoadSPVBinaryCode(filename);
        pipeline.shaderPtrs[val_stage] = MAKE_SHARED(Vulkan::Shader)(code, (VkShaderStageFlagBits)stage);
    }
};

void PipelineShadersInitializer::setShaderVariableName(const VertexShaderVariableType variable, const std::string& name) {
    assert(name.length() > 0);
    m_shader_variable_names[variable] = name;
}

std::string PipelineShadersInitializer::getShaderVariableName(const VertexShaderVariableType variable) const {
    if (m_shader_variable_names.count(variable) > 0) {
        return m_shader_variable_names.find(variable)->second;
    }
    else {
        throw std::exception("shader variable names i unset.");
    }
}

void PipelineShadersInitializer::setPipelineVertexInput(Graphics::Pipeline& pipeline, std::vector<uint32_t>&& default_attr_sizes) {
    if (m_json.count("variables") == 0 || m_json["variables"].count("attributes") == 0) {
        throw std::exception("lack of attributes informations");
    }

    static auto loaction_comparer = [](VkVertexInputAttributeDescription& lhs, VkVertexInputAttributeDescription& rhs) {
        return lhs.location > rhs.location;
    };

    std::priority_queue<
        VkVertexInputAttributeDescription,
        std::vector<VkVertexInputAttributeDescription>,
        decltype(loaction_comparer)
    > viad_queue(loaction_comparer);

    auto& json = m_json["variables"];

    for (const auto& kv : m_shader_variable_names) {
        VkVertexInputAttributeDescription vi_attribute_description = {};
        const auto& variable_name = kv.second;
        if (json["attributes"].count(variable_name) > 0) {
            const auto& variable_desc = json["attributes"][variable_name];
            if (variable_desc.count("location") > 0) {
                const auto location = variable_desc["location"].get<uint32_t>();
                if (location < 0 || location >= default_attr_sizes.size()) {
                    throw std::exception("location exceeds limitaion.");
                }
                vi_attribute_description.location = location;
            }
            if (variable_desc.count("binding") > 0) {
                vi_attribute_description.binding = variable_desc["binding"].get<uint32_t>();
            }
            const auto stype = variable_desc["basic_type"].get<std::string>();
            uint32_t type_size = GetTypeSize(stype);
            const auto vector_size = variable_desc["vector_size"].get<uint32_t>();
            vi_attribute_description.format = (VkFormat)variable_desc["type"].get<uint32_t>();
            vi_attribute_description.offset = vector_size * type_size;
            viad_queue.push(vi_attribute_description);
        }
    }

    static const auto accumulator = [&](const uint32_t& index) {
        uint32_t offset = 0;
        for (int i = 1; i <= index; ++i) {
            offset += default_attr_sizes[i - 1];
        }
        return offset;
    };

    pipeline.vertexInput.setBindingDescription(0, 32);
    while (viad_queue.size() > 0) {
        auto& viad = viad_queue.top();
        assert(viad.location < default_attr_sizes.size());
        default_attr_sizes[viad.location] = viad.offset;
        uint32_t offset = accumulator(viad.location);
        pipeline.vertexInput.setAttributeDescription(viad.binding, viad.location, viad.format, offset);
        viad_queue.pop();
    }
}

void PipelineShadersInitializer::initializePipelinePushConstantRanges(Graphics::Pipeline& pipeline) {
    if (m_json.count("variables") == 0 || m_json["variables"].count("push_constant") == 0) {
        return;
    }

    for (auto& kv : m_json["variables"]["push_constant"].get<JSON::object_t>()) {
        auto& pc_json = kv.second;
        VkPushConstantRange pcr = {};
        pcr.size = pc_json["block_size"].get<uint32_t>();
        pcr.stageFlags = pc_json["stage"].get<uint32_t>();
        m_push_constant_ranges.push_back(pcr);
    }

    pipeline.module.pushConstantRanges = m_push_constant_ranges;
}