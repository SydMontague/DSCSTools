
#include "EXPA.h"
#include <stdint.h>
#include <fstream>
#include <iostream>

#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <memory>
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/regex.hpp>

template <class T>
inline T readFromStream(std::ifstream &stream, uint32_t size = sizeof(T)) {
	char* buffer = new char[size];
	stream.read(buffer, size);

	return *reinterpret_cast<T*>(buffer);
}

template <>
inline char* readFromStream(std::ifstream &stream, uint32_t size) {
	char* buffer = new char[size];
	stream.read(buffer, size);

	return buffer;
}

struct EXPAHeader {
	uint32_t magicValue;
	uint32_t numTables;
};

struct EXPATable {
	char* tablePtr;

	uint32_t nameSize() { return *reinterpret_cast<uint32_t*>(tablePtr); }
	char* name() { return reinterpret_cast<char*>(tablePtr + 4); }
	uint32_t entrySize() { return *reinterpret_cast<uint32_t*>(tablePtr + nameSize() + 4); }
	uint32_t entryCount() { return *reinterpret_cast<uint32_t*>(tablePtr + nameSize() + 8); }
};

struct EXPATableHeader {
	uint32_t stringSize;
	char* name;
	uint32_t entrySize;
	uint32_t entryCount;
};

struct CHNKHeader {
	uint32_t magicValue;
	uint32_t numEntry;
};

struct CHNKEntry {
	uint32_t someValue;
	uint32_t size;
	char* string;
};

void writeEXPAEntry(std::ofstream &output, char* &ptr, std::string type) {

	if (type == "int") {
		ptr = ptr + ((std::size_t) ptr % 4);
		output << *reinterpret_cast<uint32_t*>(ptr);
		ptr += 4;
	}
	else if (type == "string") {
		ptr = ptr + ((std::size_t) ptr % 8);
		char* strPtr = *reinterpret_cast<char**>(ptr);
		output << std::quoted(strPtr == nullptr ? "" : std::string(strPtr + 8), '\"', '\"');
		ptr += 8;
	}
}

void extractMBE(boost::filesystem::path source, boost::filesystem::path target) {
	boost::filesystem::ifstream input(source, std::ios::in | std::ios::binary);
	boost::filesystem::ofstream output(target, std::ios::out);
	
	input.seekg(0, std::ios::end);
	std::streamoff length = input.tellg();
	input.seekg(0, std::ios::beg);

	char* data = new char[length];
	input.read(data, length);

	EXPAHeader* header = reinterpret_cast<EXPAHeader*>(data);
	std::vector<EXPATable> tables;

	uint64_t offset = 8;

	for (int i = 0; i < header->numTables; i++) {
		EXPATable table = { data + offset };
		tables.push_back(table);

		offset += table.nameSize() + 0x0C;

		if (table.nameSize() % 8 == 0)
			offset += 4;

		offset += table.entryCount() * (table.entrySize() + table.entrySize() % 8);
	}

	CHNKHeader* chunkHeader = reinterpret_cast<CHNKHeader*>(data + offset);
	offset += 8;

	for (int i = 0; i < chunkHeader->numEntry; i++) {
		uint32_t dataOffset = *reinterpret_cast<uint32_t*>(data + offset);
		uint32_t size = *reinterpret_cast<uint32_t*>(data + offset + 4);
		uint64_t ptr = reinterpret_cast<uint64_t>(data + offset);

		memcpy(data + dataOffset, &ptr, 8);
		offset += (size + 8);
	}

	std::vector<std::pair<std::string, void*>> map;

	boost::property_tree::ptree structure;
	boost::property_tree::read_json(std::string("structure.json"), structure);

	std::string formatFile;
	for each (auto var in structure) {
		if (boost::regex_search(source.string(), boost::regex{ var.first })) {
			formatFile = var.second.data();
			break;
		}
	}

	if (formatFile.empty())
		return;

	boost::property_tree::ptree format;
	boost::property_tree::read_json(formatFile, format);

	if (format.size() != tables.size())
		std::cout << "Warning: number of tables doesn't match expected values.";

	for (auto table : tables) {
		uint32_t tableHeaderSize = 0x0C + table.nameSize() + (table.nameSize() + 4) % 8;
		auto formatValue = format.get_child_optional(table.name());

		if (!formatValue.has_value()) {
			std::cout << "Error: no definition for table " << table.name() << " found." << std::endl;
			continue;
		}

		// write header
		bool first = true;
		for (auto var : formatValue.get()) {
			if (first)
				first = false;
			else
				output << ",";

			output << var.first;
		}
		output << std::endl;

		// write data
		for (int i = 0; i < table.entryCount(); i++) {
			bool first = true;
			char* localOffset = table.tablePtr + i * table.entrySize() + tableHeaderSize;

			for (auto var : formatValue.get()) {
				if (first)
					first = false;
				else
					output << ",";

				writeEXPAEntry(output, localOffset, var.second.data());
			}
			output << std::endl;
		}
	}

	/*	
		lookup file - JSON
		{
			pattern1: structureFilePath1,
			pattern2: structureFilePath2,
			patternN: structureFilePathN
		}


		structure file - JSON
		{
			tableName1: {
				fieldName1: fieldType1,
				fieldName2: fieldType2,
				fieldNameN: fieldTypeN
			},
			tableName2: {
				fieldName1: fieldType1,
				fieldName2: fieldType2,
				fieldNameN: fieldTypeN
			},
			tableNameN: {
				fieldName1: fieldType1,
				fieldName2: fieldType2,
				fieldNameN: fieldTypeN
			}
		}
	*/
}