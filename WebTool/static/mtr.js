var cpuinfo;
      var meminfo;     

      var xmlHttp;
      var xmlcpuhttp;

      function createcpuXMLHttpRequest() 
      {  
         if(window.ActiveXObject){  
            xmlcpuhttp = new ActiveXObject("Microsoft.XMLHTTP");  
         }  
         else if(window.XMLHttpRequest) {  
            xmlcpuhttp = new XMLHttpRequest();  
         }  
      }  

      function createXMLHttpRequest() 
         {  
            if(window.ActiveXObject){  
               xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");  
            }  
            else if(window.XMLHttpRequest) {  
               xmlHttp = new XMLHttpRequest();  
            }  
         }  
    
       function startCPURequest() 
         {  
            createcpuXMLHttpRequest();
            try{  
               xmlcpuhttp.open("GET", "/cpu", true);
               xmlcpuhttp.onreadystatechange = handleCPUStateChange;
               //xmlcpuhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
               xmlcpuhttp.send(null);   
            } 
            catch(exception){  
               alert("error");  
            }  
         } 

      function handleCPUStateChange() 
         {   
            if(xmlcpuhttp.readyState == 4){   
               if (xmlcpuhttp.status == 200 || xmlcpuhttp.status == 0){ 
                  cpuinfo = xmlcpuhttp.responseText;
               } 
            }
         }   

      function startMEMRequest() 
         {  
            createXMLHttpRequest();
            try{  
               xmlHttp.open("GET", "/mem", true);
               xmlHttp.onreadystatechange = handleMEMStateChange;
               //xmlHttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
               xmlHttp.send(null);   
            } 
            catch(exception){  
               alert("error");  
            }  
         } 
   
      function handleMEMStateChange() 
         {   
            if(xmlHttp.readyState == 4){   
               if (xmlHttp.status == 200 || xmlHttp.status == 0){ 
                  meminfo = xmlHttp.responseText;
               } 
            }
         }  
      

      var cpuBuf = [], memBuf = [], totalPoints = 104;
       
      var options = {xaxis:{show:false}};
 
      setInterval(function(){
         startCPURequest();
         startMEMRequest();
         if(cpuBuf.length > totalPoints)
            cpuBuf = cpuBuf.slice(cpuBuf.length-totalPoints);
         if(memBuf.length > totalPoints)
            memBuf = memBuf.slice(memBuf.length-totalPoints);
         cpuBuf.push(cpuinfo);
         memBuf.push(meminfo);
         var cpuArr = [], memArr = [];
         for (var i=0; i< cpuBuf.length; i++){
            cpuArr.push([i, cpuBuf[i]]);
            memArr.push([i, memBuf[i]]);
         }
         var cpuData = [{data:cpuArr, color:'black'}];
         var memData = [{data:memArr, color:'black'}];
         $$.plot($$("#cpuChart"), cpuData, options);
         $$.plot($$("#memChart"), memData, options);
         $$.plot($$("#bigCPU"), cpuData, options);
         $$.plot($$("#bigMEM"), memData, options);
      }, 2000)

      function record()
      {
         try{
            $$.get("/record", "record", function(){})
         }catch(exception){
            alert("error");
         }
         alert("Record it");
      }

      function resetall()
      {
         try{
            $$.get("/resetall", "reset", function(){})
         }catch(exception){
            alert("reset error");
         }
      }

      function dyniframesize()
      {
         var pTar = null;
         if(document.getElementById("contentPage")){
            pTar = document.getElementById("contentPage");
         }
         else{
            alert("error");
         }
         if(pTar && !window.opera){
            pTar.style.display="block";
            if(pTar.contentDocument && pTar.contentDocument.body.offHeight){
               pTar.height = pTar.contentDocument.body.offsetHeight + 20;
               pTar.width = pTar.contentDocument.body.scrollWidth + 20;
            }
            else if(pTar.Document && pTar.Document.body.scrollHeight){
               pTar.height = pTar.Document.body.scrollHeight;
               pTar.width = pTar.Document.body.scrollWidth;
            }
         }
      }     

      function loadAdminContent()
      {
         document.getElementById("contentPage").src = "/cfg/admin/";
      }
   
      function loadGroupContent()
      {
         document.getElementById("contentPage").src = "/cfg/groups/";
      }

 
     var prox;
     var proy;
     var proxc;
     var proyc;
     var openclose = false;
     function layer(id){
        if(openclose == false){
           openclose = true;
           show(id);
        }
        else{
           openclose = false;
           closeed(id);
        }
     }
   
     function show(id){/*--打开--*/
        clearInterval(prox);
        clearInterval(proy);
	clearInterval(proxc);
	clearInterval(proyc);
	var o = document.getElementById(id);
	o.style.display = "block";
	o.style.width = "1px";
	o.style.height = "1px"; 
	prox = setInterval(function(){openx(o,500)},10);
     }

     function openx(o,x){/*--打开x--*/
	var cx = parseInt(o.style.width);
	if(cx < x)
	{
		o.style.width = (cx + Math.ceil((x-cx)/5)) +"px";
	}
	else
	{
		clearInterval(prox);
		proy = setInterval(function(){openy(o,560)},10);
	}
     }

     function openy(o,y){/*--打开y--*/
	var cy = parseInt(o.style.height);
	if(cy < y)
	{
		o.style.height = (cy + Math.ceil((y-cy)/5)) +"px";
	}
	else
	{
		clearInterval(proy);
	}
    }

    function closeed(id){
	clearInterval(prox);
	clearInterval(proy);
	clearInterval(proxc);
	clearInterval(proyc);
	var o = document.getElementById(id);
	if(o.style.display == "block")
	{
		proyc = setInterval(function(){closey(o)},10);
	}
    }

    function closey(o){
	var cy = parseInt(o.style.height);
	if(cy > 0)
	{
		o.style.height = (cy - Math.ceil(cy/5)) +"px";
	}
	else
	{
		clearInterval(proyc);
		proxc = setInterval(function(){closex(o)},10);
	}
    }

    function closex(o){
	var cx = parseInt(o.style.width);
	if(cx > 0)
	{
		o.style.width = (cx - Math.ceil(cx/5)) +"px";
	}
	else
	{
		clearInterval(proxc);
		o.style.display = "none";
	}
    }