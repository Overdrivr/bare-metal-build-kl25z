This repository contains a blinky led, bare-metal example for
the NXP's Kinetis KL25Z Freedom board, using Ninja as build system.

With this build method, you don't need to use an IDE (Integrated Development
Environment) for developing code embedded code. You can keep using your favorite
text editor (such as [`Atom`](https://atom.io/) or [`Sublime`](https://www.sublimetext.com/)
 or whatever) and trigger your builds with a terminal.

This approach has many advantages over using a monolithic tool for development,
such as versioning-friendly, reproducible builds, portability, etc.

# Requirements

You will need:
* [Ninja](https://ninja-build.org/)
* [GNU GCC ARM Embedded toolchain](https://launchpad.net/gcc-arm-embedded).

You can make sure both of them are available system-wide by running:

```
ninja -v
arm-none-eabi-gcc -v
```

If this doesn't throw any errors, you are all set.

# The board

This example uses an [NXP freedom KL25Z board](http://www.nxp.com/products/software-and-tools/hardware-development-tools/freedom-development-boards/freedom-development-platform-for-kinetis-kl14-kl15-kl24-kl25-mcus:FRDM-KL25Z).

It is recommended to update the bootloader KL25Z with the latest, available at
[pemicro](http://www.pemicro.com/opensda/) (in category *OpenSDA Firmware*)

To update the bootloader:

1. disconnect the board
2. hold the press-button
3. connect the usb cable to the board (on the OpenSDA USB port)
4. The board should now be visible as a drive called `BOOTLOADER`. Drag and drop the downloaded file on this drive.
5. Disconnect the usb cable
6. Reconnect the usb cable. The board's drive should now have the title `FRDM-KL25Z`.

# Build & flash

To build the project, run in a terminal, inside the folder where you cloned this
repository

```
ninja
```

To flash the board, simply copy-paste the file `build/main.bin` into the board's drive.
At this point, the RGB led should blink led.

# Explanation

This build performs the following tasks (defined in `build.ninja`):

1. Build the KL25Z specific files and assemble them into library `lib_MKL25Z4.a`.
This library contains essential code, such as startup code that tells the
microcontroller what to do when powered-up, and defines the location of
peripherals in the memory.
2. Build `main.cpp` file (our blinky program).
3. Link `main.o`, using the linker file `platform/MKL25Z4/linker/gcc/MKL25Z128xxx4_flash.ld` and `lib_MKL25Z4.a`
The linker file is specific to the KL25Z because it defines for instance
the memory mappings and size.
4. At this point, we need to convert the `main.elf` file into a binary file that
is accepted by the mass-storage bootloader of the board.
We use the `arm-none-eabi-objcopy` utility to convert it into a binary form.
5. Done !

In this repository we use the following folder structure.

```
\core
\platform
  \MKL25Z4
\main
  main.cpp
```
In the `main` folder, `main.cpp` implements the final program.

The `platform` folder contains the KL25Z-specific files. Those files directly
come from the Kinetis SDK 1.3. Those files can be obtained from the [online SDK builder](https://kex.nxp.com/en/welcome).

To get the files, generate an SDK for the FRDM-KL25Z board, with ARM GCC toolchain.
Once you have downloaded the archive, unzip it. The files are located in
`platform\devices`.

# Roadmap

* Validate the build with the files from the generated archive (that step changed recently).
* Add a library
* Add a testing library and tests
