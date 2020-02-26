#pragma once
#include <boost/filesystem.hpp>

void extractMBE(boost::filesystem::path source, boost::filesystem::path target);

void packMBE(boost::filesystem::path source, boost::filesystem::path target);