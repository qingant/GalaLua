var jquery=jQuery.noConflict();

var command_url="/manage/admin/start/"
var result_url="/manage/admin/start/"

$$(document).ready(function(){
    //设置一个ajax请求的默认值
    $$.ajaxSetup({
        url: "/manage/admin/start/",
        data: { "param": "AFA start up" },
        global: false,
        type: "GET",
        success: function(data, textStatus) { $("#divResult").html(data); }
    });

    //jQuery.get(url,data,success(data, textStatus, jqXHR),dataType)
    function sendCommand(_group, _token, _command) {
        $$.get(command_url, 
               {group:_group,token:_token,command:_command}, 
               function(data,textStatus,jqXHR){
                   //再次请求result
                   getResult(data);
               },
               ,"html");
    }

    //jQuery.post(url,data,success(data, textStatus, jqXHR),dataType)
    function getResult(result_id) {
        $$.post(result_url, 
                result_id, 
                function(data,textStatus,jqXHR){
                    //展示数据
                    alert(data);
                    //再次请求result
                    if (data!="Finish") {
                        getResult(data);
                    }},
                "html")
          );
    }

    function updateShow(_group,_token) {
        $$.get(command_url, 
               {group:_group,token:_token}, 
               function(data,textStatus,jqXHR){
                   $$.select(_group+_token)=data
               },
               ,"html");
    }
);


