$def with (is_view)

$var title: 工作组配置
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <link rel="stylesheet" href="/static/content.css" type="text/css" media="screen" charset="utf-8"/>
    <script type="text/javascript" src="/static/jquery.js"></script>
    <script type="text/javascript" src="/static/base.js"></script>
    <script type="text/javascript" src="/static/jquery-ui.js"></script>
    <script type="text/javascript">
        
              
    function load_form(obj){
        document.getElementById("config_page").src = obj;
    }   
     
        
    function hide_help(){
        obj = document.getElementById("inner_help_bar");
        if(obj.style.display != "none")
            $$(obj).hide("slow");
        else
            $$(obj).show("fast");
    }       
    
    var queue = new Array();
    
    function push_to_queue(obj)
    {
        if(queue.length == 5){
            queue.shift();
            queue.push(obj);
        }
        else{
            queue.push(obj);
        }
    }    
    
    function show_help(obj){
        var parent_help_bar = document.getElementById("help_bar");
        if(parent_help_bar.style.display == "none"){
            $$(parent_help_bar).show("fast");
        }
                   
        var info = obj;
        push_to_queue(info);       
        parent_obj = document.getElementsByClassName("groups_help");
       
        var i = 0;
        var length = queue.length;
        
        for(i=0, j=length-1; i <length; i++, j--){
            parent_obj[i].innerHTML = queue[j];
            parent_obj[i].style.display = "block";
        }
   }
   
   
   
    </script>
</head>

<body>
<div class="content_show" id="content_show">
<div style="float:left;margin-left:5px;border:0px solid gray; width:1000px;">

<div style="float:left;margin-left:20px;position:relative;" scrolling="auto">
<dl class="config" id="nav_accordion">
    
        <dd>
            <ul>
                <li><a href="javascript:load_form('/cfg/groups/base/');" onMouseOver = "show_help('base');">基本配置</a></li>
                <li><a href="javascript:load_form('/cfg/groups/svc/')" onMouseOver = "show_help('svc');">处理器配置</a></li>
                <li><a href="javascript:load_form('/cfg/groups/lsr/')" onMouseOver = "show_help('lsr');">接入适配器列表</a></li>
                <li><a href="javascript:load_form('/cfg/groups/cnr/')" onMouseOver = "show_help('cnr');">接出适配器列表</a></li>
                <li><a href="javascript:load_form('/cfg/groups/log/')" onMouseOver = "show_help('log');">日志级别配置</a></li>
                <li><a href="javascript:load_form('/cfg/groups/logpattern/')" onMouseOver = "show_help('logpattern');">日志模式配置</a></li>
            </ul>
        </dd>


</dl>
</div>

<div class="help_bar" id="help_bar" style="display:block;">
    <div><a href="javascript:hide_help();" style="float:left;margin-top:5px;margin-left:5px;font-size:1em;text-decoration:none;color:black;background-color:#eeeeee;width:200px;">帮助信息栏</a></div>
    <div class="help_bar" id="inner_help_bar" style="display:block;">
        <div class="groups_help" style="display:none;background-color:white;"></div>
        <div class="groups_help" style="display:none;background-color:#e7e4ff;"></div>
        <div class="groups_help" style="display:none;background-color:#e7e4ff;"></div>
        <div class="groups_help" style="display:none;background-color:#e7e4ff;"></div>
        <div class="groups_help" style="display:none;background-color:#e7e4ff;"></div>   
    </div>
</div>



<div style="float:left; margin-left:20px;margin-top:0px;width:500px; height:auto;position:relative;">
    <iframe id="config_page"  frameborder="0" marginheight="0" frameborder="0" src="/pageContent" width="100%" height="600px" scrolling="no" onload="iFrameHeight();" ></iframe>
</div>

</div>

</div>
</div>


    <script type="text/javascript">

    function iFrameHeight(){
       var ifm = document.getElementById("config_page");
       var subWeb = document.frames ? document.frames["config_page"].document : ifm.contentDocument;
       if(ifm != null && subWeb != null){
           ifm.height = subWeb.body.scrollHeight;
       }
       
       if(document.body.scrollHeight > ifm.height)
           document.getElementById("content_show").style.height = document.body.scrollHeight;
       else
           document.getElementById("content_show").style.height = ifm.height;
                     
    }
    </script>
</body>
</html>




