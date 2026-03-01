# Better-NFCReader
A better NFC UID Reader for 3DS!

It now read 0x7 tags, which is widely used, and it even generate a QR code from the UID which you can scan using your phone!

## Building

To build this project, you will need to have `devkitARM` and `citro3d`/`citro2d` installed.

Make sure the `DEVKITARM` environment variable is set.

Then, you can use the following commands:

*   `make`: Build the project and create a `Better-NFCReader.3dsx` file.
*   `make run`: Run the project in the Citra emulator.
*   `make cia`: Create a `Better-NFCReader.cia` file.
*   `make clean`: Clean the build files.

Possible future additions:<br>
1. List of recently scanned UID's<br>
2. Save UID's to file immeditaly (.txt)<br>
3. Read out .txt with UID's and display this list on screen.<br>

Please note:<br>
This was created in the oringnal author's high school days for fun. It is designed to scan the specific type of RFID tag he's high school used. And it will only show the UID (which is the part he's high school used for their systems). It does not do anythings other than that (For now tho)<br>
                                                                                                                                         
## "Thank you" area

Thanks for the Github user "nayuki" for creating and sharing the awesome library "qrcodegen", which is used to provide UID to QR code function

Thanks for the Github user "MrJPGames" for creating the project "NFCReader", if without the awesome codebase it provides, this project is imposible to make.

Thanks for ALL devkitpro developers making this awesome toolchain. 

Thanks Thenaya developer for sharing the awesome .rsf file, without it, CIA build would be impossible

btw this project is indexed on DeepWiki now: https://deepwiki.com/cylin577/Better-NFCReader
