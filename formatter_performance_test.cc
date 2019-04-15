#include "ijson2_formatter.hh"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

int main(void) {
	//build a large complex object
	printf("Building object\n");
	ijson2::Value o0(ijson2::Value::map_type{});
	o0.object().emplace("foo",ijson2::Value::array_type{});
	auto &a0 = o0.object().at("foo").array();
	for(int i=0; i<10000; i++) {
		a0.emplace_back(nullptr);
		a0.emplace_back("boo");
		a0.emplace_back(17.42);
		a0.emplace_back(117U);
		a0.emplace_back(ijson2::Value::map_type{});
		a0.emplace_back(ijson2::Value::array_type{});
	}
	
	printf("Running...\n");
	rusage ru_start;
	getrusage(RUSAGE_SELF,&ru_start);
	
	std::string s;
	auto append = [](const char *src, size_t srcsize, void *append_context) {
		*reinterpret_cast<std::string*>(append_context) += std::string(src,srcsize);
	};
	
	for(int i=0; i<1000; i++) {
		s.clear();
		ijson2::format(o0, append,&s);
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
