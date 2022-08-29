hicolor_iconsdir = get_option('datadir') / 'icons' / 'hicolor'

icon_sizes = [16, 24, 32, 48, 64, 96]

foreach size : icon_sizes
  size_str = size.to_string()
  sxs = size_str + 'x' + size_str
  hicolor_iconsdir_size = hicolor_iconsdir / sxs / 'apps'
  install_data('icon_eiciel_' + size_str + '.png',
    rename: 'eiciel.png',
    install_dir: hicolor_iconsdir_size)
endforeach

hicolor_iconsdir_scalable = hicolor_iconsdir / 'scalable' / 'apps'
install_data('icon_eiciel.svg',
  install_dir: hicolor_iconsdir_scalable)
