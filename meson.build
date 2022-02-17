project('purpl-engine', 'c',
	version: '1.0.0',
	license: 'Apache',
	default_options: [
			'buildtype=debug',
			'c_winlibs=kernel32.lib',
			'c_std=c17',
			'default_library=shared',
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
			     language: ['c', 'objc'])
	add_global_arguments('-Wno-semicolon-before-method-body',
			     language: ['objc'])
elif compiler.get_id() == 'clang'
	add_global_arguments('-Wno-deprecated-declarations',
			     '-Wno-incompatible-pointer-types-discards-qualifiers',
			     '-Wno-pedantic',
			     '-Wno-pointer-sign',
			     '-Wno-sign-compare',
			     '-Wno-unused-function',
			     '-Wno-unused-parameter',
			     '-Wno-unused-variable',
			     language: ['c', 'objc'])
	add_global_arguments('-Wno-semicolon-before-method-body',
			     language: ['objc'])
elif compiler.get_id() == 'msvc'
	add_global_arguments('/wd4005', # macro redefinition (PHNT should do something about this)
			     '/wd4057', # pointer sign
			     '/wd4090', # discarding qualifiers
			     '/wd4100', # unused parameter
			     '/wd4115', # typedef thing
			     '/wd4127', # constant conditional expression
			     '/wd4152', # function pointer cast
			     '/wd4200', # zero-length array in struct
			     '/wd4232', # pointer to dllimport function
			     '/wd4244', # truncation
			     '/wd4245', # sign conversion
			     '/wd4267', # trunctation but different
			     '/wd4310', # ignoring qualifiers in truncating cast
			     '/wd4324', # structure padded for alignment
			     '/wd4389', # sign compare
			     '/wd4459', # declaration hides global declaration
			     '/wd4701', # uninitialized variable
			     '/wd4706', # assignment in trinary operator
			     '/wd4996', # "unsafe" functions, even though the secure ones are just as vulnerable to misuse
			     language: ['c'])
endif

subdir('engine')
subdir('demo')
