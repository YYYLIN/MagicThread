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

#ifndef _CROSS_PLANTFORM_PORT_H_
#define _CROSS_PLANTFORM_PORT_H_

#include <time.h>
#include <stdio.h>

#ifdef WIN32
#include <process.h>
#include <windows.h>
#include <io.h>
#else
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#endif

#ifdef WIN32
#define Magic_Sprintf_s sprintf_s
#define Magic_CLOCK()	clock()

/*
*���ܣ������߳�
*������
*	f = �̶߳���ָ��
*	b = Ĭ�Ͽգ�ƽ̨���ԣ�
*	c = �̺߳���ָ��
*	d = �̲߳���
*����ֵ��������ڷ���ֵ�ɹ�
*/
#define Magic_Thread_Create(f,b,c,d)				f = CreateThread(NULL, b, c, d, 0, NULL)
#define Magic_ResumeThread(a)						ResumeThread(a)
#define Magic_CloseHandle(a)						CloseHandle(a)
#define Magic_Thread_Wait(a)						WaitForSingleObject(a, INFINITE)
#define Magic_Thread_Exit(a)						TerminateThread(a, 0)

/*
*���ܣ�����������Ϣ
*������
*	a = ����ָ��
*	b = Win32ƽ̨����
*	c = Win32ƽ̨����
*	d = Win32ƽ̨���������Ϣ����
*	e = Win32ƽ̨������������
*	f = Linuxƽ̨����
*	g = Linuxƽ̨����
*/
#define Magic_Thread_SEM_init(a,b,c,d,e,f,g)		a = CreateSemaphore(b,c,d,e)
#define Magic_Thread_SEM_Wait(a)					WaitForSingleObject(a, INFINITE)
#define Magic_Thread_SEM_Wait_Time(a,t)				do{WaitForSingleObject(a, t);}while(0)
#define Magic_Thread_SEM_Post(a)					ReleaseSemaphore(a, 1, NULL)
#define Magic_Thread_SEM_destroy(a)					CloseHandle(a)
#define Magic_Thread_Mutex_Lock(a)					EnterCriticalSection(a)
#define Magic_Thread_Mutex_unLock(a)				LeaveCriticalSection(a)
#define Magic_Thread_Mutex_Init(a)					InitializeCriticalSection(a)
#define Magic_Thread_Mutex_Destroy(a)				DeleteCriticalSection(a)
#define arcoss										DWORD WINAPI
#define arcoss_return(a)							((DWORD)(a))
typedef HANDLE										Magic_THREAD;
typedef HANDLE										Magic_SEM;
typedef HANDLE										Magic_SHARED;
typedef CRITICAL_SECTION							Magic_MUTEX;
typedef int											socklen_t;

typedef unsigned int								Magic_SOCKSET;

#define Magic_Sleep(a)								Sleep(a * 1000)
#define Magic_MSleep(a)								Sleep(a)
#define Magic_ProgressID							_getpid
#define Magic_CloseSocket(a)						closesocket(a)
#define Magic_File_Open(a, b, c)					fopen_s(&a, b, c)
#define Magic_File_Read(a, b, c, d)					fread(a, b, c, d)
#define Magic_File_Write(a, b, c, d)				fwrite(a, b, c, d)
#define Magic_File_Close(a)							fclose(a)
#define Magic_File_Access(a, b)						_access(a, b)
#define Magic_File_CreateDirectory(a, b)			_mkdir(a)

#define DLL_OPEN(a)									LoadLibraryA(a)
#define DLL_CLOSE(a)								FreeLibrary(a)
#define DLL_GetFunction(a, b)						GetProcAddress(a, b)
#define DLL_HANDLE									HINSTANCE

#define S_THREAD									__declspec(thread)

#else
#define Magic_Sprintf_s snprintf

#define Magic_Thread_Create(f,b,c,d)				(pthread_create(&f,b,c,d) == 0)
#define Magic_ResumeThread(a)
#define Magic_CloseHandle(a)
#define Magic_Thread_Wait(a)						pthread_join(a, NULL)
#define Magic_Thread_Exit(a)						pthread_cancel(a)
#define Magic_Thread_SEM_init(a,b,c,d,e,f,g)		(sem_init(&a,f,g) == 0)
#define Magic_Thread_SEM_Wait(a)					sem_wait(&a)
#define Magic_Thread_SEM_Wait_Time(a,t)				do{												\
														struct timespec ts;							\
														clock_gettime(CLOCK_REALTIME, &ts);			\
														unsigned long msecs = t;					\
														unsigned long secs = msecs/1000;						\
														unsigned long add = 0;								\
														msecs = (msecs % 1000) * 1000 * 1000 + ts.tv_nsec;	\
														add = msecs / (1000 * 1000 * 1000);			\
														ts.tv_sec += (add + secs);					\
														ts.tv_nsec = msecs % (1000 * 1000 * 1000);  \
														sem_timedwait(&a, &ts);						\
													}while(0)


#define Magic_Thread_SEM_Post(a)					sem_post(&a)
#define Magic_Thread_SEM_destroy					sem_destroy(&a)
#define Magic_Thread_Mutex_Lock(a)					pthread_mutex_lock(a)
#define Magic_Thread_Mutex_unLock(a)				pthread_mutex_unlock(a)
#define Magic_Thread_Mutex_Init(a)					pthread_mutex_init(a, NULL)
#define Magic_Thread_Mutex_Destroy(a)				pthread_mutex_destroy(a)

#define arcoss										void*
#define arcoss_return								((void*)(a))
typedef pthread_t									Magic_THREAD;
typedef	sem_t										Magic_SEM;
typedef pthread_mutex_t								Magic_MUTEX;
typedef int											Magic_SOCKSET;
typedef int											Magic_SHARED;
#define Magic_Sleep(a)								sleep(a)
#define Magic_MSleep(a)								usleep(a * 1000)
#define Magic_ProgressID							getpid

/*�ļ�����*/
#define Magic_CloseSocket(a)						close(a)
#define Magic_File_Open(a, b, c)					a = fopen(b, c)
#define Magic_File_Read(a, b, c, d)					fread(a, b, c, d)
#define Magic_File_Write(a, b, c, d)				fwrite(a, b, c, d)
#define Magic_File_Close(a)							fclose(a)
#define Magic_File_Access(a, b)						access(a, b)
#define Magic_File_CreateDirectory(a, b)			mkdir(a, b)

#define DLL_OPEN(a)									dlopen(a, RTLD_LAZY)
#define DLL_CLOSE(a)								dlclose(a)
#define DLL_GetFunction(a, b)						dlsym(a, b)
#define DLL_HANDLE									void*

#define S_THREAD									__thread

#endif


#endif