// header files
#include "main.h"
#include "logger.h"

// globals
FILE* logFile;

// functions
void InitLogger(void)
{
	// open file for logging
	if (fopen_s(&logFile, "Sandbox.log", "w") != 0)
	{
		MessageBox(NULL, TEXT("Cannot open Sandbox.log file.."), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	LogD("==== Application Started ====")
}

void UninitLogger(void)
{
	if (logFile)
	{
		LogD("\n==== Application Terminated ====");
		fclose(logFile);
		logFile = NULL;
	}
}

