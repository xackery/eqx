#include "log_stdout.h"
#include "log_types.h"
#include <stdio.h>
#include <time.h>

void EQEmu::Log::LogStdOut::OnMessage(LogType log_type, const std::string &message) {
	char time_buffer[512];
	time_t current_time;
	struct tm *time_info;
	
	time(&current_time);
	time_info = localtime(&current_time);
	
	//strftime(time_buffer, 512, "[%y/%m/%y %H:%M:%S] ", time_info);
	switch(log_type) {
	case LogTrace:
		fprintf(stdout, "[TRC] %s\n", message.c_str());
		break;
	case LogDebug:
		fprintf(stdout, "[DBG] %s\n", message.c_str());
		break;
	case LogInfo:
		fprintf(stdout, "[INF] %s\n", message.c_str());
		break;
	case LogWarn:
		fprintf(stdout, "[WRN] %s\n", message.c_str());
		break;
	case LogError:
		fprintf(stdout, "[ERR] %s\n", message.c_str());
		break;
	case LogFatal:
		fprintf(stdout, "[FTL] %s\n", message.c_str());
		break;
	default:
		fprintf(stdout, "[ALL] %s\n", message.c_str());
		break;
	}
}
