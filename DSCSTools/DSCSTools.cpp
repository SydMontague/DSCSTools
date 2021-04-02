#include "DSCSTools.h"
#include "MDB1.h"
#include "SaveFile.h"
#include "EXPA.h"
#include "AFS2.h"

// TODO expose advanced compression mode
// TODO Boost.Python
// TODO GitHub Actions
// TODO update README
// TODO split off executable
// TODO build as library

void printUse() {
	std::cout << "Modes:" << std::endl;
	std::cout << "	--extract <sourceFile> <targetFolder>" << std::endl;
	std::cout << "		Extracts the given MDB1 into a folder." << std::endl;
	std::cout << "	--extractFile <sourceFile> <targetFolder> <filePath>" << std::endl;
	std::cout << "		Extracts a file from a given MDB1 into a folder." << std::endl;
	std::cout << "	--pack <sourceFolder> <targetFile> [--disable-compression]" << std::endl;
	std::cout << "		Repacks the given folder into an encrypted MDB1." << std::endl;
	std::cout << "		Optional --disable-compression. Don't use for archives >= 4GiB!" << std::endl;
	std::cout << "	--crypt <sourceFile> <targetFolder>" << std::endl;
	std::cout << "		De- and Encrypts a mvgl file using the keys from DSCS." << std::endl;
	std::cout << "	--savedecrypt <sourceFile> <targetFolder>" << std::endl;
	std::cout << "		Decrypts a savefile (system_data.bin, 000X.bin, slot_000X.bin)." << std::endl;
	std::cout << "	--saveencrypt <sourceFile> <targetFolder>" << std::endl;
	std::cout << "		Encrypts a savefile (system_data.bin, 000X.bin, slot_000X.bin)." << std::endl;
	std::cout << "	--mbeextract <source> <targetFolder>" << std::endl;
	std::cout << "		Extracts a .mbe file or a directory of them into CSV, " << std::endl;
	std::cout << "		as long as it's structure is defined in the structure.json file." << std::endl;
	std::cout << "	--mbepack <sourceFolder> <targetFile>" << std::endl;
	std::cout << "		Repacks an .mbe folder containing CSV files back into a .mbe file " << std::endl;
	std::cout << "		as long as it's structure is found and defined in the structure.json file." << std::endl;
	std::cout << "	--afs2extract <source> <targetFolder>" << std::endl;
	std::cout << "		Extracts the given AFS2 formatted archive info a folder." << std::endl;
	std::cout << "	--afs2pack <sourceFolder> <targetFile>" << std::endl;
	std::cout << "		Repacks the given folder info the AFS2 format." << std::endl;
	std::cout << "		File order is alphabetical and relevant, filenames themselves are lost." << std::endl;
}

int main(int argc, char** argv) {
	if (argc < 4) {
		printUse();
		return 0;
	}
	boost::filesystem::path source = boost::filesystem::exists(argv[2]) ? argv[2] : boost::filesystem::current_path().append(argv[2]);
	boost::filesystem::path target = argv[3];
	
	if(!target.has_root_directory())
		target = boost::filesystem::current_path().append(argv[3]);
	
	try {
		if (strncmp("--extractFile", argv[1], 14) == 0 && argc >= 5) {
			dscstools::mdb1::extractMDB1File(source, target, argv[4]);
			std::cout << "Done" << std::endl;
		}
		else if (strncmp("--extract", argv[1], 10) == 0) {
			dscstools::mdb1::extractMDB1(source, target);
			std::cout << "Done" << std::endl;
		}
		else if (strncmp("--pack", argv[1], 7) == 0) {
			bool compress = argc < 5 || (strncmp("--disable-compression", argv[4], 22) != 0);
			dscstools::mdb1::packMDB1(source, target, compress ? dscstools::mdb1::CompressMode::normal : dscstools::mdb1::CompressMode::none, std::cout);
			std::cout << "Done" << std::endl;
		}
		else if (strncmp("--crypt", argv[1], 8) == 0) {
			dscstools::mdb1::cryptFile(source, target);
			std::cout << "Done" << std::endl;
		}
		else if (strncmp("--savedecrypt", argv[1], 14) == 0) {
			dscstools::savefile::decryptSaveFile(source, target);
			std::cout << "Done" << std::endl;
		}
		else if (strncmp("--saveencrypt", argv[1], 14) == 0) {
			dscstools::savefile::encryptSaveFile(source, target);
			std::cout << "Done" << std::endl;
		}
		else if (strncmp("--mbeextract", argv[1], 13) == 0) {
			dscstools::mbe::extractMBE(source, target);
			std::cout << "Done" << std::endl;
		}
		else if (strncmp("--mbepack", argv[1], 10) == 0) {
			dscstools::mbe::packMBE(source, target);
			std::cout << "Done" << std::endl;
		}
		else if (strncmp("--afs2extract", argv[1], 14) == 0) {
			dscstools::afs2::extractAFS2(source, target);
			std::cout << "Done" << std::endl;
		}
		else if (strncmp("--afs2pack", argv[1], 11) == 0) {
			dscstools::afs2::packAFS2(source, target);
			std::cout << "Done" << std::endl;
		}
		else {
			printUse();
		}
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}

	return 0;
}

