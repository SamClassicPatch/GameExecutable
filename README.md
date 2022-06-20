# Custom Serious Sam Executable

Original source code of the executable is taken from the `SeriousSam` project from [Serious Engine 1.10](https://github.com/Croteam-official/Serious-Engine).

This project is based purely on Serious Engine 1 SDK (v1.05 or v1.07) and this repository should reside alongside the `Engine` folder (just like `EntitiesMP`, `GameMP` etc.).

A VS2013+ compatible version can be found at https://github.com/DreamyCecil/SeriousSam_SDK107

## Building

To compile the executable, you'll need to use a compiler from Microsoft Visual C++ 6.0.

Full guide: https://github.com/DreamyCecil/SeriousSam_SDK107#building

## Notes

By default, engine libraries are taken from the game's `Bin` directory. If you wish to change that (for example take libraries from the sources directory), manually add new paths to libraries (**Project properties** -> **VC++ Directories** -> **Include Directories**).

Once the project is compiled, a custom executable (`SeriousSam_Patch.exe` by default) is automatically placed into the `Bin` directory two levels above the project directory (e.g `C:/SeriousSam/Bin` if the sources are in `C:/SeriousSam/Sources/SamExePatch`).

To change the path where to copy the executable (if you wish to put sources separately from the game), consider changing the path in the custom build step (**Project properties** -> **Build Events** -> **Post-Build Event** -> **Command Line**).

## License

This project is licensed under the GNU GPL v2 (see LICENSE file).

Some of the code included with the SDK may not be licensed under the GNU GPL v2:

* DirectX8 SDK (Headers & Libraries) (`d3d8.h`, `d3d8caps.h` and `d3d8types.h` located in `D3D8` folder) by Microsoft
