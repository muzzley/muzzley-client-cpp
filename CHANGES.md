# Version 0.0.2

- Throwing **muzzley::AssertionException** when sending signaling message with protocol version 2.0 (after initApp/initUser)
- Throwing **muzzley::AssertionException** when sending pub/sub message with protocol version 1.2 (after connectApp/connectUser)
- Added **muzzley::Message** and **muzzley::Subscription** classes, for protocol abstraction
- Implemented the Pub/Sub communication pattern support
- Corrected a memory leak with class muzzley::JSONObj
- Changed muzzley::JSONObj hierarchy, now extends from *std::share_ptr*
