// Direct3D rendering initialization

#include "engine.h"
#include "render.h"

IDXGIFactory7 *g_dxgi_factory;

bool engine_directx_create_dxgi_factory(void)
{
	PURPL_LOG(RENDER_LOG_PREFIX "Creating IDXGIFactory1\n");
	return SUCCEEDED(CreateDXGIFactory1(UUIDOF(IDXGIFactory7), &g_dxgi_factory));
}

bool engine_directx_init(void)
{
	if (g_engine->dev) {
		PURPL_LOG(RENDER_LOG_PREFIX "Enabling debug layer\n");
		ID3D12Debug *debug;
		if (!(SUCCEEDED(D3D12GetDebugInterface(UUIDOF(ID3D12Debug), &debug)))) {
			PURPL_LOG(RENDER_LOG_PREFIX "Failed to enable debug layer\n");
			engine_directx_shutdown();
			return false;
		}
		ID3D12Debug_EnableDebugLayer(debug);
		ID3D12Debug_Release(debug);
	}

	if (!engine_directx_create_dxgi_factory()) {
		engine_directx_shutdown();
		return false;
	}

	engine_directx_create_device();

	return true;
}

bool engine_directx_begin_frame(uint64_t delta)
{
	return false;
}

bool engine_directx_end_frame(uint64_t delta)
{
	return false;
}

void engine_directx_shutdown(void) {}
