# DSCSTools
DSCSTools are modding tools for the Steam release of "Digimon Story Cyber Sleuth: Complete Edition", although they may also work with other releases and other games that use the same file formats.

# Current Features
* Decrypt and Encrypt .steam.mvgl files
  * some of the assets (DSDB, DSDBA, DSDBP, DSDBPse, DSDBS, DSDBse and DSDBSP) of the PC release are encrypted, the tool can decrypt and encrypt files
* Extract MDB1 archives
* Repack/Create MDB1 archives
  * archives get recreated from scratch, files can be added, removed and modified at will
  * optionally without compressing the file (faster, larger file size) as long as the final file size is less than 4GiB

## Planned Features
Soon™

# Usage
Currently only a command line interface is available. It is recommended to use a Windows system, due to the MDB1 format using Windows file separator.
The tool *should* work on *nix systems, albeit behaving a bit differently, but is not as well tested.

```
En/Decrypt:   DSCSTools --crypt <sourceFile> <targetFile>
Extract:      DSCSTools --extract <sourceFile> <targetDirectory>
Repack:       DSCSTools --pack <sourceDirectory> <targetFile> [--disable-compression]
```

# Build
The project uses CMake. On Windows you can use Visual Studio's CMake integration.

On *nix systems compiling should be a simple

```
$ git clone git@github.com:SydMontague/DSCSTools.git
$ cd <project dir>
$ cmake .
$ make
```

With the binary being located in /DSCSTools/

## Requirements
* Boost 1.65 or higher
* CMake 3.10 or higher
* A compiler

The tool uses the [doboz compression library](https://voxelium.wordpress.com/2011/03/19/doboz-compression-library-with-very-fast-decompression/). [License Notice](https://github.com/SydMontague/DSCSTools/blob/master/libs/doboz/COPYING.txt)

# Contact
* Discord: SydMontague#8056, or in the [Digimon Discord Community](https://discord.gg/0VODO3ww0zghqOCO)
* directly on GitHub
* E-Mail: sydmontague@web.de
* Reddit: [/u/Sydmontague](https://reddit.com/u/sydmontague)

# Disclaimer
This project is work-in-progress and I'm still learning how to properly use C++ and CMake, so excuse me if I make some nooby mistakes. :)


