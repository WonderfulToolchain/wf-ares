higan, the multi-system emulator
================================

higan emulates a number of classic videogame consoles of the 1980s and 1990s,
allowing you to play classic games on a modern general-purpose computer.

As of v102,
higan has top-tier support for the following consoles:

  - Nintendo Super Famicom/Super Nintendo Entertainment System,
    including addon hardware:
      - Super Game Boy
      - Sufami Turbo
  - Nintendo Game Boy Advance

It also includes some level of support for these consoles:

  - BS-X Satellaview addon for the Super Famicom
  - Nintendo Famicom/Nintendo Entertainment System
  - Nintendo Game Boy
  - Nintendo Game Boy Color
  - Sega Master System
  - Sega Game Gear
  - Sega Megadrive/Genesis
  - NEC PC Engine/TurboGrafx 16 (but not the CD-ROM² System/TurboGrafx-CD)
  - NEC SuperGrafx
  - Bandai Wonderswan
  - Bandai Wonderswan Color

higan is actively supported on
FreeBSD 10 and above, and
Microsoft Windows 7 and above.
It also includes some level of support
for GNU/Linux and macOS.

higan is officially spelled with a lowercase "h", not a capital.

About this document
-------------------

This is the unofficial higan README,
a community-maintained introduction and reference.
It may be out of date
by the time you read this,
and it may contain errors or omissions.
If you find something that's wrong,
or you have a suggestion,
see "Unofficial higan resources" below.

Official higan resources
------------------------

  - [Official homepage](https://byuu.org/emulation/higan/)
  - [Official forum](https://board.byuu.org/viewforum.php?f=4)

Unofficial higan resources
--------------------------

  - [Source code repository](https://gitlab.com/higan/higan/)
    archives official higan releases
    and WIP snapshots
    since approximately v067r21.
  - [Quark shader repository](https://github.com/hizzlekizzle/quark-shaders)
    collects shaders that higan can use
    to add special effects like TV scanlines to its video output,
    or smarter algorithms for scaling up to modern PC resolutions.
    See [Installing custom shaders][shaders] below for details.
  - [Mercurial Magic](https://github.com/hex-usr/Mercurial-Magic/)
    is a tool for converting MSU-1 games and mods into a format
    higan can use.
    See [Importing MSU-1 games][msu1] below for details.

[shaders]: #installing-custom-shaders
[msu1]: #importing-msu-1-games

There are also other projects
based on current or older versions of higan,
in whole or in part,
that you might want to check out.

  - [Mednafen](https://mednafen.github.io/)
    is another multi-system emulator.
    Its SNES emulation is based on bsnes v059,
    from the time before bsnes was renamed to higan.
  - [BizHawk](http://tasvideos.org/BizHawk.html)
    is another multi-system emulator,
    specialising in the creation of
    tool-assisted speedruns.
    Its SNES emulation is based on bsnes v087.
  - [nSide](https://github.com/hex-usr/nSide)
    is a fork of higan that greatly enhances
    its NES emulation support,
    and adds minor features to the other cores too.
    It also restores the "balanced" SNES emulation core
    that was removed from higan in v099,
    which is less CPU intensive
    than the current accuracy-focussed core.
  - [bsnes-plus](https://github.com/devinacker/bsnes-plus)
    is a fork of bsnes v073
    that adds improved support for debugging SNES software.

Installing and uninstalling higan
=================================

The best way to install higan
depends on what platform you're using,
as well as whether you want to use official binaries
or compile the source-code from scratch.

Installing an official release on Windows
-----------------------------------------

Official higan releases are distributed in [7-zip][7z] archives.
You will need to install 7-zip,
or another compatible archiving tool,
to install higan.

[7z]: http://www.7-zip.org/

Once you have a suitable archiving tool,
extract the contents of the higan archive into a new folder.

When you're done,
the new folder should contain `higan.exe` and `icarus.exe`
along with other assorted files and directories
that describe the systems higan emulates.

You may put that folder wherever you like.

To run higan, open the `higan.exe` file.

Before you can actually play games,
you'll need to [import them](#importing-games)
and [configure higan](#configuring-higan).
If you want to play Game Boy Advance games,
you will need [a GBA BIOS](#installing-the-gba-bios).

Uninstalling an official release on Windows
-------------------------------------------

Delete the folder containing `higan.exe`
and the other associated data from the original archive.

To remove higan's configuration:

 1. Press Win+R to open the Run dialog
 2. Type `%LOCALAPPDATA%` and press Enter
    to open the folder where higan's configuration data lives
 3. Delete the subdirectories named `icarus` and `higan`
    if they exist.

You might also want to remove the games imported into higan's library
(including in-game saves and save-states):

 1. Press Win+R to open the Run dialog
 2. Type `%USERPROFILE%` and press Enter
    to open the folder where higan keeps its game library
 3. Delete the folder named `Emulation` if it exists

Compiling from source on Windows
--------------------------------

You will need a copy of the higan source-code.
If you download an official release from the higan homepage,
you will need [7-zip][7z] or a compatible tool to extract it.
Alternatively,
you may obtain higan source code from
[the unofficial git repo](https://gitlab.com/higan/higan/)
using the Git source-code management tool,
or by clicking the download button on the right-hand side of the web-page
and choosing an archive format.

You will need a C++ compiler to compile higan.
We recommend installing [TDM64-GCC][tdm],
preferably the latest version
but anything newer than 4.9 should be fine.
higan does not support building with clang++
(Clang is still not quite there yet for Windows)
nor Microsoft Visual C++
(last we checked, it didn't support all the C++ features higan uses).

**Note:** Make sure you get TDM64-GCC,
not TDM-GCC.
When compiled in x86 (32-bit) mode,
higan may crash at startup
because gcc targeting x86 does not support
Windows' structured exception handling (SEH).
Also,
historically in x86 mode
gcc has miscompiled a part of the NES emulation core.
See the higan forum
[for](https://board.byuu.org/viewtopic.php?p=41977#p41977)
[details](https://board.byuu.org/viewtopic.php?p=42253#p42253).

Once you've installed mingw-w64,
open a command-prompt window,
type `g++ --version`
then press Enter
to check it's installed correctly.
You should see a message like

    g++ 1.2.3 20010101
    Copyright (C) 2001 Free Software Foundation, Inc.
    This is free software; see the source for copying conditions.  There is NO
    warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

...except it should mention the version of mingw that you installed
and the corresponding dates.
If you see an error message like "command not found"
or "bad command or filename",
you may need to add mingw's "bin" folder
to your computer's `%PATH%`.
See the mingw documentation for help with that.

Once mingw is installed and available from the command prompt:

 1. Put the higan source code in some convenient location,
    like `C:\higan-src`
 2. Open the command-prompt
 3. Type `cd C:\higan-src`
    (or wherever you put the higan source)
    and press Enter
 4. Type `mingw32-make -C icarus compiler=g++` and press Enter
    to build the icarus import tool
 5. Type `mingw32-make -C higan compiler=g++` and press Enter
    to build the main higan executable

[tdm]: http://tdm-gcc.tdragon.net/download

Installing a compiled build on Windows
--------------------------------------

 1. In Windows Explorer,
    create the folder where you want higan to live
 2. Assuming you built higan in `C:\higan-src`,
    copy `C:\higan-src\icarus\out\icarus.exe`
    into the new folder
 3. Copy `C:\higan-src\icarus\Database` and its contents
    into the new folder
 4. Copy `C:\higan-src\higan\out\higan.exe`
    into the new folder
 5. Copy all the `*.sys` directories
    in `C:\higan-src\higan\systems`
    into the new folder

The new folder should now contain
`icarus.exe`,
`higan.exe`,
a folder named `Database`,
and half a dozen folders named after the systems higan emulates
with `.sys` at the end.
This is what you would get by downloading an official build,
as described under
[Installing an official release on Windows][instwin]
above.

[instwin]: #installing-an-official-release-on-windows

Before you can actually play games,
you'll need to [import them](#importing-games)
and [configure higan](#configuring-higan).
If you want to play Game Boy Advance games,
you will need [a GBA BIOS](#installing-the-gba-bios).

Uninstalling a compiled build on Windows
----------------------------------------

The process is the same as
[Uninstalling an official release on Windows][uninstwin]
above. You may also wish to delete the higan source folder.

[uninstwin]: #uninstalling-an-official-release-on-windows

Compiling from source on Linux
------------------------------

You will need a copy of the higan source-code.
If you download an official release from the higan homepage,
you will need [7-zip][7z] or a compatible tool to extract it.
Alternatively,
you may obtain higan source code from
[the unofficial git repo](https://gitlab.com/higan/higan/)
using the Git source-code management tool,
or by clicking the download button on the right-hand side of the web-page
and choosing an archive format.

You will also need GCC 4.9 or higher,
including the C and C++ compiler,
GNU Make,
and development files
(headers, etc.)
for the following libraries:

  - GTK 2.x
  - PulseAudio
  - Mesa
  - gtksourceview 2.x
  - Cairo
  - SDL 1.2
  - libXv
  - libAO
  - OpenAL
  - udev

On a Debian-derived Linux distribution,
you can install everything you need with a command like:

    sudo apt-get install build-essential libgtk2.0-dev libpulse-dev \
        mesa-common-dev libgtksourceview2.0-dev libcairo2-dev libsdl1.2-dev \
        libxv-dev libao-dev libopenal-dev libudev-dev

Once you have all the dependencies installed:

 1. Put the higan source code in some convenient location,
    like `~/higan-src`
 2. Open a terminal window
 3. Type `cd ~/higan-src`
    (or wherever you put the higan source)
    and press Enter
 4. Type `make -C icarus compiler=g++` and press Enter
    to build the icarus import tool
 5. Type `make -C higan compiler=g++` and press Enter
    to build the main higan executable

Installing a compiled build on Linux
------------------------------------

Assuming you have successfully compiled higan
as described in the previous section:

 1. Open a terminal window
 2. Type `cd ~/higan-src`
    (or wherever you put the higan source)
    and press Enter
 3. Type `make -C icarus install` and press Enter
    to install icarus and its game database
 4. Type `make -C higan install` and press Enter
    to install higan and its supporting files

This installs higan and its associated data files
into the `~/.local` directory hierarchy.

To confirm higan is installed correctly,
type `higan` in a terminal and press Enter.
If the higan window appears,
everything is working.
On the other hand,
if you get an error message like "command not found",
you should double-check that the directory `~/.local/bin`
is included in your `$PATH` environment variable
by running the following command in a terminal:

    echo "$PATH" | tr ':' '\n' | grep ~/.local/bin

If the above command prints the full path of `~/.local/bin`
(for example: `/home/yourname/.local/bin`)
then you should be good.
If it prints nothing,
you need to add the following line to `~/.profile`:

    export PATH=~/.local/bin:$PATH

(this line must be in `~/.profile` because
most GUIs do not read any other files at login)

If you also have a `~/.bash_profile`,
make sure it reads the contents of `~/.profile`
with a line like this:

    source ~/.profile

You will need to log out and log back in
for changes to `~/.profile` or `~/.bash_profile`
to take effect.

Before you can actually play games,
you'll need to [import them](#importing-games)
and [configure higan](#configuring-higan).
If you want to play Game Boy Advance games,
you will need [a GBA BIOS](#installing-the-gba-bios).

Uninstalling a compiled build on Linux
--------------------------------------

To uninstall higan,
as installed by the above instructions:

 1. Open a terminal window
 2. Type `cd ~/higan-src`
    (or wherever you put the higan source)
    and press Enter
 3. Type `make -C icarus uninstall` and press Enter
 4. Type `make -C higan uninstall` and press Enter

To remove higan's configuration,
delete the directory `~/.config/higan` as well.

To remove the games imported into higan's library
(including in-game saves and save-states),
delete the directory `~/Emulation`.

You may also wish to delete the higan source directory.

Installing the GBA BIOS
-----------------------

For most of the systems higan emulates,
the console itself contains (almost) no actual software,
so emulating the system does not require
infringing the copyright of the hardware manufacturer.
However,
the Game Boy Advance is different:
every device contains a standard library of software routines
for [common functions games require][bios],
often called a "BIOS"
by analogy with the Basic Input/Output System
used in IBM PC compatibles.

For the same legal reasons that commercial games
cannot be distributed with emulators,
the GBA BIOS cannot be distributed with higan,
but is required for GBA software to run.

If you have a real GBA and a flashcart,
the Internet contains many tools
that will extract the BIOS image so it can be copied
to your desktop computer.
The correct GBA BIOS file is exactly 16384 bytes long,
and has the SHA-256 hash
fd2547724b505f487e6dcb29ec2ecff3af35a841a77ab2e85fd87350abd36570.

Once you have the correct BIOS file:

 1. rename it to `bios.rom`
      - if you're using Windows,
        turn off "hide extensions for known file types"
        so you don't wind up with a file called
        `bios.rom.dat`
        or whatever the file's original extension was.
 2. Copy the file into higan`s `Game Boy Advance.sys` directory,
    alongside the `manifest.bml` file that is already there.
      - In Windows,
        find `Game Boy Advance.sys` in the same folder
        as `higan.exe`
      - In Linux,
        find `Game Boy Advance.sys` in
        `~/.local/share/higan/`

**Note:**
If you upgrade this version of higan to a newer version,
make sure the `bios.rom` file
winds up in the `Game Boy Advance.sys` directory
of the new version.

[bios]: http://problemkaputt.de/gbatek.htm#biosfunctions

Importing games
===============

What's a game folder?
---------------------

Importing regular games
-----------------------

Importing games with co-processor firmware
------------------------------------------

Importing BS-X games
--------------------

Importing Sufami Turbo games
----------------------------

Importing Super Game Boy games
------------------------------

Importing MSU-1 games
---------------------

Configuring higan
=================

Drivers
-------

Installing custom shaders
-------------------------

Controls
--------

  - mapping PC inputs to emulated controllers
  - configuring which emulated controllers
    are connected to the emulated system

Notes on specific emulation cores
=================================

The WonderSwan rotates!

Shaders look weird on SNES because of interlace/hi-res

GBA in-game saves are Difficult

PSG volume for the Mega Drive
(see https://board.byuu.org/viewtopic.php?p=42158#p42158 for info)

Frequently Asked Questions
==========================

where is the vsync option?

exclusive full-screen?

phones and tablets
