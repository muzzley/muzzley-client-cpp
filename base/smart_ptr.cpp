
#include <base/smart_ptr.h>

namespace muzzley {
	pthread_key_t __memory_key;
}

muzzley::smart_counter::smart_counter() : __pointed(0) {
}

muzzley::smart_counter::~smart_counter() {
}

void muzzley::smart_counter::add() {
	this->__pointed++;
}

size_t muzzley::smart_counter::release() {
	if (this->__pointed != 0) {
		this->__pointed--;
	}
	return this->__pointed;
}

muzzley::smart_ref_table::smart_ref_table() : str_map(HASH_SIZE) {
}

muzzley::smart_ref_table::~smart_ref_table() {
}

muzzley::smart_counter* muzzley::smart_ref_table::add(void* ptr) {
	ostringstream oss;
	oss << ptr << flush;
	str_map<smart_counter*>::iterator found = this->find(oss.str());
	if (found != this->end()) {
		(*found)->second->add();
		return (*found)->second;
	}
	else {
		smart_counter* sc = new smart_counter();
		this->insert(pair<string, smart_counter*>(oss.str(), sc));
		sc->add();
		return sc;
	}
}

size_t muzzley::smart_ref_table::release(void* ptr) {
	ostringstream oss;
	oss << ptr << flush;
	str_map<smart_counter*>::iterator found = this->find(oss.str());
	if (found != this->end()) {
		return (*found)->second->release();
	}

	return 0;
}

void muzzley::smart_ref_table::remove(void* ptr) {
	ostringstream oss;
	oss << ptr << flush;
	str_map<smart_counter*>::iterator found = this->find(oss.str());
	if (found != this->end()) {
		this->erase(found);
	}
}
