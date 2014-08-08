var express = require('express');
var router = express.Router();

router.get('/', function(req, res) {
    res.render('index.html');
});

router.get('/meter', function(req, res) {
    var val = (Math.random() * 100).toFixed(1) + 50;
    console.log(val);
    res.send({value:[val]});
});

router.get('/meter1', function(req, res) {
    var val = (Math.random() * 100).toFixed(1) + 20;
    console.log(val);
    res.send({value:[val]});
});

router.get('/analysis', function(req,res){
   res.render('analysis.html')
});
module.exports = router;
