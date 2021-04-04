add_library(doboz STATIC Common.h Compressor.cpp Compressor.h Decompressor.cpp Decompressor.h Dictionary.cpp Dictionary.h)
set_property(TARGET doboz PROPERTY POSITION_INDEPENDENT_CODE ON)