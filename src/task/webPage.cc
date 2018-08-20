///
/// @file    webPage.cc
/// @author  
///

#include "webPage.h"
#include "cppLog.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <string>
using std::cout;
using std::endl;
using std::ifstream;
using std::regex_search;
using std::log;
using std::sqrt;
using std::string;

namespace cc
{
	WebPage::WebPage()
	{
		//processDoc(_doc,conf,jieba);
	}

	void WebPage::processWebData(int docId,const string & title,const string & url,const string & content){//处理传入的数据
		_docId = docId;
		_docTitle = title;
		_docUrl = url;
		_docContent = content;
		//========================
		//cout << _docId << endl;
		//cout << _docTitle << endl;
		//cout << _docUrl << endl;
		//cout << _docContent << endl;
		//========================
		//统计词频
		auto & jieba = WordSegmentation::getInstance();
		auto & conf = Configuration::getInstance();
		vector<string> vecWords = jieba.cutStr(_docContent);//得到分词后的单词
		set<string> stopWords = conf.getStopWordList();//得到停用词集
		//去停用词,统计词频
		_wordsMap.clear();
		for(const auto & e:vecWords){
			if(stopWords.count(e) == 0){
				_wordsMap[e] += 1;
			}
		}
		//按照词频排序：先转换成vector
		vector<pair<string,int> > vecWordFreq(_wordsMap.begin(),_wordsMap.end());
		std::sort(vecWordFreq.begin(),vecWordFreq.end(),compator());
		//得到按照词频排序的前20个单词
		_topWords.clear();
		_topWords.reserve(32);
		for(size_t i = 0;i < TOPK_NUMBER && i < vecWordFreq.size();++i){
			//===================================
			//cout << vecWordFreq[i].first << " ";
			//===================================
			_topWords.push_back(vecWordFreq[i].first);
		}
			//===================================
			//cout << endl;
			//===================================
	}
	void WebPage::processDoc(const string & doc,Configuration & conf,WordSegmentation & jieba){//对格式化文档进行处理
#if 0
		//使用正则表达式提取信息
		std::regex rId("(?:<docid>)((.|\n)*)(?:</docid>)");
		std::regex rTitle("(?:<title>)((.|\n)*)(?:</title>)");
		std::regex rLink("(?:<link>)((.|\n)*)(?:</link>)");
		std::regex rCont("(?:<content>)((.|\n)*)(?:</content>)");
		std::smatch smId;
		std::smatch smTitle;
		std::smatch smLink;
		std::smatch smCont;
		if(!regex_search(doc,smId,rId) || !regex_search(doc,smTitle,rTitle) || !regex_search(doc,smLink,rLink) || !regex_search(doc,smCont,rCont)){
			cout<<"@["<<__FILE__<<"::"<<__FUNCTION__<<"]:>>\n";
			cout << "regex_search error" << endl;
			return ;
		}
		_doc = doc;
		_docId = std::stoi(smId.str(1));
		_docTitle = smTitle.str(1);
		_docUrl = smLink.str(1);
		_docContent = smCont.str(1);
#endif
		//使用正则表达式会遇到原因不明的段错误,改用string的find
		auto docidB = doc.find("<docid>");
		auto docidE = doc.find("</docid>");
		auto titleB = doc.find("<title>");
		auto titleE = doc.find("</title>");
		auto linkB = doc.find("<link>");
		auto linkE = doc.find("</link>");
		auto contentB = doc.find("<content>",linkE);
		auto contentE = doc.rfind("</content>");
		auto npos = std::string::npos;
		if(docidB == npos || docidE == npos || titleB == npos || titleE == npos 
				|| linkB == npos || linkE == npos || contentB == npos || contentE == npos){
			logErrorLoc("search docItem error");
		}
		_doc = doc;
		docidB += string("<docid>").size();
		_docId = stoi(doc.substr(docidB,docidE - docidB));
		titleB += string("<title>").size();
		_docTitle = doc.substr(titleB,titleE - titleB);
		linkB += string("<link>").size();
		_docUrl = doc.substr(linkB,linkE - linkB);
		contentB += string("<content>").size();
		_docContent = doc.substr(contentB,contentE - contentB);
		//统计词频
		vector<string> vecWords = jieba.cutStr(_docContent);//得到分词后的单词
		set<string> stopWords = conf.getStopWordList();//得到停用词集
		//去停用词,统计词频
		_wordsMap.clear();
		for(const auto & e:vecWords){
			if(stopWords.count(e) == 0){
				_wordsMap[e] += 1;
			}
		}
		//按照词频排序：先转换成vector
		vector<pair<string,int> > vecWordFreq(_wordsMap.begin(),_wordsMap.end());
		std::sort(vecWordFreq.begin(),vecWordFreq.end(),compator());
		//得到按照词频排序的前20个单词
		_topWords.clear();
		_topWords.reserve(32);
		for(size_t i = 0;i < TOPK_NUMBER && i < vecWordFreq.size();++i){
			_topWords.push_back(vecWordFreq[i].first);
		}
	}

	int WebPage::getDocId(){
		return _docId;
	}

	string WebPage::getDoc(){
		return _doc;
	}

	string WebPage::getUrl(){
		return _docUrl;
	}

	string WebPage::getTitle(){
		return _docTitle;
	}

	map<string,int> & WebPage::getWordMap(){
		return _wordsMap;
	}

	bool operator==(const WebPage & lhs,const WebPage & rhs){//判断两篇文档是否相等
		set<string> topKWords;//topk单词集合
		size_t n = std::min(lhs._topWords.size(),rhs._topWords.size());
		n = n > 10 ? 10:n;//最多统计top10个单词的交集
		for(size_t i = 0;i < n;++i){
			string w = lhs._topWords[i];
			if(topKWords.count(w) == 0){
				topKWords.insert(w);
			}
		}
		int cnt = 0;//交集的个数
		for(size_t i = 0;i < n;++i){
			if(topKWords.count(rhs._topWords[i])){
				++cnt;
			}
		}
		double rate = (cnt * 1.0)/(n*1.0);//重复率
		return rate > 0.6;//重复率大于0.6,便认为重复了
	}

	bool operator<(const WebPage & lhs,const WebPage & rhs){//对文档按照docId进行排序
		return lhs._docId < rhs._docId;
	}


	void WebPage::calcTopK(vector<string> & wordsVec,int k,set<string> & stopWordList){//求取文档的topk词集
		if (k <= 0) return ;
		wordsVec.clear();
		wordsVec.reserve(k);
		size_t n = _topWords.size();
		size_t i = 0;
		int j = 0;
		while(i < n && j < k){
			if(stopWordList.count(_topWords[i]) == 0){
				wordsVec.push_back(_topWords[i]);
				++j;
			}
			++i;
		}
	}

	string WebPage::getSummary(const vector<string> & queryWords){
		istringstream iss(_docContent);
		vector<string> result;
		string line;
		while(iss >> line){
			for(auto & word : queryWords){
				auto pos = line.find(word);
				if(pos != string::npos){
					auto beg = line.rfind("。",pos);
					auto end = line.find("。",pos);
					if(beg == string::npos){
						beg = 0;
					}else{
						beg += 3;//不要头部的句号,"。"的unicode长度是3字节
					}
					if(end == string::npos){
						end = line.size();
					}else{
						end += 3;//保留尾部的句号,"。"的unicode长度是3字节
					}
					result.push_back(line.substr(beg,end-beg));
					break;
				}
			}
			if(result.size() >= 5){
				break;
			}
		}
		string summary;
		for(auto & elem: result) {
			summary += elem + '\n';
		}
		return summary;
	}

};//end of namespace
