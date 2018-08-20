///
/// @file    configuration.cc
/// @author
///

#include "cppLog.h"
#include "configuration.h"
#include <unistd.h>
namespace cc
{
	Configuration & Configuration::getInstance(){
		return _instance;
	}
	Configuration::Configuration(const string & confFilePath)
		:_confFilePath(confFilePath)
	{
		std::ifstream ifs;
		ifs.open(_confFilePath);
		if(!ifs.good()){
			logErrorLoc("open confFile error");
			ifs.close();
			exit(EXIT_FAILURE);
		}
		string line,item,value;
		while(getline(ifs,line)){
			if(line.empty() || (!line.empty() && line[0] == '#') || (line.size() > 1 && line[0] == '/' && line[1] == '/')) continue;
			std::istringstream iss(line);
			iss>>item>>value;	
			_confMap.insert(std::map<string,string>::value_type(item,value));
		}
		auto it = _confMap.find("StopWordsDictPath");
		if(it == _confMap.end()){
			logErrorLoc("get stopWordDict error");
		}else{
			string stopWordDir = it->second;
			std::ifstream ifs(stopWordDir);
			if(!ifs.good()){
				logErrorLoc("open stopWordDict error");
			}else{//将停用词加入停用词集合
				string stopWords;
				while(ifs >> stopWords){
					if(_stopWordList.find(stopWords)== _stopWordList.end()){
						_stopWordList.insert(stopWords);
					}
				}
			}
		}
		ifs.close();
	}

	set<string> Configuration::getStopWordList(){
		return _stopWordList;
	}
	map<string,string> & Configuration::getConfMap(){
		return _confMap;
	}

	Configuration Configuration::_instance("../configuration/configuration.conf");

}//namespace cc
