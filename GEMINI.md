# Gemini Code Understanding

## Project Overview

This project is a Nintendo 3DS homebrew application called "Better-NFCReader". Its purpose is to read the UID (Unique ID) from NFC/RFID tags, specifically type 0x7 tags. The application displays the UID of a scanned tag on the 3DS screen.

The project is written in C++ and uses the `citro2d` and `citro3d` libraries for 2D and 3D graphics, and the `ctru` library for low-level access to the 3DS hardware, including the NFC reader. The application is built using the devkitARM toolchain and a Makefile.

## Building and Running

The project uses a `Makefile` for building and running the application. The following commands are available:

*   **`make`**: Compiles the project and creates a `Better-NFCReader.3dsx` file, which can be run by 3DS homebrew launchers.
*   **`make run`**: Builds the project and runs it in the Citra 3DS emulator. (Requires Citra to be installed and in the system's PATH).
*   **`make cia`**: Creates a `Better-NFCReader.cia` file, which can be installed on a modified 3DS console.
*   **`make cci`**: Creates a `Better-NFCReader.3ds` file.
*   **`make clean`**: Removes all build files.

To build the project, you need to have `devkitARM` and `citro2d`/`citro3d` installed and configured. The `DEVKITARM` environment variable must be set.

## Development Conventions

*   The main application logic is in `source/main.cpp`.
*   The code is written in C++11.
*   The project uses a custom `Makefile` for building.
*   Header files are expected to be in an `include` directory (which is currently missing).
*   Resources, such as fonts, are stored in the `romfs` directory and included in the application bundle.
*   The application follows a simple state machine pattern (`NFC_App_State`) to manage the scanning and tag detection states.
*   The UI is rendered using the `citro2d` library.
