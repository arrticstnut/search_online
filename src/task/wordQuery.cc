///
/// @file    wordQuery.cc
/// @author  
///

#include "wordQuery.h"
#include "cppLog.h"
#include <mysql/mysql.h>
#include "redisPool.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <json/json.h>
#include <unistd.h>
using std::cout;
using std::endl;

namespace cc
{

	WordQuery::WordQuery(Configuration & conf)
		:_conf(conf)
	{
		loadLibrary();
	}

	void WordQuery::loadLibrary(){//加载网页库到内存
		auto  confMap = Configuration::getInstance().getConfMap();
		string ip = confMap["MysqlServerIp"];
		string user = confMap["MysqlServerUser"];
		string password = confMap["MysqlServerPassword"];
		string database = confMap["MysqlSeverDatabase"];
		string table = confMap["WebPageTable"];
		if(ip.empty() || user.empty() || password.empty() || database.empty() || table.empty()){
			logErrorLoc("database argu error");
			exit(1);
		}
		MYSQL *conn;
		MYSQL_RES *res;
		MYSQL_ROW row;
		conn = mysql_init(NULL);

		//设置utf8格式
		if(mysql_set_character_set( conn, "utf8" )){ 
			std::ostringstream oss;
			oss << "Error making set character: " << mysql_error(conn);
			logErrorLoc(oss.str());
		} 

		if(!mysql_real_connect(conn,ip.c_str(),user.c_str(),password.c_str(),database.c_str(),0,NULL,0)) {
			std::ostringstream oss;
			oss << "Error connecting to database: " << mysql_error(conn);
			logErrorLoc(oss.str());
			exit(1);
		}else{
			//printf("Connected...\n");
		}
		string query = string("select * from ") + table;
		int t = mysql_query(conn,query.c_str());
		if(t) {
			std::ostringstream oss;
			oss << "Error making query: " << mysql_error(conn);
			logErrorLoc(oss.str());
			exit(1);
		}else{
			//printf("Query made...\n");
			res = mysql_use_result(conn);
			if(res) {
				while((row=mysql_fetch_row(res))!=NULL) {
					//printf("num=%d\n",mysql_num_fields(res));//列数
					int docId = stoi(row[0]);
					string title = row[1];
					string url = row[2];
					string content = row[3];
					WebPage wp;
					wp.processWebData(docId,title,url,content);
					_pageLib.insert(make_pair(docId,wp));
				}
			}
			mysql_free_result(res);
		}
		mysql_close(conn);

		//从redis中读取倒排索引表
		string serverIp = confMap["RedisServerIp"];
		string serverPort = confMap["RedisServerPort"];
		string invertIndexKey = confMap["RedisInvertIndexKey"];
		if(serverIp.empty() || serverPort.empty() || invertIndexKey.empty()){
			logErrorLoc("get conf of redis error");
		}
		int port = stoi(serverPort);
		//实例化redis
		RedisPool redis(serverIp,port);
		string invertKey = confMap["RedisInvertIndexKey"];
		if(invertKey.empty()){
			logErrorLoc("get conf of redisInvertKey error");
			return;
		}


		vector<string> result;
		redis.ExecuteCmdBy_2_InputArgs("HGETALL",invertKey,result);

		int size = result.size();
		for(int i = 0;i < size-1;i += 2){
			string word = result[i];
			//解析字符串
			Json::Value root;
			Json::Reader reader;
			if(!reader.parse(result[i+1],root)){
				logErrorLoc("parse string to json error");
				return;
			}
			Json::Value::Members members;
			members = root.getMemberNames();
			for(auto it = members.begin();it != members.end();++it){
				string docIdStr = *it;
				double tfidf = root[docIdStr].asDouble();
				int docId = stoi(docIdStr);
				if(_invertIndexTable.count(word) == 0){
					_invertIndexTable[word] = set<pair<int,double> >{{docId,tfidf}};
				}else{
					_invertIndexTable[word].insert(make_pair(docId,tfidf));
				}
			}
			//===========================
			//for(const auto & p1:_invertIndexTable){
			//	cout << p1.first << endl;
			//}
			////===========================
		}
		logInfo("[--load ok--]");
#if 0
		string word;
		double weight;
		while(std::getline(ifsInvertIndex,line)){
			std::istringstream iss(line);
			iss >> word >> docId >> weight;//第一组倒排索引信息
			_invertIndexTable[word] = set<pair<int,double> >{{docId,weight}};
			while(iss >> docId >> weight){//剩余的倒排索引信息
				_invertIndexTable[word].insert(make_pair(docId,weight));
			}
		}
#endif

	}

#if 0
	void WordQuery::loadLibrary(){//加载网页库到内存
		auto confMap = _conf.getConfMap();
		auto it = confMap.find("PageLibDir");
		if(it == confMap.end()){
			logErrorLoc("find confMap error");
			return;
		}
		string pageLibDir = it->second;
		std::ifstream ifsPageLib(pageLibDir + "pageLib.dat");
		std::ifstream ifsOffsetLib(pageLibDir + "offsetLib.dat");
		std::ifstream ifsInvertIndex(pageLibDir + "invertIndex.dat");
		if(!ifsPageLib.good() || !ifsOffsetLib.good() || !ifsInvertIndex.good()){
			logErrorLoc("open file error");
			return;
		}

		//读取网页库
		string line;
		int docId;
		long startPos,len;
		while(std::getline(ifsOffsetLib,line)){
			//将line转化成istringstream来读取配置信息
			std::istringstream iss(line);
			iss >> docId >> startPos >> len;
			//从ifs读取给定长度的字节
			ifsPageLib.seekg(startPos);//定位到指定的开头
			string doc(len,'0');//构造len长的string
			ifsPageLib.read(&doc[0],len);//读len长字符到string种
			WebPage wp(doc);
			wp.processDoc(_conf,WordSegmentation::getInstance());
			_pageLib.insert(make_pair(docId,wp));
		}


		//读取倒排索引信息
		string word;
		double weight;
		while(std::getline(ifsInvertIndex,line)){
			std::istringstream iss(line);
			iss >> word >> docId >> weight;//第一组倒排索引信息
			_invertIndexTable[word] = set<pair<int,double> >{{docId,weight}};
			while(iss >> docId >> weight){//剩余的倒排索引信息
				_invertIndexTable[word].insert(make_pair(docId,weight));
			}
		}
	}
#endif

#if 0
	// _invertIndexTable倒排索引格式unordered_map<word,set<pair<docId,weight> > > 
	vector<string,set<int> > docHasWords;
	vector<int,set<string> > iswordsOfDoc;//和查询词有交集文档，格式：vector<docId,set<word> >
	for(auto & word:queryWords){
		if(_invertIndexTable.count(word)){

		}
	}
#endif
	vector<double> WordQuery::getQueryWordsWeightVector(const vector<string> & queryWords){//计算查询词的权重值
		// _invertIndexTable倒排索引格式unordered_map<word,set<pair<docId,weight> > > 
		vector<double> queryWeights;
		int N = _pageLib.size();//网页文档总数
		for(const auto & word:queryWords){
			int df = 1;//文档频率
			if(_invertIndexTable.count(word)){
				df += _invertIndexTable[word].size();
			}
			double w = 1.0 * std::log(N*1.0 / df);//tf-idf
			queryWeights.push_back(w);
		}
		//归一化
		double sum = 0;
		for(auto & val:queryWeights){
			sum += val * val;
		}
		sum = std::sqrt(sum);
		for(auto & w:queryWeights){
			w = w*1.0 / sum;
		}
		return queryWeights;
	}

	bool compareByWeight(const pair<int,double> & v1,const pair<int,double> & v2){//按相似度排序
		return v1.second > v2.second;
	}
	vector<int> WordQuery::executeQuery(const vector<string> & queryWords,vector<pair<int,vector<double> > > & resultVec){//执行查询
		////=============================
		//for(auto & q:queryWords){
		//	cout << q << endl;
		//}
		////=============================
		auto queryWeights = getQueryWordsWeightVector(queryWords);//计算查询次的权重
		vector<pair<int,double> > docIdByWeight;//文档Id，和与查询词的余弦相似度
		//计算余弦相似度
		for(auto & elem:resultVec){
			int docId = elem.first;
			vector<double> resultWeights = elem.second;
			double dotQR = 0.0;//x*y
			double uniQ = 0.0;//|x|
			double uniR = 0.0;//|y|
			for(size_t i = 0;i < queryWeights.size() && i < resultWeights.size();++i){
				double wq = queryWeights[i];
				double wr = resultWeights[i];
				dotQR += wq * wr;
				uniQ += wq * wq;
				uniR += wr * wr;
			}
			uniQ = std::sqrt(uniQ);
			uniR = std::sqrt(uniR);
			double cosSamilarity = dotQR / (uniQ * uniR);//余弦相似度
			docIdByWeight.push_back(make_pair(docId,cosSamilarity));
		}
		std::sort(docIdByWeight.begin(),docIdByWeight.end(),compareByWeight);//按相似度排序
		vector<int> retDocs;//返回的排序的文档id
		for(auto & elem:docIdByWeight){
			retDocs.push_back(elem.first);
		}
		return retDocs;
	}

	string WordQuery::doQuery(const string & str){//执行查询，返回结果
		auto confMap = _conf.getConfMap();
		auto stopWords = _conf.getStopWordList();
		//分词
		vector<string> queryWords = WordSegmentation::getInstance().cutStr(str);
		//================
		cout << "TEST1:: "<< str << endl;
		//================
		//去停用词
		for(auto it = queryWords.begin();it != queryWords.end();){
			if(stopWords.count(*it)){
				it = queryWords.erase(it);
			}else{
				++it;
			}
		}

		//查找包含单词的文档
		//格式：map<docId,set<pair<wordId,weight> > >,wordId是指word在queryWords的下标,weight是指word在docId的网页中的权重
		map<int,set<pair<int,double> > >  docsOfWords;//map<docId,set<pair<wordId,weight> > >
		for(size_t i = 0;i < queryWords.size();++i){
			string word = queryWords[i];//单词
			if(_invertIndexTable.count(word)){
				set< pair <int,double> >  docIdAndWeights = _invertIndexTable[word];
				for(auto & key:docIdAndWeights){
					int docId = key.first;//文档Id
					double weight = key.second;//单词在docId文档中的权重
					if(docsOfWords.count(docId)){
						docsOfWords[docId].insert(make_pair(i,weight));
					}else{
						docsOfWords[docId] = set<pair<int,double>> {{docId,weight}};
					}
				}
			}
		}
		//找到包含所有的查词单词的文档
		vector<pair<int,vector<double> > > resultVec;
		for(auto & elem:docsOfWords){
			if(elem.second.size() == queryWords.size()){
				int docId = elem.first;
				vector<double> weights;
				for(auto & w:elem.second){//获得权重
					weights.push_back(w.second);
				}
				resultVec.push_back(make_pair(docId,weights));
			}
		}

		//得到排序的文档id
		vector<int> docIdVec;
		string ret;
		if(!resultVec.empty()){
			docIdVec = executeQuery(queryWords,resultVec);
			ret = createJson(docIdVec,queryWords);
			return ret;
		}else{
			return returnNoAnswer();
		}
	}

	string WordQuery::createJson(vector<int> & docIdVec,const vector<string> & queryWords){
		Json::Value root;
		Json::Value docments;
		int cnt = 0;
		for(auto & docId:docIdVec){
			string summary = _pageLib[docId].getSummary(queryWords);
			string title = _pageLib[docId].getTitle();
			string url = _pageLib[docId].getUrl();
			Json::Value doc;
			doc["title"] = title;
			doc["summary"] = summary;
			doc["url"] = url;
			docments.append(doc);
			++cnt;
			if(cnt >= 100){//最多记录100条
				break;
			}
		}
		root["files"] = docments;
		Json::StyledWriter writer;
		return writer.write(root);
	}

	string WordQuery::returnNoAnswer(){
		Json::Value root;
		Json::Value documents;
		Json::Value elem;
		elem["title"] = "404, not found";
		elem["summary"] = "sorry，没有查询到你想要的信息!";
		elem["url"] = "https://www.baidu.com";
		documents.append(elem);
		root["files"] = documents;
		Json::StyledWriter writer;
		return writer.write(root);
	}
};//end of namespace
