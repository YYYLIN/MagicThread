/*
�����̺߳ʹ���������
*/

#include "Magic_Thread.h"
#include "string"
using namespace Magic::Management;

int main() {
	bool _result = CreateThreadManagement();
	if (!_result) {
		printf("CreateThreadManagementʧ��!!");
		return false;
	}

	THREAD_OBJECT _THREAD_OBJECT = CreateThreadObject("Thread1", THREAD_LOOP_RUN, THREAD_MESSAGE_WAIT);
	//������Ϣ��Thread1����
	SendMessageTo("Thread1", 0, 0, [](MM_MESS) {
		printf("��Thread1�д���1\n");
	});
	//������Ϣ��Thread1�������ݲ���
	std::string _Data = "���ǻ���!";
	SendMessageTo(_THREAD_OBJECT, 0, 0, [_Data](MM_MESS) {
		printf("��Thread1�д���:%s\n", _Data.c_str());
	});

	printf("��MainThread�д���\n");

	//������Ϣ��Thread1�������ҵȴ��������
	SendMessageTo("Thread1", 0, 0, [&_Data](MM_MESS) {
		printf("��Thread1��ɱ��:%s\n", _Data.c_str());
		_Data = "��������!";
	}, true);

	printf("%s", _Data.c_str());

	ShutdownThreadManagement();

	getchar();
}