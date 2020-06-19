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


#include "Magic_Thread_Management.h"

namespace Magic
{
	namespace Management
	{
		Message::Message() :m_MessageType(0), m_Message(0), m_pThreadObject(0)
		{

		}

		Message::Message(const unsigned int& _MessageType, const long long& _Message, const Callback_Message& _CallBack, ThreadObject* _pThreadObject)
			: m_MessageType(_MessageType), m_Message(_Message), m_CallBack(_CallBack), m_pThreadObject(_pThreadObject)
		{

		}

		ThreadObject::ThreadObject()
		{
			m_ThreadTypeMode = THREAD_RUN_ONCE;
			m_ThreadRunState = THREAD_STATE_TIMEOUT;
			m_ThreadMessageMode = THREAD_MESSAGE_NO_WAIT;
			m_ThreadWaitTime = MAGIC_WAIT_INFINITE;
		}

		ThreadObject::ThreadObject(ThreadTypeMode _ThreadTypeMode, ThreadRunState _ThreadRunState, const std::string& _name, ThreadMessageMode _ThreadMessageMode)
		{
			m_ThreadTypeMode = _ThreadTypeMode;
			m_ThreadRunState = _ThreadRunState;
			m_ThreadMessageMode = _ThreadMessageMode;
			m_ThreadWaitTime = MAGIC_WAIT_INFINITE;
			m_Name = _name;
		}

		void ThreadPoolObject::Updata()
		{
			Message _Message;
			Magic_Thread_SEM_Wait(m_queue_SEM);
			Magic_Thread_Mutex_Lock(&m_MessageMutex);
			size_t _Number = m_queue_Message.size();
			if (_Number)
			{
				_Message = m_queue_Message.front();
				m_queue_Message.pop();
			}
			Magic_Thread_Mutex_unLock(&m_MessageMutex);

			if (_Number)
				SystemThread::Instance()->SendMessageTo(_Message.m_MessageType, _Message.m_Message, _Message.m_CallBack);
		}

		S_THREAD ThreadObject* SystemThread::m_S_T_pThreadObject = 0;
		S_THREAD ThreadPoolObject* SystemThread::m_S_T_pThreadPoolObject = 0;
		SystemThread* SystemThread::m_S_pSystemThread = 0;

		SystemThread::SystemThread()
		{
			m_S_pSystemThread = this;
		}

		SystemThread::~SystemThread()
		{
			Magic_Thread_Mutex_Lock(&m_Mutex);
			for (auto _auto = m_map_ThreadObject.begin(); _auto != m_map_ThreadObject.end(); _auto++)
			{
				if (m_S_T_pThreadObject != &_auto->second)
					Shutdown(&_auto->second);
			}
			Magic_Thread_Mutex_unLock(&m_Mutex);

			IsDeleteThread();

			m_S_pSystemThread = 0;
			Magic_Thread_Mutex_Destroy(&m_MutexPoolObject);
			Magic_Thread_Mutex_Destroy(&m_Mutex);

			//发送主线程退出消息
			MessageHandle(m_S_T_pThreadObject, MESSAGE_THREAD_CLOSE, (long long)m_S_T_pThreadObject);
		}

		SystemThread* SystemThread::Instance()
		{
			return m_S_pSystemThread;
		}

		bool SystemThread::Initialize(ThreadMessageMode threadmessagemode)
		{
			Magic_Thread_Mutex_Init(&m_Mutex);
			Magic_Thread_Mutex_Init(&m_MutexPoolObject);

			m_S_T_pThreadObject = (ThreadObject*)Create(MAGIC_MAIN_THREAD_NAME, THREAD_LOOP_RUN, threadmessagemode, false);
			if (!m_S_T_pThreadObject)
				return false;

			return true;
		}

		THREAD_OBJECT SystemThread::Create(const char* _name, ThreadTypeMode _ThreadTypeMode, ThreadMessageMode _ThreadMessageMode, bool _IsNewThread)
		{
			THREAD_OBJECT _THREAD_OBJECT = 0;
			Magic_Thread_Mutex_Lock(&m_Mutex);
			m_map_ThreadObject.insert(std::make_pair(_name, ThreadObject(_ThreadTypeMode, THREAD_STATE_RUN, _name, _ThreadMessageMode)));

			auto _findTO = m_map_ThreadObject.find(_name);
			if (_findTO != m_map_ThreadObject.end())
			{
				_THREAD_OBJECT = &_findTO->second;
				m_set_ThreadObject.insert(&_findTO->second);
				Magic_Thread_Mutex_Init(&_findTO->second.m_MessageMutex);
				Magic_Thread_SEM_init(_findTO->second.m_Queue_SEM, NULL, 0, LONG_MAX, NULL, NULL, 0);
				Magic_Thread_SEM_init(_findTO->second.m_Synch_SEM, NULL, 0, LONG_MAX, NULL, NULL, 0);
				if (_IsNewThread) {
					if (Magic_Thread_Create(_findTO->second.m_Thread, NULL, ThreadFunction, (void*)(&_findTO->second)))
						Magic_ResumeThread(_findTO->second.m_Thread);
				}
			}
			Magic_Thread_Mutex_unLock(&m_Mutex);
			return _THREAD_OBJECT;
		}

		THREAD_POOL_OBJECT SystemThread::CreatePool(const char* _name, unsigned int _ThreadNumber)
		{
			THREAD_POOL_OBJECT  _THREAD_POOL_OBJECT = 0;

			Magic_Thread_Mutex_Lock(&m_MutexPoolObject);
			m_map_Srting_ThreadPoolObject.insert(std::make_pair(_name, ThreadPoolObject()));
			auto _findTO = m_map_Srting_ThreadPoolObject.find(_name);
			if (_findTO != m_map_Srting_ThreadPoolObject.end())
			{
				_THREAD_POOL_OBJECT = &_findTO->second;
				_findTO->second.m_Name = _name;
				Magic_Thread_Mutex_Init(&_findTO->second.m_MessageMutex);
				Magic_Thread_SEM_init(_findTO->second.m_queue_SEM, NULL, 0, LONG_MAX, NULL, NULL, 0);

				for (unsigned int a = 0; a < _ThreadNumber; a++)
				{
					char _text[256];
					Magic_Sprintf_s(_text, 256, "%s_%d", _name, a);
					THREAD_OBJECT _THREAD_OBJECT = Create(_text, THREAD_LOOP_RUN, THREAD_MESSAGE_WAIT, true);
					SendMessageTo(_THREAD_OBJECT, 0, 0,
						[_THREAD_POOL_OBJECT](MESSAGE_TYPE, MESSAGE)
					{
						m_S_T_pThreadPoolObject = (ThreadPoolObject*)_THREAD_POOL_OBJECT;
						m_S_T_pThreadObject->m_ThreadMessageMode = THREAD_MESSAGE_NO_WAIT;
					});

					MonitorThread(_THREAD_OBJECT, BindClassFunctionObject(&ThreadPoolObject::Updata, &_findTO->second));
					if (_THREAD_OBJECT)
						_findTO->second.m_vec_ThreadObject.push_back((ThreadObject*)_THREAD_OBJECT);
				}
			}
			Magic_Thread_Mutex_unLock(&m_MutexPoolObject);

			return _THREAD_POOL_OBJECT;
		}

		void SystemThread::ShutdownPool(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT)
		{
			ThreadPoolObject* _pThreadPoolObject = (ThreadPoolObject*)_THREAD_POOL_OBJECT;
			if (!_pThreadPoolObject)
				return;

			Magic_Thread_Mutex_Lock(&m_MutexPoolObject);

			Magic_Thread_Mutex_Lock(&_pThreadPoolObject->m_MessageMutex);
			for (auto _auto : _pThreadPoolObject->m_vec_ThreadObject)
			{
				Shutdown(_auto);
				Magic_Thread_SEM_Post(_pThreadPoolObject->m_queue_SEM);
			}
			Magic_Thread_Mutex_unLock(&_pThreadPoolObject->m_MessageMutex);

			for (auto _auto : _pThreadPoolObject->m_vec_ThreadObject)
			{
				bool _IsHave;
				Magic_Thread_Mutex_Lock(&m_Mutex);
				_IsHave = m_set_ThreadObject.find(_auto) != m_set_ThreadObject.end();
				Magic_Thread_Mutex_unLock(&m_Mutex);
				if (_IsHave)
				{
					Magic_Thread_Wait(_auto->m_Thread);
				}
			}

			Magic_Thread_SEM_destroy(_pThreadPoolObject->m_queue_SEM);
			Magic_Thread_Mutex_Destroy(&_pThreadPoolObject->m_MessageMutex);

			std::string _Name = _pThreadPoolObject->m_Name;
			m_map_Srting_ThreadPoolObject.erase(_Name);

			Magic_Thread_Mutex_unLock(&m_MutexPoolObject);
		}

		bool SystemThread::SetWaitTime(THREAD_OBJECT _THREAD_OBJECT, unsigned long time) {
			return this->SendMessageTo(_THREAD_OBJECT, 0, 0,
				[time, _THREAD_OBJECT](MM_MESS) {
				ThreadObject* _pThreadObject = (ThreadObject*)_THREAD_OBJECT;
				_pThreadObject->m_ThreadWaitTime = time;
			});
		}

		bool SystemThread::MonitorThread(THREAD_OBJECT _THREAD_OBJECT, const Callback_Void& _CallBack) {
			Callback_Void _BufferCallback = _CallBack;

			return SendMessageTo(_THREAD_OBJECT, 0, 0, [_BufferCallback](MESSAGE_TYPE _MessageType, MESSAGE _Message) {
				m_S_T_pThreadObject->m_vec_Callback.push_back(_BufferCallback);
			});
		}

		bool SystemThread::MonitorThreadPool(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, const Callback_Void& _CallBack) {

			Callback_Void _BufferCallback = _CallBack;

			return SendMessageToPool(_THREAD_POOL_OBJECT, 0, 0, [_BufferCallback](MESSAGE_TYPE _MessageType, MESSAGE _Message) {
				m_S_T_pThreadObject->m_vec_Callback.push_back(_BufferCallback);
			});
		}

		bool SystemThread::MonitorThreadMessage(THREAD_OBJECT _THREAD_OBJECT, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack)
		{
			Callback_Message _BufferCallback = _CallBack;

			return SendMessageTo(_THREAD_OBJECT, 0, 0, [_BufferCallback, _MessageType](MESSAGE_TYPE, MESSAGE _Message) {
				auto _MointorVec = m_S_T_pThreadObject->m_umap_MonitorFunction.find(_MessageType);
				if (_MointorVec != m_S_T_pThreadObject->m_umap_MonitorFunction.end())
					_MointorVec->second.push_back(_BufferCallback);
				else
					m_S_T_pThreadObject->m_umap_MonitorFunction.insert(std::make_pair(_MessageType, std::vector<Callback_Message>({ _BufferCallback })));
			});
		}

		bool SystemThread::MonitorThreadPoolMessage(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack)
		{
			Callback_Message _BufferCallback = _CallBack;

			return SendMessageToPool(_THREAD_POOL_OBJECT, 0, 0, [_BufferCallback, _MessageType](MESSAGE_TYPE, MESSAGE _Message) {
				auto _MointorVec = m_S_T_pThreadObject->m_umap_MonitorFunction.find(_MessageType);
				if (_MointorVec != m_S_T_pThreadObject->m_umap_MonitorFunction.end())
					_MointorVec->second.push_back(_BufferCallback);
				else
					m_S_T_pThreadObject->m_umap_MonitorFunction.insert(std::make_pair(_MessageType, std::vector<Callback_Message>({ _BufferCallback })));
			});
		}

		bool SystemThread::SendMessageTo(THREAD_OBJECT _THREAD_OBJECT, MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack, bool _Synch)
		{
			ThreadObject* _pThreadObject = (ThreadObject*)_THREAD_OBJECT;
			if (!_pThreadObject)
				return false;
			bool _IsHave;
			Magic_Thread_Mutex_Lock(&m_Mutex);
			_IsHave = m_set_ThreadObject.find(_pThreadObject) != m_set_ThreadObject.end();
			Magic_Thread_Mutex_unLock(&m_Mutex);
			if (!_IsHave)
				return false;

			bool _ThreadMessageMode = false;

			ThreadObject* _pSendThreadObject = 0;
			if (_Synch && _pSendThreadObject != m_S_T_pThreadObject) {
				_pSendThreadObject = m_S_T_pThreadObject;
			}

			Magic_Thread_Mutex_Lock(&_pThreadObject->m_MessageMutex);
			_pThreadObject->m_queue_Message.push_back(Message(_MessageType, _Message, _CallBack, _pSendThreadObject));
			_ThreadMessageMode = _pThreadObject->m_ThreadMessageMode == THREAD_MESSAGE_WAIT;
			Magic_Thread_Mutex_unLock(&_pThreadObject->m_MessageMutex);
			if (_ThreadMessageMode)
				Magic_Thread_SEM_Post(_pThreadObject->m_Queue_SEM);

			//同步模式，等待其他线程处理完后再返回
			if (_pSendThreadObject)
				Magic_Thread_SEM_Wait(_pSendThreadObject->m_Synch_SEM);

			return true;
		}

		bool SystemThread::SendMessageTo(MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack, bool _Synch)
		{
			return SendMessageTo(m_S_T_pThreadObject, _MessageType, _Message, _CallBack, _Synch);
		}

		bool SystemThread::SendMessageToPool(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack, bool _Synch)
		{
			ThreadPoolObject* _pThreadPoolObject = (ThreadPoolObject*)_THREAD_POOL_OBJECT;
			if (!_pThreadPoolObject)
				return false;

			ThreadObject* _pSendThreadObject = 0;
			if (_Synch && m_S_T_pThreadObject) {
				_pSendThreadObject = m_S_T_pThreadObject;
			}

			Magic_Thread_Mutex_Lock(&_pThreadPoolObject->m_MessageMutex);
			_pThreadPoolObject->m_queue_Message.push(Message(_MessageType, _Message, _CallBack, _pSendThreadObject));
			Magic_Thread_Mutex_unLock(&_pThreadPoolObject->m_MessageMutex);

			Magic_Thread_SEM_Post(_pThreadPoolObject->m_queue_SEM);

			//同步模式，等待其他线程处理完后再返回
			if (_pSendThreadObject)
				Magic_Thread_SEM_Wait(_pSendThreadObject->m_Synch_SEM);

			return true;
		}

		THREAD_OBJECT SystemThread::GetTHREAD_OBJECT(const char* _name)
		{
			THREAD_OBJECT _THREAD_OBJECT = 0;
			Magic_Thread_Mutex_Lock(&m_Mutex);
			auto _findTO = m_map_ThreadObject.find(_name);
			if (_findTO != m_map_ThreadObject.end())
				_THREAD_OBJECT = &_findTO->second;
			Magic_Thread_Mutex_unLock(&m_Mutex);

			return _THREAD_OBJECT;
		}

		THREAD_OBJECT SystemThread::GetTHREAD_POOL_OBJECT(const char* _name)
		{
			THREAD_POOL_OBJECT _THREAD_POOL_OBJECT = 0;
			Magic_Thread_Mutex_Lock(&m_MutexPoolObject);
			auto _findTO = m_map_Srting_ThreadPoolObject.find(_name);
			if (_findTO != m_map_Srting_ThreadPoolObject.end())
				_THREAD_POOL_OBJECT = &_findTO->second;
			Magic_Thread_Mutex_unLock(&m_MutexPoolObject);

			return _THREAD_POOL_OBJECT;
		}

		void SystemThread::GetTHREAD_OBJECT_Name(THREAD_OBJECT _THREAD_OBJECT, char* _name, int _size)
		{
			ThreadObject* _pThreadObject = (ThreadObject*)_THREAD_OBJECT;

			if (!_pThreadObject)
				return;
			bool _IsHave;
			Magic_Thread_Mutex_Lock(&m_Mutex);
			_IsHave = m_set_ThreadObject.find(_pThreadObject) != m_set_ThreadObject.end();
			Magic_Thread_Mutex_unLock(&m_Mutex);
			if (!_IsHave)
				return;

			Magic_Thread_Mutex_Lock(&_pThreadObject->m_MessageMutex);
			Magic_strcpy_s(_name, _size, _pThreadObject->m_Name.c_str());
			Magic_Thread_Mutex_unLock(&_pThreadObject->m_MessageMutex);
		}

		void SystemThread::GetTHREAD_POOL_OBJECT_Name(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, char* _name, int _size)
		{
			ThreadPoolObject* _pThreadPoolObject = (ThreadPoolObject*)_THREAD_POOL_OBJECT;

			if (!_pThreadPoolObject)
				return;

			Magic_Thread_Mutex_Lock(&_pThreadPoolObject->m_MessageMutex);
			Magic_strcpy_s(_name, _size, _pThreadPoolObject->m_Name.c_str());
			Magic_Thread_Mutex_unLock(&_pThreadPoolObject->m_MessageMutex);
		}

		void SystemThread::Shutdown(const char* _name)
		{
			Shutdown(GetTHREAD_OBJECT(_name));
		}

		void SystemThread::Shutdown(THREAD_OBJECT _THREAD_OBJECT)
		{
			ThreadObject* _pThreadObject = (ThreadObject*)_THREAD_OBJECT;

			if (!_pThreadObject)
				return;
			bool _IsHave;
			Magic_Thread_Mutex_Lock(&m_Mutex);
			_IsHave = m_set_ThreadObject.find(_pThreadObject) != m_set_ThreadObject.end();
			Magic_Thread_Mutex_unLock(&m_Mutex);
			if (!_IsHave)
				return;

			Magic_Thread_Mutex_Lock(&_pThreadObject->m_MessageMutex);
			_pThreadObject->m_ThreadRunState = THREAD_STOP;
			if (_pThreadObject->m_ThreadMessageMode == THREAD_MESSAGE_WAIT)
				Magic_Thread_SEM_Post(_pThreadObject->m_Queue_SEM);
			Magic_Thread_Mutex_unLock(&_pThreadObject->m_MessageMutex);


		}

		bool SystemThread::TerminateThread(THREAD_OBJECT _THREAD_OBJECT) {
			ThreadObject* _pThreadObject = (ThreadObject*)_THREAD_OBJECT;
			if (m_S_T_pThreadObject == _pThreadObject) {
				return false;
			}
			Magic_Thread_Mutex_Lock(&m_Mutex);
			Magic_Thread_Mutex_Lock(&_pThreadObject->m_MessageMutex);
			//在强行结束线程时，必须保证被结束的线程没有占用锁。所导致的程序锁死问题。和资源未释放
			Magic_Thread_Terminate(_pThreadObject->m_Thread);
			Magic_Thread_Wait(_pThreadObject->m_Thread);

			Magic_Thread_Mutex_unLock(&m_Mutex);
			Magic_Thread_Mutex_unLock(&_pThreadObject->m_MessageMutex);

			//发送释放线程资源的消息
			Shutdown(_THREAD_OBJECT);

			return true;
		}

		Callback_Void SystemThread::UpdataStop(MAP_SRTING_THREADOBJECT::iterator& _auto)
		{
			if (m_S_T_pThreadObject != &_auto->second && _auto->second.m_ThreadRunState == THREAD_STOP)
			{
				std::string key = _auto->first;
				Magic_THREAD Thread = _auto->second.m_Thread;
				ThreadObject* threadobject = &_auto->second;
				Magic_MUTEX _Mutex = _auto->second.m_MessageMutex;
				Magic_SEM _SEM = _auto->second.m_Queue_SEM;
				Magic_SEM _Synch_SEM = _auto->second.m_Synch_SEM;
				return [this, key, Thread, threadobject, _Mutex, _SEM, _Synch_SEM]() {
					Magic_MUTEX _mutex = _Mutex;
					Magic_SEM _sem = _SEM;
					Magic_SEM _synch_sem = _Synch_SEM;

					Magic_Thread_Wait(Thread);
					Magic_CloseHandle(Thread);

					//缓存关闭完成消息回调
					std::vector<Callback_Message> vec_MonitorVec;
					auto _MonitorVec = threadobject->m_umap_MonitorFunction.find(MESSAGE_THREAD_CLOSED);
					if (_MonitorVec != threadobject->m_umap_MonitorFunction.end()) {
						vec_MonitorVec = _MonitorVec->second;
					}

					Magic_Thread_Mutex_Lock(&m_Mutex);
					m_set_ThreadObject.erase(threadobject);
					Magic_Thread_Mutex_unLock(&m_Mutex);

					Magic_Thread_Mutex_Lock(&m_Mutex);
					Magic_Thread_Mutex_Lock(&_mutex);
					m_map_ThreadObject.erase(key);
					Magic_Thread_Mutex_unLock(&_mutex);
					Magic_Thread_Mutex_unLock(&m_Mutex);

					Magic_Thread_SEM_destroy(_synch_sem);
					Magic_Thread_SEM_destroy(_sem);

					Magic_Thread_Mutex_Destroy(&_mutex);

					for (auto i = vec_MonitorVec.begin(); i != vec_MonitorVec.end(); i++) {
						i->operator()(MESSAGE_THREAD_CLOSED, (long long)threadobject);
					}
				};
			}
			else {
				return 0;
			}
		}

		void SystemThread::IsDeleteThread() {
			std::vector<Callback_Void> vec_Delete_Callback;
			Magic_Thread_Mutex_Lock(&m_Mutex);
			for (auto _auto = m_map_ThreadObject.begin(); _auto != m_map_ThreadObject.end(); _auto++) {
				auto a = UpdataStop(_auto);
				if (a) {
					vec_Delete_Callback.push_back(a);
				}
			}
			Magic_Thread_Mutex_unLock(&m_Mutex);

			for (auto& a : vec_Delete_Callback) {
				a();
			}
		}

		void SystemThread::Updata()
		{
			do
			{
				IsDeleteThread();
				ThreadMessageHandle(m_S_T_pThreadObject);
				ThreadHandle(m_S_T_pThreadObject);
			} while (m_S_T_pThreadObject->m_ThreadRunState != THREAD_STOP);
		}

		arcoss SystemThread::ThreadFunction(void* _data)
		{
			ThreadObject* _pThreadObject = (ThreadObject*)_data;
			m_S_T_pThreadObject = _pThreadObject;

			//如果只运行一次，那么直接关闭循环
			if (_pThreadObject->m_ThreadTypeMode == THREAD_RUN_ONCE) {
				_pThreadObject->m_ThreadRunState = THREAD_STOP;
			}

			do
			{
				ThreadMessageHandle(_pThreadObject);
				ThreadHandle(_pThreadObject);
			} while (_pThreadObject->m_ThreadRunState != THREAD_STOP);

			MessageHandle(_pThreadObject, MESSAGE_THREAD_CLOSE, (long long)_pThreadObject);

			_pThreadObject->m_ThreadRunState = THREAD_STOP;
			return arcoss_return(0);
		}

		void ThreadMessageHandle(ThreadObject* _pThreadObject)
		{
			if (_pThreadObject->m_ThreadMessageMode == THREAD_MESSAGE_WAIT)
				Magic_Thread_SEM_Wait_Time(_pThreadObject->m_Queue_SEM, _pThreadObject->m_ThreadWaitTime);
			Magic_Thread_Mutex_Lock(&_pThreadObject->m_MessageMutex);
			_pThreadObject->m_Last_queue_Message = _pThreadObject->m_queue_Message;
			_pThreadObject->m_queue_Message.clear();
			Magic_Thread_Mutex_unLock(&_pThreadObject->m_MessageMutex);
			//处理上一个循环收集到的消息
			for (auto& a : _pThreadObject->m_Last_queue_Message) {
				if (a.m_CallBack) {
					a.m_CallBack(a.m_MessageType, a.m_Message);
				}

				MessageHandle(_pThreadObject, a.m_MessageType, a.m_Message);

				if (a.m_pThreadObject) {
					Magic_Thread_SEM_Post(a.m_pThreadObject->m_Synch_SEM);
				}
			}
			_pThreadObject->m_Last_queue_Message.clear();
		}

		void ThreadHandle(ThreadObject* _pThreadObject) {
			for (auto& _allback : _pThreadObject->m_vec_Callback) {
				_allback();
			}
		}

		void MessageHandle(ThreadObject* _pThreadObject, const unsigned int& _MessageType, const long long& _Message) {
			auto _MonitorVec = _pThreadObject->m_umap_MonitorFunction.find(_MessageType);
			if (_MonitorVec != _pThreadObject->m_umap_MonitorFunction.end()) {
				for (auto& _allback : _MonitorVec->second) {
					_allback(_MessageType, _Message);
				}
			}
		}
	}
}
