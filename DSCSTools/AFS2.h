#pragma once
#include <boost/filesystem.hpp>

namespace dscstools {
	namespace afs2 {
		void extractAFS2(const boost::filesystem::path source, const boost::filesystem::path target);

		void packAFS2(const boost::filesystem::path source, const boost::filesystem::path target);
	}
}