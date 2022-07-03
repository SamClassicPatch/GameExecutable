# Serious Sam Classic Patch

This custom Serious Sam Classic patch includes a lot of modern enhancements that aren't present in the original code, nor the official Steam patches.

Original source code of the executable is taken from the `SeriousSam` project from [Serious Engine 1.10](https://github.com/Croteam-official/Serious-Engine).

Complete description of the [latest patch release](https://github.com/SamClassicPatch/GameExecutable/releases/latest) can be found on the [project wiki](https://github.com/SamClassicPatch/GameExecutable/wiki).

## Building

Before building the code, make sure to clone [`CoreLib`](https://github.com/SamClassicPatch/CoreLib) project in the same directory as this repository and build it.

To compile the executable, you'll need to use a compiler from Microsoft Visual C++ 6.0.

Full guide: https://github.com/DreamyCecil/SeriousSam_SDK107#building

## Notes

Once the project is compiled, an executable file should appear in the `Bin` directory near the solution file and also automatically placed into the `Bin` directory two levels above the project directory (e.g. `C:/SeriousSam/Bin` if the sources are in `C:/SeriousSam/Sources`).

To change the path where to copy the executable (if you wish to put sources separately from the game), consider changing the path in the post-build event (**Project properties** -> **Build Events** -> **Post-Build Event** -> **Command Line**).

## License

This project is licensed under the GNU GPL v2 (see LICENSE file).
