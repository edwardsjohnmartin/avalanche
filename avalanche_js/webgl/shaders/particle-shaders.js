//Particle Vertex Shader
var particleVertexShaderSource = `
  attribute vec3 vPosition;

  uniform mat4 mvMatrix;
  uniform mat4 pMatrix;

  void main() {
    gl_PointSize = 5.0;
    gl_Position = pMatrix * mvMatrix * vec4(vPosition, 1);
  }
`

//Particle Fragment Shader
var particleFragmentShaderSource = `
  void main() {
      gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
  }
`
