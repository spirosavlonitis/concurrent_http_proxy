default:
	gcc *.c -o http_proxy

run: default
	./http_proxy
