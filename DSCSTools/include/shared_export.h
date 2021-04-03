#pragma once

#ifdef _WIN32
	#define SHARED_EXPORT __declspec(dllexport)
#else
	#define SHARED_EXPORT
#endif