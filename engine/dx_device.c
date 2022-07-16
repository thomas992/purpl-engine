// Direct3D device selection and creation code

#include "engine.h"
#include "render.h"

ID3D12Device9 *g_d3d_device;

static size_t adapter_get_vram(DXGI_ADAPTER_DESC3 *desc)
{
	size_t vram;

	if (desc->DedicatedVideoMemory != 0)
		vram = desc->DedicatedVideoMemory;
	else
		vram = desc->DedicatedSystemMemory;

	return vram;
}

bool engine_directx_create_device(void)
{
	HRESULT result;
	IDXGIAdapter4 *adapter;
	DXGI_ADAPTER_DESC3 adapter_desc;
	double adapter_vram;

	PURPL_LOG(RENDER_LOG_PREFIX "Creating device\n");
	result = IDXGIFactory7_EnumAdapters1(g_dxgi_factory, g_engine->device_idx, (IDXGIAdapter1 **)&adapter);
	if (!SUCCEEDED(result)) {
		PURPL_LOG(RENDER_LOG_PREFIX "Failed to get adapter %u: HRESULT 0x%X\n", g_engine->device_idx, result);
		return false;
	}

	IDXGIAdapter4_GetDesc3(adapter, &adapter_desc);
	adapter_vram = (double)adapter_get_vram(&adapter_desc);
	PURPL_LOG(RENDER_LOG_PREFIX "Using adapter %u %ls (%.0lf GB/%.0lf MiB VRAM, PCI ID %0.4x:%0.4x, revision %u)\n",
		  g_engine->device_idx, adapter_desc.Description, round(adapter_vram / 1000 / 1000 / 1000),
		  adapter_vram / 1024 / 1024, adapter_desc.VendorId, adapter_desc.DeviceId, adapter_desc.Revision);

	result = D3D12CreateDevice((IUnknown *)adapter, D3D_FEATURE_LEVEL_12_1, UUIDOF(ID3D12Device1), &g_d3d_device);
	if (!SUCCEEDED(result)) {
		PURPL_LOG(RENDER_LOG_PREFIX "Failed to create device with feature level 12_1: HRESULT 0x%X\n", result);
		IDXGIAdapter4_Release(adapter);
		return false;
	}

	PURPL_LOG(RENDER_LOG_PREFIX "Created device with handle 0x%" PRIXPTR "\n", (uintptr_t)g_d3d_device);

	IDXGIAdapter4_Release(adapter);
	return true;
}
