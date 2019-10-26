// DSCSTools.cpp : Defines the entry point for the application.
//

#include <ciso646>
#include "../libs/doboz/Compressor.h"
#include "../libs/doboz/Decompressor.h"
#include "DSCSTools.h"
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>
#include <iomanip>
#include <boost/sort/sort.hpp>
#include <boost/range.hpp>


#define DO_COMPRESSION false

template <class T>
inline T readFromBuffer(char* buffer) {
	return *reinterpret_cast<T*>(buffer);
}

struct Node {
	int16_t val1;
	uint16_t left = 0;
	uint16_t right = 0;
	std::string name;
};

struct Header {
	uint32_t magicValue;
	uint16_t count1;
	uint16_t count2;
	uint32_t count3;
	uint32_t dataStart;
	uint32_t totalSize;
};

struct HeaderEntry1 {
	int16_t val1;
	uint16_t dataId;
	uint16_t left;
	uint16_t right;
};

struct HeaderEntry2 {
	char extension[4];
	char name[0x3C];

	const std::string toString() {
		std::string str(name);
		str.append(".");
		str.append(extension, 4);
		return str;
	}
};

struct HeaderEntry3 {
	uint32_t offset;
	uint32_t size;
	uint32_t compSize;
};

struct HeaderEntryCombined {
	uint32_t id;
	HeaderEntry1 h1;
	HeaderEntry2 h2;
	HeaderEntry3 h3;
};

void dumpMDB1(boost::filesystem::path source, boost::filesystem::path target) {
	boost::filesystem::ifstream input(source, std::ios::in | std::ios::binary);

	char* headerBuffer = new char[0x14];
	input.read(headerBuffer, 0x14);

	Header header = readFromBuffer<Header>(headerBuffer);

	if (header.magicValue != 0x3142444D) {
		std::cout << "Error: not a MDB1 file." << std::endl;
		std::cout << source << std::endl;
		std::cout << header.magicValue << std::endl;
		return;
	}

	HeaderEntry1* entry1 = new HeaderEntry1[header.count1];
	HeaderEntry2* entry2 = new HeaderEntry2[header.count2];
	HeaderEntry3* entry3 = new HeaderEntry3[header.count3];
	char* entry1Buffer = new char[header.count1 * sizeof(HeaderEntry1)];
	char* entry2Buffer = new char[header.count2 * sizeof(HeaderEntry2)];
	char* entry3Buffer = new char[header.count3 * sizeof(HeaderEntry3)];
	input.read(entry1Buffer, header.count1 * sizeof(HeaderEntry1));
	input.read(entry2Buffer, header.count2 * sizeof(HeaderEntry2));
	input.read(entry3Buffer, header.count3 * sizeof(HeaderEntry3));
	entry1 = reinterpret_cast<HeaderEntry1*>(entry1Buffer);
	entry2 = reinterpret_cast<HeaderEntry2*>(entry2Buffer);
	entry3 = reinterpret_cast<HeaderEntry3*>(entry3Buffer);

	std::vector<HeaderEntryCombined> entries;

	std::cout << std::setw(0x3C) << "Filename" << std::setw(6) << "ID" << std::setw(6) << "Val1" << std::setw(6) << "Data" << std::setw(6) << "Val3" << std::setw(6) << "Val4" << std::setw(9) << "Offset" << std::setw(9) << "Size" << std::setw(9) << "CSize" << std::endl;
	for (int i = 0; i < header.count1; i++) {
		HeaderEntry1 e1 = entry1[i];
		HeaderEntry2 name = entry2[i];

		HeaderEntry3 data = e1.dataId == 0xFFFF ? HeaderEntry3() : entry3[e1.dataId];

		HeaderEntryCombined bla;
		bla.h1 = e1;
		bla.h2 = name;
		bla.h3 = data;
		bla.id = i;

		entries.push_back(bla);
	}

	struct bla {
		inline bool operator() (const HeaderEntryCombined &x, const HeaderEntryCombined &y) {
			return std::string(x.h2.extension) < std::string(y.h2.extension);
			//return x.id < y.id;
		}
	};

	//std::sort(entries.begin(), entries.end(), bla());

	struct QueueEntry {
		HeaderEntryCombined h;
		int depth;
	};

	std::deque<QueueEntry> queue;
	queue.push_back({ entries[0], 0 });

	HeaderEntryCombined leftEmpty;
	strncpy(leftEmpty.h2.extension, "leftempty", 10);
	leftEmpty.h1.val1 = -2;
	HeaderEntryCombined rightEmpty;
	strncpy(rightEmpty.h2.extension, "rightempty", 11);
	rightEmpty.h1.val1 = -2;

	while (!queue.empty()) {
		QueueEntry en = queue.front();
		HeaderEntryCombined e = en.h;
		queue.pop_front();

		HeaderEntry1 e1 = e.h1;
		HeaderEntry2 name = e.h2;
		HeaderEntry3 data = e.h3;
		int i = e.id;

		std::cout << std::setfill(' ') << std::setw(en.depth * 2) << "" << std::setw(4) << std::setfill('0') << e1.val1 << " " << name.extension << std::setfill(' ') << std::endl;

		if (e1.val1 == -2)
			continue;

		if (entries[e1.left].h1.val1 > e1.val1)
			queue.push_front({ entries[e1.left], en.depth + 1 });
		//else 
			//queue.push_front({ leftEmpty, en.depth + 1 });

		if (entries[e1.right].h1.val1 > e1.val1)
			queue.push_front({ entries[e1.right], en.depth + 1 });
		//else
			//queue.push_front({ rightEmpty, en.depth + 1 });
	}

	for (auto e : entries) {
		HeaderEntry1 e1 = e.h1;
		HeaderEntry2 name= e.h2;
		HeaderEntry3 data = e.h3;
		int i = e.id;

		std::cout << std::setw(0x3C) << name.extension << std::setw(6) << i << std::setw(6) << e1.val1 << std::setw(6) << e1.dataId << std::setw(6) << e1.left << std::setw(6) << e1.right << std::setw(9) << std::setbase(16) << data.offset << std::setw(9) << data.size << std::setw(9) << data.compSize << std::setbase(10) << std::endl;
	}

	delete entry1Buffer;
	delete entry2Buffer;
	delete entry3Buffer;
}

void extractMDB1(boost::filesystem::path source, boost::filesystem::path target) {
	doboz::Decompressor decomp;
	boost::filesystem::ifstream input(source, std::ios::in | std::ios::binary);

	char* headerBuffer = new char[0x14];
	input.read(headerBuffer, 0x14);

	Header header = readFromBuffer<Header>(headerBuffer);

	if (header.magicValue != 0x3142444D) {
		std::cout << "Error: not a MDB1 file." << std::endl;
		std::cout << source << std::endl;
		std::cout << header.magicValue << std::endl;
		return;
	}

	HeaderEntry1* entry1 = new HeaderEntry1[header.count1];
	HeaderEntry2* entry2 = new HeaderEntry2[header.count2];
	HeaderEntry3* entry3 = new HeaderEntry3[header.count3];
	char* entry1Buffer = new char[header.count1 * sizeof(HeaderEntry1)];
	char* entry2Buffer = new char[header.count2 * sizeof(HeaderEntry2)];
	char* entry3Buffer = new char[header.count3 * sizeof(HeaderEntry3)];
	input.read(entry1Buffer, header.count1 * sizeof(HeaderEntry1));
	input.read(entry2Buffer, header.count2 * sizeof(HeaderEntry2));
	input.read(entry3Buffer, header.count3 * sizeof(HeaderEntry3));
	entry1 = reinterpret_cast<HeaderEntry1*>(entry1Buffer);
	entry2 = reinterpret_cast<HeaderEntry2*>(entry2Buffer);
	entry3 = reinterpret_cast<HeaderEntry3*>(entry3Buffer);

	if (header.dataStart != input.tellg())
		std::cout << "Error: stream position does not match header value! Current: " << input.tellg() << " Expected: " << header.dataStart << std::endl;

	for (int i = 0; i < header.count1; i++) {
		HeaderEntry1 e1 = entry1[i];
		HeaderEntry2 name = entry2[i];

		if (e1.val1 == 0xFFFF || e1.dataId == 0xFFFF)
			continue;

		HeaderEntry3 data = entry3[e1.dataId];

		boost::filesystem::path path(target.string() + name.toString());
		boost::filesystem::create_directories(path.parent_path());
		boost::filesystem::ofstream output(path, std::ios::out | std::ios::binary);

		char* outputArr = new char[data.size];
		input.seekg(data.offset + header.dataStart);

		if (data.compSize == data.size)
			input.read(outputArr, data.size);
		else {
			char* dataArr = new char[data.compSize];
			input.read(dataArr, data.compSize);
			doboz::Result result = decomp.decompress(dataArr, data.compSize, outputArr, data.size);

			if (result != doboz::RESULT_OK) {
				std::cout << "Error: " << result << std::endl;
			}


			delete dataArr;
		}

		output.write(outputArr, data.size);
		output.close();
		delete outputArr;
	}

	// cleanup
	delete headerBuffer;
	delete entry1Buffer;
	delete entry2Buffer;
	delete entry3Buffer;
}

/* 
	Val1 - tree sorting value
	Val3 - left Node ID
	Val4 - right Node ID

	leftNode.val1 <= rightNod.val1
	parentNode.val1 < childNode.val1
		if not true, exit condition is met?

	val1 -> bit number to check for comparison

	init tree with empty file (val1 = 0xFFFF, val4 = 1)
	insert first file (alphabetically?), val1 = 0

	calc minimum prefix(?) for each path for each already added path, take the max value
	sort, lowest to highest
	pick lowest value




	--!> bitwise compressed trie! <!--
*/


std::vector<Node> generateTree(boost::filesystem::path path) {
	std::vector<std::string> fileNames;

	boost::filesystem::recursive_directory_iterator itr(path);

	for (auto i : itr) {
		if (boost::filesystem::is_regular_file(i)) {
			std::string ext = i.path().extension().string().substr(1, 5);

			if (ext == "pvr")
				ext = "img ";

			if (ext.length() == 3)
				ext = ext.append(" ");

			char name[0x40];
			strncpy(name, ext.c_str(), 4);
			strncpy(name + 4, boost::filesystem::relative(i.path(), path).replace_extension("").string().c_str(), 0x3C);

			fileNames.push_back(std::string(name));
		}
	}

	struct QueueEntry {
		uint16_t parentNode;
		int16_t val1;
		std::vector<std::string> list;
		bool left;
	};

	std::vector<Node> nodes;
	std::deque<QueueEntry> queue;

	Node n = { 0xFFFF, 0, 0, "" };

	nodes.push_back(n);
	queue.push_front({ 0, -1, fileNames, false });

	while (!queue.empty()) {
		QueueEntry entry = queue.front();
		queue.pop_front();
		std::vector<std::string> f = entry.list;
		Node &parent = nodes[entry.parentNode];

		int16_t i = entry.val1 + 1;
		std::string breaking = "";

		std::vector <std::string> nodeless;
		std::vector <std::string> withNode;

		for (auto file : f) {
			bool hasNode = false;

			for (auto node : nodes)
				if (node.name == file) {
					hasNode = true;
					break;
				}

			if (hasNode)
				withNode.push_back(file);
			else
				nodeless.push_back(file);
		}

		if (nodeless.size() == 0) {
			int i = 0;
			for (; i < nodes.size(); i++)
				if (nodes[i].name == f[0])
					break;

			if (entry.left)
				parent.left = i;
			else
				parent.right = i;

			continue;
		}

		for (; i < 512; i++) {
			for (auto file : nodeless) {
				if (withNode.size() == 0)
					breaking = file;

				for (auto node : withNode) {
					if ((((node[i >> 3] >> (i & 7)) & 1) != ((file[i >> 3] >> (i & 7)) & 1))) {
						breaking = file;
						break;
					}
				}

				if (breaking != "")
					break;
			}

			if (breaking != "")
				break;
		}

		Node child;
		child.val1 = i;
		child.name = breaking;

		if (entry.left)
			parent.left = nodes.size();
		else
			parent.right = nodes.size();

		std::vector<std::string> left;
		std::vector<std::string> right;

		for (auto file : f) {
			if ((file[i >> 3] >> (i & 7)) & 1)
				right.push_back(file);
			else
				left.push_back(file);
		}

		if (left.size() > 0) queue.push_front({ static_cast<uint16_t>(nodes.size()), i, left, true });
		if (right.size() > 0) queue.push_front({ static_cast<uint16_t>(nodes.size()), i, right, false });
		nodes.push_back(child);
	}

	return nodes;
}

// TODO multi core compression
void packMDB1(const boost::filesystem::path source, boost::filesystem::path target) {
	if (!boost::filesystem::is_directory(source)) {
		std::cout << "Error: source path is not a directory" << std::endl;
		return;
	}

	if (!boost::filesystem::exists(target.parent_path()))
		boost::filesystem::create_directories(target.parent_path());

	std::vector<boost::filesystem::path> files;
	std::vector<Node> nodes = generateTree(source);

	boost::filesystem::recursive_directory_iterator itr(source);
	
	for (auto i : itr) {
		if(boost::filesystem::is_regular_file(i))
			files.push_back(i);
	}

	std::vector<HeaderEntry1> header1(files.size() + 1);
	std::vector<HeaderEntry2> header2(files.size() + 1);
	std::vector<HeaderEntry3> header3;

	boost::filesystem::ofstream output(target, std::ios::out | std::ios::binary);
	doboz::Compressor comp;

	uint32_t dataStart = 0x14 + (1 + files.size()) * 0x08 + (1 + files.size()) * 0x40 + (files.size()) * 0x0C;

	Header header = { 0x3142444D, files.size() + 1, files.size() + 1, files.size(), dataStart };
	

	HeaderEntry1 root = { 0xFFFF, 0xFFFF, 0, 1 };
	header1[0] = root;
	header2[0] = HeaderEntry2();

	uint32_t offset = 0;
	int i = 1;
	for (auto file : files) {
		HeaderEntry1 entry1;
		entry1.val1 = i;
		entry1.dataId = header3.size();
		entry1.left = i;
		entry1.right = i;

		HeaderEntry2 entry2;

		std::string ext = file.extension().string().substr(1, 5);
		if (ext.length() == 3)
			ext = ext.append(" ");

		strncpy(entry2.extension, ext.c_str(), 4);
		strncpy(entry2.name, boost::filesystem::relative(file, source).replace_extension("").string().c_str(), 0x3C);

		int nodeId = 0;
		Node treeNode;
		for (; nodeId < nodes.size(); nodeId++)
			if (nodes[nodeId].name == entry2.extension) {
				treeNode = nodes[nodeId];
				break;
			}

		entry1.val1 = treeNode.val1;
		entry1.left = treeNode.left;
		entry1.right = treeNode.right;
		
		boost::filesystem::ifstream input(file, std::ios::in | std::ios::binary);
		input.seekg(0, std::ios::end);
		uint32_t length = input.tellg();
		input.seekg(0, std::ios::beg);

		char* data = new char[length];
		char* outputData = new char[comp.getMaxCompressedSize(length)];
		input.read(data, length);

		size_t destSize = 0;
		if (DO_COMPRESSION) {
			doboz::Result res = comp.compress(data, length, outputData, comp.getMaxCompressedSize(length), destSize);

			if (res != doboz::RESULT_OK) {
				std::cout << "Error: " << res << std::endl;
			}

		}
		else {
			delete outputData;
			outputData = data;
			destSize = length;
		}

		HeaderEntry3 entry3;
		entry3.offset = offset;
		entry3.size = length;
		entry3.compSize = destSize;
		
		output.seekp(dataStart + offset);
		output.write(outputData, destSize);
		offset += destSize;

		header1[nodeId] = entry1;
		header2[nodeId] = entry2;

		header3.push_back(entry3);
		input.close();
		delete data;
		if(DO_COMPRESSION)
			delete outputData;
		i++;
	}

	std::cout << header1.size() << std::endl;
	std::cout << header2.size() << std::endl;
	std::cout << header3.size() << std::endl;

	output.seekp(0x14);
	for (HeaderEntry1 entry : header1)
		output.write(reinterpret_cast<char*>(&entry), 0x08);
	for (HeaderEntry2 entry : header2)
		output.write(reinterpret_cast<char*>(&entry), 0x40);
	for (HeaderEntry3 entry : header3)
		output.write(reinterpret_cast<char*>(&entry), 0x0C);

	output.seekp(0x00);
	output.seekp(0, std::ios::end);
	uint32_t length = output.tellp();
	output.seekp(0, std::ios::beg);

	header.totalSize = length;
	output.write(reinterpret_cast<char*>(&header), 0x14);
	output.close();
}



void outputTree(boost::filesystem::path path) {
	std::vector<Node> nodes = generateTree(path);


	struct BBB {
		Node node;
		int depth;
	};
	std::deque<BBB> bla;
	bla.push_front({ nodes[0], 0 });

	for (int i = 0; i < nodes.size(); i++) {
		Node n = nodes[i];
		std::cout << std::setw(0x3C) << n.name << std::setw(5) << i << std::setw(5) << n.val1 << std::setw(5) << n.left << std::setw(5) << n.right << std::endl;
	}

	while (!bla.empty()) {
		BBB en = bla.front();
		bla.pop_front();

		std::cout << std::setfill(' ') << std::setw(en.depth * 2) << "" << std::setw(4) << std::setfill('0') << en.node.val1 << " " << en.node.name << std::setfill(' ') << std::endl;

		if (en.node.val1 < nodes[en.node.left].val1)
			bla.push_front({ nodes[en.node.left], en.depth + 1 });

		if (en.node.val1 < nodes[en.node.right].val1)
			bla.push_front({ nodes[en.node.right], en.depth + 1 });
	}
}

int main()
{
	
	//outputTree("E:\\CS Hacking\\data\\DSDBse");

	packMDB1("E:\\CS Hacking\\DSCSTool\\DSDBS\\", "E:\\CS Hacking\\DSCSTool\\DSDBS.steam.mvgl.decrypt");
	//packMDB1("C:\\Users\\Syd\\Projects\\DSCSTools\\DSDBS", "E:\\CS Hacking\\DSCSTool\\DSDBS.steam.mvgl.decrypt");
	//dumpMDB1("E:\\CS Hacking\\original\\DSDBse.decrypt.bin", "C:\\Users\\Syd\\Projects\\DSCSTools\\output2\\");
	//dumpMDB1("E:\\CS Hacking\\DSCSTool\\DSDBS.steam.mvgl.decrypt", "C:\\Users\\Syd\\Projects\\DSCSTools\\output2\\");
	//packMDB1("C:\\Users\\Syd\\Projects\\DSCSTools\\output2\\", "C:\\Users\\Syd\\Projects\\DSCSTools\\bla\\DSDBP.decrypt.bin.bin");
	//extractMDB1("E:\\CS Hacking\\original\\DSDBS.decrypt.bin", "E:\\CS Hacking\\DSCSTool\\DSDBS\\");
	//extractMDB1("E:\\CS Hacking\\DSCSTool\\DSDBS.steam.mvgl.decrypt", "E:\\CS Hacking\\DSCSTool\\DSDBS\\");

	std::cout << "Done" << std::endl;
	std::cin.get();
	return 0;
}

