#version 330 core

in VS_OUT {
  flat int type;
} fs_in;

out vec4 color;

void main()
{
  switch(fs_in.type)
  {
    case 0:
      color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
      break;
    case 1:
      color = vec4(0.51f, 0.706f, 0.275f, 1.0f);
      break;
    case 2:
      color = vec4(0.698f, 0.133f, 0.133f, 1.0f);
      break;
    default:
      color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
  }
}
