#include <stdio.h>
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCommandLIne, int nCommandShow)
{
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
	// Receive from the named pipe and output string
	while(ReadFile(hNamedPipe, sBuffer, sizeof(sBuffer) / sizeof(sBuffer[0]), &dwStringLength, NULL))if(dwStringLength)WriteConsole(hStandardOutput, sBuffer, dwStringLength, NULL, NULL);
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

