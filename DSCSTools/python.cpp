#define BOOST_PYTHON_STATIC_LIB 

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <iostream>

#include "include/MDB1.h"
#include "include/EXPA.h"
#include "include/AFS2.h"
#include "include/SaveFile.h"

namespace dscstools {
		// MDB1
		void _py_extractMDB1(const std::string source, const std::string target) {
			boost::filesystem::path _source = boost::filesystem::exists(source) ? source : boost::filesystem::current_path().append(source);
			boost::filesystem::path _target = target;
			mdb1::extractMDB1(_source, _target);
		}

		void _py_extractMDB1File(const std::string source, const std::string target, const std::string fileName) {
			boost::filesystem::path _source = boost::filesystem::exists(source) ? source : boost::filesystem::current_path().append(source);
			boost::filesystem::path _target = target;
			mdb1::extractMDB1File(_source, _target, fileName);
		}

		mdb1::ArchiveInfo _py_getArchiveInfo(const std::string source) {
			boost::filesystem::path _source = boost::filesystem::exists(source) ? source : boost::filesystem::current_path().append(source);
			return mdb1::getArchiveInfo(_source);
		}

		void _py_packMDB1(const std::string source, const std::string target, const mdb1::CompressMode mode = mdb1::CompressMode::normal, const bool useStdout = true) {
			boost::filesystem::path _source = boost::filesystem::exists(source) ? source : boost::filesystem::current_path().append(source);
			boost::filesystem::path _target = target;
			if(useStdout)
				mdb1::packMDB1(_source, _target, mode, std::cout);
			else
				mdb1::packMDB1(_source, _target, mode);
		}

		void _py_crypt(const std::string source, const std::string target) {
			boost::filesystem::path _source = boost::filesystem::exists(source) ? source : boost::filesystem::current_path().append(source);
			boost::filesystem::path _target = target;
			mdb1::cryptFile(_source, _target);
		}

		// MBE
		void _py_extractMBE(const std::string source, const std::string target) {
			boost::filesystem::path _source = boost::filesystem::exists(source) ? source : boost::filesystem::current_path().append(source);
			boost::filesystem::path _target = target;
			mbe::extractMBE(_source, _target);
		}

		void _py_packMBE(const std::string source, const std::string target) {
			boost::filesystem::path _source = boost::filesystem::exists(source) ? source : boost::filesystem::current_path().append(source);
			boost::filesystem::path _target = target;
			mbe::packMBE(_source, _target);
		}

		// AFS2
		void _py_extractAFS2(const std::string source, const std::string target) {
			boost::filesystem::path _source = boost::filesystem::exists(source) ? source : boost::filesystem::current_path().append(source);
			boost::filesystem::path _target = target;
			afs2::extractAFS2(_source, _target);
		}

		void _py_packAFS2(const std::string source, const std::string target) {
			boost::filesystem::path _source = boost::filesystem::exists(source) ? source : boost::filesystem::current_path().append(source);
			boost::filesystem::path _target = target;
			afs2::packAFS2(_source, _target);
		}

		// SaveFile
		void _py_encryptSaveFile(const std::string source, const std::string target) {
			boost::filesystem::path _source = boost::filesystem::exists(source) ? source : boost::filesystem::current_path().append(source);
			boost::filesystem::path _target = target;
			savefile::encryptSaveFile(_source, _target);
		}

		void _py_decryptSaveFile(const std::string source, const std::string target) {
			boost::filesystem::path _source = boost::filesystem::exists(source) ? source : boost::filesystem::current_path().append(source);
			boost::filesystem::path _target = target;
			savefile::decryptSaveFile(_source, _target);
		}

		BOOST_PYTHON_MODULE(DSCSTools)
		{
			using namespace boost::python;

			enum_<mdb1::CompressMode>("CompressMode").value("none", mdb1::CompressMode::none).value("normal", mdb1::CompressMode::normal).value("advanced", mdb1::CompressMode::advanced);
			enum_<mdb1::ArchiveStatus>("ArchiveStatus").value("decrypted", mdb1::ArchiveStatus::decrypted).value("encrypted", mdb1::ArchiveStatus::encrypted).value("invalid", mdb1::ArchiveStatus::invalid);

			class_<std::vector<mdb1::FileInfo>>("FileList", no_init)
				.def(vector_indexing_suite<std::vector<mdb1::FileInfo>>());

			class_<mdb1::ArchiveInfo>("ArchiveInfo", no_init)
				.def_readonly("ArchiveStatus", &mdb1::ArchiveInfo::status)
				.def_readonly("FileCount", &mdb1::ArchiveInfo::fileCount)
				.def_readonly("DataStart", &mdb1::ArchiveInfo::dataStart)
				.def_readonly("Files", &mdb1::ArchiveInfo::fileInfo);

			class_<mdb1::FileInfo>("FileInfo", no_init)
				.def("FileName", make_function([](mdb1::FileInfo& a) { return a.name.toString(); }, default_call_policies(), boost::mpl::vector<std::string, mdb1::FileInfo&>()))
				.def("DataOffset", make_function([](mdb1::FileInfo& a) { return a.data.offset; }, default_call_policies(), boost::mpl::vector<int, mdb1::FileInfo&>()))
				.def("DataSize", make_function([](mdb1::FileInfo& a) { return a.data.size; }, default_call_policies(), boost::mpl::vector<int, mdb1::FileInfo&>()))
				.def("DataCompressedSize", make_function([](mdb1::FileInfo& a) { return a.data.compSize; }, default_call_policies(), boost::mpl::vector<int, mdb1::FileInfo&>()));
			
			def("extractMDB1", _py_extractMDB1);
			def("extractMDB1File", _py_extractMDB1File);
			def("getArchiveInfo", _py_getArchiveInfo);
			def("packMDB1", _py_packMDB1);
			def("crypt", _py_crypt);

			def("extractMBE", _py_extractMBE);
			def("packMBE", _py_packMBE);

			def("extractAFS2", _py_extractAFS2);
			def("packAFS2", _py_packAFS2);

			def("encryptSaveFile", _py_encryptSaveFile);
			def("decryptSaveFile", _py_decryptSaveFile);
		}

}