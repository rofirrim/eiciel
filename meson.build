# Eiciel
project('eiciel', 'cpp',
  version : '0.9.13',
  meson_version : '>=0.54',
  license: 'GPL2+')

package_name = 'eiciel'
email = 'rofirrim@gmail.com'

conf_data = configuration_data()

# A few configuration variables that autoconf provides
conf_data.set_quoted('VERSION', meson.project_version())
conf_data.set_quoted('PACKAGE_VERSION', meson.project_version())
conf_data.set_quoted('PACKAGE', package_name)
pkgdatadir = get_option('prefix') / get_option('datadir') / package_name
conf_data.set_quoted('PKGDATADIR', pkgdatadir)

# Dependencies
gtkmm3 = dependency('gtkmm-3.0', version: '>= 3.0.0')
giomm = dependency('giomm-2.4', version: '>= 2.50.0')
libnautilus_extensions3 = dependency('libnautilus-extension',
                                     version: '>= 3.0.0')
thread_dep = dependency('threads')

# Compiler
compiler = meson.get_compiler('cpp')

# Headers
has_sys_acl_h = compiler.has_header('sys/acl.h')
conf_data.set('HAVE_SYS_ACL_H', has_sys_acl_h)

has_acl_libacl_h = compiler.has_header('acl/libacl.h')
conf_data.set('HAVE_ACL_LIBACL_H', has_acl_libacl_h)

has_xattr_h = compiler.has_header('attr/xattr.h')
conf_data.set('HAVE_ATTR_XATTR_H', has_xattr_h)

user_attributes_feature = get_option('user-attributes')
conf_data.set('ENABLE_USER_XATTR',
              user_attributes_feature.enabled())

libacl = compiler.find_library('libacl')

has_acl_get_perm = compiler.has_function('acl_get_perm', dependencies: libacl)
conf_data.set('HAVE_ACL_GET_PERM', has_acl_get_perm)

has_acl_get_perm_np = compiler.has_function('acl_get_perm_np',
                                            dependencies: libacl)
conf_data.set('HAVE_ACL_GET_PERM_NP', has_acl_get_perm_np)

# Internationalization
i18n = import('i18n')
subdir('po')
conf_data.set_quoted('GETTEXT_PACKAGE', package_name)

# Configuration file
configure_file(output: 'config.hpp',
               configuration: conf_data)

# Programs
pkg_config = find_program('pkg-config')
sed = find_program('sed')
install_program = find_program('install')

# Common sources
common_sources = [
  'src/acl_manager.cpp',
  'src/acl_manager.hpp',
  'src/eiciel_main_window.cpp',
  'src/eiciel_main_window.hpp',
  'src/eiciel_main_window_controller.cpp',
  'src/eiciel_main_window_controller.hpp',
  'src/eiciel_acl_list.cpp',
  'src/eiciel_acl_list.hpp',
  'src/eiciel_acl_list_controller.cpp',
  'src/eiciel_acl_list_controller.hpp',
  'src/eiciel_participant_list.cpp',
  'src/eiciel_participant_list.hpp',
  'src/eiciel_participant_list_controller.cpp',
  'src/eiciel_participant_list_controller.hpp',
  'src/eiciel_participant_target.hpp',
  'src/eiciel_enclosed_editor_window_controller.hpp',
  'src/eiciel_enclosed_editor_window_controller.cpp',
  'src/eiciel_enclosed_editor_window.hpp',
  'src/eiciel_enclosed_editor_window.cpp',
  'src/acl_list.hpp',
  'src/participant_list.hpp',
  'src/acl_element_kind.hpp',
  'src/cellrenderer_acl.hpp',
  'src/cellrenderer_acl.cpp'
]

xattr_sources = [
  'src/xattr_manager.hpp',
  'src/xattr_manager.cpp',
  'src/eiciel_xattr_window.cpp',
  'src/eiciel_xattr_window.hpp',
  'src/eiciel_xattr_controller.cpp',
  'src/eiciel_xattr_controller.hpp',
  'src/xattr_list_model.hpp'
]

if user_attributes_feature.enabled()
  common_sources += xattr_sources
endif

# Standalone binary
standalone_sources = common_sources
standalone_sources += [
  'src/eiciel_standalone.cpp',
  'src/eiciel_container.hpp',
  'src/eiciel_container.cpp'
]

executable('eiciel',
  standalone_sources,
  install : true,
  dependencies : [gtkmm3, giomm, libnautilus_extensions3, libacl, thread_dep])


# Plugin for nautilus
plugin_sources = common_sources
plugin_sources += ['src/eiciel_nautilus_page.hpp',
                   'src/eiciel_nautilus_page.cpp']

nautilusextensiondir = get_option('nautilus-extension-dir')
if nautilusextensiondir == ''
  r = run_command(pkg_config.path(), '--variable=extensiondir',
                  'libnautilus-extension')
  nautilusextensiondir = r.stdout().strip()
endif

shared_module('eiciel-nautilus',
  plugin_sources,
  install : true,
  install_dir : nautilusextensiondir,
  dependencies : [gtkmm3, giomm, libnautilus_extensions3, libacl, thread_dep])

# Desktop file
desktop_file_data = configuration_data()
desktop_file_data.set('DSK_VERSION', '1.1')
desktop_file_data.set('DSK_bindir', get_option('prefix') / get_option('bindir'))
desktop_file_data.set('DSK_pkgdatadir', pkgdatadir)
desktop_file = configure_file(output: 'org.roger-ferrer.Eiciel.desktop',
        input: 'src/eiciel.desktop.in',
        configuration: desktop_file_data)

desktopfilesdir = get_option('datadir') / 'applications'
install_data(desktop_file,
        install_dir : desktopfilesdir)

# Appdata
appdatadir = get_option('datadir') / 'metainfo'
install_data('src/org.roger-ferrer.Eiciel.appdata.xml',
             install_dir : appdatadir)

# Images
eicielimagesdir = pkgdatadir / 'img'
install_data(
  'img/others-default.png',
  'img/others.png',
  'img/group-acl-default.png',
  'img/group-acl.png',
  'img/group-default.png',
  'img/group.png',
  'img/mask-default.png',
  'img/mask.png',
  'img/user-acl-default.png',
  'img/user-acl.png',
  'img/user-default.png',
  'img/user.png',
  install_dir : eicielimagesdir)

# Icons.
hicolor_iconsdir = get_option('datadir') / 'icons' / 'hicolor'
meson.add_install_script('img/install_icons.sh',
        install_program.path(),
        hicolor_iconsdir)

install_man('man/eiciel.1')

# Documents
eicieldocdir = get_option('datadir') / 'gnome' / 'help' / 'eiciel' / 'C'

install_data(
  'doc/C/add.page',
  'doc/C/concepts.page',
  'doc/C/default.page',
  'doc/C/edit.page',
  'doc/C/index.page',
  'doc/C/mask.page',
  'doc/C/open.page',
  'doc/C/remove.page',
  'doc/C/requirements.page',
  'doc/C/view.page',
  'doc/C/xattr.page',
  install_dir : eicieldocdir)

eicieldocfiguresdir = get_option('datadir') / 'gnome' / 'help' / 'eiciel' / 'C' / 'figures'

install_data(
  'doc/C/figures/acl_entry.png',
  'doc/C/figures/acl_permissions.png',
  'doc/C/figures/mask_detail.png',
  'doc/C/figures/open_file_nautilus.png',
  'doc/C/figures/open_file.png',
  'doc/C/figures/screen_xattr.png',
  'doc/C/figures/ugo_permissions.png',
  'img/others-default.png',
  'img/others.png',
  'img/group-acl-default.png',
  'img/group-acl.png',
  'img/group-default.png',
  'img/group.png',
  'img/mask-default.png',
  'img/mask.png',
  'img/user-acl-default.png',
  'img/user-acl.png',
  'img/user-default.png',
  'img/user.png',
  install_dir : eicieldocfiguresdir)

summary(
  {
    'bindir': get_option('prefix') / get_option('bindir'),
    'nautilus-extension-dir': nautilusextensiondir
  },
  section: 'Directories')

# vim: sw=2 expandtab ts=2 ft=meson
