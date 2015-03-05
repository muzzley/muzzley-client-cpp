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


#include <muzzley/log/log.h>

#include <time.h>
#include <muzzley/text/convert.h>

namespace muzzley {
	short int log_lvl = 5;
	ostream* log_fd = NULL;

	const char* log_lvl_names[] = {
		"\033[1;35m\033[4;35msys\033[0m     | ",
		"\033[1;31m\033[4;31merror\033[0m   | ",
		"\033[1;33m\033[4;33mwarning\033[0m | ",
		"\033[1;34m\033[4;34minfo\033[0m    | ",
		"\033[1;36m\033[4;36mdebug\033[0m   | "
	};
}

void muzzley::log(string _prefix, string _text, muzzley::LogLevel _level) {
	if (_level <= muzzley::log_lvl) {
		string _time;
		muzzley::tostr(_time, time(NULL), "%F %T");
		muzzley::replace(_text, "\n", "");
		(*muzzley::log_fd) << muzzley::log_lvl_names[_level] << "\033[0;37m" << _time << "\033[0m | \033[4;32m" << _prefix << "\033[0m | " << _text << endl << flush;
	}
}

void muzzley::log(string _text, muzzley::LogLevel _level) {
	if (_level <= muzzley::log_lvl) {
		string _time;
		muzzley::tostr(_time, time(NULL), "%F %T");
		muzzley::replace(_text, "\n", "");
		(*muzzley::log_fd) << muzzley::log_lvl_names[_level] << "\033[0;37m" << _time << "\033[0m | " << _text << endl << flush;
	}
}

void muzzley::process_mem_usage(double& vm_usage, double& resident_set) {
	using std::ios_base;
	using std::ifstream;
	using std::string;

	vm_usage = 0.0;
	resident_set = 0.0;

	ifstream stat_stream("/proc/self/stat", ios_base::in);

	string pid, comm, state, ppid, pgrp, session, tty_nr;
	string tpgid, flags, minflt, cminflt, majflt, cmajflt;
	string utime, stime, cutime, cstime, priority, nice;
	string O, itrealvalue, starttime;

	unsigned long vsize;
	long rss;

	stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
			>> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
			>> utime >> stime >> cutime >> cstime >> priority >> nice
			>> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

	stat_stream.close();

	long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
	vm_usage = vsize / 1024.0;
	resident_set = rss * page_size_kb;
}

void muzzley::log_mem_usage() {
	double _vm;
	double _resident;
	muzzley::process_mem_usage(_vm, _resident);
	string _text("virtual_memory: ");
	muzzley::tostr(_text, _vm);
	_text.insert(_text.length(), "kb | resident_memory: ");
	muzzley::tostr(_text, _resident);
	_text.insert(_text.length(), "kb");
	muzzley::log(_text, muzzley::debug);
}
