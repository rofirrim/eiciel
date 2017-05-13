#!/bin/bash -e

INSTALL_DATA="$1 -m 644"
hicolor_iconsdir="$2"

for icon_size in 16 24 32 48 64 96;
do
		${INSTALL_DATA} -D "${MESON_SOURCE_ROOT}/img/icon_eiciel_${icon_size}.png" "${MESON_INSTALL_DESTDIR_PREFIX}/${hicolor_iconsdir}/${icon_size}x${icon_size}/apps/eiciel.png";
done
${INSTALL_DATA} -D "${MESON_SOURCE_ROOT}/img/icon_eiciel.svg" "${MESON_INSTALL_DESTDIR_PREFIX}/${hicolor_iconsdir}/scalable/apps/eiciel.svg"
