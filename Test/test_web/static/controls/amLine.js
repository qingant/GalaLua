function amLine(element) {
    this.update = repaint;
    this.resize = setSize;
    this.setValue = setValue;
    this.getType = function(){
        return 'amLine';
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

    var name = 'amLineChart';
    var srcUrl = null;
    var timerID = 0;
    var label = 'amLineChart';
    var timerInterval = 1000;
    var canvas = document.getElementById(element);
    var context = canvas.getContext('2d');
    var sizeFixed = false;

    var data = {
        labels: ['00', '01', '02','03', '04', '05','06', '07', '08','09', '10'],
        datasets: [
            {
                label: "Line Chart1",
                fillColor : "rgba(95,178,255,0.5)",
                strokeColor : "rgba(95,178,255,1)",
                pointColor : "rgba(95,178,255,1)",
                pointStrokeColor : "#fff",
                pointHighlightFill : "#fff",
                pointHighlightStroke : "rgba(95,178,255,1)",
                data: [0,1,2,3,4,5,6,7,8,9,10]
            }
        ]
    };

    var options = {
        scaleShowGridLines : true,
        bezierCurve: false,
        scaleShowGridLines : true,
        scaleShowLabels: true,
        datasetStroke : true,
        datasetStrokeWidth : 1,
        animation: false,
        showTooltips: false
    }

    var lineChart = new Chart(context).Line(data, options);

    repaint();

    function repaint(){
        lineChart.update();
        drawTitle();
    }

    function updateValue(){
        if(!srcUrl)
            return;
        counter += 1;
        $.ajax({
            url:srcUrl,
            dataType:"json",
            success:function(res) {
                var val = res.value[0];
                setValue(val);
            }
        });
    }

    var counter = 15;
    function setValue(val){
        lineChart.addData([val],counter +'');
        lineChart.removeData();
        repaint();
    }

    function setSize(w,h,fixed){
        if(w < 200)
            w = 200;
        if(h < 200)
            h = 200;
        sizeFixed = fixed || false;
        canvas.width = w;
        canvas.height = h;
        canvas.style.width = canvas.width + 'px';
        canvas.style.height = canvas.height + 'px';
        lineChart.destroy();
        lineChart = new Chart(context).Line(data, options);
    }

    function drawTitle(){
        context.textAlign = 'center';
        context.fillStyle = 'black';
        context.font = '12pt Microsoft YeHei';
        context.fillText(label,canvas.width * 0.5, 20, canvas.width);
    }
}