///
/// @file    cppLog.cc
///

#include "cppLog.h"
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Priority.hh>

#include <iostream>
using std::cout;
using std::endl;
namespace cc
{

	CppLog * CppLog::_pInstance = NULL;
	std::mutex CppLog::_mutex;

	CppLog * CppLog::getInstance() {
		if(NULL == _pInstance){
			std::lock_guard<std::mutex> gurad(_mutex);//用保护类加锁
			if(NULL == _pInstance){//再判断一次
				_pInstance = new CppLog();
			}
		}
		return _pInstance;
	}

	void CppLog::destroyInstance() {
		if(_pInstance) {
			delete _pInstance;
			_pInstance = nullptr;
		}
	}

	CppLog::CppLog()
		: _category(Category::getRoot().getInstance("D"))
	{
		PatternLayout * ptnLy1 = new PatternLayout();
		ptnLy1->setConversionPattern("%d: %p %c:%m%n");
		PatternLayout * ptnLy2 = new PatternLayout();
		ptnLy2->setConversionPattern("%d: %p %c:%m%n");

		OstreamAppender * pOsAppd = new OstreamAppender("pOsAppd", &cout);
		pOsAppd->setLayout(ptnLy1);

		FileAppender * pFileAppd = new FileAppender("pFileAppd", ".log");
		pFileAppd->setLayout(ptnLy2);

		_category.addAppender(pOsAppd);
		_category.addAppender(pFileAppd);
		_category.setPriority(Priority::DEBUG);
	}

	CppLog::~CppLog() {
		Category::shutdown();
	}

	void CppLog::error(const char * msg) {
		_category.error(msg);
	}

	void CppLog::warn(const char * msg) {
		_category.warn(msg);
	}

	void CppLog::info(const char * msg) {
		_category.info(msg);
	}

	void CppLog::debug(const char * msg) {
		_category.debug(msg);
	}

	void logError(const char * msg) {
		CppLog::getInstance()->error(msg);
	}

	void logWarn(const char * msg) {
		CppLog::getInstance()->warn(msg);
	}

	void logInfo(const char * msg) {
		CppLog::getInstance()->info(msg);
	}

	void logDebug(const char * msg) {
		CppLog::getInstance()->debug(msg);
	}
}//end of namespace
