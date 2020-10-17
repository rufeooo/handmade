#include <stdbool.h>

#include <windows.h>

#define NORMAL_PRIORITY 0x20

int main(int argc, char** argv, char** env)
{
	int i = 0;
	while (env[i]) {
		puts(env[i]);
		i += 1;
	}
	puts("--------");
	STARTUPINFOA start_info = {0};
	start_info.cb = sizeof(start_info);
	PROCESS_INFORMATION proc_info = {0};

	if (!CreateProcessA(NULL, "cl", NULL, NULL, false, NORMAL_PRIORITY, NULL, NULL, &start_info, &proc_info)) 
	{
		puts("failed to start process");
	}
	DWORD ret = WaitForSingleObject(proc_info.hProcess, INFINITE);
	printf("Exit Code (%ERRORLEVEL%): %lu\n", ret);
	DWORD err = GetLastError();
	printf("GetLastError() %lu\n", err);
	return 0;
}
