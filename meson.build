project('openmic', 'c', version: '0.1.0-prealpha', license: 'GPLv3')

if target_machine.system() == 'android' or target_machine.system() == 'linux'
  module_ext = 'so'
elif target_machine.system() == 'darwin'
  module_ext = 'dylib'
elif target_machine.system() == 'windows'
  mdoule_ext = 'dll'
else
  error('Unsuported target machine')
endif

path_prefix = get_option('prefix')
path_bindir = join_paths(path_prefix, get_option('bindir'))
path_datadir = join_paths(path_prefix, get_option('datadir'))
path_libdir = join_paths(path_prefix, get_option('libdir'))
path_includedir = join_paths(path_prefix, get_option('includedir'))

enable_cli = get_option('enable_cli')
enable_gui = get_option('enable_gui')
enable_tui = get_option('enable_tui')

config_data = configuration_data()
config_data.set('version', meson.project_version())
config_data.set('version_major', meson.project_version().split('.')[0])
config_data.set('version_minor', meson.project_version().split('.')[1])
config_data.set('version_patch', meson.project_version().split('.')[2].split('-')[0])
config_data.set('version_tag', meson.project_version().split('-')[1])
config_data.set('module_ext', module_ext)
config_data.set('prefix', path_prefix)
config_data.set('datadir', path_datadir)
config_data.set('libdir', path_libdir)
config_data.set('OPENMIC_CLI', enable_cli)
config_data.set('OPENMIC_GUI', enable_gui)
config_data.set('OPENMIC_TUI', enable_tui)
configure_file(input: join_paths(meson.source_root(), 'data/config.h.in'), output: 'openmic-config.h', configuration: config_data)

basedeps = [dependency('gstreamer-1.0'), dependency('glib-2.0'), dependency('gmodule-2.0')]

if enable_tui
  basedeps += [dependency('ncurses')]
endif
if enable_gui
  basedeps += [dependency('gtk+-3.0')]
endif

lib_sources = [join_paths(meson.source_root(), 'src/lib/context.c'), join_paths(meson.source_root(), 'src/lib/module-loader.c'),
  join_paths(meson.source_root(), 'src/lib/module-manager.c'), join_paths(meson.source_root(), 'src/lib/module.c'),
  join_paths(meson.source_root(), 'src/lib/node.c'), join_paths(meson.source_root(), 'src/lib/openmic.c')]
lib_headers = [join_paths(meson.source_root(), 'include/OpenMic/context.h'), join_paths(meson.source_root(), 'include/OpenMic/module-loader.h'),
  join_paths(meson.source_root(), 'include/OpenMic/module-manager.h'), join_paths(meson.source_root(), 'include/OpenMic/module.h'),
  join_paths(meson.source_root(), 'include/OpenMic/node.h'), join_paths(meson.source_root(), 'include/OpenMic/openmic.h')]
both_libraries('openmic', lib_sources, soversion: 0, version: meson.project_version().split('-')[0],
  include_directories: [include_directories('include')], dependencies: basedeps,
  install: true,
  install_dir: path_libdir)
install_headers(join_paths(meson.build_root(), 'openmic-config.h'), install_dir: path_includedir)
install_headers(lib_headers, install_dir: join_paths(path_includedir, 'OpenMic'))

if enable_cli
  executable('openmic-cli', join_paths(meson.source_root(), 'src/cli/main.c'),
    install: true,
    install_dir: path_bindir)
endif
if enable_gui
  executable('openmic-gui', join_paths(meson.source_root(), 'src/gui/main.c'),
    install: true,
    install_dir: path_bindir)
endif
if enable_tui
  executable('openmic-tui', join_paths(meson.source_root(), 'src/tui/main.c'),
    install: true,
    install_dir: path_bindir)
endif
