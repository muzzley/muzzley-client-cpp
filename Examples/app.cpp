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
		cout << "Activity created with id " << _data["d"]["activityId"] << endl << flush;
		return true;
	});

	_client.on(muzzley::ParticipantJoined, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {

		// string widget = "gamepad";
		string widget = "webview";
		long participant_id = _data["d"]["participant"]["id"];
		string name = _data["d"]["participant"]["name"];

		cout << "Participant " << name << "(" << participant_id << ")" << " joined." << endl << flush;
		cout << "Chaning widget to a... " << widget << endl << flush;

		string participant_string;
		muzzley::tostr(participant_string, _data);
		cout << "Participant string: " << participant_string << endl << flush;

		// Change widget to the native gamepad
		// _client.changeWidget(participant_id, widget, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		// 	cout << "Widget successfully changed" << endl << flush;
		// });

		// Change widget to a custom WebView widget
		muzzley::JSONObj _widget_opts;
		_widget_opts <<
		  "widget" << widget <<
		  "params" << JSON (
				"uuid" << "58b54ae9-c94d-4c71-a4f1-6412b2376b1d" <<
				"orientation" << "portrait"
	  	);
		_client.changeWidget(participant_id, _widget_opts, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
			cout << "Widget successfully changed" << endl << flush;
		});
		return true;
	});

	_client.on(muzzley::WidgetAction, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		string widget_data;
		muzzley::tostr(widget_data, _data["d"]);
		cout << "Native Widget action event fired: " << widget_data << endl << flush;
		return true;
	});

	// Register listener to be invoked when app receives a signal message.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::SignalingMessage, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {

		string action;
		action.assign((string) _data["d"]["a"]);

		cout << "Received signaling message for action " << action << endl << flush;
		cout << "with data " << _data["d"]["d"] << endl << flush;
		cout << "and isReplyNeeded " << _client.isReplyNeeded(_data) << endl << flush;

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