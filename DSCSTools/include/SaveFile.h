#pragma once
#include <boost/filesystem.hpp>
#include "shared_export.h"

namespace dscstools {
	namespace savefile {
		/**
			Encrypts the PC save file given by sourceFile into targetFile.
		*/
		void SHARED_EXPORT decryptSaveFile(boost::filesystem::path source, boost::filesystem::path target);

		/**
			Decrypts the PC save file given by sourceFile into targetFile.
		*/
		void SHARED_EXPORT encryptSaveFile(boost::filesystem::path source, boost::filesystem::path target);
	}
}