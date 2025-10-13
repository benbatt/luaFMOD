project('luaFMOD', 'c')

sources = [
  'src/bank.c',
  'src/bus.c',
  'src/callbacks.c',
  'src/channel.c',
  'src/constants.c',
  'src/coresystem.c',
  'src/eventdescription.c',
  'src/eventinstance.c',
  'src/logging.c',
  'src/luaFMOD.c',
  'src/platforms/windows.c',
  'src/sound.c',
  'src/structures.c',
  'src/studiosystem.c',
  'src/vca.c',
]

lua = dependency('lua5.1')

fmod_library_directory = meson.project_source_root() + '/external/FMOD/lib/'

if build_machine.system() == 'windows' or build_machine.system() == 'cygwin'
  make_delayload_lib = find_program('build/make-delayload-lib.sh')

  fmodL = custom_target('fmodL',
    input: [fmod_library_directory + 'fmodL_vc.lib'],
    output: 'fmodL_delayed.lib',
    command: [make_delayload_lib, 'fmodL', '@INPUT@', 'fmodL.def'],
  )

  fmodstudioL = custom_target('fmodstudioL',
    input: [fmod_library_directory + 'fmodstudioL_vc.lib'],
    output: 'fmodstudioL_delayed.lib',
    command: [make_delayload_lib, 'fmodstudioL', '@INPUT@', 'fmodstudioL.def'],
  )

  fmod = declare_dependency(include_directories: 'external/FMOD/inc', link_with: [fmodL, fmodstudioL])
else
  compiler = meson.get_compiler('c')

  fmodL = compiler.find_library('fmodL', dirs: fmod_library_directory)
  fmodstudioL = compiler.find_library('fmodstudioL', dirs: fmod_library_directory)

  fmod = declare_dependency(include_directories: 'external/FMOD/inc', dependencies: [fmodL, fmodstudioL])
endif

library('luaFMOD',
  name_prefix: '',
  sources: sources,
  dependencies: [fmod, lua],
  c_args: ['-DLUAFMOD_DYNAMIC', '-Wno-error=incompatible-pointer-types'],
)
