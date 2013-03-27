$def with ()

<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <link rel="stylesheet" href="/static/cpu_mem.css" type="text/css" media="screen" charset="utf-8"/>
    <script type="text/javascript" src="/static/jquery.js"></script>
    <script type="text/javascript" src="/static/base.js"></script>
    <script type="text/javascript" src="/static/jquery.flot.js"></script>
    <script type="text/javascript" src="/static/jquery-ui.js"></script>
    <script type="text/javascript">
    var cpuinfo;
    var meminfo;     
        
    var isCpu = 0;
    var isMem = 0;
        
    var cpuBuf = [], memBuf = [], totalPoints = 104;
       
    var options = {xaxis:{show:false}};
    
    var cpu_top;
    var cpu_left;
    var mem_top;
    var mem_left;
    
    var data_cpu_top;
    var data_cpu_left;
    var data_mem_top;
    var data_mem_left;
        
    setInterval(function()
    {
        if(isCpu == 1){
            $$.get('/monitor/sys/widget/cpu', function(data){
            cpuinfo = data;
            }, "Text");
            if(cpuBuf.length > totalPoints)
                cpuBuf = cpuBuf.slice(cpuBuf.length-totalPoints);
         
            var cpu_max_info = parseFloat(cpuinfo) + parseFloat(cpuinfo)*0.1;
            var cpu_min_info;
            
            if((parseFloat(cpuinfo)-parseFloat(cpuinfo)*0.1) > 0)
                cpu_min_info = parseFloat(cpuinfo) - parseFloat(cpuinfo)*0.1;
            else
                cpu_min_info = 0;
                
            cpuBuf.push(cpuinfo);    
            var cpuArr = [];    
                
            for (var i=0; i< cpuBuf.length; i++){
                cpuArr.push([i, cpuBuf[i]]);
            }    
            
            var options_cpu = {xaxis:{show:false},
                                            yaxis:{max:cpu_max_info, min:cpu_min_info},
                                          };
            var cpuData = [{data:cpuArr, color:'black', shadowSize:'DEDFA8'}];
            $$.plot($$("#bigCPU"), cpuData, options_cpu);
        }
        else
            return 0;
    }, 2000)                                    
                
    setInterval(function()
    {
        if(isMem == 1){
            $$.get('/monitor/sys/widget/mem', function(data){
                meminfo = data;
            }, "Text");
            if(memBuf.length > totalPoints)
                memBuf = memBuf.slice(memBuf.length-totalPoints);
           
            var mem_max_info = parseFloat(meminfo) + parseFloat(meminfo)*0.1;
            var mem_min_info;
          
            if((parseFloat(meminfo)-parseFloat(meminfo)*0.1) > 0)
                mem_min_info = parseFloat(meminfo) - parseFloat(meminfo)*0.1;
            else
                mem_min_info = 0;
                
            memBuf.push(meminfo);    
            var memArr = [];    
                
            for (var i=0; i< memBuf.length; i++){
                memArr.push([i, memBuf[i]]);
            }    
            
            var options_mem = {xaxis:{show:false},
                                              yaxis:{max:mem_max_info, min:mem_min_info},
                                              };
            var memData = [{data:memArr, color:'black', shadowSize:'DEDFA8'}];
            $$.plot($$("#bigMEM"), memData, options_mem);
        }
        else
            return 0;
    }, 2000)     




    function record()
    {
        try{
            $$.get("/monitor/sys/record", "record", function(){})
        }catch(exception){
            alert("error");
        }
        alert("Record it");
    }

    function resetall()
    {
        try{
            $$.get("/monitor/sys/resetall", "reset", function(){});
            $$.get("/monitor/sys/widget/meminfo", "mem_info", function(data){
                document.getElementById("mem_info").innerHTML = data;
            }, "text");
        }catch(exception){
            alert("reset error");
        }
    }

    function info_display_cpu(obj){
        if(isCpu == 0)
            isCpu = 1;
        
        var init_page = document.getElementById("init_content");
        if(init_page.style.display != "none"){
            init_page.style.display = "none";
        }
      
        $$("#"+obj).show("slow");
    }      
   
    function info_display_mem(obj){
        if(isMem == 0)
            isMem = 1;
        
        var init_page = document.getElementById("init_content");
        if(init_page.style.display != "none"){
            init_page.style.display = "none";
        }
      
        $$("#"+obj).show("slow");
    }   
   
    function hide_cpu(obj){
        cpu_top = $$("#inner_cpu_info_pic").offset().top;
        cpu_left = $$("#inner_cpu_info_pic").offset().left;
        isCpu = 0;    
        if(document.getElementById(obj).style.display != "none")
            $$("#"+obj).hide("slow");
    }
    
    function hide_mem(obj){
        mem_top = $$("#inner_mem_info_pic").offset().top;
        mem_left = $$("#inner_mem_info_pic").offset().left;
        isMem = 0;    
        if(document.getElementById(obj).style.display != "none")
            $$("#"+obj).hide("slow");
    }    
    
    
    $$(function(){  
        $$("#inner_cpu_info_pic").draggable({containment:"#convas_content", scroll:false});  
        $$("#inner_mem_info_pic").draggable({containment:"#convas_content", scroll:false});
    }); 

    function show_submenu(){
        if(document.getElementById("sub_nav").style.display == "none")
            $$("#sub_nav").show("slow");
        else
            $$("#sub_nav").hide("slow");        
    }
    
    
    function save_layout(){
        if(isCpu == 0){
            data_cpu_top = 0;
            data_cpu_left = 220;
        }
        else{
            data_cpu_top = $$("#inner_cpu_info_pic").offset().top;
            data_cpu_left = $$("#inner_cpu_info_pic").offset().left;
        }
        
        if(isMem == 0){
            data_mem_top = 150;
            data_mem_left = 220;
        }
        else{
            data_mem_top = $$("#inner_mem_info_pic").offset().top;
            data_mem_left = $$("#inner_mem_info_pic").offset().left;
        }
        
        $$.getJSON("/monitor/sys/layout", 

                     {
                        user:"$session.username",
                        c_top:data_cpu_top,
                        c_left:data_cpu_left,
                        m_top:data_mem_top,
                        m_left:data_mem_left,
                        c_bloon:isCpu,
                        m_bloon:isMem
                      },
                       
                      function(data){
                      });
                      
    }
    
    function load_layout(){
        var cpu_obj = document.getElementById("inner_cpu");
        var mem_obj = document.getElementById("inner_mem")
        $$.getJSON("/monitor/sys/load_current_layout",

                         {
                           user:"$session.username"
                          },
                          
                          function(data){
                              cpu_obj.style.top = data.c_top + 'px';
                              cpu_obj.style.left = data.c_left + 'px';
                              mem_obj.style.top = data.m_top + 'px';
                              mem_obj.style.left = data.m_left + 'px';
                              isCpu = data.c_bloon;
                              isMem = data.m_bloon;
                              var init_page = document.getElementById("init_content");
                              if(init_page.style.display != "none"){
                                  init_page.style.display = "none";
                              }
                              if(isCpu == 1){
                                  $$("#inner_cpu").show("slow");
                              }
                              if(isMem == 1){
                                  $$("#inner_mem").show("slow");
                              }
                          });
       }
                          
    </script>
    
</head>

<body onload="resetall();load_layout()">
<div class="CM_page">
   <div class="list_block">
   <ul class="inner_nav">
        <li><a href="javascript:show_submenu()">硬件信息</a></li>
            <ul id="sub_nav" class="sub_nav" style="display:none;">
                <li><a href="javascript:info_display_cpu('inner_cpu')">CPU信息</a></li>
                <li><a href="javascript:info_display_mem('inner_mem')">Memory信息</a></li>
            </ul>
        <li><a href="javascript:save_layout();">保存布局</a></li>
    </ul>
   </div>

<div class="convas_content" id="convas_content" style="width:780px; height:700px;float:right;box-shadow:0px 0px 3px #222;margin-top:5px;">

   <div class="init_content" id="init_content">
      <h2>这里是监控页面， 您可以在这里查看系统的CPU信息和内存信息， 并且实时监控CPU和内存的使用率和空闲率. </h2>
   </div>
   
   <div class="inner_cpu_info" id="inner_cpu">
      <div class="inner_cpu_info_pic" id="inner_cpu_info_pic">
          <div style="text-align:center;font-size:0.85em">CPU使用率<a href="javascript:hide_cpu('inner_cpu');" style="float:right;margin-top:10px;font-size:0.8em;text-decoration:none;">close</a></div>
          <div class="bigCPU" id="bigCPU"></div>   
      </div>
   </div>
   
   <div class="inner_mem_info" id="inner_mem">
      <div class="inner_mem_info_pic" id="inner_mem_info_pic">
       <div style="text-align:center;font-size:0.85em">内存空闲率<a href="javascript:hide_mem('inner_mem');" style="float:right;margin-top:10px;font-size:0.8em;text-decoration:none;">close</a></div>
       <div class="bigMEM" id="bigMEM"></div>
      </div>
   </div>

   
</div>

</div>
            <div class="inner_mem_info_text" id="inner_mem_info_text" style="width:200px; float:left;">
         <p id="mem_info">This should be the Memory infomation.</p> 
      </div>

</body>
</html>
