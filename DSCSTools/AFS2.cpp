#include "AFS2.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdint.h>

#define AFS2_MAGIC_VALUE '2SFA'

struct AFS2Header {
	uint32_t magic;
	uint32_t flags;
	uint32_t numFiles;
	uint32_t blockSize;
};

void extractAFS2(const boost::filesystem::path source, const boost::filesystem::path target) {
	if (boost::filesystem::exists(target) && !boost::filesystem::is_directory(target)) {
		std::cout << "Error: Target path exists and is not a directory, aborting." << std::endl;
		return;
	}
	if (!boost::filesystem::is_regular_file(source)) {
		std::cout << "Error: Source path doesn't point to a file, aborting." << std::endl;
		return;
	}

	boost::filesystem::ifstream input(source, std::ios::in | std::ios::binary);

	AFS2Header header;
	input.read((char*) &header, 0x10);

	if (header.magic!= AFS2_MAGIC_VALUE) {
		std::cout << "Error: not a MDB1 file. Value: " << header.magic<< std::endl;
		return;
	}

	uint16_t* fileIds = new uint16_t[header.numFiles];
	input.read((char*) fileIds, header.numFiles * 2);

	uint32_t* offsets = new uint32_t[header.numFiles + 1];
	input.read((char*) offsets, (header.numFiles + 1) * 4);

	if (input.tellg() != offsets[0])
		std::cout << "AFS2: Didn't reach expected end of header." << std::endl;

	boost::filesystem::create_directories(target);

	for (int i = 0; i < header.numFiles; i++) {
		input.seekg((std::streampos) ((uint32_t) input.tellg() + header.blockSize - 1) & -header.blockSize);
		uint32_t size = offsets[i + 1] - (uint32_t) input.tellg();

		char* data = new char[size];
		input.read(data, size);

		std::stringstream sstream;
		sstream << std::setw(6) << std::setfill('0') << std::hex << i << ".hca";

		boost::filesystem::path path(target / sstream.str());
		boost::filesystem::ofstream output(path, std::ios::out | std::ios::binary);
		
		output.write(data, size);
		output.close();
		delete data;
	}

	input.close();
}

void packAFS2(const boost::filesystem::path source, const boost::filesystem::path target) {
	if (!boost::filesystem::is_directory(source)) {
		std::cout << "Error: source path is not a directory." << std::endl;
		return;
	}

	if (!boost::filesystem::exists(target))
		boost::filesystem::create_directories(target.parent_path());
	else if (!boost::filesystem::is_regular_file(target)) {
		std::cout << "Error: target path already exists and is not a file." << std::endl;
		return;
	}

	boost::filesystem::ofstream output(target, std::ios::out | std::ios::binary);

	std::vector<boost::filesystem::path> files;

	for (auto i : boost::filesystem::directory_iterator(source))
		if (boost::filesystem::is_regular_file(i))
			files.push_back(i);

	AFS2Header header;
	header.magic = AFS2_MAGIC_VALUE;
	header.flags = 0x00020402;
	header.numFiles = files.size();
	header.blockSize = 0x20;

	output.write((char*) &header, 0x10);

	uint16_t* id = new uint16_t[header.numFiles];
	uint32_t* offsets = new uint32_t[header.numFiles + 1];

	offsets[0] = 0x10 + header.numFiles * 0x06 + (header.numFiles % 2) * 2 + 4;

	for (int i = 0; i < files.size(); i++) {
		output.seekp((offsets[i] + header.blockSize - 1) & -header.blockSize);

		boost::filesystem::ifstream input(files[i], std::ios::in | std::ios::binary);
		input.seekg(0, std::ios::end);
		std::streamoff length = input.tellg();
		input.seekg(0, std::ios::beg);

		char* buffer = new char[length];
		input.read(buffer, length);
		output.write(buffer, length);

		id[i] = i;
		offsets[i+1] = output.tellp();

		input.close();
		delete buffer;
	}

	output.seekp(0x10);
	output.write((char*) id, header.numFiles * 2);
	output.seekp((header.numFiles % 2) * 2, std::ios_base::cur);
	output.write((char*) offsets, (header.numFiles + 1) * 4);
	output.close();
}