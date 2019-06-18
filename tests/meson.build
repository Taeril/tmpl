
main_lib = static_library('main', 'main.cpp')

version_exe = executable('version', 'version.cpp', link_with: main_lib,
  dependencies: tmpl_dep,
)
test('version', version_exe)

basics_exe = executable('basics', 'basics.cpp', link_with: main_lib,
  dependencies: tmpl_dep,
)
test('basics', basics_exe)

filters_exe = executable('filters', 'filters.cpp', link_with: main_lib,
  dependencies: tmpl_dep,
)
test('filters', filters_exe)

