#type vertex
#version 330 core

#ifdef EXPLICIT_ATTRIB_LOCATION
    #extension GL_ARB_explicit_attrib_location: enable
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout (location = 0)
#endif
in vec3 position;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout (location = 1)
#endif
in vec2 uv;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout (location = 2)
#endif
in float tid;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout (location = 3)
#endif
in vec4 color;

out DATA {
    vec3 position;
    vec2 uv;
    float tid;
    vec4 color;
} vs_out;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(position, 1.0f);
    vs_out.position = position;
    vs_out.uv = uv;
    vs_out.tid = tid;
    vs_out.color = color;
}

#type fragment
#version 330 core

#ifdef EXPLICIT_ATTRIB_LOCATION
    #extension GL_ARB_explicit_attrib_location: enable
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout (location = 0)
#endif
out vec4 color;

in DATA {
    vec3 position;
    vec2 uv;
    float tid;
    vec4 color;
} fs_in;

uniform sampler2D textures[32];

void main()
{
    vec4 texColor;
    if (fs_in.tid != 0) {
        if (fs_in.tid <= 16)
            if (fs_in.tid <= 8)
                if (fs_in.tid <= 4)
                    if (fs_in.tid <= 2)
                        if (fs_in.tid <= 1)
                            texColor = texture(textures[0], fs_in.uv);
                        else
                            texColor = texture(textures[1], fs_in.uv);
                    else
                        if (fs_in.tid <= 3)
                            texColor = texture(textures[2], fs_in.uv);
                        else
                            texColor = texture(textures[3], fs_in.uv);
                else
                    if (fs_in.tid <= 6)
                        if (fs_in.tid <= 5)
                            texColor = texture(textures[4], fs_in.uv);
                        else
                            texColor = texture(textures[5], fs_in.uv);
                    else
                        if (fs_in.tid <= 7)
                            texColor = texture(textures[6], fs_in.uv);
                        else
                            texColor = texture(textures[7], fs_in.uv);
            else
                if (fs_in.tid <= 12)
                    if (fs_in.tid <= 10)
                        if (fs_in.tid <= 9)
                            texColor = texture(textures[8], fs_in.uv);
                        else
                            texColor = texture(textures[9], fs_in.uv);
                    else
                        if (fs_in.tid <= 11)
                            texColor = texture(textures[10], fs_in.uv);
                        else
                            texColor = texture(textures[11], fs_in.uv);
                else
                    if (fs_in.tid <= 14)
                        if (fs_in.tid <= 13)
                            texColor = texture(textures[12], fs_in.uv);
                        else
                            texColor = texture(textures[13], fs_in.uv);
                    else
                        if (fs_in.tid <= 15)
                            texColor = texture(textures[14], fs_in.uv);
                        else
                            texColor = texture(textures[15], fs_in.uv);
        else
            if (fs_in.tid <= 24)
                if (fs_in.tid <= 20)
                    if (fs_in.tid <= 18)
                        if (fs_in.tid <= 17)
                            texColor = texture(textures[16], fs_in.uv);
                        else
                            texColor = texture(textures[17], fs_in.uv);
                    else
                        if (fs_in.tid <= 19)
                            texColor = texture(textures[18], fs_in.uv);
                        else
                            texColor = texture(textures[19], fs_in.uv);
                else
                    if (fs_in.tid <= 22)
                        if (fs_in.tid <= 21)
                            texColor = texture(textures[20], fs_in.uv);
                        else
                            texColor = texture(textures[21], fs_in.uv);
                    else
                        if (fs_in.tid <= 23)
                            texColor = texture(textures[22], fs_in.uv);
                        else
                            texColor = texture(textures[23], fs_in.uv);
            else
                if (fs_in.tid <= 28)
                    if (fs_in.tid <= 26)
                        if (fs_in.tid <= 25)
                            texColor = texture(textures[24], fs_in.uv);
                        else
                            texColor = texture(textures[25], fs_in.uv);
                    else
                        if (fs_in.tid <= 27)
                            texColor = texture(textures[26], fs_in.uv);
                        else
                            texColor = texture(textures[27], fs_in.uv);
                else
                    if (fs_in.tid <= 30)
                        if (fs_in.tid <= 29)
                            texColor = texture(textures[28], fs_in.uv);
                        else
                            texColor = texture(textures[29], fs_in.uv);
                    else
                        if (fs_in.tid <= 31)
                            texColor = texture(textures[30], fs_in.uv);
                        else
                            texColor = texture(textures[31], fs_in.uv);
        texColor *= fs_in.color;
        /* color = vec4(vec3(1.0 - texColor), texColor.a); */ /* inverted */
        float average = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
        color = vec4(average, average, average, 1.0);
    } else {
        color = fs_in.color;
    }
}
