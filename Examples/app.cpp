#define DEBUG 1

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <muzzley.h>

using namespace std;
using namespace __gnu_cxx;

int main(int argc, char* argv[]) {

	// Create the Muzzley client
	muzzley::Client _client;

	// Register listener to be invoked when app succesfully logs in.
	//
	// Don't bother to store the returned deviceId or sessionId, the Client class already does that,
	// access it through the 'getDeviceId'  and 'getSessionId' method.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::AppLoggedIn, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		_client.createActivity();
		return true;
	});

	// Register listener to be invoked when activity is sucessfully created.
	//
	// Don't bother to store the returned activityId, the Client class already does that,
	// access it through the 'getActivityId' method.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::ActivityCreated, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		if (!!_data["s"] && ((string) _data["d"]["activityId"]) != "121345") {

			muzzley::JSONObj _s_data;
			_s_data <<
				"w"<< "gamepad" <<
				"c"<< "ba" <<
				"v"<< "a" <<
				"e"<< "press";

			_client.sendSignal("changeWidget", _s_data, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
				cout << "signal accepted by server" << endl << flush;
			});
		}
		return true;
	});

	// Register listener to be invoked when app receives a signal message.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::SignalingMessage, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {

		// Switch on the Intel(R) Galileo LED 13
		// (pure c++ version)
		ofstream _export;
		_export.open("/sys/class/gpio/export", ios::out | ios::trunc);
		ofstream _direction;
		_direction.open("/sys/class/gpio/gpio3/direction", ios::out | ios::trunc);
		ofstream _value;
		_value.open("/sys/class/gpio/gpio3/value", ios::out | ios::trunc);

		_export << "3" << flush;
		_export.close();

		_direction << "out" << flush;
		_direction.close();

		_value << "1" << flush;
		_value.close();

		// Switch on the Intel(R) Galileo LED 13
		// (syscall version)
		if (system("echo 3 > /sys/class/gpio/export") == -1) {
			cout << "error" << endl << flush;
		}
		if (system("echo out > /sys/class/gpio/gpio3/direction") == -1) {
			cout << "error" << endl << flush;
		}
		if (system("echo 1 > /sys/class/gpio/gpio3/value") == -1) {
			cout << "error" << endl << flush;
		}

		return true;
	});

	// Connects the application to the Muzzley server.
	//
	// It will start the application loop synchronously,
	// i.e. the program will not execute anything below this line.
	_client.connectApp("c1bc8d7d6bc38873");

	return 0;
}
