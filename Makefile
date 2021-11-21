PHONY: mygl
mygl:
	# Dependencies are installed with sudo for now
	g++ --std=c++17 -Iincludes src/mygl.cpp src/stb_image.cpp src/Shader.cpp src/TextureCache.cpp src/Mesh.cpp src/Model.cpp src/glad.c -g -ldl -lglfw -lassimp -o build/mygl;
	./build/mygl;
