var express = require('express');
var bodyParser = require('body-parser');
var router = require('./routes/index');
var ejs = require('ejs');
var path = require('path');

var app = express();
module.exports = app;

app.set('port', process.env.PORT || 3000);
app.engine('.html', ejs.__express );
app.set('view engine', 'html');

app.set('views', path.join(__dirname, 'views'));
app.use(express.static(path.join(__dirname, 'statics')));

// body parser
app.use(bodyParser.urlencoded({extended:true}));
app.use(bodyParser.json());

app.use(router);

// catch 404 and forward to error handler
app.use(function(req, res) {
    console.log(req.body);
    res.status = 404;
    res.send('Access Deny!');
});

var server = app.listen(app.get('port'), function(){
    console.log('Server listening on port :', server.address().port, '...');
});
