all:
	make -C liblog
	cp -r liblog/liblog.a  echo-server/liblog.a
	cp -r liblog/liblog.a  tuling-server/liblog.a

	make -C libtulingApi
	cp -r libtulingApi/libtulingApi.a  tuling-server/libtulingApi.a
	make -C tuling-server
	make -C echo-server

clean:
	make clean -C echo-server
	make clean -C tuling-server
	make clean -C liblog
	rm -rf  echo-server/liblog.a
	rm -rf  tuling-server/liblog.a

	rm -rf  tuling-server/libtulingApi.a


