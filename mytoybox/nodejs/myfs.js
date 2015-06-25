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

function walk(currentDirPath, callback) {
    var fs = require('fs'), path = require('path');
    fs.readdirSync(currentDirPath).forEach(function(name) {
        var filePath = path.join(currentDirPath, name);
        try {
			var stat = fs.statSync(filePath);
			callback(filePath, stat);
			
			//if (stat.isFile()) {
				//callback(filePath, stat);
			//} else if (stat.isDirectory()) {
				//walk(filePath, callback);
			//}
		} catch (e) {
			console.log("failed to access " + filePath);
		}
	
    });
}

if (typeof String.prototype.startsWith != 'function') {
  String.prototype.startsWith = function (str){
    return this.slice(0, str.length) == str; // http://stackoverflow.com/questions/646628/how-to-check-if-a-string-startswith-another-string
  };
}

//var dict = require("dict"); //  https://www.npmjs.com/package/dict

var pathlist = new Object();
function walkcallback(filePath, stat) {
	console.log(filePath);
	pathlist[filePath] = stat;
}

function writeNewLine(res) { 
	res.write('</br>\n');
}

const PORT=8621;
var http = require('http');
var os = require('os');
var fs=require('fs');
var path=require('path');
var filepath; // script path name
filepath = __filename;
http.createServer(function (req, res) {
	
	var url = require("url");
	var pathname = url.parse(req.url).pathname;
	
	if(pathname.startsWith("/ls/")) {
		res.writeHead(200, {'Content-Type': 'text/html'});		
		res.write('<html><body>');
		res.write('url pathname is ' +pathname+'\n');
		var fspath = pathname.slice(3, pathname.length);
		console.log(fspath);
		walk(fspath, walkcallback);
		writeNewLine(res);
		res.write('<pre>');
		Object.keys(pathlist).forEach(function(key) {
			//var val = o[key];
			res.write(key + '\n');
		});
		res.write('</pre>');
		pathlist = new Object(); // release the huge object
		res.end(filepath + ' ' + getDateTime() +"\n</body></html>\n");
		return;
	}
	if(pathname.startsWith("/get/")) {
		var fspath = pathname.slice(4, pathname.length);			
		var mime = require('mime');	// npm install mime
		var filename = path.basename(pathname);
		var mimetype = mime.lookup(pathname);		
		res.setHeader('Content-disposition', 'attachment; filename='+filename);
		res.setHeader('Content-type', mimetype);
		var stream = fs.createReadStream(fspath, { bufferSize: 64 * 1024 });
		stream.pipe(res);		
		return;
	}
	res.writeHead(200, {'Content-Type': 'text/plain'});
	res.write('Invalid request ' + pathname);
	res.write('\nsample usage:\n');
	res.write('http://'+req.headers.host+'/get/home/onega/aspen/findchange.sh\n');
	res.write('http://'+req.headers.host+'/ls/home/onega\n');
	res.end();
	return;
	
}).listen(PORT);
console.log('Server pid' +process.pid+' running at http://'+os.hostname()+':'+PORT.toString()+'/');


	console.log('http://'+os.hostname()+':'+PORT.toString()+'/get'+__filename+'\n');
	console.log('http://'+os.hostname()+':'+PORT.toString()+'/ls'+process.env.HOME+'\n');

