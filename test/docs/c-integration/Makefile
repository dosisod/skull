all:
	skull hello.sk -- hello.c -no-pie
	./hello
	skull hello2.sk -m _skull_main -- hello2.c -no-pie -o hello2
	./hello2