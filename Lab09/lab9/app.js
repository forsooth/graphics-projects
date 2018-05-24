// https://webglfundamentals.org/webgl/lessons/webgl-fundamentals.html
// load webGL and tie it to the canvas
// Look over this code and then consider these resources for loading (multiple) textures:
//   https://webglfundamentals.org/webgl/lessons/webgl-3d-textures.html
//   https://webglfundamentals.org/webgl/lessons/webgl-2-textures.html
var canvas = document.getElementById("c");
var gl = canvas.getContext("webgl");
if(!gl)
  alert("no webgl");

// Shaders
var vertexShaderSrc = `
attribute vec4 a_position;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;

void main() {
  gl_Position = a_position;

  // Pass the texcoord to the fragment shader.
  v_texcoord = a_texcoord;
}
`;

var fragmentShaderSrc = `
precision mediump float;

// Passed in from the vertex shader.
varying vec2 v_texcoord;

// The base texture.
uniform sampler2D u_texture;
uniform sampler2D b_texture;
uniform vec2 lightcoord;


void main() {
  vec4 lightray = vec4(v_texcoord.x, v_texcoord.y, 0, 0) - vec4(lightcoord.x, lightcoord.y, -0.75, 0);

  // TODO: change the color based on the texture color, bump map, and light location
   gl_FragColor = texture2D(u_texture, v_texcoord) * dot(lightray, texture2D(b_texture, v_texcoord));
}
`;

// compile the shader programs:
function createShader(gl, type, source) {
  var shader = gl.createShader(type);
  gl.shaderSource(shader, source);
  gl.compileShader(shader);
  var success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
  if (success) {
    return shader;
  }

  console.log(gl.getShaderInfoLog(shader));
  gl.deleteShader(shader);
}

var vertexShader   = createShader(gl,gl.VERTEX_SHADER,vertexShaderSrc);
var fragmentShader = createShader(gl,gl.FRAGMENT_SHADER,fragmentShaderSrc);
var shaderProgram  = gl.createProgram();
gl.attachShader(shaderProgram, vertexShader);
gl.attachShader(shaderProgram, fragmentShader);
gl.linkProgram(shaderProgram);
if(! gl.getProgramParameter(shaderProgram, gl.LINK_STATUS))
  alert("error making shader program" + gl.getProgramInfoLog(shaderProgram));


// load images
var brickImageLoaded = false;

var brickImg = new Image();
brickImg.onload = function(){
  brickImageLoaded = true;
  imgsLoaded();
  console.log("brick loaded");
};
brickImg.src = "brick.png";

var bumpImageLoaded = false;

var bumpImg = new Image();
bumpImg.onload = function(){
  bumpImageLoaded = true;
  imgsLoaded();
  console.log("bump loaded");
};
bumpImg.src = "bump.png";

function imgsLoaded(){
  console.log("imgs loaded")
  if(brickImageLoaded && bumpImageLoaded){
    console.log("init");
    // initial webGL conditions
    gl.useProgram(shaderProgram);
    // look up in shader where the vertex data needs to go.
    var positionLocation = gl.getAttribLocation(shaderProgram, "a_position");
    var texcoordLocation = gl.getAttribLocation(shaderProgram, "a_texcoord");
    var textureLocation  = gl.getUniformLocation(shaderProgram, "u_texture");
    var bumpLocation     = gl.getUniformLocation(shaderProgram, "b_texture");
    var lightLocation   = gl.getUniformLocation(shaderProgram, "lightcoord");

    gl.uniform2f(lightLocation, 0, 0);

    // Create a buffer for positions
    var positionBuffer = gl.createBuffer();

    // provide texture coordinates for the rectangle.
    var texcoordBuffer = gl.createBuffer();



    // Initialize texture and load image
    var brickTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, brickTexture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, brickImg);
    gl.generateMipmap(gl.TEXTURE_2D);


    // Initialize texture and load image
    var bumpTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, bumpTexture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, bumpImg);
    gl.generateMipmap(gl.TEXTURE_2D);

    //set viewport size (map clipspace to worldspace)
    gl.viewport(0,0,500, 500);
    // The rest of the coordinates (verts) will be in clipspace (x,y: -1 ~ 1)

    // Enable and Bind the position buffer.
    gl.enableVertexAttribArray(positionLocation);
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

    // Tell the position attribute how to get data out of buffers (ARRAY_BUFFER)
    var size = 2;          // 2 components per iteration (x,y)
    var type = gl.FLOAT;   // the data is 32bit floats
    var normalize = false; // don't normalize the data
    var stride = 0;        // 0 = move forward size * sizeof(type) each iteration to get the next position
    var offset = 0;        // start at the beginning of the buffer
    gl.vertexAttribPointer(positionLocation, size, type, normalize, stride, offset);
    setRectangle(gl,0,0,1,1); //load geometry

    // Enable and Bind the textcoordBuffer
    gl.enableVertexAttribArray(texcoordLocation);

    // Bind the position buffer.
    gl.bindBuffer(gl.ARRAY_BUFFER, texcoordBuffer);
    gl.vertexAttribPointer(texcoordLocation, size, type, normalize, stride, offset);
    setRectangle(gl,0,0,1,1); // load geometry

    // Clear the canvas
    gl.clearColor(0, 1, 0, 1);
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.activeTexture(gl.TEXTURE0)
    gl.bindTexture(gl.TEXTURE_2D, brickTexture);
    gl.uniform1i(textureLocation, 0);

    gl.activeTexture(gl.TEXTURE1)
    gl.bindTexture(gl.TEXTURE_2D, bumpTexture);
    gl.uniform1i(bumpLocation, 1);

    // Render
    var primitiveType = gl.TRIANGLES;
    var offset = 0;
    var count = 6;
    gl.drawArrays(primitiveType, offset, count);
    }
}

// helper function
function setRectangle(gl, x, y, width, height) {
  var x1 = x;
  var x2 = x + width;
  var y1 = y;
  var y2 = y + height;
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
     x1, y1,
     x2, y1,
     x1, y2,
     x1, y2,
     x2, y1,
     x2, y2,
  ]), gl.STATIC_DRAW);
}
