engine_options = ['-DPURPL_BUILD=1']

if build_machine.cpu_family() == 'x86'
	engine_options += '-DPURPL_X86=1'
elif build_machine.cpu_family() == 'x86_64'
	engine_options += '-DPURPL_X86=1'
	engine_options += '-DPURPL_X86_64=1'
endif

if get_option('enable_meming')
	engine_options += '-DPURPL_ENABLE_MEMING=1'
endif

sdl = dependency('sdl2', version: '>=2.0.0')

if build_machine.system() == 'darwin'
	metal = dependency('metal')
	libs = [sdl, metal]
else
	vulkan = dependency('vulkan')
	libs = [sdl, vulkan]
endif

engine_include_dir = include_directories('include')
engine_incdirs = include_directories('include', '../deps/phnt', '../deps/stb')
engine = shared_library('engine', 
			'src/core/globals.c',
			'src/core/init.c',
			'src/core/log.c',
			
			'src/util/error.c',
			'src/util/fs.c',
			'src/util/misc.c',
			'src/util/stb.c',
			'src/util/string.c',
			
			include_directories: engine_incdirs,
			dependencies: libs,
			c_args: engine_options)
