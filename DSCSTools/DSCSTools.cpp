// DSCSTools.cpp : Defines the entry point for the application.
//

#include "DSCSTools.h"
#include "MDB1.h"

void printUse() {
	std::cout << "Modes:" << std::endl;
	std::cout << "	--extract <sourceFile> <targetFolder>" << std::endl;
	std::cout << "		Extracts the given decrypted MDB1 into a folder." << std::endl;
	std::cout << "	--pack <sourceFolder> <targetFile> [--disable-compression]" << std::endl;
	std::cout << "		Repacks the given folder into a decrypted MDB1." << std::endl;
	std::cout << "		Optional --disable-compression. Don't use for archives >= 4GiB!" << std::endl;
	std::cout << "	--crypt <sourceFile> <targetFolder>" << std::endl;
	std::cout << "		De- and Encrypts a file using the keys from DSCS." << std::endl;
}

int main(int argc, char** argv)
{
	if (argc < 4) {
		printUse();
		return 0;
	}
	
	boost::filesystem::path source = boost::filesystem::current_path().append(argv[2]);
	boost::filesystem::path target = boost::filesystem::current_path().append(argv[3]);

	if (strncmp("--extract", argv[1], 10) == 0) {
		extractMDB1(source, target);
		std::cout << "Done" << std::endl;
	}
	else if (strncmp("--pack", argv[1], 7) == 0) {
		bool compress = argc < 5 || (argv[4] == "--disable-compression");
		packMDB1(source, target, compress);
		std::cout << "Done" << std::endl;
	}
	else if (strncmp("--crypt", argv[1], 8) == 0) {
		cryptFile(source, target);
		std::cout << "Done" << std::endl;
	}
	else {
		printUse();
	}

	return 0;
}

