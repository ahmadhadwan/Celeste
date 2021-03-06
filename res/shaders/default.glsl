#type vertex
#version 460 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in int tid;
layout (location = 3) in vec4 color;

out DATA {
    vec4 position;
    vec2 uv;
    int tid;
    vec4 color;
} vs_out;

uniform mat4 projection;

void main()
{
    gl_Position = projection * position;
    vs_out.position = position;
    vs_out.uv = uv;
    vs_out.tid = tid;
    vs_out.color = color;
}

#type fragment
#version 460 core

layout (location = 0) out vec4 color;

in DATA {
    vec4 position;
    vec2 uv;
    flat int tid;
    vec4 color;
} fs_in;

uniform sampler2D textures[32];

void main()
{
    vec4 texColor;
    switch (fs_in.tid)
    {
        case 1:  texColor = texture(textures[ 0], fs_in.uv); break;
        case 2:  texColor = texture(textures[ 1], fs_in.uv); break;
        case 3:  texColor = texture(textures[ 2], fs_in.uv); break;
        case 4:  texColor = texture(textures[ 3], fs_in.uv); break;
        case 5:  texColor = texture(textures[ 4], fs_in.uv); break;
        case 6:  texColor = texture(textures[ 5], fs_in.uv); break;
        case 7:  texColor = texture(textures[ 6], fs_in.uv); break;
        case 8:  texColor = texture(textures[ 7], fs_in.uv); break;
        case 9:  texColor = texture(textures[ 8], fs_in.uv); break;
        case 10: texColor = texture(textures[ 9], fs_in.uv); break;
        case 11: texColor = texture(textures[10], fs_in.uv); break;
        case 12: texColor = texture(textures[11], fs_in.uv); break;
        case 13: texColor = texture(textures[12], fs_in.uv); break;
        case 14: texColor = texture(textures[13], fs_in.uv); break;
        case 15: texColor = texture(textures[14], fs_in.uv); break;
        case 16: texColor = texture(textures[15], fs_in.uv); break;
        case 17: texColor = texture(textures[16], fs_in.uv); break;
        case 18: texColor = texture(textures[17], fs_in.uv); break;
        case 19: texColor = texture(textures[18], fs_in.uv); break;
        case 20: texColor = texture(textures[19], fs_in.uv); break;
        case 21: texColor = texture(textures[20], fs_in.uv); break;
        case 22: texColor = texture(textures[21], fs_in.uv); break;
        case 23: texColor = texture(textures[22], fs_in.uv); break;
        case 24: texColor = texture(textures[23], fs_in.uv); break;
        case 25: texColor = texture(textures[24], fs_in.uv); break;
        case 26: texColor = texture(textures[25], fs_in.uv); break;
        case 27: texColor = texture(textures[26], fs_in.uv); break;
        case 28: texColor = texture(textures[27], fs_in.uv); break;
        case 29: texColor = texture(textures[28], fs_in.uv); break;
        case 30: texColor = texture(textures[29], fs_in.uv); break;
        case 31: texColor = texture(textures[30], fs_in.uv); break;
        case 32: texColor = texture(textures[31], fs_in.uv); break;
        default: color = fs_in.color; return;
    }

    color = texColor * fs_in.color;
}
