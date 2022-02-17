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

# Not bad considering almost all of these are SDL's fault
message('Compiling with ' + compiler.get_id())
if compiler.get_id() == 'gcc'
	add_global_arguments('-Wno-cast-function-type',
			     '-Wno-deprecated-declarations',
			     '-Wno-discarded-qualifiers',
			     '-Wno-implicit-fallthrough',
			     '-Wno-pedantic',
			     '-Wno-pointer-sign',
			     '-Wno-sign-compare',
			     '-Wno-unused-function',
			     '-Wno-unused-parameter',
			     '-Wno-unused-variable',
			     language: ['c'])
elif compiler.get_id() == 'clang'
	add_global_arguments('-Wno-deprecated-declarations',
			     '-Wno-incompatible-pointer-types-discards-qualifiers',
			     '-Wno-pedantic',
			     '-Wno-pointer-sign',
			     '-Wno-sign-compare',
			     '-Wno-unused-function',
			     '-Wno-unused-parameter',
			     '-Wno-unused-variable',
			     language: ['c'])
elif compiler.get_id() == 'msvc'
	add_global_arguments('/wd4100', # unused parameter
			     '/wd4127', # constant conditional expression
			     '/wd4152', # function pointer cast
			     '/wd4244', # truncation
			     '/wd4389', # sign compare
			     language: ['c'])
endif

subdir('engine')
subdir('demo')
