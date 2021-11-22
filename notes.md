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

`discard` is a keyword for not assigning a color for a fragment. This is useful for skipping fragments with no alpha, but doesn't help with blending. Blending must be enabled with `glEnable(GL_BLEND);`. `glBlendFunc(sfactor, dfactor)` is used to set the blending method, eg

glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

Can set separate behaviour for the RGB and A channels using

glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

Can also change the default operator for src/dst using `glBlendEquation(mode)`, where default is `GL_FUNC_ADD`.

`glEnable(GL_CULL_FACE)` enables face culling. Shocker. Can change the type of face to cull with `glCullFace(GL_FRONT)`.

# Framebuffer
```cpp
unsigned int FBO;
glGenFramebuffers(1, &FBO);
glBindFramebuffer(GL_FRAMEBUFFER, FBO);
```
Can specify read/write separately with GL_READ_FRAMEBUFFER / GL_DRAW_FRAMEBUFFER

glReadPixels?

Framebuffers need to meet a set of requirements to be valid. Status can be checked with `glCheckFramebufferStatus(GL_FRAMEBUFFER)` which can return any of [these](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/%67lCheckFramebufferStatus.xhtml) statuses, eg, `GL_FRAMEBUFFER_COMPLETE`.

`glBindFramebuffer(GL_FRAMEBUFFER, 0)` is needed to bind the default framebuffer which is what is visible.

`glDeleteFramebuffers(1, &FBO)` to delete a framebuffer.

## Attachments
Texture or renderbuffer. Texture buffers allow read/write, while renderbuffers only allow write but are faster. Actually, renderbuffers can be read from but only using `glReadPixels` which is slow.

### Texture Buffer
Generate a texture that matches the screen resolution but don't populate it with any starting data.
```cpp
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
```
It's possible to call glViewport before rendering to render to a smaller buffer.

```cpp
//                     target          attachment            textarget      texture  mipmap level
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
```
Other types of attachment are `GL_DEPTH_ATTACHMENT` (which requires GL_DEPTH_COMPONENT as the texture format) and `GL_STENCIL_ATTACHMENT` (which requires GL_STENCIL_INDEX as the texture format). A single texture can store both by using `GL_DEPTH_STENCIL_ATTACHMENT` like so:
```cpp
glTexImage2D(
  GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, 
  GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
```

### Render Buffer
Ideal for data that doesn't need to be read directly, eg, depth/stencil, or off-screen renders to be swapped out.
```cpp
unsigned int RBO;
glGenRenderbuffers(1, &RBO);
glBindRenderbuffer(GL_RENDERBUFFER, RBO);
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
```

# GLSL Features
gl_FragCoord: Available in GLSL, contains the (x, y, z) co-ordinate. Z is the depth buffer. X, Y are the screen space coordinates.

# Depth Options
glDepthMask(GL_FALSE);  // Disables writing to the depth buffer.
glDepthFunc(...);  // How the depth comparison is done, eg, GL_LESS.

# Stencil
Stencil buffer is used to cull parts of the image. Can set up the stencil buffer however we like.

glEnable(GL_STENCIL_TEST)
// Add the stencil bit to what's being cleared
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
// Stencil mask is ANDed with the color buffer
glStencilMask(0xFF)  // Nothing is modified
glStencilMask(0x00)  // Everything is culled

glStencilFunc(func, ref, mask)
- func is the comparison func, eg, GL_LESS
- ref is the reference value, ie, what's compared against
- mask is the mask value to compare with

// Example
glStencilFunc(GL_EQUAL, 1, 0xFF)  // Only draws color values that are 1

glStencilOp(sfail, dpfail, dppass) // default is glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP)
