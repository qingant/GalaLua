function AjaxRequest()
{
   if(window.XMLHttpRequest()){
      try{
         this.request = new XMLHttpRequest();
      } catch(e){
         this.request = null;
      }
   }
   else if (window.ActiveXObject){
      try{
         this.request = new ActiveXObject("Msxml2.XMLHTTP");
      } catch(e) {
         try{
            this.request = new ActiveXObject("Microsoft.XMLHTTP");
         } catch(e) {
            this.request = null;
         }
      }
   }

   if(this.request == null)
      alert("ajax initlize error");
}

AjaxRequest.prototype.send = function(type, url, handler, postDataType, postData)
{
   if(this.request != null){
      this.request.abort();

      try{
         this.request.onreadystatechange = handler; //a function to handler data process
         this.request.open(type, url, true);  //ajax open function
      
         if(type.toLowerCase() == "get"){
            this.request.send(null);
         }
         else{
            this.request.setRequestHeader("Content-Type", postDataType);
            this.request.send(postData);
         }
      }catch(e){
         alert("ajax error communication\n" + e);
      }
   }
}

AjaxRequest.prototype.getReadyState = function(){
   return this.request.readyState;
}

AjaxRequest.prototype.getStatus = function(){
   return this.request.status;
}

AjaxRequest.prototype.getResponseText = function(){
   return this.request.responseText;
}

AjaxRequest.prototype.getResponseXML = function(){
   return this.request.responseXML;
}

var req = new AjaxRequest();

         
