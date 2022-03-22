#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

char GetInput(HANDLE hStandardInput);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCommandLIne, int nCommandShow)
{
	COMMTIMEOUTS timeout;
	FILE *fReopenedStandardError;
	FILE *fReopenedStandardInput;
	FILE *fReopenedStandardOutput;
	HANDLE hStandardError;
	HANDLE hStandardInput;
	HANDLE hStandardOutput;
	TCHAR sBuffer[0x400];
	DWORD dwStringLength;
	HANDLE hNamedPipe;
	AllocConsole();
	hStandardError = GetStdHandle(STD_ERROR_HANDLE);
	hStandardInput = GetStdHandle(STD_INPUT_HANDLE);
	hStandardOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	freopen_s(&fReopenedStandardError, "CONOUT$", "w", stderr);
	freopen_s(&fReopenedStandardInput, "CONIN$", "r", stdin);
	freopen_s(&fReopenedStandardOutput, "CONOUT$", "w", stdout);
	// Create a named pipe.
	hNamedPipe = CreateNamedPipeA
	(
		"\\\\.\\pipe\\haribos",//lpName
		PIPE_ACCESS_DUPLEX,//dwOpenMode
		PIPE_TYPE_BYTE | PIPE_WAIT,//dwPipeMode
		1,//nMaxInstances
		0x400,//nOutBufferSize
		0x400,//nInBufferSize
		NMPWAIT_USE_DEFAULT_WAIT,//nDefaultTimeOut
		NULL//lpSecurityAttributes
	);
	if(hNamedPipe != INVALID_HANDLE_VALUE)
	{
		LPCSTR message = "succeeded in creating a named pipe\n";
		WriteConsole(hStandardError, message, lstrlen(message), NULL, NULL);
	}
	else
	{
		LPCSTR message = "failed to create a named pipe\n";
		WriteConsole(hStandardError, message, lstrlen(message), NULL, NULL);
	}
	// Wait for client.
	if(ConnectNamedPipe(hNamedPipe, NULL))
	{
		LPCSTR message = "succeeded in connecting to a virtual machine\n";
		WriteConsole(hStandardError, message, lstrlen(message), NULL, NULL);
	}
	else
	{
		LPCSTR message = "failed to connect to a virtual machine\n";
		WriteConsole(hStandardError, message, lstrlen(message), NULL, NULL);
	}
	// Set timeout.
	timeout.ReadIntervalTimeout = 1;
	timeout.ReadTotalTimeoutMultiplier = 1;
	timeout.ReadTotalTimeoutConstant = 1;
	timeout.WriteTotalTimeoutMultiplier = 1;
	timeout.WriteTotalTimeoutConstant = 1;
	SetCommTimeouts(hNamedPipe, &timeout);
	timeout.ReadIntervalTimeout = 1;
	timeout.ReadTotalTimeoutMultiplier = 1;
	timeout.ReadTotalTimeoutConstant = 1;
	timeout.WriteTotalTimeoutMultiplier = 1;
	timeout.WriteTotalTimeoutConstant = 1;
	SetCommTimeouts(hStandardInput, &timeout);
	// Receive from the named pipe and output string.
	while(ReadFile(hNamedPipe, sBuffer, sizeof(sBuffer) / sizeof(sBuffer[0]), &dwStringLength, NULL))
	{
		char input;
		if(dwStringLength)WriteConsole(hStandardOutput, sBuffer, dwStringLength, NULL, NULL);
		memset(sBuffer, '\0', sizeof(sBuffer) / sizeof(sBuffer[0]));
		input = GetInput(hStandardInput);
		if(input)WriteFile(hNamedPipe, &input, 1, NULL, NULL);
	}
	DisconnectNamedPipe(hNamedPipe);
	CloseHandle(hNamedPipe);
	fclose(fReopenedStandardError);
	fclose(fReopenedStandardInput);
	fclose(fReopenedStandardOutput);
	CloseHandle(hStandardError);
	CloseHandle(hStandardInput);
	CloseHandle(hStandardOutput);
	FreeConsole();
	return EXIT_SUCCESS;
}

char GetInput(HANDLE hStandardInput)
{
	INPUT_RECORD input;
	DWORD length;
	if(WSAWaitForMultipleEvents(1, &hStandardInput, FALSE, 1, TRUE) != WSA_WAIT_EVENT_0)return '\0';
	if(!ReadConsoleInput(hStandardInput, &input, 1, &length))return '\0';
	if(input.EventType != KEY_EVENT)return '\0';
	if(!input.Event.KeyEvent.bKeyDown)return '\0';
	return input.Event.KeyEvent.uChar.AsciiChar;
}

