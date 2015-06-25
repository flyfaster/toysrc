<META HTTP-EQUIV="refresh" CONTENT="9">
<script type="text/jscript">
// see http://www.w3schools.com/jsref/tryit.asp?filename=tryjsref_win_setinterval_clearinterval
var myVar = setInterval(function(){ myTimer() }, 1000);

function myTimer() {
    var d = new Date();
    var t = d.toLocaleTimeString();
    document.getElementById("demo").innerHTML = t;
}

function myStopFunction() {
    clearInterval(myVar);
}
</script>