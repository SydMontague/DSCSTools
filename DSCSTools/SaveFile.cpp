#include "SaveFile.h"
#include <boost/multiprecision/cpp_int.hpp>

std::pair<std::array<char, 11>, uint64_t> calculateFileKey(boost::filesystem::path filename) {
	uint64_t staticKey = 0x1415926535897932;
	uint64_t dynamicKey;

	if (filename.filename().string().substr(0, 5) == "slot_")
		dynamicKey = *reinterpret_cast<uint64_t*>("@Tokomon");
	else if (filename.filename().string() == "system_data.bin")
		dynamicKey = *reinterpret_cast<uint64_t*>("@Dagomon");
	else
		dynamicKey = *reinterpret_cast<uint64_t*>("@Lilimon");

	uint64_t val = dynamicKey ^ staticKey;
	std::array<char, 11> key1;
	key1[0] = (char) (val >> 0x08);
	key1[1] = (char) (val >> 0x30);
	key1[2] = (char) (val >> 0x18);
	key1[3] = (char) (val >> 0x00);
	key1[4] = (char) (val >> 0x10);
	key1[5] = (char) (val >> 0x03);
	key1[6] = (char) (val >> 0x28);
	key1[7] = (char) (val >> 0x15);
	key1[8] = (char) (val >> 0x20);
	key1[9] = (char) (val >> 0x2F);
	key1[10] = (char) (val >> 0x38);

	return std::make_pair(key1, val);
}

void decryptSaveFile(boost::filesystem::path source, boost::filesystem::path target) {
	if (!boost::filesystem::is_regular_file(source)) {
		std::cout << "Error: source path is not a regular file." << std::endl;
		return;
	}
	if (!boost::filesystem::is_regular_file(target) && boost::filesystem::exists(target)) {
		std::cout << "Error: target path is not a regular file." << std::endl;
		return;
	}

	boost::filesystem::ifstream input(source, std::ios::in | std::ios::binary);
	boost::filesystem::ofstream output(target, std::ios::out | std::ios::binary);

	auto fileKey = calculateFileKey(source.filename());
	std::array<char, 11> key1 = fileKey.first;
	uint64_t val = fileKey.second;

	input.seekg(0, std::ios::end);
	std::streamoff length = input.tellg();
	input.seekg(0, std::ios::beg);

	uint32_t size = length;
	uint32_t offset = 0;
	uint32_t remaining = size;

	char* buffer = new char[size];
	input.read(buffer, size);

	{ // rotate bits step
		boost::multiprecision::uint128_t magic = 0x801302D26B3BEAE5;
		uint64_t initialVector = (uint64_t) ((magic * val) >> 0x4E);
		uint32_t rotateParameter = ((uint32_t) val) - initialVector * 0x7FED;

		while (remaining) {
			uint32_t read = remaining < 16 ? remaining : 16;

			uint32_t tmp2 = (rotateParameter * (uint64_t) 0x24924925) >> 32;
			int32_t rotateCount = (((((rotateParameter - tmp2) >> 1) + tmp2) >> 2) * 7) - rotateParameter - 1;

			uint64_t valueSum = 0;
			for (uint32_t i = 0; i < read; i++) {
				uint8_t val = buffer[offset + i];
				for (int j = 0; j < -rotateCount; j++)
					val = (val >> 1) | (val << 7);

				valueSum += val;
				buffer[offset + i] = val;
			}

			uint64_t tmp = ((uint64_t) 0x72C62A25 * valueSum) >> 0x28;
			tmp2 = (rotateParameter * 0x10DCD + 1) + (valueSum - (tmp * 0x23B)) * 2;
			rotateParameter = tmp2 - (((uint64_t) 0x40004001 * tmp2) >> 0x3D) * 0x7FFF7FFF;

			offset += 0x10;
			remaining -= 0x10;
		}
	}
	{ // xor and math step
		boost::multiprecision::uint128_t magic2 = 0x3B2153E7529FE1FF;
		uint64_t tmp = (uint64_t) ((val * magic2) >> 64);
		uint32_t init = (uint32_t) ((val) -(((((val - tmp) >> 1) + tmp) >> 0xF) * 0xCFF7));

		uint32_t charSum = 0;

		for (uint32_t i = 0; i < size; i++) {
			uint8_t value = buffer[i];

			uint64_t localMagic1 = 0xAB8F69E3;
			uint64_t localMagic2 = 0x2E8BA2E9;

			value = value - (uint8_t) (((localMagic1 * charSum) >> 0x27) * 0x41);
			value = value - (uint8_t) charSum;

			uint32_t tmp = (localMagic2 * i) >> 0x21;
			uint32_t keyOffset = i - ((tmp + (tmp >> 0x1F)) * 0xB);

			value = value ^ key1[keyOffset];
			value = value ^ (uint8_t) init;
			buffer[i] = value;
			charSum += value;

			uint64_t tmp2 = init * 0x10DCD + 0x0D;
			init = tmp2 - (((tmp2 * 0x40004001) >> 0x3D) * 0x7FFF7FFF);
		}
	}

	output.write(buffer, size);
	input.close();
	output.close();
	delete buffer;
}

void encryptSaveFile(boost::filesystem::path source, boost::filesystem::path target) {
	if (!boost::filesystem::is_regular_file(source)) {
		std::cout << "Error: source path is not a regular file." << std::endl;
		return;
	}
	if (!boost::filesystem::is_regular_file(target) && boost::filesystem::exists(target)) {
		std::cout << "Error: target path is not a regular file." << std::endl;
		return;
	}

	boost::filesystem::ifstream input(source, std::ios::in | std::ios::binary);
	boost::filesystem::ofstream output(target, std::ios::out | std::ios::binary);

	auto fileKey = calculateFileKey(source.filename());
	std::array<char, 11> key1 = fileKey.first;
	uint64_t val = fileKey.second;

	input.seekg(0, std::ios::end);
	std::streamoff length = input.tellg();
	input.seekg(0, std::ios::beg);

	uint32_t size = length;
	uint32_t offset = 0;
	uint32_t remaining = size;

	char* buffer = new char[size];
	input.read(buffer, size);

	{ // xor and math step
		boost::multiprecision::uint128_t magic2 = 0x3B2153E7529FE1FF;
		uint64_t tmp = (uint64_t) ((val * magic2) >> 64);
		uint32_t init = (uint32_t) ((val) -(((((val - tmp) >> 1) + tmp) >> 0xF) * 0xCFF7));

		uint32_t charSum = 0;

		for (uint32_t i = 0; i < size; i++) {
			uint8_t value = buffer[i];

			uint64_t localMagic1 = 0xAB8F69E3;
			uint64_t localMagic2 = 0x2E8BA2E9;
			uint32_t tmp = (localMagic2 * i) >> 0x21;
			uint32_t keyOffset = i - ((tmp + (tmp >> 0x1F)) * 0xB);

			value = value ^ (uint8_t) init;
			value = value ^ key1[keyOffset];
			value = value + (uint8_t) charSum;
			value = value + (uint8_t) (((localMagic1 * charSum) >> 0x27) * 0x41);
			charSum += (uint8_t) buffer[i];
			buffer[i] = value;

			uint64_t tmp2 = init * 0x10DCD + 0x0D;
			init = tmp2 - (((tmp2 * 0x40004001) >> 0x3D) * 0x7FFF7FFF);
		}
	}
	{ // rotate bits step
		boost::multiprecision::uint128_t magic = 0x801302D26B3BEAE5;
		uint64_t initialVector = (uint64_t) ((magic * val) >> 0x4E);
		uint32_t rotateParameter = ((uint32_t) val) - initialVector * 0x7FED;

		while (remaining) {
			uint32_t read = remaining < 16 ? remaining : 16;

			uint32_t tmp2 = (rotateParameter * (uint64_t) 0x24924925) >> 32;
			int32_t rotateCount = (((((rotateParameter - tmp2) >> 1) + tmp2) >> 2) * 7) - rotateParameter - 1;

			uint64_t valueSum = 0;
			for (uint32_t i = 0; i < read; i++) {
				uint8_t val = buffer[offset + i];
				valueSum += val;

				for (int j = 0; j < -rotateCount; j++)
					val = (val << 1) | (val >> 7);

				buffer[offset + i] = val;
			}

			uint64_t tmp = ((uint64_t) 0x72C62A25 * valueSum) >> 0x28;
			tmp2 = (rotateParameter * 0x10DCD + 1) + (valueSum - (tmp * 0x23B)) * 2;
			rotateParameter = tmp2 - (((uint64_t) 0x40004001 * tmp2) >> 0x3D) * 0x7FFF7FFF;

			offset += 0x10;
			remaining -= 0x10;
		}
	}

	output.write(buffer, size);
	input.close();
	output.close();
	delete buffer;
}
