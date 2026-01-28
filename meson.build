project('fqw-receiver', 'cpp',
  default_options: [
    'cpp_std=c++20',
    'c_std=c11'
]
)

devkit_dep = dependency('fqw-devkit',
  fallback: ['fqw-devkit', 'devkit_dep'],
  required: true
)

libxml2_proj = subproject('libxml2')
libxml2_dep = libxml2_proj.get_variable('xml_dep')

cpp = meson.get_compiler('cpp')

poco_foundation = cpp.find_library('PocoFoundation', required: true)
poco_util = cpp.find_library('PocoUtil', required: true)
poco_net = cpp.find_library('PocoNet', required: true)
poco_xml = cpp.find_library('PocoXML', required: true)
poco_json = cpp.find_library('PocoJSON', required: true)

all_deps = [
  poco_foundation,
  poco_util,
  poco_net,
  poco_xml,
  poco_json,
  devkit_dep,
  libxml2_dep
]

src_files = [
  'src/platform/main.cpp',
  'src/app/handlers/ReceivePartHandler.cpp',
  'src/app/handlers/UploadHandler.cpp',
  'src/app/handlers/ReceivePartHandler.cpp',
  'src/app/ReceiveFactory.cpp',
  'src/app/ReceiveServer.cpp',
  'src/app/Utils.cpp'
]

include_dirs = [
  'src',
  'src/app',
  'src/app/handlers',
  'subprojects',
  'subprojects/libxml2/include'
]

executable(
  'main',
  src_files,
  include_directories: include_dirs,
  cpp_args: '-std=c++20',
  dependencies: all_deps
)

# DYLD_LIBRARY_PATH=/usr/local/lib ./build/main