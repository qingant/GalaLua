function amBar(element) {
    this.resize = setSize;
    this.setValue = setValue;
    this.update = repaint;
    this.getType = function(){
        return 'amBar';
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

    var name = 'amBarChart';
    var srcUrl = null;
    var timerID = 0;
    var label = 'amBarChart'
    var timerInterval = 1000;
    var canvas = document.getElementById(element);
    var context = canvas.getContext('2d');
    var sizeFixed = false;

    var data = {
        labels: ['00', '01', '02','03', '04', '05','06', '07', '08','09', '10'],
        datasets: [
            {
                label: "Bar Chart1",
                fillColor: "rgba(95,178,255,0.5)",
                strokeColor: "rgba(95,178,255,0.8)",
                highlightFill: "rgba(95,178,255,0.75)",
                highlightStroke: "rgba(95,178,255,1)",
                data: [0,1,2,3,4,5,6,7,8,9,10]
            }
        ]
    };

    var options = {
        barShowStroke: false,
        animation: false,
        showTooltips: false
    }

    var barChart = new Chart(context).Bar(data, options);
    repaint();

    function repaint(){
        barChart.update();
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
        barChart.addData([val],counter +'');
        barChart.removeData();
        repaint();
    }

    function setSize(w,h,fixed){
        sizeFixed = fixed || false;
        if(w < 200)
            w = 200;
        if(h < 200)
            h = 200;
        canvas.width = w;
        canvas.height = h;
        canvas.style.width = canvas.width + 'px';
        canvas.style.height = canvas.height + 'px';
        barChart.width = canvas.width;
        barChart.height = canvas.height;
        barChart.destroy();
        barChart = new Chart(context).Bar(data, options);
    }

    function drawTitle(){
        context.textAlign = 'center';
        context.fillStyle = 'black';
        context.font = '12pt Microsoft YeHei';
        context.fillText(label,canvas.width * 0.5, 20, canvas.width);
    }
}