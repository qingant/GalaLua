$def with(groups)

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="/static/content.css" type="text/css" media="screen" charset="utf-8"/>
<script type="text/javascript" src="/static/jquery.js"></script>
<script type="text/javascript" src="/static/base.js"></script>
<script type="text/javascript" src="/static/jquery-ui.js"></script>

<script>
    function get_info(obj, obj1){
        $$.get('/manage/group_info', {"group":obj, "token":obj1}, function(data){$$("#manage_group_content").html(data) }, "text");
    }

    function group_control(obj0, obj1){
        var confirm_choice = confirm("Are you sure?", function(){}, null);
        if (confirm_choice){
            $$.get('/manage/group_control', 
                   {"group":obj0, "command":obj1}, 
                   function(data){
                       alert(data); 
                   },
                   "text");
        }
    }
        
    
    function start_process(obj0, obj1){
        var confirm_choice = confirm("Are you sure?", function(){}, null);
        if (confirm_choice){
            $$.get('/manage/control_start', 
                       {"group":obj0, "token":obj1}, 
                       function(data){
                           alert(data); 
                           var token = obj1.substring(0,3);
                           get_info(obj0, token); },
                            "text");
        }
    }
    
    function stop_process(obj0, obj1){
        var confirm_choice = confirm("Are you sure?", function(){}, null);
        if (confirm_choice){
            $$.get('/manage/control_stop', 
                       {"group":obj0, "token":obj1}, 
                       function(data){
                           alert(data);
                           var token = obj1.substring(0,3);
                           get_info(obj0, token);}, 
                           "text");
        }
    }
    
    $$(function(){
        $$("#manage_group_nav").accordion({collapsible:true});
    });
</script>
    
</head>
<body>
<div class="manage_group_page">
    <div class="manage_group_nav" id="manage_group_nav">
    $for group in groups:
        <dt>工作组:  $group[0]</dt>
            <ul id="$group[0]">
                <li><a href="javascript:get_info($group[0], 'cnr');">接出适配器</a></li>
                <li><a href="javascript:get_info($group[0], 'svc');">处理器</a></li>
                <li><a href="javascript:get_info($group[0], 'lsr');">接入适配器</a></li>
                <li><a href="javascript:group_control($group[0], 'start');">启动工作组</a></li>
                <li><a href="javascript:group_control($group[0], 'stop');">停止工作组</a></li>
            </ul>
    </div>

    <div class="manage_group_content" id="manage_group_content">
    </div>
    
</div>
</body>
</html>

