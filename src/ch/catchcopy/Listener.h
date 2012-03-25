#ifndef LISTENER_H
#define LISTENER_H

#include "PluginInterface_Listener.h"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define PIPE_TIMEOUT 5000
#define BUFSIZE 4096

typedef struct
{
   OVERLAPPED oOverlap;
   HANDLE hPipeInst;
   TCHAR chRequest[BUFSIZE];
   DWORD cbRead;
   TCHAR chReply[BUFSIZE];
   DWORD cbToWrite;
} PIPEINST, *LPPIPEINST;

VOID DisconnectAndClose(LPPIPEINST);
BOOL CreateAndConnectInstance(LPOVERLAPPED);
BOOL ConnectToNewClient(HANDLE, LPOVERLAPPED);
VOID GetAnswerToRequest(LPPIPEINST);

VOID WINAPI CompletedWriteRoutine(DWORD, DWORD, LPOVERLAPPED);
VOID WINAPI CompletedReadRoutine(DWORD, DWORD, LPOVERLAPPED);
char * toHex(const char *str);

class Listener : public PluginInterface_Listener
{
public:
	Listener(PluginInterface_Listener_Return * returnObject);
	/// \brief put this plugin in listen mode
	bool listen();
	/// \brief put close the listen
	void close();
	/// \brief send when copy is finished
	void transferFinished(unsigned int orderId,bool withError);
	/// \brief send when copy is canceled
	void transferCanceled(unsigned int orderId);
private:
	HANDLE hConnectEvent;
	OVERLAPPED oConnect;
	LPPIPEINST lpPipeInst;
	DWORD dwWait, cbRet;
	BOOL fSuccess, fPendingIO;
};

#endif // LISTENER_H
