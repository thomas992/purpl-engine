project('purpl-engine', 'c',
	version: '1.0.0',
	license: 'Apache',
	default_options: [
			'buildtype=debug',
			'c_winlibs=kernel32',
			'c_std=c17',
			'default_library=shared',
			'strip=true',
			'warning_level=3',
			'werror=true'
	]
)

compiler = meson.get_compiler('c')

if compiler.get_id() == 'gcc'
	add_project_arguments('-Wno-pointer-sign', '-Wno-discarded-qualifiers', language: ['c'])
elif compiler.get_id() == 'clang'
	add_project_arguments('-Wno-pointer-sign', '-Wno-incompatible-pointer-types-discards-qualifiers', language: ['c'])
endif

subdir('engine')
subdir('demo')
