 ///
 /// @file    startServer.cc
 /// @author
 /// @date    2018-07-22 08:42:35
 ///
#include "cppLog.h"
#include "configuration.h"
#include "wordSegmentation.h"
#include "wordQuery.h"
#include "onlineServer.h"
#include <string>
#include <unistd.h>

void start(){
	cc::logInfo("[--server start--]");
	cc::Configuration & conf = cc::Configuration::getInstance();
	cc::OnlineServer ser(conf);
	cc::logInfo("[--server close--]");
}

int main(){
	//daemon(1,0);
	start();
	return 0;
}
