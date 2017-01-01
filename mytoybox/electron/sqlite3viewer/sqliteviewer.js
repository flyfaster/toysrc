var fs = require("fs");
var file = "test.db";
var exists = fs.existsSync(file);

if(!exists) {
  console.log("Creating DB file.");
  fs.openSync(file, "w");
}

var sqlite3 = require("sqlite3").verbose();
var db = new sqlite3.Database(file);

// 

var sql_get_table_list = "SELECT name FROM sqlite_master WHERE type='table'";

db.serialize(function() {
  if(!exists) {
    db.run("CREATE TABLE table1 (thing TEXT, data INT)");
    for(var i=0; i<12; i++) {
      db.run("INSERT INTO table1 VALUES (?, ?)", ['t1 '+i.toString(), 100+i]);
    }
    db.run("CREATE TABLE table2 (thing TEXT, datareal REAL)");
    for(var i=0; i<12; i++) {
      db.run("INSERT INTO table2 VALUES (?, ?)", ['t2 '+i.toString(), 200+i]);
    }
    db.run("CREATE TABLE table3 (thing TEXT, datanum NUM)");
    for(var i=0; i<8; i++) {
      db.run("INSERT INTO table3 VALUES (?, ?)", ['t3 '+i.toString(), 300+i]);
    }
  }
  
  db.each(sql_get_table_list, function(err, row) {
    
    // <a href="#" onClick="showAbout2();"> Video</a>
    var table_name_item = '<a href="#" onClick="showTable(\'' + row.name+'\');">' + row.name + '</a>';
    table_name_item = '<a href="#" onClick="showTable(this);">' + row.name + '</a>';
    //table_name_item = '<a href="#" onClick="showTable();">' + row.name + '</a>';
    console.log(  row.name + " " + table_name_item);
    showTableList(table_name_item);
  });


});

db.close();

function showTable(event)
{
    console.log("Table name clicked: " + event.innerHTML);
}

function showTable2()
{
    console.log("Table name clicked: " + event.target.innerHTML);
}

function showAbout2()
{
    console.log("bonjour");
}

function showTableList(list_item_text) {
    var entry = document.createElement('li');
    //entry.appendChild(document.createTextNode(list_item_text));
    entry.innerHTML = list_item_text;
    document.getElementById("TableList").appendChild(entry);    
}