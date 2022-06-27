# Patched Serious Sam Executable

This project contains code for building custom Serious Sam Classic executable files that includes a number of modern enhancements that aren't present in the original code, nor in the official Steam patch.

Original source code of the executable is taken from the `SeriousSam` project from [Serious Engine 1.10](https://github.com/Croteam-official/Serious-Engine).

This project is based on **Serious Engine 1 SDK**, meaning that it's perfectly compatible with vanilla classic games.

## Building

Before building the code, make sure to load in the submodules. Use `git submodule update --init --recursive command` to load files for all submodules.

To compile the executable, you'll need to use a compiler from Microsoft Visual C++ 6.0.

Full guide: https://github.com/DreamyCecil/SeriousSam_SDK107#building

## Notes

Once the project is compiled, a custom executable file should appear in the `Bin` directory near the solution file and also automatically placed into the `Bin` directory one level above the project directory (e.g. `C:/SeriousSam/Bin` if the sources are in `C:/SeriousSam/SamExePatch`).

To change the path where to copy the executable (if you wish to put sources separately from the game), consider changing the path in the post-build event (**Project properties** -> **Build Events** -> **Post-Build Event** -> **Command Line**).

## License

This project is licensed under the GNU GPL v2 (see LICENSE file).

Some of the code included with the SDK may not be licensed under the GNU GPL v2:

* DirectX8 SDK (Headers & Libraries) (`d3d8.h`, `d3d8caps.h` and `d3d8types.h` located in `Includes` folder) by Microsoft
