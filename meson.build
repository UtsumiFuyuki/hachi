project('hachi', 'cpp',
    version: '1.0')

incdirs = 'include'
sources = files(
    'src/main.cpp',
    'src/chip8.cpp')

deps = dependency('raylib')

executable('hachi', sources, include_directories: incdirs, dependencies: deps, override_options: ['cpp_std=c++23'])