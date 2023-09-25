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

// USBHID线程对象
static std::vector<DeveicMessage> g_DeveicMessage;

void HIDThread() {

}

void USBHIDThread() {
	for (auto& a : g_DeveicMessage) {
		Magic_Sleep(5);

		// 底层处理完成，发送回复消息到HID库
		std::vector<unsigned char> data = { 33,44,55,66 };
		SendMessageTo("HID", std::string("ReplyFlash_") + a.device, SetMessageParam(data, std::string("Test Mssage OK"), MAGIC_NULL_PARAM, MAGIC_NULL_PARAM));
	}
}

void sendHID(const std::string& device, const std::vector<unsigned char>& data) {
	// 将消息发送给USBHID处理
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
	// 监听指定设备的ReplyFlash消息
	MonitorThreadMessage("HID", std::string("ReplyFlash_") + deveic, [&replyData, &replyMsg](const std::string& key, const MESSAGE_TRANSFER_FUNC& message) {
		message(&replyData, &replyMsg, 0, 0);

		// 返回0，此函数只监听调用一次
		return 0;
		}, &waitMessage);

	printf("send Msg Start\n");
	std::vector<unsigned char> data = { 0,45,4,3,2 };
	// 调用HID接口进行下发消息
	SendMessageTo("HID", 0, 0, [deveic, data](MM_MESS) {
		sendHID(deveic, data);
		});

	// 等待底层接口回复消息
	WaitMessage(waitMessage);

	MonitorThreadMessage("HID", MESSAGE_THREAD_CLOSED_S, [&replyData, &replyMsg](const std::string& key, const MESSAGE_TRANSFER_FUNC& message) {

		// 返回0，此函数只监听调用一次
		return 0;
	}, &waitMessage);

	ShutdownThreadObject("HID");

	WaitMessage(waitMessage);

	printf("Reply Msg: %s\n", replyMsg.c_str());
}