# DSCSTools
DSCSTools are modding tools for the Steam release of "Digimon Story Cyber Sleuth: Complete Edition", although they may also work with other releases and other games that use the same file formats.

# Current Features
* Decrypt and Encrypt .steam.mvgl files
  * some of the assets (DSDB, DSDBA, DSDBP, DSDBPse, DSDBS, DSDBse and DSDBSP) of the PC release are encrypted, the tool can decrypt and encrypt files
* Extract MDB1 archives
* Repack/Create MDB1 archives
  * archives get recreated from scratch, files can be added, removed and modified at will
  * optionally without compressing the file (faster, larger file size) as long as the final file size is less than 4GiB
* Extract and Rebuild AFS2 archives
  * The resulting files are, like all sound files in the HCA format. You can use [vgmstream](https://github.com/vgmstream/vgmstream) to convert them.
* Decrypt and Encrypt PC save files
* Extract and import basic MBE files
  * currently supports int8, int16, int32, float, int32 array and string field types

## Planned Features
* a user friendly GUI

# Usage
Currently only a command line interface is available. It is recommended to use a Windows system, due to the MDB1 format using Windows file separators.
The tool *should* work on *nix systems, but is not as well tested.

To create mods it is strong recommended to use Pherakki's [SimpleDSCSModManager](https://github.com/Pherakki/SimpleDSCSModManager).

**Do not use Microsoft Excel to modify extracted CSV files, it does *not* create RFC 4180 compliant CSV.** Use LibreOffice/OpenOffice as an alternative.

```
En/Decrypt:   DSCSTools --crypt <sourceFile> <targetFile>
Extract:      DSCSTools --extract <sourceFile> <targetDirectory>
Repack:       DSCSTools --pack <sourceDirectory> <targetFile> [--disable-compression]
Save Decrypt: DSCSTools --savedecrypt <sourceFile> <targetFile>
Save Encrypt: DSCSTools --saveencrypt <sourceFile> <targetFile>
MBE Extract:  DSCSTools --mbeextract <source> <targetFolder>
MBE Repack:   DSCSTools --mbepack <sourceFolder> <targetFile>
AFS2 Extract: DSCSTools --afs2extract <source> <targetFolder>
AFS2 Repackk: DSCSTools --afs2pack <sourceFolder> <targetFile>
```

## MBE Structure files
In order for the MBE functions to work it needs to assume a data structure. For this a `structure.json` must be present in the `structures` folder of the tool.
It contains simple `regexPattern: structureDefinition.json` associations. The tool match the currently handled file path with the patterns in the structure.json and pick the first match.
You may need to maintain the folder structure of the files (most notably text and message files) for the tool to detect them properly.

The structure definition is another JSON file following this format:
Currently supported field types are `byte` (int8), `short` (int16), `int` (int32), `float`, `int array` and `string`.

```
{
	tableName1: {
		fieldName1: fieldType1,
		fieldName2: fieldType2,
		fieldNameN: fieldTypeN
	},
	tableName2: {
		fieldName1: "string",
		fieldName2: "int",
		fieldNameN: "int"
	},
	tableNameN: {
		fieldName1: fieldType1,
		fieldName2: fieldType2,
		fieldNameN: fieldTypeN
	}
}
```


# Build
The project uses CMake. On Windows you can use Visual Studio's CMake integration.

On *nix systems compiling should be a simple

```
$ git clone git@github.com:SydMontague/DSCSTools.git
$ cd <project dir>
$ cmake .
$ make install
```

With the binary being located in /DSCSTools/

## Requirements
* Boost 1.66 or higher
* CMake 3.10 or higher
* A C++ compiler

The tool uses the [doboz compression library](https://voxelium.wordpress.com/2011/03/19/doboz-compression-library-with-very-fast-decompression/). [License Notice](https://github.com/SydMontague/DSCSTools/blob/master/libs/doboz/COPYING.txt)

# Contact
* Discord: SydMontague#8056, or in the [Digimon Discord Community](https://discord.gg/0VODO3ww0zghqOCO)
* directly on GitHub
* E-Mail: sydmontague@web.de
* Reddit: [/u/Sydmontague](https://reddit.com/u/sydmontague)

# Other DSCS Modding Projects/Tools
* [SimpleDSCSModManager](https://github.com/Pherakki/SimpleDSCSModManager) by Pherakki
* [Blender-Tools-for-DSCS](https://github.com/Pherakki/Blender-Tools-for-DSCS/) by Pherakki
* [NutCracker](https://github.com/SydMontague/NutCracker)
  * a decompiler for the game's Squirrel script files

# Disclaimer
This project is work-in-progress and I'm still learning how to properly use C++ and CMake, so excuse me if I make some nooby mistakes. :)


