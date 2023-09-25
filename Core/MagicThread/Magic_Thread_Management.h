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

#ifndef _MAGIC_THREAD_MANAGEMENT_H_
#define _MAGIC_THREAD_MANAGEMENT_H_

#include "Cross_Platform_Port.h"
#include "Magic_Thread.h"
#include <queue>
#include <set>
#include <map>
#include <unordered_map>

namespace Magic
{
	namespace Management
	{
		struct ThreadObject;

		enum ThreadRunState
		{
			THREAD_STATE_RUN,
			THREAD_STATE_TIMEOUT,
			THREAD_STOP
		};

		struct Message
		{
			unsigned int m_MessageType; // 事件id消息
			long long m_Message;
			std::string messageKey;  // 事件文本消息
			MESSAGE_TRANSFER_FUNC messageTransferFunc; // 传递事件参数的函数
			Callback_Message m_CallBack;
			ThreadObject* m_pThreadObject;
			unsigned int messageMode;
			Message();
			Message(const unsigned int& _MessageType, const long long& _Message, const std::string& key, const MESSAGE_TRANSFER_FUNC& messageFunc, const Callback_Message& _CallBack, ThreadObject* _pThreadObject, unsigned int _messageMode);
		};

		struct WAIT_MESSAGE_SYNC {
			Magic_SEM messageSynchSEM;
			Magic_MUTEX m_mutex;
		};

		struct CALL_BACK_MONITOR_KEY
		{
			bool isWait;
			WAIT_MESSAGE_SYNC waitMessage;
			Callback_Message_Key callback;
		};

		struct ThreadObject
		{
			typedef std::unordered_map<MESSAGE_TYPE, std::vector<Callback_Message>> UMAP_VEC_CALLBACK;
			typedef std::unordered_map<std::string, std::vector<CALL_BACK_MONITOR_KEY>> UMAP_VEC_CALLBACK_KEY;

			std::vector<Callback_Void> m_vec_Callback;
			Magic_THREAD m_Thread;
			std::string m_Name;
			ThreadTypeMode m_ThreadTypeMode;
			ThreadMessageMode m_ThreadMessageMode;
			ThreadRunState m_ThreadRunState;
			unsigned long m_ThreadWaitTime;
			unsigned long long m_ThreadObjectId;

			std::vector<Message> m_queue_Message;
			std::vector<Message> m_Last_queue_Message;
			std::vector<Message>* p_queue_Message;
			std::vector<Message>* p_last_queue_Message;
			Magic_SEM m_Queue_SEM;
			Magic_SEM m_Synch_SEM;

			UMAP_VEC_CALLBACK m_umap_MonitorFunction;
			UMAP_VEC_CALLBACK_KEY m_umap_KeyMonitorFunction;
			Magic_MUTEX m_MessageMutex;

			ThreadObject();
			ThreadObject(ThreadTypeMode _ThreadTypeMode, ThreadRunState _ThreadRunState, const std::string& _name, ThreadMessageMode _ThreadMessageMode);
		};

		class ThreadPoolObject
		{
		public:
			void Updata();
		public:
			std::string m_Name;
			std::vector<THREAD_OBJECT> m_vec_ThreadObject;

			std::queue<Message> m_queue_Message;

			Magic_MUTEX m_MessageMutex;
			Magic_SEM m_queue_SEM;
		};

		class SystemThread
		{
			typedef std::map<std::string, ThreadObject> MAP_SRTING_THREADOBJECT;
			typedef std::map<std::string, ThreadPoolObject> MAP_SRTING_THREADPOOLOBJECT;
		public:
			SystemThread();
			~SystemThread();

			static SystemThread* Instance();

			bool Initialize(ThreadMessageMode threadmessagemode);

			void Shutdown();

			THREAD_OBJECT Create(const char* _name, ThreadTypeMode _ThreadTypeMode, ThreadMessageMode _ThreadMessageMode, bool _IsNewThread);

			THREAD_POOL_OBJECT CreatePool(const char* _name, unsigned int _ThreadNumber);

			void ShutdownPool(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT);

			bool SetWaitTime(THREAD_OBJECT _THREAD_OBJECT, unsigned long time);

			bool SetMode(THREAD_OBJECT _THREAD_OBJECT, ThreadMessageMode _ThreadMessageMode);

			bool MonitorThread(THREAD_OBJECT _THREAD_OBJECT, const Callback_Void& _CallBack);

			bool MonitorThreadPool(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, const Callback_Void& _CallBack);

			bool MonitorThreadMessage(THREAD_OBJECT _THREAD_OBJECT, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack);

			bool MonitorThreadMessage(THREAD_OBJECT _THREAD_OBJECT, const std::string& key, const Callback_Message_Key& _CallBack, WAIT_MESSAGE* waitMessage = 0);

			unsigned int WaitMessage(WAIT_MESSAGE waitMessage, unsigned long timeout = MAGIC_WAIT_INFINITE);

			bool MonitorThreadPoolMessage(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack);

			bool SendMessageTo(THREAD_OBJECT _THREAD_OBJECT, MESSAGE_TYPE _MessageType, MESSAGE _Message, const std::string& key, const MESSAGE_TRANSFER_FUNC& messageTransfer, const Callback_Message& _CallBack = nullptr, bool _Synch = false);

			bool SendMessageTo(MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack = nullptr, bool _Synch = false);

			bool SendMessageTo(THREAD_OBJECT _THREAD_OBJECT, const std::string& key, const MESSAGE_TRANSFER_FUNC& messageTransfer);

			bool SendMessageToPool(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack, bool _Synch = false);

			THREAD_OBJECT GetNowTHREAD_OBJECT() { return m_S_T_ThreadObjectId; }

			THREAD_POOL_OBJECT GetNowTHREAD_POOL_OBJECT() { return (void*)m_S_T_pThreadPoolObject; }

			THREAD_OBJECT GetTHREAD_OBJECT(const char* _name);

			THREAD_POOL_OBJECT GetTHREAD_POOL_OBJECT(const char* _name);

			void GetTHREAD_OBJECT_Name(THREAD_OBJECT _THREAD_OBJECT, char* _name, int _size);

			void GetTHREAD_POOL_OBJECT_Name(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, char* _name, int _size);

			void Shutdown(const char* _name);

			void Shutdown(THREAD_OBJECT _THREAD_OBJECT);

			bool TerminateThread(THREAD_OBJECT _THREAD_OBJECT);

			int GetLastError();

			unsigned long long GetThreadSize();

		protected:
			void DeleteThreadMessage(ThreadObject* pThreadObject);
			void DeleteThreadMemory(ThreadObject* pThreadObject);
		public:
			void Updata();
		private:
			static arcoss ThreadFunction(void* _data);
		private:
			MAP_SRTING_THREADOBJECT m_map_ThreadObject;
			std::map<unsigned long long, ThreadObject*> m_set_ThreadObject;

			MAP_SRTING_THREADPOOLOBJECT m_map_Srting_ThreadPoolObject;

			Magic_MUTEX m_Mutex, m_MutexPoolObject;

			static S_THREAD ThreadObject* m_S_T_pThreadObject;
			static S_THREAD THREAD_OBJECT m_S_T_ThreadObjectId;
			static S_THREAD ThreadPoolObject* m_S_T_pThreadPoolObject;
			static S_THREAD int m_S_T_Error;

			static SystemThread* m_S_pSystemThread;
		};

		void ThreadMessageHandle(ThreadObject* _pThreadObject, bool isWait = true);
		void ThreadHandle(ThreadObject* _pThreadObject);

		void MessageHandle(ThreadObject* _pThreadObject, const unsigned int& _MessageType, const long long& _Message);
		void MessageHandleKey(ThreadObject* _pThreadObject, const std::string& key, const MESSAGE_TRANSFER_FUNC& messageTransferFunc);
		void MessageHandleVecKey(std::vector<CALL_BACK_MONITOR_KEY>* _MonitorVec, const std::string& key, const MESSAGE_TRANSFER_FUNC& messageTransferFunc);
	}
}



#endif