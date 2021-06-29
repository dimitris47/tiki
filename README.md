# Tiki

Tiki is a simple project organizer. You can add projects and priority sorted tasks to your projects. You can also mark tasks as done if you don't want to permanently delete them.

HOW TO INSTALL:

- WINDOWS:

Run the latest `.exe` file.
If after the installation the program doesn't start, install `vc_redist.exe` from the installation folder.

- MAC:

Download and copy the latest `.dmg` image file to the Applications directory.

- LINUX:

a. from source:

1. Donwload the code or clone the repo
2. Open extracted directory in terminal and run:
   `qmake tiki.pro CONFIG+=release && make && sudo make install && make clean`
   
You might need to install `libqt5gui5`, `g++` and `make` packages to your system before compiling.

b. Debian & Debian-based systems:

Download the latest `.deb` file from releases and run it with your package manager.

c. RPM-based systems:

Download the latest `.rpm` file and run it with your package manager.

You might need to have `libqt5gui5` installed before running the binary.
