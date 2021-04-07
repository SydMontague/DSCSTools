# DSCSTools
DSCSTools are tools to extract and repack the game files for the Steam release of "Digimon Story Cyber Sleuth: Complete Edition". 
They should work with other releases of the game as well as potentially other games based on the same engine, with some caveats.

A library version to be linked by other tools is also available.

# Current Features
* Extract MDB1 (.mvgl) archives
  * will work with encrypted and decrypted archives
* Extract single file from MDB1 archive
* Repack/Create MDB1 (.mvgl) archives
  * archives get recreated from scratch, files can be added, removed and modified at will
  * output will be encrypted, for use with non-PC platforms you have to decrypt the file first
  * optional: with advanced compression, storing identical data only once. ~5% size improvement
  * optional: without compressing the file (faster build), final archive must be <= 4 GiB in size
* Extract and import MBE files
  * currently supports int8, int16, int32, float, int32 array and string field types
* Extract and Rebuild AFS2 archives
  * The resulting files are, like all sound files, in the HCA format. You can use [vgmstream](https://github.com/vgmstream/vgmstream) to convert them.
* Decrypt and Encrypt .steam.mvgl files
  * some of the assets (DSDB, DSDBA, DSDBP, DSDBPse, DSDBS, DSDBse and DSDBSP) of the PC release are encrypted, the tool can decrypt and encrypt files
* Decrypt and Encrypt PC save files

# Usage
The tool itself is command line only. The MBE functionality requires you to have the `structures` folder relative from where you're calling the tool from.
Hence it's recommended to navigate to the folder containing the DSCSTools binary first.

Pherakki's [SimpleDSCSModManager](https://github.com/Pherakki/SimpleDSCSModManager) provides an easy to use GUI for the tools, while also serving as a general mod manager. 

**Do not use Microsoft Excel to modify extracted CSV files, it does *not* create RFC 4180 compliant CSV.** Use LibreOffice/OpenOffice as an alternative.

```
Extract:      DSCSTools --extract <sourceFile> <targetDirectory>
File Extract: DSCSTools --extractFile <sourceFile> <targetDirectory> <fileName>
Repack:       DSCSTools --pack <sourceDirectory> <targetFile> [--disable-compression|--advanced-compression]
MBE Extract:  DSCSTools --mbeextract <source> <targetFolder>
MBE Repack:   DSCSTools --mbepack <sourceFolder> <targetFile>
AFS2 Extract: DSCSTools --afs2extract <source> <targetFolder>
AFS2 Repackk: DSCSTools --afs2pack <sourceFolder> <targetFile>
En/Decrypt:   DSCSTools --crypt <sourceFile> <targetFile>
Save Decrypt: DSCSTools --savedecrypt <sourceFile> <targetFile>
Save Encrypt: DSCSTools --saveencrypt <sourceFile> <targetFile>
```

## MBE Structure files
In order for the MBE functions to work it needs to assume a data structure. For this a `structure.json` must be present in the `structures` folder of the tool or whereever you're calling it from.
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

## Library
The tool can also be used as a library by other software. You can find the shared library+documented headers in the downloads.

You can also use the library as Python module. Simply install the shared library in a place your Python program searchs for modules and you should be able
to import it via `import DSCSTools`. Note, on Windows you'll have to rename the `DSCSTools.dll` to `DSCSTools.pyd`.

Please note, the currently prebuild binaries only work on Python 3.8. Other Python versions require the library to be recompiled.

For Python documentation head to the [Wiki Entry](https://github.com/SydMontague/DSCSTools/wiki/Python-Interfaces)


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

The tool uses:
* the [doboz compression library](https://voxelium.wordpress.com/2011/03/19/doboz-compression-library-with-very-fast-decompression/). [License Notice](https://github.com/SydMontague/DSCSTools/blob/master/libs/doboz/COPYING.txt)
* AriaFallah's [csv-parser](https://github.com/AriaFallah/csv-parser). [License Notice](https://github.com/SydMontague/DSCSTools/blob/master/libs/csv-parser/LICENSE)

# Contact
* Discord: SydMontague#8056, or in either the [Digimon Modding Community](https://discord.gg/cb5AuxU6su) or [Digimon Discord Community](https://discord.gg/0VODO3ww0zghqOCO)
* directly on GitHub
* E-Mail: sydmontague@web.de
* Reddit: [/u/Sydmontague](https://reddit.com/u/sydmontague)

# Other DSCS Modding Projects/Tools
* [SimpleDSCSModManager](https://github.com/Pherakki/SimpleDSCSModManager) by Pherakki
* [Blender-Tools-for-DSCS](https://github.com/Pherakki/Blender-Tools-for-DSCS/) by Pherakki
* [NutCracker](https://github.com/SydMontague/NutCracker)
  * a decompiler for the game's Squirrel script files
