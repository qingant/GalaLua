
function amMeter(element) {
    this.update = repaint;
    this.resize = setSize;
    this.setValue = setValue;
    this.getType = function(){
        return 'amSpacer';
    }
    this.getSize = function(){
        return [canvas.width,canvas.height];
    }
    this.fixedSize = function() {
        return sizeFixed;
    }
    this.holder = function(){
        return canvas;
    }
    this.setName = function(text){
        name = text;
    }
    this.getName = function(){
        return name;
    }
    this.setLabel = function(text){
        label = text;
        repaint();
    }
    this.getLabel = function(){
        return label;
    }
    this.setUnit = function(text){
        unit = text;
        repaint();
    }
    this.setInterval = function(interval){
        if(timerID)
            clearTimeout(timerID);
        timerID = setInterval(updateValue,interval);
        timerInterval = interval;
    }
    this.getInterval = function(){
        return timerInterval;
    }
    this.setUrl = function(url){
        srcUrl = url;
    }
    this.getUrl = function(){
        return srcUrl;
    }

    var name = 'amMeter';
    var srcUrl = null;
    var timerID = 0;
    var timerInterval = 1000;
    var currValue = 0;
    var unit = '%';
    var label = 'CPU Loading'
    var canvas = document.getElementById(element);
    var context = canvas.getContext("2d");
    var width = canvas.width;
    var height = canvas.height;
    var radius = parseInt(Math.min(width,height) * 0.5);
    var shadowSize = parseInt(Math.max(radius * 0.04, 2));
    var xCent = width * 0.5;
    var yCent = height * 0.5;
    var sizeFixed = false;

    context.smoothing = true;
    context.antiliasing = true;

    radius -= shadowSize;

    repaint();

    function updateValue(){
        if(!srcUrl)
            return;
        $.ajax({
            url:srcUrl,
            dataType:"json",
            success:function(res) {
                var val = res.value[0];
                setValue(val);
            }
        });
    }

    function setValue(val){
        var count = 0;
        var step = (val - currValue) / 10;
        var id = setInterval(function(){
            currValue += step;
            repaint();
            if( ++count >= 10)
                clearTimeout(id);
        }, 15);
    }

    function setSize(w,h,fixed){
        if(width == w && height == h)
            return;
        if(w < 200)
            w = 200;
        if(h < 200)
            h = 200;
        sizeFixed = fixed || false;
        radius = parseInt(Math.min(w,h) * 0.5);
        canvas.width = w;
        canvas.height = h;
        canvas.style.width = canvas.width + 'px';
        canvas.style.height = canvas.height + 'px';

        width = canvas.width;
        height = canvas.height;

        shadowSize = parseInt(Math.max(radius * 0.04, 2));
        xCent = width * 0.5;
        yCent = height * 0.5;
        radius -= shadowSize;
        repaint();
    }

    function repaint(){
        context.clearRect(0,0,canvas.width,canvas.height);
        drawPanel();
        drawTicks();
        drawPointer();
        drawCenter();
        drawValueAndLabel();
    }

    function drawPanel(){
        context.save();
        context.translate(xCent,yCent);
        context.shadowColor = '#000';
        context.shadowBlur = shadowSize ;
        var grad = context.createRadialGradient(0,0,0,0,0,radius * 3);
        grad.addColorStop(1.0,'rgb(30,30,30)');
        grad.addColorStop(0.25,'rgb(30,30,30)');
        grad.addColorStop(0.0,'rgb(80,80,80)');
        context.fillStyle = grad;
        context.beginPath();
        context.arc(0, 0, radius, 0, Math.PI * 2);
        context.fill();
        context.restore();
    }

    function drawTicks(){
        var len = -radius * 0.8;
        var xPos= -radius + shadowSize * 1.5;
        var angle = Math.PI / 15;

        context.save();
        context.shadowBlur = 0;
        context.lineCap = 'round';
        context.strokeStyle='rgb(30,255,30)';
        context.lineWidth = shadowSize * 0.5;

        context.translate(xCent,yCent);
        context.rotate(-Math.PI/6);
        context.beginPath();
        context.moveTo(xPos,0);
        context.lineTo(len,0);
        for(var i = 1; i <= 10; ++i)
        {
            context.rotate(angle);
            context.moveTo(xPos,0);
            context.lineTo(len * 1.07,0);
            if( i == 7 ) {
                context.stroke();
                context.beginPath();
                context.strokeStyle="rgb(255,255,0)";
            }
            else if ( i == 9 ) {
                context.stroke();
                context.beginPath();
                context.strokeStyle="rgb(255,0,0)";
            }
            context.rotate(angle);
            context.moveTo(xPos,0);
            context.lineTo(len,0);
        }
        context.stroke();
        context.restore();

        angle = angle * 2;
        xPos = len + shadowSize * 3;
        var yPos = shadowSize;
        var fontSize = parseInt(shadowSize * 2.5);

        context.save();
        context.fillStyle='#ccc';
        context.font = (fontSize > 4 ? fontSize : 4) + "px Arial";
        context.translate(xCent,yCent);
        context.rotate(-Math.PI/6);
        context.translate(xPos, yPos);
        context.rotate(-Math.PI*0.5);
        context.fillText("00", 0,0);
        for(var j = 1; j <= 10; ++j)
        {
            context.rotate(Math.PI*0.5);
            context.translate(-xPos, -yPos);
            context.rotate(angle);
            context.translate(xPos, yPos);
            context.rotate(-Math.PI * 0.5);
            context.fillText(j * 10 + '', 0, 0);
        }
        context.restore();
    }

    function drawPointer(){
        var wid = shadowSize * 0.5;
        var len = radius * 0.85;
        var angle = (currValue * 2.4 - 120) / 180 * Math.PI;

        context.save();
        context.fillStyle='#d40000';
        context.shadowColor = '#000';

        context.translate(xCent,yCent);
        context.rotate(angle);
        context.beginPath();
        context.moveTo(0, -len);
        context.lineTo(0, -len);
        context.lineTo(wid, 0);
        context.lineTo(-wid, 0);
        context.lineTo(0, -len);
        context.fill();
        context.restore();
    }

    function drawCenter(){
        var rad = shadowSize * 2.5;
        context.save();
        context.translate(xCent,yCent);
        context.shadowColor = "rgb(20,20,20)";
        context.shadowBlur = 3;
        var grad = context.createLinearGradient(-rad,-rad,rad,rad);
        grad.addColorStop(0.0,"rgb(160,160,160)");
        grad.addColorStop(0.8,"rgb(40,40,40)");
        grad.addColorStop(1.0,"rgb(20,20,20)");
        context.fillStyle = grad;
        context.beginPath();
        context.arc(0, 0, rad, 0, Math.PI * 2);
        context.fill();
        context.restore();
    }

    function drawValueAndLabel(){
        var fontSize = parseInt(shadowSize * 2.5);
        context.save();
        context.fillStyle='#ccc';
        context.font = (fontSize > 4 ? fontSize : 4) + "px Arial";
        context.textAlign = 'center';
        context.translate(xCent,yCent);
        context.fillText(currValue.toFixed(1) + '', 0, -radius * 0.5, radius * 0.866);
        context.fillText('Unit: ' + unit, 0, -radius * 0.3, radius * 0.95);
        context.fillText(label, 0, radius * 0.5, radius * 0.866);
        context.restore();
    }
}

