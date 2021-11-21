PHONY: mygl
mygl:
	g++ -Iincludes src/mygl.cpp src/Shader.cpp src/glad.c -g -ldl -lglfw -o build/mygl;
	./build/mygl;
