fiber:main.cpp common.cpp
	g++ -rdynamic main.cpp common.cpp fiber.cpp -I ./include -o fiber