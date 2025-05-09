# Eiciel
project('eiciel', 'cpp',
  version : '0.10.1',
  meson_version : '>=0.57',
  license: 'GPL2+',
  default_options: ['cpp_std=c++17'])

package_name = 'eiciel'
email = 'rofirrim@gmail.com'

conf_data = configuration_data()

gnome = import('gnome')

# A few configuration variables that autoconf provides
# FIXME: Do we still need them?
conf_data.set_quoted('VERSION', meson.project_version())
conf_data.set_quoted('PACKAGE_VERSION', meson.project_version())
conf_data.set_quoted('PACKAGE', package_name)
datadir = get_option('prefix') / get_option('datadir')
conf_data.set_quoted('DATADIR', datadir)
pkgdatadir = datadir / package_name
conf_data.set_quoted('PKGDATADIR', pkgdatadir)
localedir = datadir / 'locale'
conf_data.set_quoted('LOCALEDIR', localedir)

# Dependencies
gtkmm4 = dependency('gtkmm-4.0', version: '>= 4.6')
giomm = dependency('giomm-2.68', version: '>= 2.68')
thread_dep = dependency('threads')
nautilus_plugin_feature = get_option('nautilus-plugin')
if nautilus_plugin_feature.enabled()
libnautilus_extensions4 = dependency('libnautilus-extension-4',
                                      version: '>= 43')
endif

# Compiler
compiler = meson.get_compiler('cpp')

# Headers
has_sys_acl_h = compiler.has_header('sys/acl.h')
conf_data.set('HAVE_SYS_ACL_H', has_sys_acl_h)

has_acl_libacl_h = compiler.has_header('acl/libacl.h')
conf_data.set('HAVE_ACL_LIBACL_H', has_acl_libacl_h)

has_sys_xattr_h = compiler.has_header('sys/xattr.h')
conf_data.set('HAVE_SYS_XATTR_H', has_sys_xattr_h)

user_attributes_feature = get_option('user-attributes')
conf_data.set('ENABLE_USER_XATTR',
              user_attributes_feature.enabled())

# Libraries
libacl = compiler.find_library('acl')

has_acl_get_perm = compiler.has_function('acl_get_perm', dependencies: libacl)
conf_data.set('HAVE_ACL_GET_PERM', has_acl_get_perm)

has_acl_get_perm_np = compiler.has_function('acl_get_perm_np',
                                            dependencies: libacl)
conf_data.set('HAVE_ACL_GET_PERM_NP', has_acl_get_perm_np)

# Internationalization
subdir('po')

# Include dirs

# Programs
pkg_config = find_program('pkg-config')
sed = find_program('sed')
install_program = find_program('install')

# Configuration file
configure_file(output: 'config.h',
               configuration: conf_data)

# Source
subdir('src')

# Icons.
subdir('img')

# Documentation
subdir('doc')

# Manpage(s)
subdir('man')

# GNOME post-install duties
gnome.post_install(gtk_update_icon_cache : true)

summary(
  {
    'bindir': get_option('prefix') / get_option('bindir'),
    'nautilus-extension-dir': nautilusextensiondir
  },
  section: 'Directories')

# vim: sw=2 expandtab ts=2 ft=meson
