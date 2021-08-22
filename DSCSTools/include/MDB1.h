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

		/**
			Decompresses a file that has been compressed using the doboz library.
		*/
		void SHARED_EXPORT dobozDecompress(const boost::filesystem::path source, const boost::filesystem::path target);

		/**
			Compresses a file using the doboz library.
		*/
		void SHARED_EXPORT dobozCompress(const boost::filesystem::path source, const boost::filesystem::path target);

		/**
			Returns the ArchiveInfo of an MDB1 archive given by sourceArchive. 
			See the ArchiveInfo strcture. The encryption status of the archive doesn't matter.
		*/
		ArchiveInfo SHARED_EXPORT getArchiveInfo(const boost::filesystem::path source);

		/**
			Extracts a file with the internal path filePath from a MDB1 archive given by sourceArchive into targetPath. 
			The encryption status of the archive doesn't matter.
		*/
		void SHARED_EXPORT extractMDB1File(const boost::filesystem::path source, const boost::filesystem::path target, std::string fileName, const bool decompress = true);

		/**
			Extracts a MDB1 archive given by sourceArchive and extracts it into targetPath or throws an error if something went wrong. 
			The encryption status of the archive doesn't matter.
		*/
		void SHARED_EXPORT extractMDB1(const boost::filesystem::path source, const boost::filesystem::path target, const bool decompress = true);

		/**
			Packs a folder given by sourcePath into an encrypted MDB1 archive saved at targetArchive. 
			You can optionally set the compression mode and which output stream to print progress reports to. 
			
			Valid values for compressMode are:
				dscstools::mdb1::CompressMode::none -> no compression at all
				dscstools::mdb1::CompressMode::normal -> regular compression
				dscstools::mdb1::CompressMode::advanced -> regular compression with duplicate detection
		*/
		void SHARED_EXPORT packMDB1(const boost::filesystem::path source, const boost::filesystem::path target, CompressMode compress = normal, std::ostream& progressStream = nullStream);

		/**
			Performs synchronous en-/decryption of the given sourceFile and saves it into targetFile.
		*/
		void SHARED_EXPORT cryptFile(const boost::filesystem::path source, const boost::filesystem::path target);
	}
}