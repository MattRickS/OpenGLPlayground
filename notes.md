OpenGL is a specification that the graphics driver.  
GLFW is a framework for interacting with OpenGL.  
GLAD is a library that loads the OpenGL function pointers.  

`glxinfo` command for checking OpenGL version information

OpenGL has certain objects which can be stored in the GPU and referenced via ID
    VBO: Vertex Buffer Object, GL_ARRAY_BUFFER. Stores vertex attributes, potentially many in one array.
    EBO: Element Buffer Object, GL_ELEMENT_ARRAY_BUFFER. Stores the indices of vertices per triangle.
    VAO: Vertex Array Object, stores the bindings made for VBO/EBO so that it can be swapped in/out with a single ID.

Using an EBO requires using `glDrawElements` instead of `glDrawArrays`.

OpenGL core is a stricter version of the language. For example, it requires VAOs to be used.

Minimum of 16 texture units available. Can be accessed with `GL_TEXTURE8` or `GL_TEXTURE0 + 8`.
