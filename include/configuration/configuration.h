///
/// @file    configuration.h
/// @author
///
#ifndef __CC_CONFIGURATION_H__
#define __CC_CONFIGURATION_H__
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <mutex>
using std::map;
using std::string;
using std::cout;
using std::endl;
using std::set;
using std::vector;
using std::mutex;
namespace cc
{
	class Configuration
	{
		
		public:
			static Configuration & getInstance();
		public:
			map<string,string> & getConfMap();
			set<string> getStopWordList();
		private:
			Configuration(const string &);
			Configuration(const Configuration &) = delete;
			Configuration & operator=(const Configuration &) = delete;
		private:
			string _confFilePath;
			map<string,string> _confMap;
			set<string> _stopWordList;//停用词
			static Configuration _instance;
	};
}//namespace cc
#endif
