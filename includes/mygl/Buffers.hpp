#include <iostream>

#include <GLFW/glfw3.h>


class FrameBuffer
{
public:
    unsigned int ID;

    FrameBuffer()
    {
        glGenFramebuffers(1, &ID);
        glBindFramebuffer(GL_FRAMEBUFFER, ID);
    }

    void use()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ID);
    }
};


/*
For now this is just a depth/stencil render buffer. Should have a way of easily configuring it's type.
*/
class RenderBuffer
{
public:
    unsigned int width, height;
    unsigned int ID;

    RenderBuffer(unsigned int width, unsigned int height) : width(width), height(height)
    {
        glGenRenderbuffers(1, &ID);
        glBindRenderbuffer(GL_RENDERBUFFER, ID); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);  
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    void use()
    {
        glBindRenderbuffer(GL_RENDERBUFFER, ID);
    }
};


/*
For now this is just a colour buffer. Should have a way of easily configuring it's type.
*/
// TODO: Can the Texture class just have an option to be used as a buffer?
class TextureBuffer
{
public:
    unsigned int width, height;
    unsigned int ID;

    TextureBuffer(unsigned int width, unsigned int height) : width(width), height(height)
    {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // attach it to currently bound framebuffer object
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0);
    }

    void use()
    {
        glBindTexture(GL_TEXTURE_2D, ID);
    }
};