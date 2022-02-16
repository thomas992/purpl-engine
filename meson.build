project('purpl', 'c', version: '1.0.0',
	default_options: ['c_std=c17', 'default_library=shared', 'backend_startup_project=demo'])

compiler = meson.get_compiler('c')

if compiler.get_id() == 'gcc'
	add_project_arguments('-Werror', '-Wno-pointer-sign', '-Wno-discarded-qualifiers', language: ['c'])
elif compiler.get_id() == 'clang'
	add_project_arguments('-Werror', '-Wno-pointer-sign', '-Wno-incompatible-pointer-types-discards-qualifiers', language: ['c'])
elif compiler.get_id() == 'msvc'
	add_project_arguments('/WX', language: ['c'])
endif

subdir('engine')
subdir('demo')
