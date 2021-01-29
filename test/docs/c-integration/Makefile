all:
	skull hello.sk -- hello.c -no-pie
	./hello
	skull hello2.sk -c
	cc .hello2.sk.o hello2.c -no-pie -o hello2
	./hello2