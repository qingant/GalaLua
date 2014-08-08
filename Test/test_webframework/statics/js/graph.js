
require(
    [ 'echarts', 'echarts/chart/line' ],
    function(ec) {
        var option = {
            title : {
                text: 'Commands Processed'
            },
            // tooltip : {
            //     trigger: 'axis'
            // },
            color:['#00c3d1'],
            legend: {
                data:['Commands Processed']
            },
            calculable : true,
            xAxis : [
                {
                    type : 'category',
                    boundaryGap : false,
                    data : ['周一','周二','周三','周四','周五','周六','周日']
                }
            ],
            yAxis : [
                {
                    type : 'value'
                }
            ],
            series : [
                {
                    name:'Commands Processed',
                    type:'line',
                    smooth:false,
                    itemStyle: {
                        normal: { areaStyle: {type: 'default'} },
                    },
                    data:[10, 12, 21, 54, 60, 83, 71,99]
                }
            ]
        };  

        var graph = ec.init(document.getElementById('graph'));                                            
        graph.setOption(option);
        
        function updateData(){        
             $.ajax({
                url:"/graph",
                dataType:"json",
                success:function(res) {
                    var val = res.data;
                    graph.addData([[0,val,false,false]]);
                }
            });
        }

        var timer;
        clearInterval(timer);
        timer = setInterval( updateData, 1000 );
    }
);  