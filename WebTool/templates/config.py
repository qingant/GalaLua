$def with ()

<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <link rel="stylesheet" href="/static/basic.css" type="text/css" media="screen" charset="utf-8"/>
    <script type="text/javascript" src="/static/jquery.js"></script>
    <script type="text/javascript" src="/static/base.js"></script>
    <script type="text/javascript" src="/static/ajax.js"></script>
    <script type="text/javascript" src="/static/jquery.flot.js"></script>
    <script type="text/javascript">
    
      function SetWinHeight(obj) 
      { 
         var win=obj; 
         if (document.getElementById) 
         { 
            if (win && !window.opera) 
            { 
               if (win.contentDocument && win.contentDocument.body.offsetHeight) 
                  win.height = win.contentDocument.body.offsetHeight; 
               else if(win.Document && win.Document.body.scrollHeight) 
                  win.height = win.Document.body.scrollHeight; 
            } 
         } 
      }

      function loadAdminContent()
      {
         document.getElementById("contentInnerPage").src = "/cfg/admin/";
      }
   
      function loadGroupContent()
      {
         document.getElementById("contentInnerPage").src = "/cfg/groups/";
      }
    </script>
</head>

<body>

<div class="page">
   <div class="nav">
    <dl class="config">
        <dt><span>隐藏</span>配置</dt>
        <dd>
            <ul>
                <li><a href="javascript:loadAdminContent();" >系统配置</a></li>
                <li><a href="javascript:loadGroupContent();" >工作组配置</a></li>
                
            </ul>
        </dd>        
 
    </dl>
   </div>

   <iframe id="contentInnerPage" class="content" frameborder="0" marginheight="0" frameborder="0" src="/pageContent" width="680px" height="600px" scrolling="auto"></iframe>

</div>

    <script type="text/javascript">
        var dts=document.getElementsByTagName("dt");
        for(var i=0,j=dts.length;i<j;i++){
            var menu=dts[i].getElementsByTagName("span")[0],
                nextDD=dts[i].nextSibling;
            while(nextDD.nodeType != 1){
                nextDD=nextDD.nextSibling;
            }
            menu.onclick=(function(obj){
                return function(){
                    if(this.innerHTML=="隐藏"){
                        this.innerHTML ="展开";
                        obj.style.display="none";
                        if(this.parentNode.id=="last_dt"){
                            this.parentNode.className="last-dt";
                        }
                    } else{
                        this.innerHTML="隐藏";
                        obj.style.display="block";
                        if(this.parentNode.id=="last_dt"){
                            this.parentNode.className="";
                        }
                    }
                }
            })(nextDD);
        }


//chart zoon out 
    var bc = document.getElementById("bigChart");
    var dx,dy,mx,my,mouseD;
    var odrag;
    var isIE = document.all ? true : false;

    document.onmousedown = function(e){
       var e = e ? e : event;
       if(e.button == (document.all ? 1 : 0))
       {
          mouseD = true;
       }
    }

    document.onmouseup = function(){
       mouseD = false;
       odrag = "";
       if(isIE)
       {
          bc.releaseCapture();
          bc.filters.alpha.opacity = 100;
       }
       else
       {
          window.releaseEvents(bc.MOUSEMOVE);
          bc.style.opacity = 1;
       }
    }


    bc.onmousedown = function(e){
       odrag = this;
       var e = e ? e : event;
       if(e.button == (document.all ? 1 : 0))
       {
          mx = e.clientX;
          my = e.clientY;
          bc.style.left = bc.offsetLeft + "px";
          bc.style.top = bc.offsetTop + "px";
       if(isIE)
       {
          bc.setCapture();
          bc.filters.alpha.opacity = 50;
       }
       else
       {
          window.captureEvents(Event.MOUSEMOVE);
          bc.style.opacity = 0.5;
       }
       } 
   }

    document.onmousemove = function(e){
    var e = e ? e : event;

    if(mouseD==true && odrag)
    {
       var mrx = e.clientX - mx;
       var mry = e.clientY - my;
       bc.style.left = parseInt(bc.style.left) +mrx + "px";
       bc.style.top = parseInt(bc.style.top) + mry + "px";
       mx = e.clientX;
       my = e.clientY;

    } 
    }

   



    </script>
</body>
</html>
