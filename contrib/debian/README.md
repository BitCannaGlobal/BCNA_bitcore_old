
Debian
====================
This directory contains files used to package bitcannad/bitcanna-qt
for Debian-based Linux systems. If you compile bitcannad/bitcanna-qt yourself, there are some useful files here.

## bitcanna: URI support ##


bitcanna-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install bitcanna-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your bitcannaqt binary to `/usr/bin`
and the `../../share/pixmaps/bitcanna128.png` to `/usr/share/pixmaps`

bitcanna-qt.protocol (KDE)

