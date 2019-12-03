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
In order to use the save file encryption/decryption batch scripts, extract the release to C:\DSCSTools\ (Admin access may be required), and copy all files in "C:\Users\<username>\AppData\Local\BANDAI NAMCO Entertainment\Digimon Story Cyber Sleuth Complete Edition\Saved\SaveGames\<SteamUserID>\ to the folder. After that, double-click on savedecrypt.bat to decrypt the save(s) (decrypted files have _dec appended to the end of their names). Finally, edit each file to your liking (most users only need 000X_dec.bin, since this contains ALL in-game data; the only other file that may need editing is slot_000X_dec.bin, and that is only if your game settings are corrupt somehow, which can possibly happen if you try to cheat with Cheat Engine in crazy ways).

Currently only a command line interface is available. It is recommended to use a Windows system, due to the MDB1 format using Windows file separator.
The tool *should* work on *nix systems, albeit behaving a bit differently, but is not as well tested.

```
En/Decrypt:   DSCSTools --crypt <sourceFile> <targetFile>
Extract:      DSCSTools --extract <sourceFile> <targetDirectory>
Repack:       DSCSTools --pack <sourceDirectory> <targetFile> [--disable-compression]
Decrypt Save: DSCSTools --savedecrypt <sourceFile> <targetFile>
Encrypt Save: DSCSTools --saveencrypt <sourceFile> <targetFile>
```

If moving a Switch save to PC, rename each file and add "_dec" to the end, delete the first 32 (0x20) bytes using a hex editor, then copy the files to the DSCSTools directory and run saveencrypt.bat to encrypt them properly. This process is untested, as I do not have a Switch console to test it on. Let me know if this works for you.

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


