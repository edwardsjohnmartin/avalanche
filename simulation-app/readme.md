<h1>Simulation App</h1>
<br/>
<h2>Table of Contents</h2>
<ol>
    <li>Purpose</li>
    <li>
        Technologies and Resources
        <ul>
            <li>NodeJS, Express, and Socket.io</li>
            <li>C++</li>
            <li>GYP and v8</li>
            <li>VueJS</li>
            <li>Webgl2</li>
        </ul>
    </li>
    <li>Future Features</li>
    <li>How to set up the Application</li>
</ol>
<br/>
<h2>Purpose</h2>
<p>The purpose of this application is to provide a framework for exposing multiple different simulations to the web. The original version of this application was written in C++ using OpenGL inside of SDL, but it has since been ported to a standalone OpenGL implementation. The OpenGL implementation is the basis of this application.</p>
<br/>
<h2>Technologies and Resources – NodeJS, Express, EJS, and Socket.io</h2>
<p>NodeJS was used to allow a full javascript we stack to be utilized, as NodeJS is known for its performance. Furthermore, since NodeJS utilizes the v8 engine, we can create C/C++ addons to allow for access to existing, mature code bases as well as fast, customized computations. This functionality made it so that the already existing simulation code would not have to be rewritten in a different language.</p>
<br/>
<p>Express was the web framework of choice for this application over MeteorJS because MeteorJS comes with a lot of bloat in the form of features that are not needed for this application. Express is very lightweight, and yields a lot of space for customization.</p>
<br/>
<p>EJS stands for effective javascript templating, and is a templating framework that renders html server side before it is sent to the client. This is useful for adding additional logic to templates before rendering them, and allows for the removal of redundant code.</p>
<br/>
<p>Finally, socket.io was used because it is the most supported and robust web socket implementation for NodeJS at the time of writing this. This technology allows for real time communications between the client and server either the client can send data, or the server can push data to the client. When using web sockets, it is important to note that this data is being passed back and forth through a continuously open TCP/IP connection between the client and the server. AJAX cannot replace this functionality, as it is not able to enable the server to push data to clients, and it slows down the transfer of data because a new TCP/IP connection is made for each call. Do not attempt to replace web sockets with an AJAX solution.</p>
<br/>
<a href="https://nodejs.org/en/">NodeJS</a>
<br/>
<a href="https://expressjs.com/">ExpressJS</a>
<br/>
<a href="http://ejs.co/">EJS</a>
<br/>
<a href="https://socket.io/">Socket.io</a>
<br/>
<h2>Technologies and Resources – C++</h2>
<p>Because our original simulation was written in C++, it was kept to continue reaping the benefits of its speed. We are using a C++ addon for node to manage and run the simulations.</p>
<br/>
<a href="http://www.cplusplus.com/">C++</a>
<br/>
<h2>Technologies and Resources – GYP and v8</h2>
<p>GYP is a tool that is used to support building cross-platform C++ applications, similar to CMake. Since the official NodeJS documentation for C/C++ addons utilizes a plugin called node-gyp to create addons, GYP was utilized. With our C++ addons, the v8 engine is utilized to create javascript objects to return to the caller. It is worth noting that NodeJS, GYP, and the v8 engine were all developed by Google.</p>
<br/>
<p>If the installation for node-gyp hangs for a long amount of time, default to using the manual installation method.</p>
<br/>
<a href="https://gyp.gsrc.io/index.md">GYP</a>
<br/>
<a href="https://developers.google.com/v8/">v8</a>
<br/>
<a href="https://github.com/nodejs/node-gyp">node-gyp</a>
<br/>
<h2>Technologies and Resources – Webgl2</h2>
<p>Webgl2 was used over Webgl because it provides more access to features that OpenGL currently has. This made it easier to convert the application from OpenGL to the web.</p>
<br/>
<a href="https://www.khronos.org/registry/webgl/specs/latest/2.0/">Webgl2</a>
<br/>
<h2>Future Features</h2>
<br/>
Features planned for the future include:
<ul>
    <li>Adding multiple types simulations.</li>
    <li>Allowing multiple clients to view the same simulation via a simulation group.</li>
    <li>Improving upon the old C++ code.</li>
    <li>Removing xlib completely.</li>
    <li>Wrapping up any TODO stubs in the application.</li>
    <li>Camera Controls.</li>
    <li>Fix issues with other web browsers.</li>
</ul>
<br/>
<h2>How to set up the Application</h2>
<ol>
    <li>Pull the application from Github</li>
    <li>Install NodeJS and NPM.</li>
    <li>Ensure that Python 2.7 is installed on your machine and that the proper build tools for C++ are installed.</li>
    <li>In the command line, run <code>npm install -g node-gyp</code>.</li>
    <li>Now, in the <code>../simulation-app/</code> directory, attempt to run <code>npm install</code>. If this fails, <code>run node-gyp configure</code> and then <code>node-gyp build</code>. Then, retry <code>npm install</code>.</li>
    <li>Now, there should be both a <code>node_modules</code> folder and a <code>build</code> folder in the <code>../simulation-app/</code> directory.</li>
</ol>
<br/>
<p>After the project is set up, to rebuild the simulation, run the command <code>node-gyp rebuild</code> in the <code>../simulation-app/</code> directory. To run the application, use node server, and then the application will be running at <code>localhost:3000</code>. To allow changes to be made while the server is running, look into installing nodemon for node. If this is installed, simply run the application using <code>nodemon</code> in the command line.</p>
<br/>
<h2>Last Update</h2>
<br/>
Zackary Hall - April 20th, 2017
<br/>
<h2>Created</h2>
<br/>
Zackary Hall - April 11th, 2017
<br/>