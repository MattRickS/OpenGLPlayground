PHONY: learnopengl
learnopengl:
	g++ src/${FILE}.cpp src/Shader.cpp src/glad.c -g -ldl -lglfw -o bin/${FILE};
	./bin/${FILE};
