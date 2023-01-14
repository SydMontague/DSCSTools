#pragma once
#include "shared_export.h"
#include <filesystem>

namespace dscstools {
	namespace afs2 {
		/**
			Extracts the AFS2 archive given by sourceFile into targetPath.
		*/
		void SHARED_EXPORT extractAFS2(const std::filesystem::path source, const std::filesystem::path target);

		/**
			Packs the folder given by sourcePath into an AFS2 archive saved into targetFile.
		*/
		void SHARED_EXPORT packAFS2(const std::filesystem::path source, const std::filesystem::path target);
	}
}