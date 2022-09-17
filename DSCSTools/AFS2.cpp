#include "include/AFS2.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdint.h>

namespace dscstools {
	namespace afs2 {
		constexpr auto AFS2_MAGIC_VALUE = '2SFA';

		struct AFS2Header {
			uint32_t magic;
			uint32_t flags;
			uint32_t numFiles;
			int32_t blockSize;
		};

		void extractAFS2(const boost::filesystem::path source, const boost::filesystem::path target) {
			if (boost::filesystem::exists(target) && !boost::filesystem::is_directory(target))
				throw std::invalid_argument("Error: Target path exists and is not a directory, aborting.");
			if (!boost::filesystem::is_regular_file(source))
				throw std::invalid_argument("Error: Source path doesn't point to a file, aborting.");

			boost::filesystem::ifstream input(source, std::ios::in | std::ios::binary);

			AFS2Header header;
			input.read((char*)&header, 0x10);

			if (header.magic != AFS2_MAGIC_VALUE) 
				throw std::invalid_argument("Error: not an AFS2 file. Value: ");

			auto fileIds = std::make_unique<uint16_t[]>(header.numFiles);
			input.read(reinterpret_cast<char*>(fileIds.get()), header.numFiles * 2);

			auto offsets = std::make_unique<uint32_t[]>(header.numFiles + 1LL);
			input.read(reinterpret_cast<char*>(offsets.get()), (header.numFiles + 1) * 4);

			if (input.tellg() < header.blockSize)
				input.seekg(header.blockSize);
			if (input.tellg() != offsets[0])
				throw std::invalid_argument("AFS2: Didn't reach expected end of header.");

			if(target.has_parent_path())
				boost::filesystem::create_directories(target);

			for (size_t i = 0; i < header.numFiles; i++) {
				input.seekg((std::streampos) ((uint32_t)input.tellg() + header.blockSize - 1) & -header.blockSize);
				uint32_t size = offsets[i + 1] - (uint32_t)input.tellg();

				auto data = std::make_unique<char[]>(size);
				input.read(data.get(), size);

				std::stringstream sstream;
				sstream << std::setw(6) << std::setfill('0') << std::hex << i << ".hca";

				boost::filesystem::path path(target / sstream.str());
				boost::filesystem::ofstream output(path, std::ios::out | std::ios::binary);

				output.write(data.get(), size);
			}
		}

		void packAFS2(const boost::filesystem::path source, const boost::filesystem::path target) {
			if (!boost::filesystem::is_directory(source)) 
				throw std::invalid_argument("Error: source path is not a directory.");

			if (!boost::filesystem::exists(target)) {
				if (target.has_parent_path())
					boost::filesystem::create_directories(target.parent_path());
			}
			else if (!boost::filesystem::is_regular_file(target))
				throw std::invalid_argument("Error: target path already exists and is not a file.");

			boost::filesystem::ofstream output(target, std::ios::out | std::ios::binary);

			std::vector<boost::filesystem::path> files;

			for (auto i : boost::filesystem::directory_iterator(source))
				if (boost::filesystem::is_regular_file(i))
					files.push_back(i);

			AFS2Header header;
			header.magic = AFS2_MAGIC_VALUE;
			header.flags = 0x00020402;
			header.numFiles = (uint32_t)files.size();
			header.blockSize = 0x20;

			output.write((char*)&header, 0x10);

			auto id = std::make_unique<uint16_t[]>(header.numFiles);
			auto offsets = std::make_unique<uint32_t[]>(header.numFiles + 1);

			offsets[0] = 0x10 + header.numFiles * 0x06 + 4;
			if (offsets[0] < header.blockSize)
				offsets[0] = header.blockSize;

			for (size_t i = 0; i < files.size(); i++) {
				output.seekp((offsets[i] + header.blockSize - 1) & -header.blockSize);

				boost::filesystem::ifstream input(files[i], std::ios::in | std::ios::binary);
				input.seekg(0, std::ios::end);
				std::streamoff length = input.tellg();
				input.seekg(0, std::ios::beg);

				auto buffer = std::make_unique<char[]>(length);
				input.read(buffer.get(), length);
				output.write(buffer.get(), length);

				id[i] = (uint16_t)i;
				offsets[i + 1] = (uint32_t)output.tellp();
			}

			output.seekp(0x10);
			output.write(reinterpret_cast<char*>(id.get()), header.numFiles * 2);
			//output.seekp((header.numFiles % 2) * 2, std::ios_base::cur);
			output.write(reinterpret_cast<char*>(offsets.get()), (header.numFiles + 1) * 4);
		}
	}
}