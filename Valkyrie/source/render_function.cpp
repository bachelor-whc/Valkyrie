#include "utility.h"
#include "valkyrie/render_function.h"

Valkyrie::RenderFunction::RenderFunction() {

}

Valkyrie::RenderFunction::~RenderFunction() {

}

void Valkyrie::RenderFunction::render(const std::vector<void*>& data, uint32_t current_buffer) {
	implement(data, current_buffer);
}