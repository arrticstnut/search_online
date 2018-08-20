///
/// @file    wordSegmentation.h
/// @author  
///
#ifndef __CC_WORDSEGMENTATION_H__
#define __CC_WORDSEGMENTATION_H__

#include "Jieba.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using namespace cppjieba;
//const char* const DICT_PATH = "./dict/jieba.dict.utf8";
//const char* const HMM_PATH = "./dict/hmm_model.utf8";
//const char* const USER_DICT_PATH = "./dict/user.dict.utf8";
//const char* const IDF_PATH = "./dict/idf.utf8";
//const char* const STOP_WORD_PATH = "./dict/stop_words.utf8";

namespace cc
{
	class WordSegmentation
	{
		public:
			static WordSegmentation & getInstance();
		private:
			WordSegmentation(
					string  dict_path = "../include/wordSegmentation/cppjiebaDict/dict/jieba.dict.utf8",
					string  hmm_path = "../include/wordSegmentation/cppjiebaDict/dict/hmm_model.utf8",
					string  user_dict_path = "../include/wordSegmentation/cppjiebaDict/dict/user.dict.utf8",
					string  idf_path = "../include/wordSegmentation/cppjiebaDict/dict/idf.utf8",
					string  stop_word_path = "../include/wordSegmentation/cppjiebaDict/dict/stop_words.utf8"
					);
			WordSegmentation(const WordSegmentation &) = delete;
			WordSegmentation & operator=(const WordSegmentation &) = delete;
		public:
			vector<string> cutFile(const string & fileName);//对传入的文件分词
			vector<string> cutStr(const string & str);//对传入的字符串分词
			void save(const string &);//分词后保存到文件
			vector<string> getCutWords();//得到分词后的结果
		//private:
		//	//路径是相对于 .exe 的
		//	string _dict_path ;//jieba词典路径
		//	string _hmm_path ;//hmm模型路径
		//	string _user_dict_path ;//用户自定义词典路径
		//	string _idf_path ;//倒排词典路径
		//	string _stop_word_path ;//停用词词典路径
		private:
			cppjieba::Jieba _cppjieba;
			vector<string> _vecWords;//分词后的结果
			static WordSegmentation instance;
	};

};//end of namespace

#endif
