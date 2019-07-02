project('tmpl', 'cpp',
  version: '0.1.0',
  license: 'MIT', 
  default_options: ['warning_level=3', 'cpp_std=c++17']
)

cpp = meson.get_compiler('cpp')
if cpp.get_argument_syntax() == 'gcc'
  add_project_arguments('-Wconversion', language: 'cpp')
endif

#fmt_dep = dependency('fmt')
# mkdir subprojects && meson wrap install fmt
fmt_dep = dependency('fmt', fallback: ['fmt', 'fmt_dep'])

subdir('include')
subdir('src')
if(not get_option('skip_tests'))
  subdir('tests')
endif

