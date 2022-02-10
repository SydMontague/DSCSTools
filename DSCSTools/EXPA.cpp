
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
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include "../libs/csv-parser/parser.hpp"

#define PADDING_BYTE (unsigned char) 0xCC

namespace dscstools {
	namespace mbe {
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

		inline boost::property_tree::ptree matchStructureName(const boost::property_tree::ptree& format, const std::string& structureName, const std::string& sourceName)
		{
			auto formatValue = format.get_child_optional(structureName);
			if (!formatValue) {
				// Scan all table definitions to find a matching regex expression, if any
				for (auto& kv : format) {
					if (boost::regex_search(structureName, boost::regex{ kv.first })) {
						formatValue = kv.second;
						break;
					}
				}
				if (!formatValue)
					throw std::runtime_error("Error: no definition for table " + std::string(structureName) + " found. " + sourceName);
			}

			return formatValue.get();
		}

		uint32_t getEntrySize(const std::string &type, uint32_t currentSize) {
			if (type == "byte")
				return 1;
			if (type == "short")
				return 2 + ((uint32_t)currentSize % 2);
			if (type == "int")
				return 4 + ((uint32_t)currentSize % 4);
			if (type == "float")
				return 4 + ((uint32_t)currentSize % 4);
			else if (type == "string")
				return 8 + ((uint32_t)currentSize % 8);
			else if (type == "int array")
				return 16 + ((uint32_t)currentSize % 8);

			return 0;
		}

		void writeEXPAEntry(std::ofstream &output, char* &ptr, const std::string &type) {
			// TODO remove boilerplate
			if (type == "int") {
				ptr = ptr + ((std::size_t) ptr % 4);
				output << *reinterpret_cast<int32_t*>(ptr);
				ptr += 4;
			}
			else if (type == "float") {
				ptr = ptr + ((std::size_t) ptr % 4);
				output << *reinterpret_cast<float*>(ptr);
				ptr += 4;
			}
			else if (type == "string") {
				ptr = ptr + ((std::size_t) ptr % 8);
				char* strPtr = *reinterpret_cast<char**>(ptr);
				output << std::quoted(strPtr == nullptr ? "" : std::string(strPtr + 8), '\"', '\"');
				ptr += 8;
			}
			else if (type == "byte") {
				ptr = ptr + ((std::size_t) ptr % 1);
				output << (int32_t) *reinterpret_cast<int8_t*>(ptr);
				ptr += 1;
			}
			else if (type == "short") {
				ptr = ptr + ((std::size_t) ptr % 2);
				output << *reinterpret_cast<int16_t*>(ptr);
				ptr += 2;
			}
			else if (type == "int array") {
				ptr = ptr + ((std::size_t) ptr % 8);
				uint32_t elemCount = *reinterpret_cast<uint32_t*>(ptr);
				ptr += 4;
				ptr = ptr + ((std::size_t) ptr % 8);
				int32_t* arrPtr = *reinterpret_cast<int32_t**>(ptr);

				for (uint32_t i = 0; i < elemCount; i++)
					output << arrPtr[i + 2] << ((i != (elemCount - 1)) ? " " : "");
				ptr += 8;
			}
		}

		boost::property_tree::ptree getStructureFile(boost::filesystem::path source) {
			boost::property_tree::ptree format;

			boost::property_tree::ptree structure;
			boost::property_tree::read_json(std::string("structures/structure.json"), structure);

			std::string formatFile;
			for (auto var : structure) {
				if (boost::regex_search(source.string(), boost::regex{ var.first })) {
					formatFile = var.second.data();
					break;
				}
			}

			if (formatFile.empty())
				throw std::runtime_error("Error: No fitting structure file found for " + source.string());

			boost::property_tree::read_json("structures/" + formatFile, format);

			return format;
		}

		void extractMBEFile(boost::filesystem::path source, boost::filesystem::path target) {
			if (!boost::filesystem::exists(target)) {
				if (target.has_parent_path())
					boost::filesystem::create_directories(target);
			}
			else if (!boost::filesystem::is_directory(target))
				throw std::invalid_argument("Error: target path is not a directory.");

			boost::filesystem::ifstream input(source, std::ios::in | std::ios::binary);
			input.seekg(0, std::ios::end);
			std::streamoff length = input.tellg();
			input.seekg(0, std::ios::beg);

			std::unique_ptr<char[]> data = std::make_unique<char[]>(length);
			input.read(data.get(), length);

			EXPAHeader* header = reinterpret_cast<EXPAHeader*>(data.get());

			if (header->magicValue != 0x41505845) // "EXPA"
				throw std::invalid_argument("Error: source file " + source.filename().string() + " is not in EXPA format.");

			std::vector<EXPATable> tables;
			uint32_t offset = 8;

			for (uint32_t i = 0; i < header->numTables; i++) {
				EXPATable table = { data.get() + offset };
				tables.push_back(table);

				offset += table.nameSize() + 0x0C;

				if (table.nameSize() % 8 == 0)
					offset += 4;

				offset += table.entryCount() * (table.entrySize() + table.entrySize() % 8);
			}

			CHNKHeader* chunkHeader = reinterpret_cast<CHNKHeader*>(data.get() + offset);
			offset += 8;

			for (uint32_t i = 0; i < chunkHeader->numEntry; i++) {
				uint32_t dataOffset = *reinterpret_cast<uint32_t*>(data.get() + offset);
				uint32_t size = *reinterpret_cast<uint32_t*>(data.get() + offset + 4);
				uint64_t ptr = reinterpret_cast<uint64_t>(data.get() + offset);

				memcpy(data.get() + dataOffset, &ptr, 8);
				offset += (size + 8);
			}

			boost::property_tree::ptree format = getStructureFile(source);
			auto filename = source.filename().string();

			for (auto table : tables) {
				uint32_t tableHeaderSize = 0x0C + table.nameSize() + (table.nameSize() + 4) % 8;
				auto& formatValue = matchStructureName(format, table.name(), filename);

				boost::filesystem::path outputPath = target / source.filename() / (table.name() + std::string(".csv"));
				if (outputPath.has_parent_path())
					boost::filesystem::create_directories(outputPath.parent_path());
				boost::filesystem::ofstream output(outputPath, std::ios::out);

				// write header
				bool first = true;
				for (auto var : formatValue) {
					if (first)
						first = false;
					else
						output << ",";

					output << var.first;
				}
				output << std::endl;

				// write data
				for (uint32_t i = 0; i < table.entryCount(); i++) {
					bool first = true;
					char* localOffset = table.tablePtr + i * (table.entrySize() + table.entrySize() % 8) + tableHeaderSize;

					for (auto var : formatValue) {
						if (first)
							first = false;
						else
							output << ",";

						writeEXPAEntry(output, localOffset, var.second.data());
					}
					output << std::endl;
				}
			}
		}

		void extractMBE(boost::filesystem::path source, boost::filesystem::path target) {
			if (!boost::filesystem::exists(source))
				throw std::invalid_argument("Error: input path does not exist.");
			if (boost::filesystem::equivalent(source, target))
				throw std::invalid_argument("Error: input and output path must be different!");

			if (boost::filesystem::is_directory(source))
				for (auto file : boost::filesystem::directory_iterator(source))
					extractMBEFile(file, target);
			else if (boost::filesystem::is_regular_file(source))
				extractMBEFile(source, target);
			else
				throw std::invalid_argument("Error: input is neither directory nor file.");
		}

		// folder input, file output
		void packMBE(boost::filesystem::path source, boost::filesystem::path target) {
			if (!boost::filesystem::exists(source))
				throw std::invalid_argument("Error: input path does not exist.");
			if (boost::filesystem::equivalent(source, target))
				throw std::invalid_argument("Error: input and output path must be different!");
			if (!boost::filesystem::is_directory(source))
				throw std::invalid_argument("Error: input path is not a directory.");
			if (!boost::filesystem::exists(target)) {
				if(target.has_parent_path())
					boost::filesystem::create_directories(target.parent_path());
			}
			else if (!boost::filesystem::is_regular_file(target))
				throw std::invalid_argument("Error: target path already exists and is not a file.");

			boost::filesystem::ofstream output(target, std::ios::out | std::ios::binary);

			boost::property_tree::ptree format = getStructureFile(source);

			std::size_t numTables = 0;

			// write EXPA Header
			output.write("EXPA", 4);
			output.write(reinterpret_cast<char*>(&numTables), 4); // Going to overwrite this later

			struct CHNKData {
				std::string type;
				std::string data;
				uint32_t offset;
			};
			std::vector<CHNKData> chnkData;

			auto files = boost::filesystem::directory_iterator(source);
			for (auto dir_entry : files) {
				auto file = dir_entry.path();
				auto filename = file.filename().stem().string();

				++numTables;
				auto& localFormat = matchStructureName(format, filename, filename);

				// write EXPA Table header
				boost::filesystem::ifstream countInput(file, std::ios::in);
				aria::csv::CsvParser countParser(countInput);

				uint32_t entrySize = 0;
				for (auto entry : localFormat)
					entrySize += getEntrySize(entry.second.data(), entrySize);

				entrySize += entrySize % 8;
				uint32_t count = (uint32_t)std::distance(countParser.begin(), countParser.end()) - 1;

				uint32_t nameSize = (uint32_t)(filename.size() + 4) / 4 * 4;
				std::vector<char> name(nameSize);
				std::copy(filename.begin(), filename.end(), name.begin());
				std::vector<char> padding((0x0C + nameSize) % 8, 0);

				output.write(reinterpret_cast<char*>(&nameSize), 4);
				output.write(name.data(), nameSize);
				output.write(reinterpret_cast<char*>(&entrySize), 4);
				output.write(reinterpret_cast<char*>(&count), 4);
				output.write(padding.data(), (0x0C + nameSize) % 8);

				// write EXPA data, cache CHNK data
				boost::filesystem::ifstream input(file, std::ios::in);
				aria::csv::CsvParser parser(input);

				bool first = true;

				for (auto &row : parser) {
					if (localFormat.size() != row.size()) {
						std::stringstream sstream;
						sstream << "Error: structure element count differs from input element count. The wrong structure might be used?" << std::endl;
						sstream << "Expected: " << localFormat.size() << " | Found: " << row.size() << std::endl;
						throw std::runtime_error(sstream.str());
					}

					if (first) {
						first = false;
						continue;
					}

					auto itr = localFormat.begin();
					uint32_t entrySize = 0;

					for (auto &col : row) {
						std::string type = (*itr++).second.data();

						// TODO remove boilerplate
						if (type == "byte") {
							int8_t value = std::stoi(col);
							output.write(reinterpret_cast<char*>(&value), 1);
							entrySize += 1;
						}
						else if (type == "short") {
							uint32_t paddingSize = entrySize % 2;
							std::vector<char> padding(paddingSize, PADDING_BYTE);
							output.write(padding.data(), paddingSize);

							int16_t value = std::stoi(col);
							output.write(reinterpret_cast<char*>(&value), 2);
							entrySize += 2 + paddingSize;
						}
						else if (type == "int") {
							uint32_t paddingSize = entrySize % 4;
							std::vector<char> padding(paddingSize, PADDING_BYTE);
							output.write(padding.data(), paddingSize);

							int32_t value = std::stoi(col);
							output.write(reinterpret_cast<char*>(&value), 4);
							entrySize += 4 + paddingSize;
						}
						else if (type == "float") {
							uint32_t paddingSize = entrySize % 4;
							std::vector<char> padding(paddingSize, PADDING_BYTE);
							output.write(padding.data(), paddingSize);

							float value = std::stof(col);
							output.write(reinterpret_cast<char*>(&value), 4);
							entrySize += 4 + paddingSize;
						}
						else if (type == "string") {
							if (!col.empty())
								chnkData.push_back({ type, col, (uint32_t)output.tellp() + entrySize % 8 });

							uint32_t paddingSize = entrySize % 8;
							std::vector<char> padding(paddingSize, PADDING_BYTE);
							output.write(padding.data(), paddingSize);

							output.write("\0\0\0\0\0\0\0\0", 8);
							entrySize += 8 + paddingSize;
						}
						else if (type == "int array") {
							if (!col.empty())
								chnkData.push_back({ type, col, (uint32_t)output.tellp() + 8 + entrySize % 8 });

							uint32_t paddingSize = entrySize % 8;
							std::vector<char> padding(8, PADDING_BYTE);
							output.write(padding.data(), paddingSize);

							uint32_t arraySize = (uint32_t)std::count(col.begin(), col.end(), ' ') + 1;
							output.write(reinterpret_cast<char*>(&arraySize), 4);
							output.write(padding.data(), 4);
							output.write("\0\0\0\0\0\0\0\0", 8);

							entrySize += 16 + paddingSize;
						}
					}

					if (entrySize % 8 != 0) {
						std::vector<char> padding(entrySize % 8, PADDING_BYTE);
						output.write(padding.data(), entrySize % 8);
						entrySize += entrySize % 8;
					}
				}

			}


			std::size_t chunkCount = chnkData.size();
			output.write("CHNK", 4);
			output.write(reinterpret_cast<char*>(&chunkCount), 4);

			for (auto entry : chnkData) {
				// TODO remove boilerplate
				if (entry.type == "string") {
					uint32_t stringSize = (uint32_t)((entry.data.size() + 5) / 4) * 4;
					std::vector<char> data(stringSize);
					std::copy(entry.data.begin(), entry.data.end(), data.begin());

					output.write(reinterpret_cast<char*>(&entry.offset), 4);
					output.write(reinterpret_cast<char*>(&stringSize), 4);
					output.write(data.data(), stringSize);
				}
				else if (entry.type == "int array") {
					std::vector<std::string> numbers;
					boost::split(numbers, entry.data, boost::is_any_of(" "));

					uint32_t size = (uint32_t)numbers.size() * 4;
					output.write(reinterpret_cast<char*>(&entry.offset), 4);
					output.write(reinterpret_cast<char*>(&size), 4);

					for (auto number : numbers) {
						int32_t val = std::stoi(number);
						output.write(reinterpret_cast<char*>(&val), 4);
					}
				}
			}

			// Seek back to the 4th byte and write out the number of tables that were found
			output.seekp(4, std::ios::beg);
			output.write(reinterpret_cast<char*>(&numTables), 4);
		}
	}
}