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
// g++ -std=c++0x -I/usr/include/muzzley examples/user.cpp -o myuser -lmuzzley -lpthread
//

//
// Usage instructions:
// ./myuser <activity id to connect to>
//

#define DEBUG 1

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <muzzley/muzzley.h>

using namespace std;
#if !defined __APPLE__
using namespace __gnu_cxx;
#endif

int main(int argc, char* argv[]) {

	muzzley::Client _client;

	// Register listener to be invoked when user succesfully joins the activity.
	//
	// Return 'false' if you want to stop other listeners from being invoked.
	_client.on(muzzley::ActivityJoined, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
		cout << "User joined activity. Assigned device Id: " << _client.getDeviceId() << endl << flush;
		// _client.sendWidgetData("xpto", "a", "some", "value");

		muzzley::Subscription _s1;
		_s1.setNamespace("iot");
		_s1.setProfile("cb17073b-9428-4f55-9a22-26a008c0bf4e");
		_s1.setChannel("2183hroqw89");
		_s1.setComponent("bulb");
		_s1.setProperty("intensity");

		muzzley::Message _m1;
		_m1.setData(JSON(
			"io" << "w" <<
			"data" << JSON(
				"value" << 125 << 
				"unit" << "lm"
			)
		));
		_client.trigger(muzzley::Publish, _s1, _m1);

		muzzley::Message _m2;
		_m2.setData(JSON(
			"io" << "r"
		));
		_client.trigger(muzzley::Publish, _s1, _m2, [] (muzzley::Message& _data, muzzley::Client& _client) -> bool {
			_data->prettify(cout);
			cout << endl << flush;			
			return true;
		}); 
		return true;
	});

	_client.setEndpointHost("platform.office.muzzley.com");	

	// Connects the application to the Muzzley server, using the one-step-initialization process.
	//
	// Assuming that the program receives the activityId as argument.
	//
	// It will start the application loop synchronously,
	// i.e. the program will not execute anything below this line.
	_client.initUser("guest", string(argv[1]));

	return 0;
}
