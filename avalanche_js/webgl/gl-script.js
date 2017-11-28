var currentFrame; //Current render frame
var terrainData; //Terrain vertices and texture coordinates

var lastRender = new Date();
var lastFrameReq = new Date();

var gl; //Webgl

//Terrain program
var terrainProgram;
var terrainVAO;

//Particle program
var particleProgram;
var particleVAO;

var startPoint;
var endPoint;

var zoomStart;
var zoom = 0;

var aspect = 1.0;
var rotX = 0;
var rotY = 0;
var radius = 1000;

var camup = vec3(0, 1, 0);
var camfor = vec3(0, 0, 1);

var eye = vec3(0, 100, -radius);
var mvMatrix, pMatrix;

var nextFrameReady = false;

var matrixStack = new Array();
function pushMatrix() {
  matrixStack.push(mat4(mvMatrix));
}
function popMatrix() {
  mvMatrix = matrixStack.pop();
}

//Method designed to handle frame rate and adjust variables accordingly
function tick() {
    requestAnimFrame(tick);

    var now = new Date();
    var timeElapsedRender = now.getTime() - lastRender.getTime();

    //render @ 60 FPS
    if(timeElapsedRender > 16.6667) {
      updateSimulation()
      render()
    }
}

//Method designed to render the scene
function render() {
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  const fovy = 40.0;
  const near = 0.01;
  const far = 10000;
  const r = radius;

  if(rotY != 0) {
    var rotMatrix = rotate(rotY, cross(camfor, camup));
    eye = vec3(mult(rotMatrix, vec4(eye)));
    camfor = vec3(mult(rotMatrix, vec4(camfor)));
    camup = vec3(mult(rotMatrix, vec4(camup)));
  }

  if(rotX != 0) {
    var rotMatrix = rotate(rotX, camup);
    eye = vec3(mult(rotMatrix, vec4(eye)));
    camfor = vec3(mult(rotMatrix, vec4(camfor)));
  }

  var up = vec3(0, 1, 0);
  var at = vec3(0, 0, 0);
  mvMatrix = lookAt(eye, at, up);
  pMatrix = perspective(fovy, aspect, near, far);

  pushMatrix();

  mvMatrix = mult(mvMatrix, translate(-500, -400, 0));
  renderTerrain();
  renderParticles();

  popMatrix();

  lastRender = new Date();
}

function renderParticles() {
  gl.enable(gl.DEPTH_TEST);
  gl.depthFunc(gl.ALWAYS);

  gl.useProgram(particleProgram.program);

  gl.bindBuffer(gl.ARRAY_BUFFER, particleVAO.vertexBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, flatten(currentFrame.vertices), gl.STATIC_DRAW);

  gl.uniformMatrix4fv(particleProgram.mvMatrixLoc, false, flatten(mvMatrix));
  gl.uniformMatrix4fv(particleProgram.pMatrixLoc, false, flatten(pMatrix));

  gl.enableVertexAttribArray(particleProgram.vertexLoc);
  gl.vertexAttribPointer(particleProgram.vertexLoc, 3, gl.FLOAT, false, 0, 0);

  // gl.enableVertexAttribArray(lineProgram.colorLoc);
  // gl.bindBuffer(gl.ARRAY_BUFFER, floor.colorBuffer);
  // gl.vertexAttribPointer(lineProgram.colorLoc, 4, gl.FLOAT, false, 0, 0);

  gl.drawArrays(gl.POINTS, 0, currentFrame.vertices.length);
  gl.disable(gl.DEPTH_TEST);
}

function renderTerrain() {
  gl.enable(gl.DEPTH_TEST);
  gl.depthFunc(gl.LESS);

  gl.useProgram(terrainProgram.program);

  gl.enableVertexAttribArray(terrainProgram.vertexLoc);
  gl.bindBuffer(gl.ARRAY_BUFFER, terrainVAO.vertexBuffer);
  gl.vertexAttribPointer(terrainProgram.vertexLoc, 3, gl.FLOAT, false, 0, 0);

  gl.enableVertexAttribArray(terrainProgram.normalLoc);
  gl.bindBuffer(gl.ARRAY_BUFFER, terrainVAO.normalBuffer);
  gl.vertexAttribPointer(terrainProgram.normalLoc, 3, gl.FLOAT, false, 0, 0);

  // gl.enableVertexAttribArray(lineProgram.colorLoc);
  // gl.bindBuffer(gl.ARRAY_BUFFER, floor.colorBuffer);
  // gl.vertexAttribPointer(lineProgram.colorLoc, 4, gl.FLOAT, false, 0, 0);

  gl.uniformMatrix4fv(terrainProgram.mvMatrixLoc, false, flatten(mvMatrix));
  gl.uniformMatrix4fv(terrainProgram.pMatrixLoc, false, flatten(pMatrix));

  gl.drawArrays(gl.TRIANGLES, 0, terrainData.vertices.length);
}

var createTerrainProgram = function() {
  this.program = createProgram(gl, terrainVertexShaderSource, terrainFragmentShaderSource);
  gl.useProgram(this.program);

  this.vertexLoc = gl.getAttribLocation(this.program, "vPosition");
  this.normalLoc = gl.getAttribLocation(this.program, "vNormal");

  this.mvMatrixLoc = gl.getUniformLocation(this.program, "mvMatrix");
  this.pMatrixLoc = gl.getUniformLocation(this.program, "pMatrix");
}

var createParticleProgram = function() {
  this.program = createProgram(gl, particleVertexShaderSource, particleFragmentShaderSource);
  gl.useProgram(this.program);

  this.vertexLoc = gl.getAttribLocation(this.program, "vPosition");

  this.mvMatrixLoc = gl.getUniformLocation(this.program, "mvMatrix");
  this.pMatrixLoc = gl.getUniformLocation(this.program, "pMatrix");
}

function getMousePosition(e) {
  var r = canvas.getBoundingClientRect();
  var x = e.clientX - r.left - canvas.width/2;
  var y = canvas.height/2 - e.clientY + r.top;

  x /= canvas.width/2;
  y /= canvas.height/2;

  return vec2(x, y);
}

function clamp(value, min, max) {
  if(value > max) return max;
  if(value < min) return min;
  return value;
}

function processInput(mouse) {
  var r = 1;
  var x = mouse[0];
  var y = mouse[1];

  var z = 0;

  if(Math.sqrt(x*x + y*y) <= r) z = Math.sqrt(r*r - x*x - z*z);
  return [x, y, z];
}

//Set up webgl
function setUpWebgl() {
  canvas = document.getElementById("gl-canvas");
  gl = setupWebGL(canvas);
  if (!gl) { alert("WebGL isn't available"); }

  canvas.oncontextmenu = function(e) { return false; }
  window.oncontextmenu = function(e) { return false; }

  canvas.onmousedown = function(e) {
    var mouse = getMousePosition(e);

    if(e.button == 0)
      startPoint = normalize(processInput(mouse));
    if(e.button == 2) {
      zoomStart = mouse;
    }
  };

  window.onmouseup = function(e) {
    startPoint = null;
    endPoint = null;
    zoomStart = null;
    zoom = 0;
    rotX = 0;
    rotY = 0;
  };

  window.onmousemove = function(e) {
    if(startPoint) {
      var mouse = getMousePosition(e);
      endPoint = processInput(mouse);
      var current = normalize(endPoint);

      var u = normalize(cross(current, startPoint));
      var vdot = dot(current, startPoint);

      //axis = u;
      rotX = 2*(startPoint[0] - endPoint[0]);
      rotY = 2*(startPoint[1] - endPoint[1]);
    }

    if(zoomStart) {
      var zoomCurrent = getMousePosition(e);

      zoom = zoom + -Math.sign(zoomStart[1] - zoomCurrent[1]) * 5;
      zoomStart = zoomCurrent;
    }
  }

  gl.viewport(0, 0, canvas.width, canvas.height);
  aspect = canvas.width/canvas.height;

  gl.clearColor(0.0, 0.7, 0.0, 1.0);
  gl.enable(gl.DEPTH_TEST);

  // Create programs
  terrainProgram = new createTerrainProgram();
  particleProgram = new createParticleProgram();

  terrainVAO = {};
  terrainVAO.vertexBuffer = gl.createBuffer();
  terrainVAO.normalBuffer = gl.createBuffer();

  gl.bindBuffer(gl.ARRAY_BUFFER, terrainVAO.vertexBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, flatten(terrainData.vertices), gl.STATIC_DRAW);

  gl.bindBuffer(gl.ARRAY_BUFFER, terrainVAO.normalBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, flatten(terrainData.normals), gl.STATIC_DRAW);

  particleVAO = {};
  particleVAO.vertexBuffer = gl.createBuffer();

  lastRender = new Date();
  tick();
}
