#pragma once
#include <boost/filesystem.hpp>

void extractAFS2(const boost::filesystem::path source, const boost::filesystem::path target);

void packAFS2(const boost::filesystem::path source, const boost::filesystem::path target);