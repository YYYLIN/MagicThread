/*
* Copyright(c) 2019, YYYLIN
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met :
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the YYYLIN nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE YYYLIN AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED.IN NO EVENT SHALL THE YYYLIN AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _MAGIC_THREAD_H_
#define _MAGIC_THREAD_H_

#include <functional>

#ifdef WIN32

#ifdef MAGIC_THREAD_EXPORTS

#define DLL_MAGIC_THREAD_OUTPUT         __declspec(dllexport)
#define DLL_MAGIC_THREAD_OUTPUT_INPUT   __declspec(dllexport)

#else

#define DLL_MAGIC_THREAD_OUTPUT        //__declspec(dllimport)
#define DLL_MAGIC_THREAD_OUTPUT_INPUT   __declspec(dllimport)

#endif

#elif __linux__

#define DLL_MAGIC_THREAD_OUTPUT  
#define DLL_MAGIC_THREAD_OUTPUT_INPUT

#endif

#define MAGIC_WAIT_MESSAGE_ERROR      2   // 等待失败，处理错误
#define MAGIC_WAIT_MESSAGE_TIMEOUT    1   // 等待超时
#define MAGIC_WAIT_MESSAGE_OK         0   // 等待已经处理完成

#define MAGIC_MAIN_THREAD_NAME			"MAIN_THREAD"

#define MAGIC_WAIT_INFINITE				0xFFFFFFFF

#define BindClassFunctionToMessage(F) std::bind(F, this, std::placeholders::_1, std::placeholders::_2)
#define BindClassFunctionToMessageObject(F,O) std::bind(F, O, std::placeholders::_1, std::placeholders::_2)
#define BindClassFunction(F) std::bind(F, this)
#define BindClassFunctionObject(F,O) std::bind(F, O)

#define MM_MESS		Magic::Management::MESSAGE_TYPE _MessageType, Magic::Management::MESSAGE _Message
#define KEY_MESS    void*, void*, void*, void*, void*

#define MESSAGE_THREAD_CLOSE			0xFFFFFFFF
#define MESSAGE_THREAD_CLOSED			0xFFFFFFFE	//警告此消息的将在UpdataThreadManagement的线程中处理

#define MESSAGE_THREAD_CLOSE_S          "MESSAGE_THREAD_CLOSE"
#define MESSAGE_THREAD_CLOSED_S         "MESSAGE_THREAD_CLOSED"

#define MAGIC_NULL_PARAM                (void*)0

namespace Magic
{
	namespace Management
	{
		typedef unsigned int MESSAGE_TYPE;
		typedef long long MESSAGE;
		typedef void* WAIT_MESSAGE;

		enum ThreadTypeMode
		{
			THREAD_RUN_ONCE,		//只运行一直
			THREAD_LOOP_RUN			//循环运行
		};

		enum ThreadMessageMode
		{
			THREAD_MESSAGE_NO_WAIT,		//如果没有消息，着继续运行
			THREAD_MESSAGE_WAIT			//如果没有消息，着等待消息接受后再运行
		};

		typedef unsigned long long THREAD_OBJECT;
		typedef void* THREAD_POOL_OBJECT;
		typedef std::function<void(KEY_MESS)> MESSAGE_TRANSFER_FUNC;
		typedef std::function<void(MESSAGE_TYPE, MESSAGE)> Callback_Message;
		typedef std::function<unsigned int(const std::string&, const MESSAGE_TRANSFER_FUNC&)> Callback_Message_Key;
		typedef std::function<void(void)> Callback_Void;

		/*
		*功能：
		*	获取当前线程上一次的错误数据
		*参数：
		*	空
		*返回值：
		*	int 错误值
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT int GetLastError();

		/*
		*功能：
		*	获取线程管理中的线程总数量
		*参数：
		*	空
		*返回值：
		*	unsigned long long = 数量
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT unsigned long long GetThreadSize();

		/*
		*功能：
		*	是否已经创建线程管理器
		*参数：
		*	空
		*返回值：
		*	bool = true 成功 | false 失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool IsCreateThreadManagement();

		/*
		*功能：
		*	创建线程管理器
		*参数：
		*	空
		*返回值：
		*	bool = true 成功 | false 失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool CreateThreadManagement(ThreadMessageMode threadmessagemode = THREAD_MESSAGE_NO_WAIT);

		/*
		*功能：
		*	关闭线程管理器
		*参数：
		*	空
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT void ShutdownThreadManagement();

		/*
		*功能：
		*	更新线程管理器
		*参数：
		*	空
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT void UpdataThreadManagement();

		/*
		*功能：
		*	创建线程对象
		*参数：
		*	[IN]_name = 线程对象名字
		*	[IN]_ThreadTypeMode = 线程类型模式 ThreadTypeMode
		*返回值：
		*	THREAD_OBJECT = 线程对象
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT THREAD_OBJECT CreateThreadObject(
			const char* _name,
			ThreadTypeMode _ThreadTypeMode,
			ThreadMessageMode _ThreadMessageMode = THREAD_MESSAGE_NO_WAIT);

		/*
		*功能：
		*	创建线程池对象
		*参数：
		*	[IN]_name = 线程对象名字
		*	[IN]_ThreadNumber = 需要创建的线程数量
		*返回值：
		*	THREAD_POOL_OBJECT = 线程池对象
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT THREAD_POOL_OBJECT CreateThreadPoolObject(const char* _name, unsigned int _ThreadNumber);

		/*
		*功能：
		*	关闭指定线程名的线程
		*参数：
		*	[IN]_name = 线程名
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT void ShutdownThreadObject(const char* _name);

		/*
		*功能：
		*	关闭指定线程对象的线程
		*参数：
		*	[IN]_THREAD_OBJECT = 线程对象
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT void ShutdownThreadObject(THREAD_OBJECT _THREAD_OBJECT);

		/*
		*功能：
		*	关闭指定线程池对象
		*参数：
		*	[IN]_THREAD_POOL_OBJECT = 线程池名
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT void ShutdownThreadPoolObject(const char* _name);

		/*
		*功能：
		*	关闭指定线程池对象
		*参数：
		*	[IN]_THREAD_POOL_OBJECT = 线程池对象
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT void ShutdownThreadPoolObject(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT);

		/*
		*功能：
		*	关闭当前线程
		*参数：
		*	空
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT void ShutdownThreadObject();

		/*
		*功能：
		*	 立即强制结束指定线程(注意：使用此函数需要明确知道指定线程中是否可能造成内存泄露，未释放的锁问题)
		*    不在通知MESSAGE_THREAD_CLOSE消息。但是会通知MESSAGE_THREAD_CLOSED消息。
		*参数：
		*	[IN]_THREAD_OBJECT = 线程对象
		*返回值：
		*	bool = false 失败(不能结束当前线程)
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool TerminateThread(THREAD_OBJECT _THREAD_OBJECT);

		/*
		*功能：
		*	设置等待线程的最长等待时长
		*参数：
		*	_THREAD_OBJECT = 线程对象
		*	time = 等待时长(毫秒单位)
		*返回值：
		*	true 成功 false失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SetWaitTime(THREAD_OBJECT _THREAD_OBJECT, unsigned long time);
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SetWaitTime(const char* _name, unsigned long time);
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SetWaitTime(unsigned long time);

		/*
		*功能：
		*	设置是否等待线程消息
		*参数：
		*	_THREAD_OBJECT = 线程对象
		*	ThreadMessageMode = 是否等待
		*返回值：
		*	true 成功 false失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SetMode(THREAD_OBJECT _THREAD_OBJECT, ThreadMessageMode _ThreadMessageMode);
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SetMode(const char* _name, ThreadMessageMode _ThreadMessageMode);
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SetMode(ThreadMessageMode _ThreadMessageMode);

		/*
		*功能：
		*	监听线程
		*参数：
		*	_THREAD_OBJECT = 线程对象
		*	_CallBack = 处理函数
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool MonitorThread(THREAD_OBJECT _THREAD_OBJECT, const Callback_Void& _CallBack);
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool MonitorThread(const char* _name, const Callback_Void& _CallBack);

		/*
		*功能：
		*	监听线程池
		*参数：
		*	_THREAD_POOL_OBJECT = 线程池对象
		*	_CallBack = 处理函数
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool MonitorThreadPool(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, const Callback_Void& _CallBack);
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool MonitorThreadPool(const char* _name, const Callback_Void& _CallBack);

		/*
		*功能：
		*	监听线程消息
		*参数：
		*	_THREAD_OBJECT = 线程对象
		*	_MessageType = 监听消息类型
		*	_CallBack = 处理函数
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool MonitorThreadMessage(THREAD_OBJECT _THREAD_OBJECT, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack);
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool MonitorThreadMessage(const char* _name, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack);

		/*
		*功能：
		*	监听线程消息
		*参数：
		*	_THREAD_OBJECT = 线程对象
		*   _name = 监听线程对象名
		*	key = 监听消息的事件
		*	_CallBack = 处理函数：回调函数中的返回值 如果返回0代表不再重复执行。返回1代表会继续重复执行，直到返回0。
		*               如果存在等待对象，返回0时，将会触发异步等待WaitMessage函数退出。
		*   waitMessage = 如果传入等待对象。请在想要的位置调用WaitMessage函数，等待消息处理收到
		*返回值：
		*	bool = true 发送成功 | false发送失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool MonitorThreadMessage(THREAD_OBJECT _THREAD_OBJECT, const std::string& key, const Callback_Message_Key& _CallBack, WAIT_MESSAGE* waitMessage = 0);
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool MonitorThreadMessage(const char* _name, const std::string& key, const Callback_Message_Key& _CallBack, WAIT_MESSAGE* waitMessage = 0);

		/*
		*功能：
		*	异步等待传入的消息对象处理完成后再退出
		*参数：
		*	waitMessage = 需要等待的消息对象
		*   timeout = 超时时长，默认
		*   isSync = 是否同步，如果是false就会异步处理（处理其他队列事件不柱塞）
		*返回值：
		*	bool = true 发送成功 | false发送失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT unsigned int WaitMessage(WAIT_MESSAGE waitMessage, unsigned long timeout = MAGIC_WAIT_INFINITE, bool isSync = true);

		/*
		*功能：
		*	监听线程池消息
		*参数：
		*	_THREAD_POOL_OBJECT =  线程池对象
		*	_MessageType = 监听消息类型
		*	_CallBack = 处理函数
		*返回值：
		*	bool = true 发送成功 | false发送失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool MonitorThreadPoolMessage(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack);
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool MonitorThreadPoolMessage(const char* _name, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack);

		/*
		*功能：
		*	发送消息到指定线程
		*参数：
		*	[IN]_THREAD_OBJECT = 线程对象
		*	[IN]_MessageType = 消息类型
		*	[IN]_Message = 消息并且会作为函数参数传入
		*	[IN]_CallBack = 在指定线程延迟运行此函数
		*	[IN]_Synch = 必须等待消息传递到指定线程并且_CallBack和监听函数处理完成再退出。
		*警告：
		*	 如果消息类型为0着不传递消息只执行函数
		*返回值：
		*	bool = true 发送成功 | false发送失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SendMessageTo(THREAD_OBJECT _THREAD_OBJECT, MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack = nullptr, bool _Synch = false);

		/*
		*功能：
		*	发送消息到指定线程
		*参数：
		*	[IN]_name = 线程对象名
		*	[IN]_MessageType = 消息类型
		*	[IN]_Message = 消息并且会作为函数参数传入
		*	[IN]_CallBack = 在指定线程延迟运行此函数
		*	[IN]_Synch = 必须等待消息传递到指定线程并且_CallBack和监听函数处理完成再退出。
		*警告：
		*	 如果消息类型为0着不传递消息只执行函数
		*返回值：
		*	bool = true 发送成功 | false发送失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SendMessageTo(const char* _name, MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack = nullptr, bool _Synch = false);

		/*
		*功能：
		*	发送消息到当前线程
		*参数：
		*	[IN]_MessageType = 消息类型
		*	[IN]_Message = 消息并且会作为函数参数传入
		*	[IN]_CallBack = 在当前线程延迟运行此函数
		*警告：
		*	 如果消息类型为0着不传递消息只执行函数
		*返回值：
		*	bool = true 发送成功 | false发送失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SendMessageTo(MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack = nullptr);


		/*
		*功能：
		*	发送消息到指定线程
		*参数：
		*	[IN]_THREAD_OBJECT = 线程对象
		*	[IN]key = 消息事件名
		*	[IN]messageTransfer = 需要传递的参数
		*警告：
		*
		*返回值：
		*	bool = true 发送成功 | false发送失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SendMessageTo(THREAD_OBJECT _THREAD_OBJECT, const std::string& key, const MESSAGE_TRANSFER_FUNC& messageTransfer);


		/*
		*功能：
		*	发送消息到指定线程
		*参数：
		*	[IN]_name = 线程对象名
		*	[IN]key = 消息事件名
		*	[IN]messageTransfer = 需要传递的参数
		*警告：
		*
		*返回值：
		*	bool = true 发送成功 | false发送失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SendMessageTo(const char* _name, const std::string& key, const MESSAGE_TRANSFER_FUNC& messageTransfer);

		/*
		*功能：
		*	发送消息到当前线程
		*参数：
		*	[IN]key = 消息事件名
		*	[IN]messageTransfer = 需要传递的参数
		*警告：
		*
		*返回值：
		*	bool = true 发送成功 | false发送失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SendMessageTo(const std::string& key, const MESSAGE_TRANSFER_FUNC& messageTransfer);

		/*
		*功能：
		*	发送消息到指定线程池
		*参数：
		*	[IN]_name = 线程池对象名
		*	[IN]_MessageType = 消息类型
		*	[IN]_Message = 消息并且会作为函数参数传入
		*	[IN]_CallBack = 在指定线程延迟运行此函数
		*	[IN]_Synch = 同步模式在_CallBack不等于null时,
		*				 必须等待_CallBack处理完成再退出。
		*返回值：
		*	bool = true 发送成功 | false发送失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SendMessageToPool(const char* _name, MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack, bool _Synch = false);

		/*
		*功能：
		*	发送消息到指定线程池
		*参数：
		*	[IN]_THREAD_POOL_OBJECT = 线程池对象
		*	[IN]_MessageType = 消息类型
		*	[IN]_Message = 消息并且会作为函数参数传入
		*	[IN]_CallBack = 在指定线程延迟运行此函数
		*	[IN]_Synch = 同步模式在_CallBack不等于null时,
		*				 必须等待_CallBack处理完成再退出。
		*返回值：
		*	bool = true 发送成功 | false发送失败
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT bool SendMessageToPool(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack, bool _Synch = false);

		/*
		*功能：
		*	获取当前线程的线程对象
		*参数：
		*	空
		*返回值：
		*	THREAD_OBJECT = 线程对象
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT THREAD_OBJECT GetNowTHREAD_OBJECT();

		/*
		*功能：
		*	获取当前线程的线程池对象
		*参数：
		*	空
		*返回值：
		*	THREAD_POOL_OBJECT = 线程池对象
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT THREAD_POOL_OBJECT GetNowTHREAD_POOL_OBJECT();

		/*
		*功能：
		*	获取指定线程名的线程对象
		*参数：
		*	空
		*返回值：
		*	THREAD_OBJECT = 线程对象
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT THREAD_OBJECT GetTHREAD_OBJECT(const char* _name);

		/*
		*功能：
		*	获取指定线程的线程名
		*参数：
		*	[IN]_THREAD_OBJECT = 线程对象
		*	[OUT]_name = 线程名
		*	[IN]_size = _name的缓存区大小
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT void GetTHREAD_OBJECT_Name(THREAD_OBJECT _THREAD_OBJECT, char* _name, int _size);

		/*
		*功能：
		*	获取指定线程池的名
		*参数：
		*	[IN]_THREAD_POOL_OBJECT = 线程池对象
		*	[OUT]_name = 线程池名
		*	[IN]_size = _name的缓存区大小
		*返回值：
		*	空
		*/
		DLL_MAGIC_THREAD_OUTPUT_INPUT void GetTHREAD_POOL_OBJECT_Name(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, char* _name, int _size);

		template<typename T1>
		MESSAGE_TRANSFER_FUNC SetMessageParam(T1 arg1) {
			return [arg1](void* p1, void* p2, void* p3, void* p4, void* p5) {
				if (p1 != nullptr) {
					*(T1*)p1 = arg1;
				}
			};
		}

		template<typename T1, typename T2>
		MESSAGE_TRANSFER_FUNC SetMessageParam(T1 arg1, T2 arg2) {
			return [arg1, arg2](void* p1, void* p2, void* p3, void* p4, void* p5) {
				if (p1 != nullptr) {
					*(T1*)p1 = arg1;
				}
				if (p2 != nullptr) {
					*(T2*)p2 = arg2;
				}
			};
		}

		template<typename T1, typename T2, typename T3>
		MESSAGE_TRANSFER_FUNC SetMessageParam(T1 arg1, T2 arg2, T3 arg3) {
			return [arg1, arg2, arg3](void* p1, void* p2, void* p3, void* p4, void* p5) {
				if (p1 != nullptr) {
					*(T1*)p1 = arg1;
				}
				if (p2 != nullptr) {
					*(T2*)p2 = arg2;
				}
				if (p3 != nullptr) {
					*(T3*)p3 = arg3;
				}
			};
		}

		template<typename T1, typename T2, typename T3, typename T4>
		MESSAGE_TRANSFER_FUNC SetMessageParam(T1 arg1, T2 arg2, T3 arg3, T4 arg4) {
			return [arg1, arg2, arg3, arg4](void* p1, void* p2, void* p3, void* p4, void* p5) {
				if (p1 != nullptr) {
					*(T1*)p1 = arg1;
				}
				if (p2 != nullptr) {
					*(T2*)p2 = arg2;
				}
				if (p3 != nullptr) {
					*(T3*)p3 = arg3;
				}
				if (p4 != nullptr) {
					*(T4*)p4 = arg4;
				}
			};
		}

		template<typename T1, typename T2, typename T3, typename T4, typename T5>
		MESSAGE_TRANSFER_FUNC SetMessageParam(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) {
			return [arg1, arg2, arg3, arg4, arg5](void* p1, void* p2, void* p3, void* p4, void* p5) {
				if (p1 != nullptr) {
					*(T1*)p1 = arg1;
				}
				if (p2 != nullptr) {
					*(T2*)p2 = arg2;
				}
				if (p3 != nullptr) {
					*(T3*)p3 = arg3;
				}
				if (p4 != nullptr) {
					*(T4*)p4 = arg4;
				}
				if (p5 != nullptr) {
					*(T5*)p5 = arg5;
				}
			};
		}
	}
}

#endif