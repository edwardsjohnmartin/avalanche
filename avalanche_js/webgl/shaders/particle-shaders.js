//Particle Vertex Shader
var particleVertexShaderSource = `#version 300 es
    uniform mat4 modelMatrix;
    uniform mat4 viewMatrix;
    uniform mat4 projectionMatrix;
    in vec3 position; 
    in vec3 color;
    out vec4 fColor;
    
    void main() {
        gl_PointSize = 5.0;
        fColor = vec4(color, 1);
        gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1);
    }
`

//Particle Fragment Shader
var particleFragmentShaderSource = `#version 300 es
    precision mediump float;

    out vec4 outColor;
    in vec4 fColor;
    void main() {
        outColor = fColor;
    }
`