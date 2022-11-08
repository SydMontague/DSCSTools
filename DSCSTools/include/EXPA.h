#pragma once
#include "shared_export.h"
#include <filesystem>

namespace dscstools {
	namespace mbe {
		/**
			Extracts the MBE file or folder containing them given by sourcePath into targetPath.
			The structures folder must be at the root of your execution context for this to work.
		*/
		void SHARED_EXPORT extractMBE(std::filesystem::path source, std::filesystem::path target);

		/**
			Packs the given MBE folder given by sourcePath into an MBE saved into targetFile.
			The structures folder must be at the root of your execution context for this to work.
		*/
		void SHARED_EXPORT packMBE(std::filesystem::path source, std::filesystem::path target);
	}
}