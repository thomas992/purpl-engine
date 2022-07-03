// Direct3D rendering initialization

#include "engine.h"
#include "render.h"

IDXGIFactory7 *g_dxgi_factory;

bool engine_directx_create_dxgi_factory(void)
{
	HRESULT result;

	PURPL_LOG(RENDER_LOG_PREFIX "Creating IDXGIFactory7\n");
	result = CreateDXGIFactory1(UUIDOF(IDXGIFactory7), &g_dxgi_factory);
	if (!SUCCEEDED(result)) {
		PURPL_LOG(RENDER_LOG_PREFIX "Failed to create IDXGIFactory7: HRESULT %lX\n", result);
		return false;
	}

	return true;
}

bool engine_directx_init(void)
{
	PURPL_LOG(RENDER_LOG_PREFIX "Initializing Direct3D 12\n");

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

void engine_directx_shutdown(void)
{
	PURPL_LOG(RENDER_LOG_PREFIX "Shutting down Direct3D 12\n");

	if (g_d3d_device) {
		PURPL_LOG(RENDER_LOG_PREFIX "Releasing device\n");
		ID3D12Device9_Release(g_d3d_device);
	}
	if (g_dxgi_factory) {
		PURPL_LOG(RENDER_LOG_PREFIX "Releasing IDXGIFactory7\n");
		IDXGIFactory7_Release(g_dxgi_factory);
	}
}
