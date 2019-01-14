#include "ijson2_parser.hh"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>


int main() {
	FILE *fp = fopen("performance_test_input.json", "r");
	if(!fp) {
		perror("performance_test_input.json");
		return 1;
	}
	fseek(fp,0,SEEK_END);
	long bytes = ftell(fp);
	fseek(fp,0,SEEK_SET);
	
	char *buf = new char[bytes];
	fread(buf, 1, bytes, fp);
	fclose(fp);
	
	rusage ru_start;
	getrusage(RUSAGE_SELF,&ru_start);
	
	for(int i=0; i<1000; i++) {
		ijson2::Parser parser;
		parser.parse(buf, bytes);
	}
	
	rusage ru_end;
	getrusage(RUSAGE_SELF,&ru_end);
	
	double utime = ru_end.ru_utime.tv_sec - ru_start.ru_utime.tv_sec
	             + (ru_end.ru_utime.tv_usec - ru_start.ru_utime.tv_usec)/1000000.0;
	double stime = ru_end.ru_stime.tv_sec - ru_start.ru_stime.tv_sec
	             + (ru_end.ru_stime.tv_usec - ru_start.ru_stime.tv_usec)/1000000.0;
	printf("utime: %.3f\n", utime);
	printf("stime: %.3f\n", stime);

	delete[] buf;
	
	return 0;
}
