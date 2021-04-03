#pragma once
#include <boost/filesystem.hpp>
#include "shared_export.h"

namespace dscstools {
	namespace mbe {
		void SHARED_EXPORT extractMBE(boost::filesystem::path source, boost::filesystem::path target);

		void SHARED_EXPORT packMBE(boost::filesystem::path source, boost::filesystem::path target);
	}
}