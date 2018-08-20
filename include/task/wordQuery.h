///
/// @file    wordQuery.h
/// @author  
///

#ifndef __CC_WORDQUERY_H__
#define __CC_WORDQUERY_H__
#include "configuration.h"
#include "wordSegmentation.h"
#include "webPage.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unordered_map;
using std::set;
using std::pair;
using std::make_pair;
namespace cc
{
	class WordQuery
	{
		private:
		public:
			WordQuery(Configuration & conf);
			string doQuery(const string & str);//执行查询，返回结果
			void loadLibrary();//加载网页库到内存
			vector<double> getQueryWordsWeightVector(const vector<string> & queryWords);//计算查询词的权重值
			vector<int> executeQuery(const vector<string> & queryWords,vector<pair<int,vector<double> > > & resultVec);//执行查询
			string createJson(vector<int> & docIdVec,const vector<string> & queryWords);
			string returnNoAnswer();
		private:
			Configuration & _conf;//配置文件的引用
			unordered_map<int,WebPage> _pageLib;//网页库
			unordered_map<int,pair<int,int> > _offsetLib;//偏移库
			unordered_map<string,set<pair<int,double> > > _invertIndexTable;//倒排索引
	};
};//end of namespace

#endif
