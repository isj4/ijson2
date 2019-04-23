#include "ijson2_direct_formatter.hh"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

int main(void) {
	printf("Running...\n");
	rusage ru_start;
	getrusage(RUSAGE_SELF,&ru_start);
	
	std::string s;
	auto append = [](const char *src, size_t srcsize, void *append_context) {
		*reinterpret_cast<std::string*>(append_context) += std::string(src,srcsize);
	};
	
	for(int i=0; i<1000; i++) {
		s.clear();
		ijson2::DirectFormatter df(append,&s);
		
		df.open_object();
		df.begin_object_member("foo");
		df.open_array();
		for(int j=0; j<10000; j++) {
			if(j!=0) df.append_array_member_separator();
			df.append_null();
			df.append_array_member_separator();
			df.append_string("boo");
			df.append_array_member_separator();
			df.append_number(17.0);
			df.append_array_member_separator();
			df.append_number(117);
			df.append_array_member_separator();
			df.open_object();
			df.close_object();
			df.open_array();
		}
		df.close_array();
		df.close_object();
		
		df.flush();
	}
	
	rusage ru_end;
	getrusage(RUSAGE_SELF,&ru_end);
	
	double utime = ru_end.ru_utime.tv_sec - ru_start.ru_utime.tv_sec
	             + (ru_end.ru_utime.tv_usec - ru_start.ru_utime.tv_usec)/1000000.0;
	double stime = ru_end.ru_stime.tv_sec - ru_start.ru_stime.tv_sec
	             + (ru_end.ru_stime.tv_usec - ru_start.ru_stime.tv_usec)/1000000.0;
	printf("utime: %.3f\n", utime);
	printf("stime: %.3f\n", stime);

	return 0;
}
