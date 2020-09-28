all:
	make -C liblog
	cp -r liblog/liblog.a  tuling-server/liblog.a
	cp -r liblog/liblog.a  yanshee-server/liblog.a
	cp -r liblog/liblog.a  echo-server/liblog.a

	make -C libtaskList
	cp -r libtaskList/libtaskList.a  tuling-server/libtaskList.a
	cp -r libtaskList/libtaskList.a  yanshee-server/libtaskList.a
	cp -r libtaskList/libtaskList.a  echo-server/libtaskList.a

	make -C libtulingApi
	cp libtulingApi/libtulingApi.a tuling-server/libtulingApi.a -r
	make -C tuling-server
	make -C yanshee-server
	make -C echo-server

clean:
	make clean -C tuling-server
	make clean -C yanshee-server
	make clean -C echo-server
	make clean -C liblog
	rm -rf tuling-server/liblog.a yanshee-server/liblog.a echo-server/liblog.a
	rm -rf tuling-server/libtulingApi.a
