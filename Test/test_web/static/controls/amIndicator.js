function amIndicator(element) {
    this.resize = setSize;
    this.setValue = setValue;
    this.update = repaint;
    this.getType = function(){
        return 'amIndicator';
    }
    this.fixedSize = function(){
        return sizeFixed;
    }
    this.getSize = function(){
        return [canvas.width,canvas.height];
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
    }
    this.getLabel = function(){
        return label;
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

    var name = 'amIndicator';
    var srcUrl = null;
    var timerID = 0;
    var label = 'amIndicator'
    var timerInterval = 1000;
    var canvas = document.getElementById(element);
    var context = canvas.getContext('2d');
    var sizeFixed = false;
    var statusVal = 'Unknown';

    repaint();

    function repaint(){
        var height = canvas.height - 10;
        var width = canvas.width - 10;
        context.clearRect(0, 0, canvas.width, canvas.height);
        context.fillStyle = 'rgba(95,178,255,0.5)';
        context.fillRect(5, 5, width, height);
        context.textAlign = 'center';
        context.fillStyle = 'black';
        context.font = '12pt Microsoft YeHei';
        context.fillText(label, width * 0.5 + 5, height * 0.25 + 10, width);
        context.fillStyle = '#020299';
        context.font = '20pt Microsoft YeHei';
        context.fillText(statusVal, width * 0.5 + 5, height * 0.75 + 10, width);
    }

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
        statusVal = val + '';
        repaint();
    }

    function setSize(w,h,fixed){
        sizeFixed = fixed || false;
        if(w < 120)
            w = 120;
        if(h < 60)
            h = 60;
        canvas.width = w;
        canvas.height = h;
        canvas.style.width = canvas.width + 'px';
        canvas.style.height = canvas.height + 'px';
        repaint();
    }
}