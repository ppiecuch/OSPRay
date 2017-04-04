#include "render/Renderer.h"


#undef  OSP_PLUGIN_INSTANCE
#define OSP_PLUGIN_INSTANCE(external_name) extern "C" void register_plugin_instance_##external_name()
#include "static_plugins_instance.h"

namespace ospray {

	void staticInit()
	{
#undef  OSP_PLUGIN_INSTANCE
#define OSP_PLUGIN_INSTANCE(external_name) register_plugin_instance_##external_name()
#include "static_plugins_instance.h"
	}

} // :ospray
