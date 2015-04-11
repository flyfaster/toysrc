var page = require('webpage').create();
var system = require('system'),
 address, myid;

if (system.args.length != 3) 
{
  console.log('Usage: loadspeed.js <some URL> <ElementId>');
  phantom.exit();
}
address = system.args[1];
myid = system.args[2];
page.open(address, function(status) {
  console.log("Status: " + status + " " + address);
  if(status === "success") {
   var title = page.evaluate(function(){
       //onCompanyLoad(); innerHTML
       return document.getElementById("chart_anchor").outerHTML;
return document.getElementById("chart-section").outerHTML; // "chart-section"
   });
   page.render('example.png');
console.log(title);
   //console.log(page.content);
  }
  phantom.exit();
});
