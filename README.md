# DSCSTools
DSCSTools are modding tools for the Steam release of "Digimon Story Cyber Sleuth: Complete Edition", although they may also work with other releases and other games that use the same file formats.

# Current Features
* Decrypt and Encrypt .steam.mvgl files
  * some of the assets (DSDB, DSDBA, DSDBP, DSDBPse, DSDBS, DSDBse and DSDBSP) of the PC release are encrypted, the tool can decrypt and encrypt files
* Extract MDB1 archives
* Repack/Create MDB1 archives
  * archives get recreated from scratch, files can be added, removed and modified at will
  * optionally without compressing the file (faster, larger file size) as long as the final file size is less than 4GiB
* Decrypt and Encrypt PC save files
* Extract basic MBE files
  * currently supports uint32 and string field types, more to come

## Planned Features
* Advanced MBE support
  * support for all used field types
  * import/repack support

# Usage
Currently only a command line interface is available. It is recommended to use a Windows system, due to the MDB1 format using Windows file separator.
The tool *should* work on *nix systems, but is not as well tested.

In order to use the repack function for large archives (-> DSDB) it is recommended that you have at least 16 GiB of RAM installed.
An alternative is to use the DSDBA archive for any modified files, since it's contents have a higher priority than the main DSDB.

```
En/Decrypt:   DSCSTools --crypt <sourceFile> <targetFile>
Extract:      DSCSTools --extract <sourceFile> <targetDirectory>
Repack:       DSCSTools --pack <sourceDirectory> <targetFile> [--disable-compression]
Save Decrypt: DSCSTools --savedecrypt <sourceFile> <targetFile>
Save Encrypt: DSCSTools --saveencrypt <sourceFile> <targetFile>
MBE Extract:  DSCSTools --mbeextract <source> <targetFolder>
MBE Repack:   DSCSTools --mbepack <sourceFolder> <targetFile>
```

## MBE Structure files
In order for the MBE functions to work it need to assume a data structure. For this a `structure.json` must be present in the `structures` folder of the tool.
It contains simple `regexPattern: structureDefinition.json` associations. The tool match the currently handled file path with the patterns in the structure.json and pick the first match.

The structure definition is another JSON file following this format:
Currently supported field types are "string" and "int".

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
* Boost 1.65 or higher
* CMake 3.10 or higher
* A C++ compiler

The tool uses the [doboz compression library](https://voxelium.wordpress.com/2011/03/19/doboz-compression-library-with-very-fast-decompression/). [License Notice](https://github.com/SydMontague/DSCSTools/blob/master/libs/doboz/COPYING.txt)

# Contact
* Discord: SydMontague#8056, or in the [Digimon Discord Community](https://discord.gg/0VODO3ww0zghqOCO)
* directly on GitHub
* E-Mail: sydmontague@web.de
* Reddit: [/u/Sydmontague](https://reddit.com/u/sydmontague)

# Disclaimer
This project is work-in-progress and I'm still learning how to properly use C++ and CMake, so excuse me if I make some nooby mistakes. :)


