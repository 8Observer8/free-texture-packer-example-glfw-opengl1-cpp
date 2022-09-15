# build command: mingw32-make
# -mwindows - hide the console

INC = -I"E:\Libs\glfw-3.3.8-mingw-64bit\include" \
	  -I"E:\Libs\stb_image-2.27\include" \
	  -I"E:\Libs\rapidjson-master\include"

LIB = -L"E:\Libs\glfw-3.3.8-mingw-64bit\lib"

all: main.o
	g++ main.o $(LIB) -lglfw3 -lgdi32 -lopengl32 -o app.exe

main.o: main.cpp
	g++ -c $(INC) main.cpp
