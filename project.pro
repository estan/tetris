TEMPLATE = subdirs
SUBDIRS = tetris-static tetris-eda sensor simulator
tetris-static.depends = sensor
tetris-eda.depends = sensor
