#pragma once
#include "shared_export.h"
#include <filesystem>

namespace dscstools {
	namespace savefile {
		/**
			Encrypts the PC save file given by sourceFile into targetFile.
		*/
		void SHARED_EXPORT decryptSaveFile(std::filesystem::path source, std::filesystem::path target);

		/**
			Decrypts the PC save file given by sourceFile into targetFile.
		*/
		void SHARED_EXPORT encryptSaveFile(std::filesystem::path source, std::filesystem::path target);
	}
}