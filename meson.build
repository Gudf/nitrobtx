project(
    'nitrobtx',
    'c',
    meson_version: '>=1.1',
    license: 'LGPLv3',
    license_files: 'LICENSE',
    version: '0.0.2',
    default_options: [
        'buildtype=release',
        'warning_level=1',
        'werror=true',
        'c_std=gnu17',
    ],
)

add_project_arguments('-Wno-sign-conversion', language: 'c')
add_project_arguments('-D_FORTIFY_SOURCE=1', language: 'c')

subdir('include')

native = get_option('native')

libpng_dep = dependency('libpng', native: native)

nitrobtx_incdir = include_directories('include')

nitrobtx_exe = executable(
    'nitrobtx',
    sources: files(
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
    ),
    include_directories: nitrobtx_incdir,
    dependencies: libpng_dep,
    native: native,
    install: native and (meson.is_cross_build() ? false : true),
)
