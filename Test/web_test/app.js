var express = require('express');
var http = require('http');
var app = express();
app.use(express.static('.'));
app.set('port', 80);

app.get('/', function(req, res){
   res.sendfile('index.html');
});

app.get('/bar',function(req, res){
    var val = Math.random() * 10;
    res.send({data:val.toFixed(1)});
});

app.get('/line',function(req, res){
    var val = Math.random() * 50;
    res.send({data:val.toFixed(2)});
});

app.get('/graph',function(req, res){
    var val = Math.random() * 100;
    res.send({data:val.toFixed(0)});
});

app.get('/indicator',function(req, res){
    var umem = (Math.random() * 1000).toFixed(1);
    var tkey = (Math.random() * 1000).toFixed(1);
    var clients = (Math.random() * 1000).toFixed(1);
    var cmdproc = (Math.random() * 1000).toFixed(1);
    var utime = (Math.random() * 100).toFixed(1);
    res.send({
        used_memory:umem,
        total_keys:tkey,
        client_count:clients,
        cmd_processed:cmdproc,
        uptime: utime
    });
});

app.listen(app.get('port'));
console.log('Server is listening on port ' + app.get('port') + '...');
