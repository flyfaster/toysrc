function getDateTime() {
    var date = new Date();
    var hour = date.getHours();
    hour = (hour < 10 ? "0" : "") + hour;
    var min  = date.getMinutes();
    min = (min < 10 ? "0" : "") + min;
    var sec  = date.getSeconds();
    sec = (sec < 10 ? "0" : "") + sec;
    var year = date.getFullYear();
    var month = date.getMonth() + 1;
    month = (month < 10 ? "0" : "") + month;
    var day  = date.getDate();
    day = (day < 10 ? "0" : "") + day;
    return year + "/" + month + "/" + day + " " + hour + ":" + min + ":" + sec;
}

function cmd_exec(cmd, args, cb_stdout, cb_end) {
  var spawn = require('child_process').spawn,
    child = spawn(cmd, args),
    me = this;
  me.exit = 0;  // Send a cb to set 1 when cmd exits
  child.stdout.on('data', function (data) { cb_stdout(me, data) });
  child.stdout.on('end', function () { cb_end(me) });
}
foo = new cmd_exec('netstat', ['-rn'], 
  function (me, data) {me.stdout += data.toString();},
  function (me) {me.exit = 1;}
);
function log_console() {
  console.log(foo.stdout);
}

function testexec() {
setTimeout(
  // wait 0.25 seconds and print the output
  log_console,
250);
}

function writeNewLine(res) { 
	res.write('</br>\n');
}
const PORT=8621;
var http = require('http');
var os = require('os');
var fs=require('fs');
var path=require('path');
var filepath=path.join(__dirname, 'example.js' ); // script path name
filepath = __filename;
http.createServer(function (req, res) {
	res.writeHead(200, {'Content-Type': 'text/html'});
	res.write('<html>');
	res.write('<head>');	
	res.write(fs.readFileSync(path.join(__dirname, 'myclient.js' )) ); //writeNewLine(res);
	res.write('</head>');
	res.write('<body><p id="demo">'+getDateTime()+' </p>');
	if (0) {
	res.write('<pre>');
	res.write(os.tmpdir()); writeNewLine(res);
	res.write(os.endianness()); writeNewLine(res);
	res.write(os.hostname()); writeNewLine(res);
	res.write(os.type()); writeNewLine(res);
	res.write(os.platform()); writeNewLine(res);
	res.write(os.arch()); writeNewLine(res);
	res.write(os.release()); writeNewLine(res);
	res.write(os.uptime().toString()); writeNewLine(res);
	res.write(os.loadavg().toString()); writeNewLine(res);
	res.write(os.totalmem().toString()); writeNewLine(res);
	res.write(os.freemem().toString()); writeNewLine(res);
	res.write(os.cpus().toString()); writeNewLine(res);
	res.write(os.networkInterfaces().toString()); writeNewLine(res);
	res.write('Script path:' + filepath ); writeNewLine(res);
//	res.write(fs.readFileSync(filepath) ); writeNewLine(res);
	var child_process=require('child_process');
	var cmdoutput = child_process.execSync('ls -ltr ' + __dirname);
	res.write(cmdoutput);
	res.write('</pre>');
	}
	var url = require("url");
	var pathname = url.parse(req.url).pathname;
	//res.write('url pathname is ' +pathname+' current time ' + getDateTime());  writeNewLine(res);
	res.write(' current time ' + getDateTime());  writeNewLine(res);
	//
	res.write('<A HREF="javascript:history.go(0)">Click to refresh the page</A>');  writeNewLine(res);
	res.write('<FORM><INPUT TYPE="button" onClick="history.go(0)" VALUE="Refresh"></FORM>');  writeNewLine(res);
	res.write('</body></html>');
	res.end();
}).listen(PORT);
console.log('This process is pid ' + process.pid);
console.log('Server running at http://127.0.0.1:1337/');



function start(route, handle) {
function onRequest(request, response) {
var pathname = url.parse(request.url).pathname;         
console.log("Request for " + pathname + " received.");
route(handle, pathname);
response.writeHead(200, {"Content-Type": "text/plain"});
response.write("Hello World");
response.end();
}
}
