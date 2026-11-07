project(
    'nitrobtx',
    'c',
    meson_version: '>=1.1',
    license: 'GPLv3',
    license_files: 'LICENSE'
)

nitrobtx_cflags = [
    '-std=gnu17',
    '-O3',
    '-Wall',
    '-Wno-sign-conversion',
    '-Werror',
    '-D_FORTIFY_SOURCE=1',
]

nitrobtx_link_args = []

native = get_option('native')
asan = get_option('enable_asan')

if asan
    nitrobtx_cflags += '-fsanitize=address'
    nitrobtx_link_args += '-fsanitize=address'
endif

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
    c_args: nitrobtx_cflags,
    link_args: nitrobtx_link_args,
    include_directories: nitrobtx_incdir,
    dependencies: libpng_dep,
    native: native,
    install: native and (meson.is_cross_build() ? false : true),
)
