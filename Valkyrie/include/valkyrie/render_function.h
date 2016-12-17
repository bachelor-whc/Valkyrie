#ifndef _RENDER_FUNCTION_H
#define _RENDER_FUNCTION_H
#include "utility.h"

class ValkyrieRenderFunction {
public:
	ValkyrieRenderFunction();
	virtual ~ValkyrieRenderFunction();
	void render(const std::vector<void*>& data, uint32_t current_buffer);
private:
	virtual void implement(const std::vector<void*>& data, uint32_t current_buffer) = 0;
};

typedef std::shared_ptr<ValkyrieRenderFunction> ValkyrieRenderPFN;
#endif