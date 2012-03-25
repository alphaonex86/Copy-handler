#include <QMessageBox>

#include "Listener.h"

HANDLE hPipe;

Listener::Listener(PluginInterface_Listener_Return * returnObject)
	: PluginInterface_Listener(returnObject)
{
}

/// \brief put this plugin in listen mode
bool Listener::listen()
{
     // Create one event object for the connect operation.

	hConnectEvent = CreateEvent(
	   NULL,    // default security attribute
	   TRUE,    // manual reset event
	   TRUE,    // initial state = signaled
	   NULL);   // unnamed event object

	if (hConnectEvent == NULL)
	{
	   printf("CreateEvent failed with %d.\n", GetLastError());
	   return 0;
	}

	oConnect.hEvent = hConnectEvent;

     // Call a subroutine to create one instance, and wait for
     // the client to connect.

	fPendingIO = CreateAndConnectInstance(&oConnect);
	return true;
}

/// \brief put close the listen
void Listener::close()
{
}

/// \brief send when copy is finished
void Listener::transferFinished(unsigned int orderId,bool withError)
{
}

/// \brief send when copy is canceled
void Listener::transferCanceled(unsigned int orderId)
{
}

// CompletedWriteRoutine(DWORD, DWORD, LPOVERLAPPED)
// This routine is called as a completion routine after writing to
// the pipe, or when a new client has connected to a pipe instance.
// It starts another read operation.

VOID WINAPI CompletedWriteRoutine(DWORD dwErr, DWORD cbWritten,
   LPOVERLAPPED lpOverLap)
{
   LPPIPEINST lpPipeInst;
   BOOL fRead = FALSE;

// lpOverlap points to storage for this instance.

   lpPipeInst = (LPPIPEINST) lpOverLap;

// The write operation has finished, so read the next request (if
// there is no error).

   if ((dwErr == 0) && (cbWritten == lpPipeInst->cbToWrite))
      fRead = ReadFileEx(
	 lpPipeInst->hPipeInst,
	 lpPipeInst->chRequest,
	 BUFSIZE*sizeof(TCHAR),
	 (LPOVERLAPPED) lpPipeInst,
	 (LPOVERLAPPED_COMPLETION_ROUTINE) CompletedReadRoutine);

// Disconnect if an error occurred.

   if (! fRead)
      DisconnectAndClose(lpPipeInst);
}

// CompletedReadRoutine(DWORD, DWORD, LPOVERLAPPED)
// This routine is called as an I/O completion routine after reading
// a request from the client. It gets data and writes it to the pipe.

VOID WINAPI CompletedReadRoutine(DWORD dwErr, DWORD cbBytesRead,
    LPOVERLAPPED lpOverLap)
{
   LPPIPEINST lpPipeInst;
   BOOL fWrite = FALSE;

// lpOverlap points to storage for this instance.

   lpPipeInst = (LPPIPEINST) lpOverLap;

// The read operation has finished, so write a response (if no
// error occurred).

   if ((dwErr == 0) && (cbBytesRead != 0))
   {
      GetAnswerToRequest(lpPipeInst);

      fWrite = WriteFileEx(
	 lpPipeInst->hPipeInst,
	 lpPipeInst->chReply,
	 lpPipeInst->cbToWrite,
	 (LPOVERLAPPED) lpPipeInst,
	 (LPOVERLAPPED_COMPLETION_ROUTINE) CompletedWriteRoutine);
   }

// Disconnect if an error occurred.

   if (! fWrite)
      DisconnectAndClose(lpPipeInst);
}

// DisconnectAndClose(LPPIPEINST)
// This routine is called when an error occurs or the client closes
// its handle to the pipe.

VOID DisconnectAndClose(LPPIPEINST lpPipeInst)
{
// Disconnect the pipe instance.
	QMessageBox::information(NULL,"toto","DisconnectAndClose");
   if (! DisconnectNamedPipe(lpPipeInst->hPipeInst) )
   {
      printf("DisconnectNamedPipe failed with %d.\n", GetLastError());
   }

// Close the handle to the pipe instance.

   CloseHandle(lpPipeInst->hPipeInst);

// Release the storage for the pipe instance.

   if (lpPipeInst != NULL)
      GlobalFree(lpPipeInst);
}

// CreateAndConnectInstance(LPOVERLAPPED)
// This function creates a pipe instance and connects to the client.
// It returns TRUE if the connect operation is pending, and FALSE if
// the connection has been completed.

BOOL CreateAndConnectInstance(LPOVERLAPPED lpoOverlap)
{
	QMessageBox::information(NULL,"toto","CreateAndConnectInstance");
	const char prefix[]="\\\\.\\pipe\\advanced-copier-";
	char uname[1024];
	char *m_pipename;
	DWORD len=1023;
	char *data;
	// false ??
	GetUserNameA(uname, &len);
	// convert into hexa
	data = toHex(uname);
	m_pipename = (char *) malloc(sizeof(prefix)+strlen(data)+2);
#if defined(_MFC_VER)
	strcpy_s(m_pipename, _countof(prefix) ,prefix);
	strcat_s(m_pipename, sizeof(prefix)+strlen(data)+2,data);
#else
	strcpy(m_pipename, prefix);
	strcat(m_pipename, data);
#endif
	free(data);


   //LPTSTR lpszPipename = m_pipename;

   hPipe = CreateNamedPipeA(
      m_pipename,             // pipe name
      PIPE_ACCESS_DUPLEX |      // read/write access
      FILE_FLAG_OVERLAPPED,     // overlapped mode
      PIPE_TYPE_MESSAGE |       // message-type pipe
      PIPE_READMODE_MESSAGE |   // message read mode
      PIPE_WAIT,                // blocking mode
      PIPE_UNLIMITED_INSTANCES, // unlimited instances
      BUFSIZE*sizeof(TCHAR),    // output buffer size
      BUFSIZE*sizeof(TCHAR),    // input buffer size
      PIPE_TIMEOUT,             // client time-out
      NULL);                    // default security attributes
   if (hPipe == INVALID_HANDLE_VALUE)
   {
      printf("CreateNamedPipe failed with %d.\n", GetLastError());
      return 0;
   }

// Call a subroutine to connect to the new client.

   return ConnectToNewClient(hPipe, lpoOverlap);
}

BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
{
	QMessageBox::information(NULL,"toto","ConnectToNewClient");
   BOOL fConnected, fPendingIO = FALSE;

// Start an overlapped connection for this pipe instance.
   fConnected = ConnectNamedPipe(hPipe, lpo);

// Overlapped ConnectNamedPipe should return zero.
   if (fConnected)
   {
      printf("ConnectNamedPipe failed with %d.\n", GetLastError());
      return 0;
   }

   switch (GetLastError())
   {
   // The overlapped connection in progress.
      case ERROR_IO_PENDING:
	 fPendingIO = TRUE;
	 break;

   // Client is already connected, so signal an event.

      case ERROR_PIPE_CONNECTED:
	 if (SetEvent(lpo->hEvent))
	    break;

   // If an error occurs during the connect operation...
      default:
      {
	 printf("ConnectNamedPipe failed with %d.\n", GetLastError());
	 return 0;
      }
   }
   return fPendingIO;
}

VOID GetAnswerToRequest(LPPIPEINST pipe)
{
   _tprintf( TEXT("[%d] %s\n"), pipe->hPipeInst, pipe->chRequest);
   lstrcpy(pipe->chReply, L"Default answer from server");//, BUFSIZE
   pipe->cbToWrite = (lstrlen(pipe->chReply)+1)*sizeof(TCHAR);
}

// Dump UTF16 (little endian)
char * toHex(const char *str)
{
	char *p, *sz;
	size_t len;
	if (str==NULL)
		return NULL;
	len= strlen(str);
	p = sz = (char *) malloc((len+1)*4);
	// username goes hexa...
	for (size_t i=0; i<len; i++)
	{
		#if defined(_MFC_VER)
			sprintf_s(p, (len+1)*4, "%.2x00", str[i]);
		#else
			sprintf(p, "%.2x00", str[i]);
		#endif
		p+=4;
	}
	*p=0;
	return sz;
}
