#pragma once
#include <boost/filesystem.hpp>

namespace dscstools {
	namespace savefile {
		void decryptSaveFile(boost::filesystem::path source, boost::filesystem::path target);

		void encryptSaveFile(boost::filesystem::path source, boost::filesystem::path target);
	}
}