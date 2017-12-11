#include <regex>
#include <queue>
#include <valkyrie/graphics/pipeline_initializer.h>
using namespace Valkyrie;

void PipelineShadersInitializer::loadSPVBinaryCode(const std::string& filepath, const std::string& sri_filepath, VkShaderStageFlagBits stage_flag) {
    if (stage_flag == VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM) {
        // Judge by filename
        static const std::map<VkShaderStageFlagBits, std::regex> search_map = {
            { VK_SHADER_STAGE_VERTEX_BIT, std::regex("vert") },
            { VK_SHADER_STAGE_FRAGMENT_BIT, std::regex("frag") }
        };
        for (const auto& kv : search_map) {
            if (std::regex_search(filepath, kv.second)) {
                stage_flag = kv.first;
            }
        }
    }
    if (stage_flag == VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM) {
        throw std::exception("stage cannot be determined.");
    }

    Graphics::Pipeline::ShaderStage stage = Graphics::Pipeline::ShaderStage::UNKNOWN;
    switch (stage_flag)
    {
    case VK_SHADER_STAGE_VERTEX_BIT:
        stage = Graphics::Pipeline::ShaderStage::VERTEX;
        break;
    case VK_SHADER_STAGE_FRAGMENT_BIT:
        stage = Graphics::Pipeline::ShaderStage::FRAGMENT;
        break;
    default:
        assert(false);
        break;
    }

    auto code = Vulkan::Shader::LoadSPVBinaryCode(filepath);
    auto shader_ptr = MAKE_SHARED(Vulkan::Shader)(code, stage_flag);

    if (m_shader_ptrs.count(stage))
        throw std::exception("Stage have been set shader, stage can only be set to exact one shader respectively.");

    m_shader_ptrs[stage] = shader_ptr;
    std::ifstream file(sri_filepath);
    if (!file.is_open()) {
        std::string ex_message = "File " + sri_filepath + " not open.";
        throw std::exception(ex_message.c_str());
    }
    JSON json;
    file >> json;
    file.close();
    m_raw_jsons[shader_ptr] = json;
};

void PipelineShadersInitializer::initializePipelineShaders(Graphics::Pipeline& pipeline) const {
    for (const auto& kv : m_shader_ptrs) {
        const auto& stage = kv.first;
        const auto& shader_ptr = kv.second;
        assert(shader_ptr != nullptr);
        pipeline.shaderPtrs[stage] = shader_ptr;
    }
}

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
    if (m_shader_ptrs.count(Graphics::Pipeline::ShaderStage::VERTEX) > 0) {
        auto& shader_ptr = m_shader_ptrs[Graphics::Pipeline::ShaderStage::VERTEX];
        if (m_raw_jsons.count(shader_ptr) > 0 && m_shader_variable_names.size() > 0) {
            static auto loaction_comparer = [](VkVertexInputAttributeDescription& lhs, VkVertexInputAttributeDescription& rhs) {
                return lhs.location > rhs.location;
            };
            auto json = m_raw_jsons[shader_ptr];
            std::priority_queue<
                VkVertexInputAttributeDescription,
                std::vector<VkVertexInputAttributeDescription>,
                decltype(loaction_comparer)
            > viad_queue(loaction_comparer);

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
        else {
            throw std::exception("is lack of vertex shader information or variable name.");
        }
    }
    else {
        throw std::exception("is lack of vertex shader.");
    }
}

void PipelineShadersInitializer::initializePipelinePushConstantRanges(Graphics::Pipeline& pipeline) {
    for (const auto& kv : m_shader_ptrs) {
        const auto& stage = kv.first;
        const auto& shader_ptr = kv.second;
        if (m_raw_jsons.count(shader_ptr) > 0) {
            const auto& json = m_raw_jsons.find(shader_ptr)->second;

            auto& flatten = json.flatten();
            std::regex re("/(\\w+)/(\\w+)/(push_constant)");
            std::smatch match;
            for (auto it = flatten.begin(); it != flatten.end(); ++it)
            {
                auto key = it.key();
                if (it.value().is_number_integer() &&
                    it.value().get<int>() == 1 &&
                    std::regex_match(key, match, re)) {
                    const auto& v_area = match[1];
                    const auto& v_name = match[2];
                    VkPushConstantRange pcr = {};
                    switch (stage)
                    {
                    case Graphics::Pipeline::ShaderStage::VERTEX:
                        pcr.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                        break;
                    case Graphics::Pipeline::ShaderStage::FRAGMENT:
                        pcr.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                        break;
                    default:
                        assert(false);
                        break;
                    }
                    auto& jvariable = json[v_area][v_name];
                    if (jvariable["basic_type"].get<std::string>() == "block") {
                        pcr.size = jvariable["block_size"].get<uint32_t>();
                    }
                    m_push_constant_ranges[stage].resize(1);
                    m_push_constant_ranges[stage][0] = pcr;
                    break;
                }
            }
        }
    }
    unsigned short count = 0;
    for (auto& kv : m_push_constant_ranges) {
        const auto& key = kv.first;
        const auto& value = kv.second;
        for (const auto& pcr : value)
            pipeline.module.pushConstantRanges.push_back(pcr);
    }
}