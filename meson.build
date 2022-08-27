# Eiciel
project('eiciel', 'cpp',
  version : '0.99.1',
  meson_version : '>=0.54',
  license: 'GPL2+',
  default_options: ['cpp_std=c++17'])

package_name = 'eiciel'
email = 'rofirrim@gmail.com'

conf_data = configuration_data()

# A few configuration variables that autoconf provides
conf_data.set_quoted('VERSION', meson.project_version())
conf_data.set_quoted('PACKAGE_VERSION', meson.project_version())
conf_data.set_quoted('PACKAGE', package_name)
pkgdatadir = get_option('prefix') / get_option('datadir') / package_name
conf_data.set_quoted('PKGDATADIR', pkgdatadir)

# This ignores translations but otherwise it seems impossible to tell
# yelp where to find the help, specially if installed in a nonstandard
# location.
eicieldocdir = get_option('datadir') / 'gnome' / 'help' / 'eiciel' / 'C'
abs_eicieldocdir =  get_option('prefix') / eicieldocdir
conf_data.set_quoted('EICIELDOCDIR', abs_eicieldocdir)

# Dependencies
gtkmm4 = dependency('gtkmm-4.0', version: '>= 4.6')
giomm = dependency('giomm-2.68', version: '>= 2.68')
libnautilus_extensions4 = dependency('libnautilus-extension-4',
                                      version: '>= 43.rc')
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

# I need to learn how to to build against a flatpak :-?
warning('This branch still does not support nautilus extensions')

# FIXME: Enable this when we figure this out.
# has_nautilus_extension_h = compiler.has_header('nautilus/nautilus-extension.h',
#          dependencies: [libnautilus_extensions3, gtkmm4])
# if has_nautilus_extension_h
#   # conf_data.set('HAVE_NAUTILUS_NAUTILUS_EXTENSION_H', has_nautilus_extension_h)
# endif

# Internationalization
i18n = import('i18n')
subdir('po')
conf_data.set_quoted('GETTEXT_PACKAGE', package_name)

# Include dirs
incdir = include_directories('include')

# Configuration file
configure_file(output: 'config.h',
               configuration: conf_data)

# Programs
pkg_config = find_program('pkg-config')
sed = find_program('sed')
install_program = find_program('install')

# Common sources
common_sources = [
  'src/acl_item.cpp',
  'src/participant_item.cpp',
  'src/acl_manager.cpp',
  'src/acl_list_widget.cpp',
  'src/acl_list_controller.cpp',
  'src/acl_editor_widget.cpp',
  'src/enclosed_acl_editor_widget.cpp',
  'src/acl_editor_controller.cpp',
  'src/enclosed_acl_editor_controller.cpp',
  'src/participant_list_widget.cpp',
  'src/participant_list_controller.cpp',
]

xattr_sources = [
  'src/xattr_item.cpp',
  'src/xattr_manager.cpp',
  'src/xattr_editor_widget.cpp',
  'src/xattr_editor_controller.cpp',
]

if user_attributes_feature.enabled()
  common_sources += xattr_sources
endif

gnome = import('gnome')
eiciel_resources = gnome.compile_resources(
  'eiciel-resources',
  'src/eiciel.gresource.xml',
  source_dir: ['src', 'img'])

# Standalone binary
standalone_sources = common_sources
standalone_sources += [
  'src/app_window.cpp',
  'src/application.cpp',
  'src/main.cpp',
]

executable('eiciel',
  [standalone_sources, eiciel_resources],
  install : true,
  include_directories : incdir,
  dependencies : [gtkmm4, giomm, libacl, thread_dep])


# Plugin for nautilus
# FIXME: Enable this when we figure this out.

plugin_sources = common_sources
plugin_sources += ['src/nautilus_menu_provider.cpp',
                   'src/nautilus_acl_model.cpp',
                   'src/nautilus_model_provider.cpp',
                   'src/nautilus_extension.cpp']

nautilusextensiondir = get_option('nautilus-extension-dir')
if nautilusextensiondir == '' 
  nautilusextensiondir = libnautilus_extensions4.get_variable(pkgconfig : 'extensiondir')
endif

shared_module('eiciel-nautilus',
  plugin_sources,
  install : true,
  install_dir : nautilusextensiondir,
  include_directories : incdir,
  dependencies : [gtkmm4, giomm, libnautilus_extensions4, libacl, thread_dep])

# Desktop file
desktop_file_data = configuration_data()
desktop_file_data.set('DSK_VERSION', '1.1')
desktop_file_data.set('DSK_bindir', get_option('prefix') / get_option('bindir'))
desktop_file_data.set('DSK_pkgdatadir', pkgdatadir)
desktop_file = configure_file(output: 'org.roger_ferrer.Eiciel.desktop',
        input: 'src/eiciel.desktop.in',
        configuration: desktop_file_data)

desktopfilesdir = get_option('datadir') / 'applications'
install_data(desktop_file,
        install_dir : desktopfilesdir)

# Appdata
appdatadir = get_option('datadir') / 'metainfo'
install_data('src/org.roger_ferrer.Eiciel.appdata.xml',
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
