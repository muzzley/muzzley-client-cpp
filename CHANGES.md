# Version 0.0.2

- Added **muzzley::Message** and **muzzley::Subscription** classes, for protocol abstraction
- Implemented the Pub/Sub communication pattern support
- Corrected a memory leak with class muzzley::JSONObj
- Changed muzzley::JSONObj hierarchy, now extends from *std::share_ptr*
