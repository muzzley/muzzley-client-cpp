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

//
// You can compile this application with the following command:
// g++ -std=c++0x -I/usr/include/muzzley Examples/galileo_app.cpp -o myapp -lmuzzley -lpthread
//
// And run it like this:
// ./myapp
// 
// Once running, you can pair your smartphone to this app using
// the mobile muzzley application and the "Activity Id" that is
// written to the screen.
//

#define DEBUG 1

#define APP_TOKEN "3ddde2aa9832a038" // Get yours at muzzley.com
#define STATIC_ACTIVITY_ID ""        // Optional.

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <muzzley.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

int main(int argc, char* argv[]) {

	// Create the Muzzley client
	muzzley::Client _client;

	// Register listener to be invoked when activity is sucessfully created.
	_client.on(muzzley::ActivityCreated, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		cout << "Activity created with id " << _data["d"]["activityId"] << endl << flush;
		cout << "You can now pair your smartphone, using the muzzley app, to the activity id " << _data["d"]["activityId"] << endl << flush;
		return true;
	});

	_client.on(muzzley::ParticipantJoined, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		long participant_id = _data["d"]["participant"]["id"];
		string name = _data["d"]["participant"]["name"];
		cout << "Participant " << name << "(" << participant_id << ")" << " joined." << endl << flush;

		_client.changeWidget(participant_id, "switch", [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
			cout << "Widget successfully changed" << endl << flush;
		});
		return true;
	});

	_client.on(muzzley::WidgetAction, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		int _switch_value = (int) _data["d"]["v"];
		cout << "User pressed the switch to " << (_switch_value ? "1 (On)" : "0 (Off)") << endl << flush;

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

		_value << to_string(_switch_value) << flush;
		_value.close();

		// Switch on the Intel(R) Galileo LED 13
		// (syscall version)
		if (system("echo 3 > /sys/class/gpio/export") == -1) {
			cout << "error" << endl << flush;
		}
		if (system("echo out > /sys/class/gpio/gpio3/direction") == -1) {
			cout << "error" << endl << flush;
		}
		if (system( ("echo " + to_string(_switch_value) + " > /sys/class/gpio/gpio3/value").c_str() ) == -1) {
			cout << "error" << endl << flush;
		}

		return true;
	});

	// Connects the application to the Muzzley server.
	//
	// It will start the application loop synchronously,
	// i.e. the program will not execute anything below this line.
	_client.connectApp(APP_TOKEN, STATIC_ACTIVITY_ID);

	return 0;
}