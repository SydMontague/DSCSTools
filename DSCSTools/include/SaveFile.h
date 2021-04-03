#pragma once
#include <boost/filesystem.hpp>
#include "shared_export.h"

namespace dscstools {
	namespace savefile {
		void SHARED_EXPORT decryptSaveFile(boost::filesystem::path source, boost::filesystem::path target);

		void SHARED_EXPORT encryptSaveFile(boost::filesystem::path source, boost::filesystem::path target);
	}
}