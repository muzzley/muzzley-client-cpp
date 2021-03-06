/*
Copyright (c) 2014, Muzzley

Permission to use, copy, modify, and/or distribute this software for 
any purpose with or without fee is hereby granted, provided that the 
above copyright notice and this permission notice appear in all 
copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE 
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR 
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER 
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
PERFORMANCE OF THIS SOFTWARE.
*/


#pragma once

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

namespace muzzley {
	extern short int log_lvl;
	extern bool log_color;
	extern ostream* log_fd;
	extern const char* log_lvl_colored_names[];
	extern const char* log_lvl_names[];

	enum LogLevel {
		emergency = 0,
		alert = 1,
		critical = 2,
		error = 3,
		warning = 4,
		notice = 5,
		info = 6,
		debug = 7
	};

	void log(string __prefix, string __text, muzzley::LogLevel _level);
	void log(string __text, muzzley::LogLevel _level);
	void process_mem_usage(double& vm_usage, double& resident_set);
	void log_mem_usage();
	
}

