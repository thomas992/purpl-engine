project('purpl', 'c',
	default_options: ['c_std=c17', 'default_library=shared', 'backend_startup_project=demo'])

add_project_arguments('-DPURPL_BUILD=1', language: ['c'])

if build_machine.cpu_family() == 'x86'
	add_project_arguments('-DPURPL_X86=1', language: ['c'])
elif build_machine.cpu_family() == 'x86_64'
	add_project_arguments('-DPURPL_X86=1', language: ['c'])
	add_project_arguments('-DPURPL_X86_64=1', language: ['c'])
endif

if get_option('enable_meming')
	add_project_arguments('-DPURPL_ENABLE_MEMING=1', language: ['c'])
endif

sdl = dependency('sdl2', version: '>=2.0.0')
vulkan = dependency('vulkan')

engine_incdirs = include_directories('engine/include', 'deps/phnt', 'deps/stb')
engine = shared_library('engine', 
			'engine/src/core/globals.c',
			'engine/src/core/init.c',
			'engine/src/core/log.c',
			
			'engine/src/util/error.c',
			'engine/src/util/fs.c',
			'engine/src/util/misc.c',
			'engine/src/util/stb.c',
			'engine/src/util/string.c',
			
			include_directories: engine_incdirs,
			dependencies: [sdl, vulkan])
