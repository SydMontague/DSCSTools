#pragma once
#include <boost/filesystem.hpp>
#include <ostream>
#include <vector>
#include <string>
#include "shared_export.h"

namespace dscstools {
	namespace mdb1 {
		enum CompressMode { none, normal, advanced };

		enum ArchiveStatus { encrypted, decrypted, invalid };

		struct FileEntry {
			int16_t compareBit;
			uint16_t dataId;
			uint16_t left;
			uint16_t right;
		};

		struct FileNameEntry {
			char extension[4];
			char name[0x3C];

			const std::string toString() {
				std::string str(name);
				str.append(".");
				str.append(extension, 4);
				return str;
			}

			const std::string toPath() {
				std::string str = toString();
				std::replace(str.begin(), str.end(), '\\', (char)boost::filesystem::path::preferred_separator);
				return str;
			}
		};

		struct DataEntry {
			uint32_t offset;
			uint32_t size;
			uint32_t compSize;
		};

		struct FileInfo {
			FileEntry file;
			FileNameEntry name;
			DataEntry data;

			bool operator==(const FileInfo& other) {
				return false;
			}
		};

		struct ArchiveInfo {
			ArchiveStatus status = invalid;
			uint32_t magicValue = -1;
			uint16_t fileCount = -1;
			uint32_t dataStart = -1;
			std::vector<FileInfo> fileInfo;
		};

		static std::ostream nullStream(nullptr);

		ArchiveInfo SHARED_EXPORT getArchiveInfo(const boost::filesystem::path source);

		void SHARED_EXPORT extractMDB1File(const boost::filesystem::path source, const boost::filesystem::path target, std::string fileName);

		void SHARED_EXPORT extractMDB1(const boost::filesystem::path source, const boost::filesystem::path target);

		void SHARED_EXPORT packMDB1(const boost::filesystem::path source, const boost::filesystem::path target, CompressMode compress = normal, std::ostream& progressStream = nullStream);

		void SHARED_EXPORT cryptFile(const boost::filesystem::path source, const boost::filesystem::path target);
	}
}