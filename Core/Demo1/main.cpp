/*
创建线程和处理功能例子
*/

#include "Magic_Thread.h"
#include "string"
using namespace Magic::Management;

int main() {
	bool _result = CreateThreadManagement();
	if (!_result) {
		printf("CreateThreadManagement失败!!");
		return false;
	}

	THREAD_OBJECT _THREAD_OBJECT = CreateThreadObject("Thread1", THREAD_LOOP_RUN, THREAD_MESSAGE_WAIT);
	//发送消息给Thread1处理
	SendMessageTo("Thread1", 0, 0, [](MM_MESS) {
		printf("在Thread1中处理1\n");
	});
	//发送消息给Thread1处理并传递参数
	std::string _Data = "我是火车王!";
	SendMessageTo(_THREAD_OBJECT, 0, 0, [_Data](MM_MESS) {
		printf("在Thread1中处理:%s\n", _Data.c_str());
	});

	printf("在MainThread中处理\n");

	//发送消息给Thread1处理，并且等待处理完成
	SendMessageTo("Thread1", 0, 0, [&_Data](MM_MESS) {
		printf("在Thread1中杀死:%s\n", _Data.c_str());
		_Data = "火车王死了!";
	}, true);

	printf("%s", _Data.c_str());

	ShutdownThreadManagement();

	getchar();
}