#ifndef __CELESTE_GAPI_H__
#define __CELESTE_GAPI_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/vec4.h>
#include <cglm/mat4.h>
#include "common.h"

#ifndef __CELESTE_INTERNAL_GAPI_H__
    typedef struct CLSTbuffer       CLSTbuffer;
    typedef struct CLSTindexbuffer  CLSTindexbuffer;
    typedef struct CLSTvertexarray  CLSTvertexarray;
    typedef struct CLSTshader       CLSTshader;
    typedef struct CLSTtexture      CLSTtexture;
    typedef struct CLSTfont         CLSTfont;
    typedef struct CLSTframebuffer  CLSTframebuffer;
    typedef struct CLSTrenderbuffer CLSTrenderbuffer;
#endif /* __CELESTE_INTERNAL_GAPI_H__ */

/*
 * Opens the file from filepath, and creates a shader.
 *
 * On success it returns a CLSTshader pointer,
 * which must be destroyed by clstShaderDestroy().
 *
 * On error it returns NULL.
 */
CLSTshader *clstShader(char *filepath);

/*
 * Creates a shader from the provided const source.
 *
 * On success it returns a CLSTshader pointer,
 * which must be destroyed by clstShaderDestroy().
 *
 * On error it returns NULL.
 */
CLSTshader *clstShaderConstSrc(const char *src);

/*
 * Creates a shader from the provided source.
 *
 * On success it returns a CLSTshader pointer,
 * which must be destroyed by clstShaderDestroy().
 *
 * On error it returns NULL.
 */
CLSTshader *clstShaderSrc(char *src);

/*
 * Returns the default shader.
 * Celeste handles its destruction.
 */
CLSTshader *clstShaderDefault();

/*
 * Destroys the shader, and frees its memory.
 */
void clstShaderDestroy(CLSTshader *shader);

/*
 * Activates the shader.
 */
void clstShaderActivate(CLSTshader *shader);

/*
 * Deactivates the current shader.
 */
void clstShaderDeactivate();

/*
 * Sets a uniform int for the shader.
 */
void clstShaderUniformInt(CLSTshader *shader, char *uniform_name, int value);

/*
 * Sets a uniform float for the shader.
 */
void clstShaderUniformIntFloat(CLSTshader *shader, char *uniform_name, float value);

/*
 * Sets a uniform vec2 for the shader.
 */
void clstShaderUniformVec2(CLSTshader *shader, char *uniform_name, vec2 value);

/*
 * Sets a uniform vec3 for the shader.
 */
void clstShaderUniformVec3(CLSTshader *shader, char *uniform_name, vec3 value);

/*
 * Sets a uniform vec4 for the shader.
 */
void clstShaderUniformVec4(CLSTshader *shader, char *uniform_name, vec4 value);

/*
 * Sets a uniform mat4 for the shader.
 */
void clstShaderUniformMat4(CLSTshader *shader, char *uniform_name, mat4 value);

/*
 * Opens the file from filepath, and creates a texture.
 *
 * On success it returns a CLSTtexture pointer,
 * which must be destroyed by clstTextureDestroy().
 *
 * On error it returns NULL.
 */
CLSTtexture *clstTexture(char *filepath);

/*
 * Creates a texture from a loaded image.
 *
 * On success it returns a CLSTtexture pointer,
 * which must be destroyed by clstTextureDestroy().
 *
 * On error it returns NULL.
 */
CLSTtexture *clstTextureMem(unsigned char *buffer, unsigned int bufsize);

/*
 * Creates a texture from inline data.
 *
 * On success it returns a CLSTtexture pointer,
 * which must be destroyed by clstTextureDestroy().
 *
 * On error it returns NULL.
 */
CLSTtexture *clstTextureInline(unsigned char *pixels, unsigned int width, unsigned int height, unsigned int bpp);

/*
 * Destroys the texture, and frees its memory.
 */
void clstTextureDestroy(CLSTtexture *texture);

/*
 * Binds the texture.
 */
void clstTextureBind(CLSTtexture *texture);

/*
 * Unbinds the current texture.
 */
void clstTextureUnbind();

/*
 * Returns the width of a texture.
 */
unsigned int clstTextureWidth(CLSTtexture *texture);

/*
 * Returns the height of a texture.
 */
unsigned int clstTextureHeight(CLSTtexture *texture);

/*
 * Opens the file from filepath, and creates
 * a font with the provided size.
 *
 * On success it returns a CLSTfont pointer,
 * which must be destroyed by clstFontDestroy().
 *
 * On error it returns NULL.
 */
CLSTfont *clstFont(const char *filepath, float size);

/*
 * Creates a font with the provided size.
 *
 * On success it returns a CLSTfont pointer,
 * which must be destroyed by clstFontDestroy().
 *
 * On error it returns NULL.
 */
CLSTfont *clstFontMem(unsigned char *buffer, unsigned int bufsize, float size);

/*
 * Destroys the font, and frees its memory.
 */
void clstFontDestroy(CLSTfont *font);

CLSTframebuffer *clstFrameBuffer();

void clstFrameBufferDestroy(CLSTframebuffer *framebuffer);

void clstFrameBufferBind(CLSTframebuffer *framebuffer);

void clstFrameBufferUnbind();

void clstFrameBufferAttachTexture(CLSTframebuffer *framebuffer, CLSTtexture *texture);

void clstFrameBufferAttachRenderBuffer(CLSTframebuffer *framebuffer, CLSTrenderbuffer *renderbuffer);

CLSTresult clstFrameBufferComplete(CLSTframebuffer *framebuffer);

#endif /* __CELESTE_GAPI_H__ */
