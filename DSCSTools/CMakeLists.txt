cmake_minimum_required (VERSION 3.8)

include_directories(include)

SET(SOURCE_FILES
		MDB1.cpp
		include/MDB1.h
		EXPA.cpp
		include/EXPA.h
		SaveFile.cpp
		include/SaveFile.h
		AFS2.cpp
		include/AFS2.h
        python.cpp
		)
        
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")

# Add source to this project's executable.
if(BUILD_SHARED)
    add_library (DSCSTools SHARED ${SOURCE_FILES})
else()
    add_library (DSCSTools STATIC ${SOURCE_FILES})
endif()

target_link_libraries(DSCSTools PUBLIC doboz Boost::property_tree Boost::multiprecision Boost::crc Boost::asio Boost::regex Boost::system Boost::date_time Boost::mpl Boost::python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR} ${PYTHON_LIBRARIES})

# Install into /target/ folder
install (TARGETS DSCSTools DESTINATION ${CMAKE_BINARY_DIR}/target)
install (DIRECTORY structures/ DESTINATION ${CMAKE_BINARY_DIR}/target/structures)
install (DIRECTORY include/ DESTINATION ${CMAKE_BINARY_DIR}/target/include)
