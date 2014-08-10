require(
    ['echarts','echarts/chart/line'],
    function(ec) {

        var option = {
            title : {
                text: 'Memory Consumption',
            },            
            color: [ '#68bc53','#82bfd3'],
            // tooltip : {
            //     trigger: 'axis'
            // },
            legend: {
                x:'right',
                data:['Current','Max']
            },           
            xAxis : [
                {
                    type : 'category',
                    boundaryGap : false,
                    data : ['周一','周二','周三','周四','周五','周六','周日']
                }
            ],
            yAxis : [
                {
                    type : 'value',
                    axisLabel : {
                        formatter: '{value} MB'
                    },
                    splitArea : {show : true}
                }
            ],
            series : [
                {
                    name:'Current',
                    type:'line',
                    itemStyle: {
                        normal: {
                            lineStyle: {
                                width:3,
                                shadowColor : 'rgba(0,0,0,0.4)',
                                shadowBlur: 5,
                                shadowOffsetX: 3,
                                shadowOffsetY: 3
                            }
                        }
                    },
                    data:[0,0,0,0,0,0,0]
                },
                {
                    name:'Max',
                    type:'line',
                    itemStyle: {
                        normal: {
                            lineStyle: {
                                shadowColor : 'rgba(0,0,0,0.4)',
                                shadowBlur: 5,
                                shadowOffsetX: 3,
                                shadowOffsetY: 3
                            }
                        }
                    },
                    data:[0,0,0,0,0,0,0]
                }
            ]
        };

        var lineChart = ec.init(document.getElementById('line'));                                                           
        lineChart.setOption(option);

        function updateData(){
        
                    option.xAxis[0].data.push('周八');
                lineChart.setOption(option);
            $.ajax({
                url:"/line",
                dataType:"json",
                success:function(res) {
                    var val = res.data;
                    console.log(val);
                    lineChart.addData([[0,val,false,false]]);
                    lineChart.addData([[1,50,false,false]]);
                }
            });
        }

        var timer;
        clearInterval(timer);
        timer = setInterval( updateData, 1000 );
    }
); 