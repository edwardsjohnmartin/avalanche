//Terrain Vertex Shader
var terrainVertexShaderSource = `
  attribute vec3 vPosition;
  attribute vec3 vNormal;

  uniform mat4 mvMatrix;
  uniform mat4 pMatrix;

  varying mediump vec4 fColor;
  varying highp vec3 vLighting;

  void main() {
    gl_Position = pMatrix * mvMatrix * vec4(vPosition, 1);

    highp vec3 ambient = vec3(0.3, 0.3, 0.3);
    highp vec3 lightColor = vec3(1, 1, 1);
    highp vec3 direction = normalize(vec3(1, 0, 1));

    highp vec4 normal = vec4(vNormal, 1);

    highp float directional = max(dot(normal.xyz, direction), 0.0);
    vLighting = ambient + (lightColor * directional);

    fColor = vec4(0.7, 0.0, 0.0, 1.0);
  }
`

//Terrain Fragment Shader
var terrainFragmentShaderSource = `
  varying mediump vec4 fColor;
  varying highp vec3 vLighting;

  void main() {
    gl_FragColor = vec4(fColor.rgb * vLighting, fColor.a);
  }
`
