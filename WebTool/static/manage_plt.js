
var xmlHttp;
var resultXMLHttp;
var commandURL="start"
var resultURL="/manage/admin/startresult/";

//创建XMLHttpRequest对象
function createXMLHttpRequest() {
    if (window.ActiveXObject) {
        try { xmlHttp = new ActiveXObject("Msxml2.XMLHTTP"); }
        catch (e) {
            xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");}             
    }
    //非IE
    else if (window.XMLHttpRequest) {
        xmlHttp = new XMLHttpRequest();
    }
}

function createResultXMLHttpRequest() {
    if (window.ActiveXObject) {
        try { 
            resultXMLHttp = new ActiveXObject("Msxml2.XMLHTTP"); }
        catch (e) {
            resultXMLHttp = new ActiveXObject("Microsoft.XMLHTTP");}                 }
    //非IE
    else if (window.XMLHttpRequest) {
        resultXMLHttp = new XMLHttpRequest();
    }
}
//Command请求发起函数
function startCommandRequest(request,url) {
    try {
        //alert("begin StartCommandRequest");
        requestType = request;
        createXMLHttpRequest();
        xmlHttp.onreadystatechange = handleCommandStateChange;
        xmlHttp.open("GET", url, true);
        xmlHttp.send(null);
    }catch(exception) {
        alert("StartCommandRequest Error");
    }
}
//查询请求发起函数
function startResultRequest(url) {
    try {
        createResultXMLHttpRequest();
        resultXMLHttp.onreadystatechange = handleResultStateChange;
        //resultXMLHttp.open("POST", url, true);
        resultXMLHttp.open("GET", url, true);
        resultXMLHttp.send(null);
    }catch(exception) {
        alert("StartResultRequest Error");
    }
}

//Command请求状态改变的时候触发的事件函数
function handleCommandStateChange() {
    if (xmlHttp.readyState==4 && xmlHttp.status==200) {
        startResultRequest(resultURL);
    }
}
//查询请求状态改变的时候触发的事件函数
function handleResultStateChange() {
    if (resultXMLHttp.readyState==4 && resultXMLHttp.status==200) {
        //这里继续请求，并且处理好返回数据
        textDoc=resultXMLHttp.responseText;
        if (textDoc!="Finish"){
            //alert(textDoc);
            document.getElementById("inner_content").innerHTML=textDoc;
            //$$("#inner_content").append(textDoc);
            startResultRequest(resultURL);
        }
    }
}


