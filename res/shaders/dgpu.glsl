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

    if (fs_in.tid > 0) {
        texColor = texture(textures[fs_in.tid - 1], fs_in.uv);
    } else {
        color = fs_in.color;
        return;
    }

    color = texColor * fs_in.color;
}
