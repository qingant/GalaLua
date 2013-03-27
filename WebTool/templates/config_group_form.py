$def with (is_view, form)

$var title: 工作组配置
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <link rel="stylesheet" href="/static/content.css" type="text/css" media="screen" charset="utf-8"/>
    <script type="text/javascript" src="/static/jquery.js"></script>
    <script type="text/javascript" src="/static/base.js"></script>
    <script type="text/javascript" src="/static/jquery-ui.js"></script>
    <script type="text/javascript">
    
    var count = 0;
    
    var queue = window.parent.queue;
    
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
    
    function show_form_help(obj){
        var parent_help_bar = parent.window.document.getElementById("help_bar");
        if(parent_help_bar.style.display == "none"){
            $$(parent_help_bar).show("fast");
        }
                   
        var info = $$(obj).attr("help_info");
        push_to_queue(info);       
        parent_obj = parent.window.document.getElementsByClassName("groups_help");
       
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
<div class="content_show_form">
<div style="float:left;margin-left:5px;border:0px solid gray; width:600px;">

<div style="float:left;margin-left:20px;" scrolling="auto">
</div>
<div style="float:left; margin-left:20px;margin-top:17px;">
<div class="group">
$if form:
   <form method="POST">
   $form.render()
   </form>

</div>
</div>

</div>
</div>

</body>
</html>
