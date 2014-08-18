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

	muzzley::Client _client;

	// Register listener to be invoked when user succesfully joins the activity.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::ActivityJoined, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		cout << "User joined activity. Assigned device Id: " << _client.getDeviceId() << endl << flush;
		// _client.sendWidgetData("xpto", "a", "some", "value");
		return true;
	});

	// Register listener to be invoked when the app requests
	// the user to change to a certain widget.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::ChangeWidget, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		string widgetobj_str;
		muzzley::tostr(widgetobj_str, _data["d"]["d"]);
		cout << "User was requested to change to a widget. Object: " << widgetobj_str << endl << flush;

		if ((string) _data["d"]["d"]["widget"] == "gamepad") {
			// Assuming we changed to a gamepad, send a "button b pressed" event
			_client.sendWidgetData("gamepad", "ba", "press", "a");
		}
		return true;
	});

	// Register listener to be invoked when app receives a signal message.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::SignalingMessage, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		cout << "Received a signaling message:" << endl << flush;
		string signalobj_str;
		muzzley::tostr(signalobj_str, _data["d"]);
		cout << signalobj_str << endl << flush;
		return true;
	});

	// Register listener to be invoked when the activity we've joined terminates
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::ActivityTerminated, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		cout << "Activity Terminated. We either try to reconnect or exit." << endl << flush;
		return true;
	});

	// Connects the application to the Muzzley server.
	//
	// Assuming that the program receives the activityId as argument.
	//
	// It will start the application loop synchronously,
	// i.e. the program will not execute anything below this line.
	_client.connectUser("guest", string(argv[1]));

	return 0;
}
