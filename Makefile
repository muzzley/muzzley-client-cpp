package_version=0.1
build_configuration=Release

all: build_$(build_configuration)

build_Debug:  
	make -C Debug clean
	make -C Debug all

build_Release:  
	make -C Release clean
	make -C Release all

install:
	mkdir -p tmp/usr/include/muzzley
	cp base exceptions json log parsers resource stream text muzzley.h tmp/usr/include/muzzley -rf
	find tmp/usr/include/muzzley -name "*.cpp" | xargs rm -rf
	cp -rf tmp/usr/include/muzzley /usr/include/
	cp $(build_configuration)/libmuzzley.so.$(package_version) /usr/lib/
	ln -s /usr/lib/libmuzzley.so.$(package_version) /usr/lib/libmuzzley.so
	rm -rf tmp/

package:
	rm -rf Upstream/muzzley-sdk-cpp_$(package_version).tar.gz
	mkdir -p muzzley-sdk-cpp_$(package_version)/
	cp base exceptions json log parsers resource stream text Debug Release Makefile muzzley.h muzzley-sdk-cpp_$(package_version)/ -rf
	find muzzley-sdk-cpp_$(package_version)/ -name "*.o" | xargs rm -rf
	find muzzley-sdk-cpp_$(package_version)/ -name "*.d" | xargs rm -rf
	find muzzley-sdk-cpp_$(package_version)/ -name "*.so" | xargs rm -rf
	find muzzley-sdk-cpp_$(package_version)/ -name "*.so.$(package_version)" | xargs rm -rf
	tar cvzf Upstream/muzzley-sdk-cpp_$(package_version).tar.gz muzzley-sdk-cpp_$(package_version)
	rm -rf muzzley-sdk-cpp_$(package_version)/

package:
	rm -rf Upstream/muzzley-sdk-cpp_$(package_version).tar.gz
	mkdir -p muzzley-sdk-cpp_$(package_version)/
	cp base exceptions json log parsers resource stream text Debug Release Makefile README.md muzzley.h muzzley-sdk-cpp_$(package_version)/ -rf
	find muzzley-sdk-cpp_$(package_version)/ -name "*.o" | xargs rm -rf
	find muzzley-sdk-cpp_$(package_version)/ -name "*.d" | xargs rm -rf
	find muzzley-sdk-cpp_$(package_version)/ -name "*.so" | xargs rm -rf
	find muzzley-sdk-cpp_$(package_version)/ -name "*.so.$(package_version)" | xargs rm -rf
	tar cvzf Upstream/muzzley-sdk-cpp_$(package_version).tar.gz muzzley-sdk-cpp_$(package_version)
	rm -rf muzzley-sdk-cpp_$(package_version)/
