project(
    'nitrobtx',
    'c',
    meson_version: '>=1.1'
)

libpng_dep = dependency('libpng')

sources = files(
    'src/common.c',
    'src/help.c',
    'src/main.c',
    'src/ns/chunks/tex.c',
    'src/ns/container.c',
    'src/ns/resource_dict.c',
    'src/ns/resource_name.c',
    'src/ns/resource_tree.c',
    'src/options.c',
    'src/palette.c',
    'src/texture.c',
)

incdir = include_directories('include')

executable('nitrobtx', sources, include_directories : incdir, dependencies: libpng_dep)
