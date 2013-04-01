$def with (content)

<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>$content.title</title>
    <link rel="stylesheet" href="/static/basic.css" type="text/css" media="screen" charset="utf-8"/>
    <script type="text/javascript" src="/static/jquery.js"></script>
    <script type="text/javascript" src="/static/base.js"></script>
    <script type="text/javascript" src="/static/jquery.flot.js"></script>
    
    <script type="text/javascript">
   
    function contentGoto(uri){
        document.getElementById("contentPage").src = uri;
    }
   
    $$(function(){
        $$('#nav li').hover(function(){
            $$(this).children('ul').stop(true, true).show('fast');
        }, function(){
            $$(this).children('ul').stop(true, true).hide('slow');
        });
    })
    
</script>
</head>

<body>
<div class = "infobar">
    <ul>       
       <li><a href = "/user/admin/register_page" style="color:white;">register</a></li>
       <li><a href = "/user/admin/login" style="color:white;">login</a></li>
       <li>Welcom <b>$session.username</b> as <b>$session.userrole</b></li>
    </ul>
</div>

   <div class="header">
      <a href = "/index/" style="font-style:bold; float:left;font-size:3.5em;margin-left:10px;margin-top:20px;text-decoration:none;color:black;">WEB配置监控工具</a>
      <a href = "http://www.agree.com.cn" style="font-style:italic;font-family:georgia, garamond, serif;float:right;font-size:1.5em;margin-right:100px;margin-top:80px;text-decoration:none;color:black;">www.agree.com.cn</a>                    
   </div>


<div style="z-index:1000;">
    <ul id="nav" class="first" >
        <li><a href="/index/">首页</a></li>
        
        <li><a href="#">配置选项</a>
 
           <ul id="sub_menu" style="display:none;">
              <li><a href = "javascript:contentGoto('/cfg/admin/');">系统配置</a></li>
              <li><a href = "javascript:contentGoto('/cfg/groups/');">工作组配置</a></li>
           </ul>

        </li>

        <li><a href="javascript:contentGoto('/monitor/sys/')">系统监控</a></li>

        <li><a href="#">应用监控</a>
             <ul id="sub_menu" style="display:none;">
                <li><a href = "javascript:contentGoto('/monitor/sys/process');">路由表信息</a></li>
                <li><a href = "javascript:contentGoto('/monitor/sys/node_status');">节点信息</a></li>
             </ul>           
        </li>
   
        <li><a href="#">平台管理</a>
 
           <ul id="sub_menu" style="display:none;">
              <li><a href = "javascript:contentGoto('/manage/admin/look/');">平台管理</a></li>
              <li><a href = "javascript:contentGoto('/manage/group');">工作组管理</a></li>
           </ul>
        </li>
    </ul>
</div>
    
   

   <div class="iframecontent">
      <iframe id="contentPage"  frameborder="0" marginheight="0" frameborder="0" src="/pageContent" width="100%" height="900px" scrolling="no" onLoad = "iFrameHeight();" ></iframe>
   </div>

<script type="text/javascript" language="javascript">
   function iFrameHeight(){
       var ifm = document.getElementById("contentPage");
       var subWeb = document.frames ? document.frames["contentPage"].document : ifm.contentDocument;
       if(ifm != null && subWeb != null){
           ifm.height = subWeb.body.scrollHeight;
       }
   }
</script>
</body>



</html>
