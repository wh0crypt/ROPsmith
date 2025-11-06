project('ROPsmith', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'c_std=c11']
)

inc = include_directories('src', 'src/rop', 'src/include')

# --- Library sources ---
subdir('src/rop')
subdir('src/ropsmith')
subdir('src/utils')
lib_ropsmith_sources = rop_sources + ropsmith_sources + utils_sources

# --- Library ---
lib_ropsmith = static_library(
  'ropsmith',
  lib_ropsmith_sources,
  include_directories : inc
)

# --- Main executable ---
exe_ropsmith = executable(
  'ropsmith',
  'src/main.c',
  include_directories : inc,
  link_with : lib_ropsmith,
  install : true
)

# --- Subdirectory for tests ---
subdir('tests')
