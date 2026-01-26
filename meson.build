project('fqw-receiver', 'cpp',
  default_options: [
    'cpp_std=c++20'  
]
)

cpp = meson.get_compiler('cpp')

poco_foundation = cpp.find_library('PocoFoundation', required: true)
poco_util = cpp.find_library('PocoUtil', required: true)
poco_net = cpp.find_library('PocoNet', required: true)
poco_xml = cpp.find_library('PocoXML', required: true)
poco_json = cpp.find_library('PocoJSON', required: true)

src_files = [
  'src/platform/main.cpp'
]

include_dirs = [
  'src',
  'src/app',
  'src/app/handlers'
]

executable(
  'main',
  src_files,
  include_directories: include_dirs,
  cpp_args: '-std=c++20',
  link_args: [
    '-lPocoFoundation',
    '-lPocoUtil',
    '-lPocoNet',
    '-lPocoXML',
    '-lPocoJSON'
  ]
)

# DYLD_LIBRARY_PATH=/usr/local/lib ./build/main