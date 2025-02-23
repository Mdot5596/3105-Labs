#version 460

in vec3 ReflectDir;

layout (binding=0) uniform samplerCube SkyBoxTex;

uniform float ReflectFactor;
uniform vec4 MaterialColor;

layout(location =0) out vec4 FragColor;

void main()
{
  vec3 texColor=texture(SkyBoxTex,ReflectDir).rgb;

  texColor=pow(texColor, vec3(1.0/2.2));

  FragColor = vec4(mix(MaterialColor.rgb,texColor, ReflectFactor), 1.0);
}




