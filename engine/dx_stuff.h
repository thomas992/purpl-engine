// Direct3D stuff

#pragma once

#define COBJMACROS
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#define UUIDOF(x) (&IID_##x)

// Direct3D initialization
extern bool engine_directx_init(void);

// Create the device
extern bool engine_directx_create_device(void);

// Prepare to draw a frame
extern bool engine_directx_begin_frame(uint64_t delta);

// Finish commands
extern bool engine_directx_end_frame(uint64_t delta);

// Shut down Direct3D
extern void engine_directx_shutdown(void);

// Globals
extern IDXGIFactory7 *g_dxgi_factory;
extern ID3D12Device9 *g_d3d_device;
