project('detonate', 'c', 'cpp',default_options: [ 'buildtype=debug', 'c_std=c11','cpp_std=c++20'])
build_gui = true
src = []
#/*
#src += [
#    './src/core/common/memgzio.c',
#    './src/core/common/SoundSDL.cpp',
#    './src/core/common/Patch.cpp',
#    './src/core/Util.cpp'
#]*/
srcdirs = [
    '../common/deps/imgui',
    '../common/mudutils',
    '../common/player',
    'src'
]
foreach srcitem : srcdirs
  glob = run_command('python3', 'src_batch.py',srcitem)
  src += glob.stdout().strip().split('\n')
endforeach
inc = [
  include_directories(
    '../../common/deps/imgui',
    '../../common/deps',
    '../../common/deps/decoder',
    '../../common',
    '../../common/mudutils',
    '../../common/player',
    '../../common/deps/stb',
    '../../common/deps/decoder',
    '../../common/deps/decoder/opus/celt',
    '../../common/deps/decoder/opus',
    '../../common/deps/decoder/opus/silk',
    '../../common/deps/decoder/opus/libopus',
    '../../common/deps/decoder/opus/opusfile',
    'src'
  )
]
if host_machine.system() == 'linux'
sdl2dep = dependency('sdl2',
  required: true,
  static: false,
  method: 'pkg-config')
else
sdl2dep = dependency('sdl2',
  required: true,
  static: true,
  method: 'pkg-config')
endif

fdkaac_dep = dependency('fdk-aac', required : true,static:true)

if host_machine.system() == 'linux'
    dl_dep = meson.get_compiler('cpp').find_library('dl', required: true)
else
    dl_dep = dependency('', required: false)
endif
test_ldflags = []
common_ldflags = meson.get_compiler('cpp').get_supported_link_arguments (
  test_ldflags)
if host_machine.system() == 'windows'
  common_ldflags += [
    '-fno-stack-protector',
    '-lws2_32',
    '-static'
    ]
endif
add_project_link_arguments (
common_ldflags,
language: [ 'c', 'cpp' ],)
dir_base = meson.current_source_dir()
dir_install = join_paths(dir_base, 'compile_dir')
executable('detonate',install: true, gui_app:build_gui,install_dir: dir_install,include_directories : [inc],sources : [src],dependencies : [sdl2dep,fdkaac_dep,dl_dep]
)