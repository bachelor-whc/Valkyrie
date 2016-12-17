#include "utility.h"
#include "valkyrie/render_function.h"

ValkyrieRenderFunction::ValkyrieRenderFunction() {

}

ValkyrieRenderFunction::~ValkyrieRenderFunction() {

}

void ValkyrieRenderFunction::render(const std::vector<void*>& data, uint32_t current_buffer) {
	implement(data, current_buffer);
}