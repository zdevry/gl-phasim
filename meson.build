project('gl-phasim', 'cpp', 'c')

deps = [dependency('gl'), dependency('glfw3')]

src_incdir = include_directories('src')

sources = files('glad/glad.c')
subdir('src')

executable('app', sources, include_directories: src_incdir, dependencies: deps)
