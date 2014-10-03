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
	//
	// Don't bother to store the returned activityId, the Client class already does that,
	// access it through the 'getActivityId' method.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::ActivityCreated, [] (muzzley::JSONObj& _data, muzzley::Client& _client) -> bool {
		cout << "Activity created with id " << _data["d"]["activityId"] << endl << flush;
		cout << "You can now pair your smartphone, using the muzzley app, to the activity id " << _data["d"]["activityId"] << endl << flush;
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

		if (_client.isReplyNeeded(_data)) {
			_client.reply(_data, JSON(
				"s" << true <<
				"m" << "this is a testing signal so is always successful!" <<
				"d" << JSON(
					"w" << "400" <<
					"h" << "300"
					)
				));
			cout << "great, replied to a Signal Message" << endl << flush;
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