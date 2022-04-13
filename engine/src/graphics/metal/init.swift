struct purpl_instance_metal {
    
}

@_cdecl("purpl_metal_init")
func purpl_metal_init() -> Bool {
	purpl_inst.pointee.graphics.metal = UnsafeMutableRawPointer.allocate(byteCount: MemoryLayout<purpl_instance_metal>.stride, alignment: MemoryLayout<purpl_instance_metal>.alignment)
	
	purpl_inst.pointee.wnd = SDL_CreateWindow(purpl_get_initial_window_title(), Int32(SDL_WINDOWPOS_CENTERED_MASK), Int32(SDL_WINDOWPOS_CENTERED_MASK), PURPL_INITIAL_WINDOW_WIDTH, PURPL_INITIAL_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE.rawValue | SDL_WINDOW_ALLOW_HIGHDPI.rawValue | SDL_WINDOW_METAL.rawValue)
	if (purpl_inst.pointee.wnd == nil) {
		purpl_metal_shutdown()
		return false
	}

	purpl_inst.pointee.graphics_api = PURPL_GRAPHICS_API_METAL
	return true
}

@_cdecl("purpl_metal_shutdown")
func purpl_metal_shutdown() {
	purpl_inst.pointee.graphics.metal.deallocate()
}
