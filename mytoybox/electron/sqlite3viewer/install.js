var isWin = /^win/.test(process.platform);

if (isWin) {
    var spawn = require('child_process').spawn;
    var prc = spawn(__dirname + "/install_on_windows.bat",  []);    
    prc.stdout.setEncoding('utf8');
    prc.stdout.on('data', function (data) {
        var str = data.toString()
        var lines = str.split(/(\r?\n)/g);
        console.log(lines.join(""));
    });

    prc.on('close', function (code) {
        console.log('process exit code ' + code);
    });
}

if (0>1) {
    require('child_process').exec(__dirname + "/install_on_windows.bat", function (err, stdout, stderr) {
        if (err) {
            // console.log(stderr);
            return console.log(err);
        }
        console.log(stdout);
        console.log(err);
    });
}
