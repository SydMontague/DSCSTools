#pragma once
#include <boost/filesystem.hpp>
#include "shared_export.h"

namespace dscstools {
	namespace afs2 {
		void SHARED_EXPORT extractAFS2(const boost::filesystem::path source, const boost::filesystem::path target);

		void SHARED_EXPORT packAFS2(const boost::filesystem::path source, const boost::filesystem::path target);
	}
}