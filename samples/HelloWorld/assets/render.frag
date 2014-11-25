#version 330

in vec3 vPosition;
in vec3 vNormal;

out vec4 FragColor;

void main(){
    
    vec3 lightpos = vec3( 0,10,10 );
    
    FragColor.xyz = vec3( max( dot( normalize(vNormal), normalize( lightpos - vPosition ) ) , 0.0 ) )*vec3(.0,.5,1.);
    FragColor.a = 1.;
}