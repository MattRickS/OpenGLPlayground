PHONY: learnopengl
learnopengl:
	g++ -Iincludes src/${FILE}.cpp src/Shader.cpp src/glad.c -g -ldl -lglfw -o build/${FILE};
	./build/${FILE};
