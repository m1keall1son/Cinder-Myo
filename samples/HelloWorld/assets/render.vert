#version 330

in vec3 ciPosition;
in vec3 ciNormal;

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

out vec3 vPosition;
out vec3 vNormal;

void main(){
    
    vec4 pos = vec4( ciPosition, 1. );
    vPosition = vec3( ciModelView * pos );
    vNormal = ciNormalMatrix * ciNormal;
    
    gl_Position = ciModelViewProjection * pos;
    
}