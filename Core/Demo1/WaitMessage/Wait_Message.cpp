#include "Wait_Message.h"
#include "Magic_Thread.h"
#include <vector>
#include <string>
#include "../MagicThread/Cross_Platform_Port.h"

using namespace Magic::Management;

struct DeveicMessage {
	std::string device;
	std::vector<unsigned char> data;
};

// USBHID�̶߳���
static std::vector<DeveicMessage> g_DeveicMessage;

void HIDThread() {

}

void USBHIDThread() {
	for (auto& a : g_DeveicMessage) {
		Magic_Sleep(5);

		// �ײ㴦����ɣ����ͻظ���Ϣ��HID��
		std::vector<unsigned char> data = { 33,44,55,66 };
		SendMessageTo("HID", std::string("ReplyFlash_") + a.device, SetMessageParam(data, std::string("Test Mssage OK"), MAGIC_NULL_PARAM, MAGIC_NULL_PARAM));
	}
}

void sendHID(const std::string& device, const std::vector<unsigned char>& data) {
	// ����Ϣ���͸�USBHID����
	SendMessageTo("USBHID", 0, 0, [device, data](MM_MESS) {
		DeveicMessage deviceMessage;
		deviceMessage.device = device;
		deviceMessage.data = data;
		g_DeveicMessage.push_back(deviceMessage);
		});
}

void TestWaitMessage() {
	CreateThreadObject("HID", THREAD_LOOP_RUN, THREAD_MESSAGE_WAIT);
	MonitorThread("HID", HIDThread);

	CreateThreadObject("USBHID", THREAD_LOOP_RUN, THREAD_MESSAGE_WAIT);
	MonitorThread("USBHID", USBHIDThread);
	SetWaitTime("USBHID", 10);

	std::string replyMsg;
	std::vector<unsigned char> replyData;
	std::string deveic = "0x34578";

	WAIT_MESSAGE waitMessage;
	// ����ָ���豸��ReplyFlash��Ϣ
	MonitorThreadMessage("HID", std::string("ReplyFlash_") + deveic, [&replyData, &replyMsg](const std::string& key, const MESSAGE_TRANSFER_FUNC& message) {
		message(&replyData, &replyMsg, 0, 0);

		// ����0���˺���ֻ��������һ��
		return 0;
		}, &waitMessage);

	printf("send Msg Start\n");
	std::vector<unsigned char> data = { 0,45,4,3,2 };
	// ����HID�ӿڽ����·���Ϣ
	SendMessageTo("HID", 0, 0, [deveic, data](MM_MESS) {
		sendHID(deveic, data);
		});

	// �ȴ��ײ�ӿڻظ���Ϣ
	WaitMessage(waitMessage);

	MonitorThreadMessage("HID", MESSAGE_THREAD_CLOSED_S, [&replyData, &replyMsg](const std::string& key, const MESSAGE_TRANSFER_FUNC& message) {

		// ����0���˺���ֻ��������һ��
		return 0;
	}, &waitMessage);

	ShutdownThreadObject("HID");

	WaitMessage(waitMessage);

	printf("Reply Msg: %s\n", replyMsg.c_str());
}