#pragma once
#include <boost/filesystem.hpp>

void extractMDB1(const boost::filesystem::path source, const boost::filesystem::path target);

void packMDB1(const boost::filesystem::path source, const boost::filesystem::path target, bool compress = true);

void cryptFile(const boost::filesystem::path source, const boost::filesystem::path target);