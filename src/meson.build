
ver_arr = meson.project_version().split('.')
ver_data = configuration_data({
  'VERSION': meson.project_version(),
  'VERSION_MAJOR': ver_arr[0],
  'VERSION_MINOR': ver_arr[1],
  'VERSION_PATCH': ver_arr[2],
})
configure_file(
  input : 'version.hpp.in',
  output : 'version.hpp',
  configuration : ver_data
)


sources = files([
  'utils.cpp',
  'tmpl.cpp',
])

tmpl_lib = static_library('tmpl', sources,
  install : true,
  gnu_symbol_visibility : 'hidden',
  dependencies: fmt_dep,
  include_directories: [tmpl_inc, '../include/tmpl'],
)

tmpl_dep = declare_dependency(
  link_with: tmpl_lib,
  include_directories: tmpl_inc,
)


pkg = import('pkgconfig')
pkg.generate(
  name : meson.project_name(),
  filebase : meson.project_name(),
  description : 'Simple template library for C++',
  subdirs : '.',
  libraries : tmpl_lib,
  version : meson.project_version(),
)

