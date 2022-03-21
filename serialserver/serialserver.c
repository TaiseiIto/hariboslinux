#include <stdio.h>
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCommandLIne, int nCommandShow)
{
	LPCSTR csTestMessage = "Hello, World!\n";
	FILE *fReopenedStandardError;
	FILE *fReopenedStandardInput;
	FILE *fReopenedStandardOutput;
	HANDLE hStandardError;
	HANDLE hStandardInput;
	HANDLE hStandardOutput;
	AllocConsole();
	hStandardError = GetStdHandle(STD_ERROR_HANDLE);
	hStandardInput = GetStdHandle(STD_INPUT_HANDLE);
	hStandardOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	freopen_s(&fReopenedStandardError, "CONOUT$", "w", stderr);
	freopen_s(&fReopenedStandardInput, "CONIN$", "r", stdin);
	freopen_s(&fReopenedStandardOutput, "CONOUT$", "w", stdout);
	WriteConsole(hStandardOutput, csTestMessage, lstrlen(csTestMessage), NULL, NULL);
	Sleep(10000);
	fclose(fReopenedStandardError);
	fclose(fReopenedStandardInput);
	fclose(fReopenedStandardOutput);
	CloseHandle(hStandardError);
	CloseHandle(hStandardInput);
	CloseHandle(hStandardOutput);
	FreeConsole();
	return EXIT_SUCCESS;
}

