#pragma once

extern FILE* logFile;
#define LogD(...)   fprintf(logFile, __VA_ARGS__);fprintf(logFile, "\n");
#define LogE(...)   fprintf(logFile, "ERROR: ");fprintf(logFile, __VA_ARGS__);fprintf(logFile, "\n");

void InitLogger(void);
void UninitLogger(void);
