$def with(type_dict)

<html>
<head>
    <meta http-equiv="Content-Type" content="text/html" charset="utf-8">
    <link rel="stylesheet" href="/static/content.css" type="text/css" />
    <link rel="stylesheet" href="/static/jquery-ui.css" />
    <style type="text/css">
    #tabs li { float:left; margin-right:1px; margin-left:10px;height:23px; list-style-type:none; display:block;}
    #tabs li a {text-decoration:none;font-size:1.2em;}
    p{font-size:1.2em;}
    </style>

    <script type="text/javascript" src="/static/jquery.js"></script>
    <script type="text/javascript" src="/static/jquery-ui.js"></script>
    <script type="text/javascript">
        
        $$(document).ready(function(){
            var rows = $$('table').find('tbody tr').length;
            var no_rec_per_page = 20;
            var no_pages = Math.ceil(rows / no_rec_per_page);
            var $$pagenumbers = $$('<div id="pages" ></div>');
            for(var i=0; i<no_pages; i++)
            {
                $$('<span class="process_page">' + (i+1)+'</span>').appendTo($$pagenumbers);
            }
            
            $$pagenumbers.insertAfter('table');
            $$('.process_page').hover(
                function(){
                    $$(this).addClass('process_hover');
                }, function(){
                    $$(this).removeClass('process_hover');
                }
            );
            
            
            $$('table').find('tbody tr').hide();
            var tr=$$('table tbody tr');
            for(var i=0; i<=no_rec_per_page-1; i++)
            {
                $$(tr[i]).show();
            }
            	
            $$('span').click(function(event){
                $$(this).css({"background-color":"gray"}).siblings().css({"background-color":"white"});
                $$('table').find('tbody tr').hide();
                for(var i = ($$(this).text() - 1) * no_rec_per_page; i<=$$(this).text() * no_rec_per_page-1; i++)
                {
                    $$(tr[i]).show();
                }
            });
            $$("tbody tr:even").css({"background-color":"#ddd9d9"});
            var total_td = $$('table').find('tbody td').length;
            for(var i=0; i<total_td; i++){
               if((i%6) == 0){
                   var str = "tbody td:eq(" + i + ")";   
                   
                   $$(str).css({"cursor":"pointer"});
                   
                   $$(str).hover(
                          function(){
                              $$(this).addClass("process_hover");
                          },
                          function(){
                              $$(this).removeClass("process_hover");
                          }
                   );
                   
                   $$(str).click(function(){
                      get_info($$(this).text());
                      $$(function(){
                          $$("#process_info_dialog").dialog({height:300, width:300, modal:true});
                       });	
                   });
               }
            } 
        }); 
        
        function get_info(pid){
            $$.get("/monitor/sys/processDetail",{"pid":pid}, function(data){
                $$("#process_info_dialog").html(data);
            }, "text");     
       }
       
       function filter(){
           data = $$("#info_filter").val();
           alert(data);
       }

       $$(function() {
           $$( "#tabs" ).tabs();
       });
     
    $$(function(){
    $for k in type_dict.keys():
        $$( "#$k" ).accordion({collapsible: true});
    });
    
    function change_value(obj){
        var t_value = $$("#type_value").val();
        window.location.href='/monitor/sys/process/' + t_value;
    }

    </script>
</head>
<body>
<div class="process_body">
 
    <div class="process_nav">
        <dt>节点</dt>
           <ul id="node_list">
              <li>Node_One</li>
              <li>Node_Two</li>
           </ul>
    </div>



    <div class="process_info_filter">
    <span>选择类型</span>
      <select onchange="change_value(this)" id="type_value">
          <option value="">------</option>
          <option value="dev_type">Dev_type</option>
          <option value="app_type">App_type</option>
          <option value="field">Field</option>
      </select>
    </div>

    <div class="process_content">
        <div id="tabs">
            <ul>
            $for k in type_dict.keys():
                <li><a href="#$k">$k</a></li>
            </ul>

            <div class="process_content"> 
            $for i in type_dict.keys():
                <div id="$i">
                $for v in type_dict[i]:
                    <dt style="font-size:1.2em;">$v['addr']['host']:$v['addr']['gpid']:$v['addr']['port']</dt>
                    <dd>
                    <p>Name: $v['name']</p>
                    <p>Dev_type:$v['dev_type']</p>
                    <p>App_type:$v['app_type']</p>
                    <p>Field:$v['field']</p>
                    <p>Userdata:$v['userdata']</p>
                    </dd>
                </div>
            </div>
        </div>
    </div>


</div>

<script>
</script>

</body>
</html>
