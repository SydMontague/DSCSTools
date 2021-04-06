#pragma once
#include <boost/filesystem.hpp>
#include "shared_export.h"

namespace dscstools {
	namespace mbe {
		/**
			Extracts the MBE file or folder containing them given by sourcePath into targetPath.
			The structures folder must be at the root of your execution context for this to work.
		*/
		void SHARED_EXPORT extractMBE(boost::filesystem::path source, boost::filesystem::path target);

		/**
			Packs the given MBE folder given by sourcePath into an MBE saved into targetFile.
			The structures folder must be at the root of your execution context for this to work.
		*/
		void SHARED_EXPORT packMBE(boost::filesystem::path source, boost::filesystem::path target);
	}
}