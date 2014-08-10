require(
    [
        'echarts',
        'echarts/chart/bar',
        'echarts/chart/line'
    ],
    function(ec) {

        var option = {
            // tooltip : {
            //     trigger: 'axis'
            // },
            color:['#008ed1'],
            legend: {
                data:['Top Commands']
            },
            calculable : true,
            xAxis : [
                {
                    type : 'category',
                    data : ['1月','2月','3月','4月','5月','6月','7月','8月','9月','10月','11月','12月']
                }
            ],
            yAxis : [
                {
                    type : 'value',
                    splitArea : {show : true}
                }
            ],
            series : [
                {
                    name:'Top Commands',
                    type:'bar', 
                    itemStyle: {
                        normal: {
                            areaStyle: {type: 'default',color:'rgb(35,141,215)'},
                            lineStyle:{color:'rgb(35,141,215)'}
                        },
                    },      
                    data:[2.0, 4.9, 7.0, 3.2, 5.6, 6.7, 5.6, 2.2, 3.6, 2.0, 6.4, 3.3]
                }
            ]
        };
        var bar = ec.init(document.getElementById('bar'));                                                 
        bar.setOption(option);

         function updateData(){        
             $.ajax({
                url:"/bar",
                dataType:"json",
                success:function(res) {
                    var val = res.data;
                    console.log(val);
                    bar.addData([[0,val,false,false]]);
                }
            });
        }

        var timer;
        clearInterval(timer);
        timer = setInterval( updateData, 1000 );
    }
); 