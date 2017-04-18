//Terrain Vertex Shader
var terrainVertexShaderSource = `#version 300 es
    uniform mat4 modelMatrix;
    uniform mat4 viewMatrix;
    uniform mat4 projectionMatrix;
    //uniform sampler2D tex;
    in vec3 position;
    in vec2 uvCoord;
    out vec2 textureCoord;
    
    void main() {
        textureCoord = uvCoord;
        gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1);
    }
`

//Terrain Fragment Shader
var terrainFragmentShaderSource = `#version 300 es
    precision mediump float;

    uniform sampler2D tex;
    in vec2 textureCoord;
    out vec4 outColor;
    
    void main() {
        //outColor = vec4(0.2, 0.2, .2, 1.0);
        outColor = texture(tex, textureCoord);
    }
`