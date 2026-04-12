project('rgt-receiver', 'cpp',
  default_options: [
    'cpp_std=c++20',
    'c_std=c11'
]
)

devkit_dep = dependency('rgt-devkit',
  fallback: ['rgt-devkit', 'devkit_dep'],
  required: true
)

cpp = meson.get_compiler('cpp')

poco_foundation = cpp.find_library('PocoFoundation', required: true)
poco_util = cpp.find_library('PocoUtil', required: true)
poco_net = cpp.find_library('PocoNet', required: true)
poco_xml = cpp.find_library('PocoXML', required: true)
poco_json = cpp.find_library('PocoJSON', required: true)
poco_crypto = cpp.find_library('PocoCrypto', required: true)

all_deps = [
  poco_foundation,
  poco_util,
  poco_net,
  poco_xml,
  poco_json,
  poco_crypto,
  devkit_dep,
]

src_files = [
  'src/app/main.cpp',
  'src/platform/Handlers/UploadHandler.cpp',
  'src/platform/Utils.cpp'
]

include_dirs = [
  'src/platform'
]

executable(
  'rgt-receiver',
  src_files,
  include_directories: include_dirs,
  cpp_args: '-std=c++20',
  dependencies: all_deps,
  link_args: [
    '-Wl,-rpath,@executable_path/../lib',      # ищет рядом с бинарником
    '-Wl,-rpath,/opt/homebrew/lib',            # Homebrew на Apple Silicon
    '-Wl,-rpath,/usr/local/lib',               # Homebrew на Intel Mac
  ],
  cpp_args: ['-DPROJECT_NAME="rgt-receiver"']
)
