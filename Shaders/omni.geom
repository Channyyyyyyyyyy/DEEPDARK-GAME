#version 460 core
layout(triangles) in; layout(triangle_strip, max_vertices=18) out;
uniform mat4 faces[6];
void main(){ for(int f=0; f<6; f++){ gl_Layer = f; for(int i=0;i<3;i++) gl_Position = faces[f]*gl_in[i].gl_Position, EmitVertex(); EndPrimitive(); } }
