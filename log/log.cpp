
#include <log/log.h>

#include <text/convert.h>

namespace muzzley {
	short int log_lvl = 5;
	ostream* log_fd = NULL;

	const char* log_lvl_names[] = {
		"\033[1;34m\033[4;35msys\033[0m    | ",
		"\033[1;31m\033[4;31merror\033[0m  | ",
		"\033[1;33m\033[4;33mwarning\033[0m| ",
		"\033[1;32m\033[4;34minfo\033[0m   | ",
		"\033[1;35m\033[4;36mdebug\033[0m  | "
	};
}

void muzzley::log(string _text, muzzley::LogLevel _level) {
	if (_level <= muzzley::log_lvl) {
		string _time;
		muzzley::tostr(_time, time(NULL), "%F %T");
		muzzley::replace(_text, "\n", "\n       | ");
		(*muzzley::log_fd) << muzzley::log_lvl_names[_level] << "\033[1;37m" << _time << "\033[0m | " << _text << endl << flush;
	}
}
