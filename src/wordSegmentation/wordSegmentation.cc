///
/// @file    wordSegmentation.cc
/// @author  
///

#include "wordSegmentation.h"
#include "cppLog.h"
#include <string>
#include <iostream>
#include <fstream>
using std::cout;
using std::endl;
namespace cc
{

	//路径是相对于 .exe 的
	WordSegmentation::WordSegmentation(
			string  dict_path,
			string  hmm_path,
			string  user_dict_path,
			string  idf_path,
			string  stop_word_path
			)
		//:_dict_path(dict_path)
		// ,_hmm_path(hmm_path)
		// ,_user_dict_path(user_dict_path)
		// ,_idf_path(idf_path)
		// ,_stop_word_path(stop_word_path)
		:_cppjieba(dict_path.c_str(),hmm_path.c_str(),user_dict_path.c_str(),idf_path.c_str(),stop_word_path.c_str())
	{}
	void WordSegmentation::save(const string & fileOut) {
		ofstream ofs(fileOut);
		if(!ofs.good()){
			logErrorLoc("open file error");
			ofs.close();
			return;
		}
		for(auto & elem:_vecWords){
			ofs<<elem<<" ";
		}
		ofs.close();
	}

	vector<string> WordSegmentation::cutFile(const string & fileIn) {
		vector<string> res;
		string doc;
		ifstream ifs(fileIn);
		if(!ifs.good()){
			logErrorLoc("open file error");
			ifs.close();
			return _vecWords;
		}
		_vecWords.clear();
		while(ifs>>doc){
			res.clear();
			_cppjieba.Cut(doc, res);
			for(auto & elem:res){
				_vecWords.push_back(elem);
			}
		}
		ifs.close();
		return _vecWords;
	}

	vector<string> WordSegmentation::cutStr(const string & str) {
		vector<string> res;
		string sentence;
		istringstream iss(str);
		if(!iss.good()){
			logErrorLoc("open file error");
			return _vecWords;
		}
		_vecWords.clear();
		while(iss >> sentence){
			res.clear();
			_cppjieba.Cut(sentence, res);
			for(auto & elem:res){
				_vecWords.push_back(elem);
			}
		}
		return _vecWords;
	}

	vector<string> WordSegmentation::getCutWords(){
		return _vecWords;
	}

	WordSegmentation & WordSegmentation::getInstance(){
		return instance;
	}

	WordSegmentation  WordSegmentation::instance;
};//endof namespace
