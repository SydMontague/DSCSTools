#pragma once
#include <boost/filesystem.hpp>

void decryptSaveFile(boost::filesystem::path source, boost::filesystem::path target);

void encryptSaveFile(boost::filesystem::path source, boost::filesystem::path target);