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

sdl = subproject('sdl2').get_variable('sdl2_dep')

if build_machine.system() == 'darwin'
	metal = dependency('metal')
	libs = [sdl, metal]
else
	vulkan = dependency('vulkan')
	libs = [sdl, vulkan]
endif

engine_include_dir = include_directories('include')
engine_incdirs = include_directories('include', '../deps/phnt', '../deps/stb')

install_headers('include/purpl/core/coredefs.h',
		'include/purpl/core/features.h',
		'include/purpl/core/init.h',
		'include/purpl/core/inst.h',
		'include/purpl/core/log.h',
		'include/purpl/core/types.h',
		subdir: 'purpl/core')
install_headers('include/purpl/graphics/init.h',
		subdir: 'purpl/graphics')
install_headers('include/purpl/util/error.h',
		'include/purpl/util/fs.h',
		'include/purpl/util/misc.h',
		'include/purpl/util/string.h',
		'include/purpl/util/util.h',
		subdir: 'purpl/util')

install_data('../deps/licenses/dos2unix.txt',
	     '../deps/licenses/meson.txt',
	     '../deps/licenses/ninja.txt',
	     '../deps/licenses/phnt.txt',
	     '../deps/licenses/sdl2.txt',
	     '../deps/licenses/stb.txt',
	     install_dir: 'licenses')
install_data('../LICENSE',
	     rename: 'purpl.txt',
	     install_dir: 'licenses')

if host_machine.system() == 'windows'
	install_headers('include/purpl/util/private/win/ntdll.h',
			subdir: 'purpl/util/private/win')
	install_data('../tools/purpl-demo.bat',
		     install_dir: '')
endif

engine_sources = [
	'src/core/globals.c',
	'src/core/init.c',
	'src/core/log.c',
	
	'src/util/error.c',
	'src/util/fs.c',
	'src/util/misc.c',
	'src/util/stb.c',
	'src/util/string.c'
]

if build_machine.system() == 'windows'
	engine_sources += 'src/util/win/ntdll.c'
endif

engine = shared_library('engine', 
			engine_sources,
			
			include_directories: engine_incdirs,
			dependencies: libs,
			c_args: engine_options,
			install: true,
			install_dir: 'bin')
