# Common sources
incdir = include_directories('..', '../include')

utility_libraries = [
  'acl_manager.cpp',
]
if user_attributes_feature.enabled()
  utility_libraries += [ 
    'xattr_manager.cpp',
  ]
endif

common_sources = [
  utility_libraries,
  'acl_item.cpp',
  'participant_item.cpp',
  'acl_list_widget.cpp',
  'acl_list_controller.cpp',
  'acl_editor_widget.cpp',
  'enclosed_acl_editor_widget.cpp',
  'acl_editor_controller.cpp',
  'enclosed_acl_editor_controller.cpp',
  'participant_list_widget.cpp',
  'participant_list_controller.cpp',
]

if user_attributes_feature.enabled()
  xattr_sources = [
    'xattr_item.cpp',
    'xattr_editor_widget.cpp',
    'xattr_editor_controller.cpp',
  ]

  common_sources += xattr_sources
endif

gnome = import('gnome')
eiciel_resources = gnome.compile_resources(
  'eiciel-resources',
  'eiciel.gresource.xml',
  source_dir: ['../src', '../img'])

# Standalone binary
standalone_sources = common_sources
standalone_sources += [
  'i18n.cpp',
  'app_window.cpp',
  'application.cpp',
  'main.cpp',
]

executable('eiciel',
  [standalone_sources, eiciel_resources],
  install : true,
  include_directories : incdir,
  dependencies : [gtkmm4, giomm, libacl, thread_dep])


# Plugin for nautilus
plugin_sources = utility_libraries
plugin_sources += ['nautilus_menu_provider.cpp',
                   'nautilus_acl_model.cpp',
                   'nautilus_xattr_model.cpp',
                   'nautilus_model_provider.cpp',
                   'nautilus_extension.cpp',
                   'i18n.cpp']

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
        input: 'eiciel.desktop.in',
        configuration: desktop_file_data)

desktopfilesdir = get_option('datadir') / 'applications'
install_data(desktop_file,
        install_dir : desktopfilesdir)

# Appdata
appdatadir = get_option('datadir') / 'metainfo'
install_data('org.roger_ferrer.Eiciel.appdata.xml',
             install_dir : appdatadir)
