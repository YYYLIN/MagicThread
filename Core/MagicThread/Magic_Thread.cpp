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

#include "Magic_Thread.h"
#include "Magic_Thread_Management.h"

namespace Magic
{
	namespace Management
	{
		int GetLastError() {
			return SystemThread::Instance()->GetLastError();
		}

		unsigned long long GetThreadSize() {
			return SystemThread::Instance()->GetThreadSize();
		}

		bool IsCreateThreadManagement() {
			Magic_MUTEX mutex = Magic::Management::SystemThread::getGlobalMutex();
			Magic_Thread_Mutex_Lock(&mutex);
			bool result = SystemThread::Instance() != 0;
			Magic_Thread_Mutex_unLock(&mutex);

			return result;
		}

		bool CreateThreadManagement(ThreadMessageMode threadmessagemode)
		{
			Magic_MUTEX mutex = Magic::Management::SystemThread::getGlobalMutex();
			Magic_Thread_Mutex_Lock(&mutex);
			if (!SystemThread::Instance())
			{
				//创建线程管理系统
				Magic::Management::SystemThread* _pSystemThread = 0;
				_pSystemThread = new Magic::Management::SystemThread;
				bool result = _pSystemThread->Initialize(threadmessagemode);
				Magic_Thread_Mutex_unLock(&mutex);
				if (!result)
					return 0;

				return true;
			}
			else {
				Magic_Thread_Mutex_unLock(&mutex);
				return false;
			}
		}

		void ShutdownThreadManagement()
		{
			if (SystemThread::Instance())
			{
				SystemThread::Instance()->Shutdown();
				delete SystemThread::Instance();
			}
		}

		void UpdataThreadManagement()
		{
			SystemThread::Instance()->Updata();
		}

		THREAD_OBJECT CreateThreadObject(const char* _name, ThreadTypeMode _ThreadTypeMode, ThreadMessageMode _ThreadMessageMode)
		{
			return SystemThread::Instance()->Create(_name, _ThreadTypeMode, _ThreadMessageMode, true);
		}

		THREAD_POOL_OBJECT CreateThreadPoolObject(const char* _name, unsigned int _ThreadNumber)
		{
			return SystemThread::Instance()->CreatePool(_name, _ThreadNumber);
		}

		void ShutdownThreadObject(const char* _name)
		{
			SystemThread::Instance()->Shutdown(_name);
		}

		void ShutdownThreadObject(THREAD_OBJECT _THREAD_OBJECT)
		{
			SystemThread::Instance()->Shutdown(_THREAD_OBJECT);
		}

		void ShutdownThreadPoolObject(const char* _name)
		{
			SystemThread::Instance()->ShutdownPool(SystemThread::Instance()->GetTHREAD_POOL_OBJECT(_name));
		}

		void ShutdownThreadPoolObject(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT)
		{
			SystemThread::Instance()->ShutdownPool(_THREAD_POOL_OBJECT);
		}

		void ShutdownThreadObject()
		{
			SystemThread::Instance()->Shutdown(SystemThread::Instance()->GetNowTHREAD_OBJECT());
		}

		bool TerminateThread(THREAD_OBJECT _THREAD_OBJECT) {
			return SystemThread::Instance()->TerminateThread(_THREAD_OBJECT);
		}

		bool SetWaitTime(THREAD_OBJECT _THREAD_OBJECT, unsigned long time) {
			return SystemThread::Instance()->SetWaitTime(_THREAD_OBJECT, time);
		}

		bool SetWaitTime(const char* _name, unsigned long time) {
			return SystemThread::Instance()->SetWaitTime(SystemThread::Instance()->GetTHREAD_OBJECT(_name), time);
		}

		bool SetWaitTime(unsigned long time) {
			return SystemThread::Instance()->SetWaitTime(SystemThread::Instance()->GetNowTHREAD_OBJECT(), time);
		}

		bool SetMode(THREAD_OBJECT _THREAD_OBJECT, ThreadMessageMode _ThreadMessageMode) {
			return SystemThread::Instance()->SetMode(_THREAD_OBJECT, _ThreadMessageMode);
		}

		bool SetMode(const char* _name, ThreadMessageMode _ThreadMessageMode) {
			return SystemThread::Instance()->SetMode(SystemThread::Instance()->GetTHREAD_OBJECT(_name), _ThreadMessageMode);
		}

		bool SetMode(ThreadMessageMode _ThreadMessageMode) {
			return SystemThread::Instance()->SetMode(SystemThread::Instance()->GetNowTHREAD_OBJECT(), _ThreadMessageMode);
		}

		bool MonitorThread(THREAD_OBJECT _THREAD_OBJECT, const Callback_Void& _CallBack) {
			return SystemThread::Instance()->MonitorThread(_THREAD_OBJECT, _CallBack);
		}

		bool MonitorThread(const char* _name, const Callback_Void& _CallBack) {
			return SystemThread::Instance()->MonitorThread(SystemThread::Instance()->GetTHREAD_OBJECT(_name), _CallBack);
		}

		bool MonitorThreadPool(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, const Callback_Void& _CallBack) {
			return SystemThread::Instance()->MonitorThreadPool(_THREAD_POOL_OBJECT, _CallBack);
		}

		bool MonitorThreadPool(const char* _name, const Callback_Void& _CallBack) {
			return SystemThread::Instance()->MonitorThreadPool(SystemThread::Instance()->GetTHREAD_POOL_OBJECT(_name), _CallBack);
		}

		bool MonitorThreadMessage(THREAD_OBJECT _THREAD_OBJECT, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack)
		{
			return SystemThread::Instance()->MonitorThreadMessage(_THREAD_OBJECT, _MessageType, _CallBack);
		}

		bool MonitorThreadMessage(const char* _name, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack) {
			return SystemThread::Instance()->MonitorThreadMessage(SystemThread::Instance()->GetTHREAD_OBJECT(_name), _MessageType, _CallBack);
		}

		bool MonitorThreadMessage(THREAD_OBJECT _THREAD_OBJECT, const std::string& key, const Callback_Message_Key& _CallBack, WAIT_MESSAGE* waitMessage) {
			return SystemThread::Instance()->MonitorThreadMessage(_THREAD_OBJECT, key, _CallBack, waitMessage);
		}

		bool MonitorThreadMessage(const char* _name, const std::string& key, const Callback_Message_Key& _CallBack, WAIT_MESSAGE* waitMessage) {
			return SystemThread::Instance()->MonitorThreadMessage(SystemThread::Instance()->GetTHREAD_OBJECT(_name), key, _CallBack, waitMessage);
		}

		unsigned int WaitMessage(WAIT_MESSAGE waitMessage, unsigned long timeout, bool isSync) {
			return SystemThread::Instance()->WaitMessage(waitMessage, timeout, isSync);
		}

		bool MonitorThreadPoolMessage(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack)
		{
			return SystemThread::Instance()->MonitorThreadPoolMessage(_THREAD_POOL_OBJECT, _MessageType, _CallBack);
		}

		bool MonitorThreadPoolMessage(const char* _name, MESSAGE_TYPE _MessageType, const Callback_Message& _CallBack) {
			return SystemThread::Instance()->MonitorThreadPoolMessage(SystemThread::Instance()->GetTHREAD_POOL_OBJECT(_name), _MessageType, _CallBack);
		}

		bool SendMessageTo(THREAD_OBJECT _THREAD_OBJECT, MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack, bool _Synch)
		{
			return SystemThread::Instance()->SendMessageTo(_THREAD_OBJECT, _MessageType, _Message, "", nullptr, _CallBack, _Synch);
		}

		bool SendMessageTo(const char* _name, MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack, bool _Synch)
		{
			return SystemThread::Instance()->SendMessageTo(SystemThread::Instance()->GetTHREAD_OBJECT(_name), _MessageType, _Message, "", nullptr, _CallBack, _Synch);
		}

		bool SendMessageTo(MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack)
		{
			return SystemThread::Instance()->SendMessageTo(_MessageType, _Message, _CallBack);
		}

		bool SendMessageTo(THREAD_OBJECT _THREAD_OBJECT, const std::string& key, const MESSAGE_TRANSFER_FUNC& messageTransfer) {
			return SystemThread::Instance()->SendMessageTo(_THREAD_OBJECT, key, messageTransfer);
		}

		bool SendMessageTo(const char* _name, const std::string& key, const MESSAGE_TRANSFER_FUNC& messageTransfer) {
			return SystemThread::Instance()->SendMessageTo(SystemThread::Instance()->GetTHREAD_OBJECT(_name), key, messageTransfer);
		}

		bool SendMessageTo(const std::string& key, const MESSAGE_TRANSFER_FUNC& messageTransfer) {
			return SystemThread::Instance()->SendMessageTo(SystemThread::Instance()->GetNowTHREAD_OBJECT(), key, messageTransfer);
		}

		bool SendMessageToPool(const char* _name, MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack, bool _Synch)
		{
			return SystemThread::Instance()->SendMessageToPool(SystemThread::Instance()->GetTHREAD_POOL_OBJECT(_name), _MessageType, _Message, _CallBack, _Synch);
		}

		bool SendMessageToPool(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, MESSAGE_TYPE _MessageType, MESSAGE _Message, const Callback_Message& _CallBack, bool _Synch)
		{
			return SystemThread::Instance()->SendMessageToPool(_THREAD_POOL_OBJECT, _MessageType, _Message, _CallBack, _Synch);
		}

		THREAD_OBJECT GetNowTHREAD_OBJECT()
		{
			return SystemThread::Instance()->GetNowTHREAD_OBJECT();
		}

		THREAD_POOL_OBJECT GetNowTHREAD_POOL_OBJECT()
		{
			return SystemThread::Instance()->GetNowTHREAD_POOL_OBJECT();
		}

		THREAD_OBJECT GetTHREAD_OBJECT(const char* _name)
		{
			return SystemThread::Instance()->GetTHREAD_OBJECT(_name);
		}

		void GetTHREAD_OBJECT_Name(THREAD_OBJECT _THREAD_OBJECT, char* _name, int _size)
		{
			SystemThread::Instance()->GetTHREAD_OBJECT_Name(_THREAD_OBJECT, _name, _size);
		}

		void GetTHREAD_POOL_OBJECT_Name(THREAD_POOL_OBJECT _THREAD_POOL_OBJECT, char* _name, int _size)
		{
			SystemThread::Instance()->GetTHREAD_POOL_OBJECT_Name(_THREAD_POOL_OBJECT, _name, _size);
		}
	}
}