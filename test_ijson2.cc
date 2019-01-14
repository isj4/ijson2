#include "ijson2_parser.hh"
#include <stdio.h>
#include <string>
#include <string.h>

// This is a test driver for use with JSONTestSuite
// https://github.com/nst/JSONTestSuite

int main(int argc, char **argv) {
	if(argc!=2) {
		fprintf(stderr,"usage: <filename>\n");
		return -1;
	}
	
	FILE *fp = fopen(argv[1],"r");
	if(!fp) {
		perror(argv[1]);
		return 2;
	}
	
	std::string data;
	
	for(;;) {
		char buf[4096];
		size_t b = fread(buf,1,sizeof(buf),fp);
		data += std::string(buf,b);
		if(b<sizeof(buf))
			break;
	}
	
	ijson2::Parser p;
	try {
		p.parse(data.data(), data.size());
	} catch(...) {
		return 1;
	}
	
	return 0;
}
