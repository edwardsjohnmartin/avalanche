var nextFrame; //Next render frame
var currentFrame; //Current render frame
var terrainData; //Terrain vertices and texture coordinates
var lastRender = new Date();

var gl; //Webgl

//Terrain program vars
var terrainProgram;
var terrainModelMatrixLocation;
var terrainViewMatrixLocation;
var terrainProjectionMatrixLocation;
var terrainTexture;
var texture;

//Particle program vars
var particleProgram;
var particleModelMatrixLocation;
var particleViewMatrixLocation;
var particleProjectionMatrixLocation;

//Buffers
var terrainVertexBuffer;
var terrainTextureBuffer;
var particleVertexBuffer;
var particleColorBuffer;

//Vertex Array Objects
var terrainVAO;
var particleVAO;

//Matrices
var modelMatrix;
var viewMatrix;
var projectionMatrix;

var nextFrameReady = false;

//Method designed to handle frame rate and adjust variables accordingly
function tick() {
    requestAnimFrame(tick);

    var now = new Date();
    var timeElapsed = now.getTime() - lastRender.getTime();

    if (timeElapsed > app.timePerFrame && nextFrameReady) {
        currentFrame = nextFrame;
        requestNextFrame();
        render();
    }
}

//Method designed to render the scene
function render() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT); 

    //Draw the particles
    gl.useProgram(particleProgram);
	gl.bindVertexArray(particleVAO);

	gl.uniformMatrix4fv(particleModelMatrixLocation, false, flatten(modelMatrix));
	gl.uniformMatrix4fv(particleViewMatrixLocation, false, flatten(viewMatrix));
	gl.uniformMatrix4fv(particleProjectionMatrixLocation, false, flatten(projectionMatrix));

	gl.bindBuffer(gl.ARRAY_BUFFER, particleVertexBuffer);
	gl.bufferData(gl.ARRAY_BUFFER, flatten(currentFrame["vertices"]), gl.STATIC_DRAW);

	gl.bindBuffer(gl.ARRAY_BUFFER, particleColorBuffer);
	gl.bufferData(gl.ARRAY_BUFFER, flatten(currentFrame["colors"]), gl.STATIC_DRAW);

	gl.drawArrays(gl.POINTS, 0, currentFrame["colors"].length);

    //Draw the terrain
    gl.useProgram(terrainProgram);
	gl.bindVertexArray(terrainVAO);

	gl.uniformMatrix4fv(terrainModelMatrixLocation, false, flatten(modelMatrix));
	gl.uniformMatrix4fv(terrainViewMatrixLocation, false, flatten(viewMatrix));
	gl.uniformMatrix4fv(terrainProjectionMatrixLocation, false, flatten(projectionMatrix));

	gl.bindBuffer(gl.ARRAY_BUFFER, terrainVertexBuffer);
	gl.drawArrays(gl.TRIANGLES, 0, terrainData["vertices"].length);

    lastRender = new Date();
}

//Set up webgl
function setUpWebgl() {
    var canvas = document.getElementById("gl-canvas");

    //Dynamically size canvas
    canvas.height = parseInt(getComputedStyle(document.getElementById('canvas-panel')).height.slice(0, -2));
    canvas.width = parseInt(getComputedStyle(document.getElementById('canvas-panel')).width.slice(0, -2));

    // Initialize the GL context
    gl = canvas.getContext("webgl2");
    if (!gl) {
        alert("webgl2 is not available!!!");
        return;
    }

    // Set clear color
    gl.clearColor(0.96, 0.47, 0.13, 0.75);
    // Enable depth testing
    gl.enable(gl.DEPTH_TEST);
    // Near things obscure far things
    gl.depthFunc(gl.LEQUAL);
    // Clear the color as well as the depth buffer.
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    //Programs
    terrainProgram = createProgram(gl, terrainVertexShaderSource, terrainFragmentShaderSource);
    particleProgram = createProgram(gl, particleVertexShaderSource, particleFragmentShaderSource);

    //Set locations
    terrainModelMatrixLocation = gl.getUniformLocation(terrainProgram, "modelMatrix");
	terrainViewMatrixLocation = gl.getUniformLocation(terrainProgram, "viewMatrix");
	terrainProjectionMatrixLocation = gl.getUniformLocation(terrainProgram, "projectionMatrix");
	particleModelMatrixLocation = gl.getUniformLocation(particleProgram, "modelMatrix");
	particleViewMatrixLocation = gl.getUniformLocation(particleProgram, "viewMatrix");
	particleProjectionMatrixLocation = gl.getUniformLocation(particleProgram, "projectionMatrix");

    //VAOs
    terrainVAO = gl.createVertexArray();
    particleVAO = gl.createVertexArray();

    //Buffers
    terrainVertexBuffer = gl.createBuffer();
    terrainTextureBuffer = gl.createBuffer();
    particleVertexBuffer = gl.createBuffer();
    particleColorBuffer = gl.createBuffer();

    //Terrain position
    gl.bindVertexArray(terrainVAO);
    gl.bindBuffer(gl.ARRAY_BUFFER, terrainVertexBuffer);

    var terrainPosAttrib = gl.getAttribLocation(terrainProgram, "position");
	gl.enableVertexAttribArray(terrainPosAttrib);
    gl.vertexAttribPointer(terrainPosAttrib, 3, gl.FLOAT, false, 0, 0);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(terrainData["vertices"]), gl.STATIC_DRAW); //Send terrain data

    //Particle position
    gl.bindVertexArray(particleVAO);
    gl.bindBuffer(gl.ARRAY_BUFFER, particleVertexBuffer);

	var particlePosAttrib = gl.getAttribLocation(particleProgram, "position");
	gl.enableVertexAttribArray(particlePosAttrib);
	gl.vertexAttribPointer(particlePosAttrib, 3, gl.FLOAT, false, 0, 0);

    //Particle color
    gl.bindVertexArray(particleVAO);
    gl.bindBuffer(gl.ARRAY_BUFFER, particleColorBuffer);

	var particleColAttrib = gl.getAttribLocation(particleProgram, "color");
	gl.enableVertexAttribArray(particleColAttrib);
	gl.vertexAttribPointer(particleColAttrib, 3, gl.FLOAT, false, 0, 0);

    //TODO the texture appears to be mapping backwards
    //Terrain texture
    gl.useProgram(terrainProgram);
    gl.bindVertexArray(terrainVAO);
    gl.bindBuffer(gl.ARRAY_BUFFER, terrainTextureBuffer);

    var terrainTexAttrib = gl.getAttribLocation(terrainProgram, "uvCoord");
	gl.enableVertexAttribArray(terrainTexAttrib);
    gl.vertexAttribPointer(terrainTexAttrib, 2, gl.FLOAT, false, 0, 0);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(terrainData["textureCoordinates"]), gl.STATIC_DRAW); //Send terrain data

    var image = document.getElementById("texImage");

    texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);
    //gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, gl.RGB, gl.UNSIGNED_BYTE, image);
    //gl.generateMipmap(gl.TEXTURE_2D);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.uniform1i(gl.getUniformLocation(terrainProgram, "tex"), 0);

    //Set up matrices
    modelMatrix = mult(mat4(), scalem(vec3(0.01, 0.01, 0.01)));
    modelMatrix = mult(modelMatrix, translate(-800.0, -400.0, 0.0));
    modelMatrix = mult(modelMatrix, rotate(-30.0, vec3(0.0, 1.0, 0.0)));

	viewMatrix = lookAt(
		vec3(0.0, 0.0, -15.0), //eye
		vec3(0.0, 0.0, 0.0), //at
		vec3(0.0, 1.0, 0.0) //up
	);

	projectionMatrix = perspective(
		45.0, //fov
		canvas.width / canvas.height,
		0.00001, //nearZ
		1000000000.0 //farZ
	);

    //Start the animation
    tick();
}

//Onsize event
window.onresize = function() {
    var canvas = document.getElementById("gl-canvas");
    canvas.height = parseInt(getComputedStyle(document.getElementById('canvas-panel')).height.slice(0, -2));
    canvas.width = parseInt(getComputedStyle(document.getElementById('canvas-panel')).width.slice(0, -2));
};