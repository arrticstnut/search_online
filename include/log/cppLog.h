///
/// @file    cppLog.h
///

#ifndef __CC_CPPLOG_H__
#define __CC_CPPLOG_H__

#include <log4cpp/Category.hh>
#include <sstream>
#include <string>
#include <mutex>

using std::string;
using namespace log4cpp;

namespace cc
{

	//将msg加上位置参数
	#define logLocationPrefix(msg) std::string("[")\
						.append(__FILE__).append(":")\
						.append(__func__).append(":")\
						.append(std::to_string(__LINE__)).append("]").append(msg).c_str()

	//带位置参数的log接口
	#define logErrorLoc(msg) logError(logLocationPrefix(msg))
	#define logWarnLoc(msg) logWarn(logLocationPrefix(msg))
	#define logInfoLoc(msg) logInfo(logLocationPrefix(msg))
	#define logDebugLoc(msg) logDebug(logLocationPrefix(msg))

	//不带位置参数的log接口
	void logError(const char * msg);
	void logWarn(const char * msg);
	void logInfo(const char * msg);
	void logDebug(const char * msg);

	//单例模式
	class CppLog
	{
		public:
			static CppLog * getInstance();
			static void destroyInstance();

			void error(const char * msg);
			void warn(const char * msg);
			void info(const char * msg);
			void debug(const char * msg);

		private:
			CppLog();
			~CppLog();
			CppLog(const CppLog &) = delete;
			CppLog & operator=(const CppLog &) = delete;

		private:
			Category & _category;
			static std::mutex _mutex;
			static CppLog * _pInstance;
	};


}//end of namespace
#endif

