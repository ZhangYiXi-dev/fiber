fiber:main.cpp common.cpp
	g++ -rdynamic main.cpp common.cpp fiber.cpp log.cpp zyx_thread.cpp -I ./include -lpthread -o fiber 