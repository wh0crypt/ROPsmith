project('ROPsmith',
  [
    'c', 'cpp'
  ],
  version: '0.1',
  default_options: [
    'cpp_std=c++23',
    'c_std=c11',
    'warning_level=3',
    'werror=false'
  ]
)

# --- Common directories ---
core_dir = 'src/core'
utils_dir = 'src/utils'
include_dir = 'src/include'

# --- Library sources ---
subdir(core_dir)
subdir(utils_dir)
sources = core_sources + utils_sources
incdir = [include_directories('src')]

zlib_dep = dependency('zlib', required : false)
if not zlib_dep.found()
  warning('zlib not found, trying to use system library manually')
  cxx = meson.get_compiler('cpp')
  zlib_dep = cxx.find_library('z', required : true)
endif

# --- Library ---
lib_ropsmith = static_library(
  'ropsmith',
  sources,
  include_directories : incdir,
  dependencies : [zlib_dep]
)

# --- Main executable ---
executable(
  'ropsmith',
  [
    'src/main.cpp',
    'src/program.cpp'
  ],
  include_directories : incdir,
  link_with : lib_ropsmith,
  install : true
)

# --- Subdirectory for tests ---
if get_option('enable-tests')
  subdir('test')
else
  message('Tests are disabled. To enable, run Meson with -Denable-tests=true')
endif
