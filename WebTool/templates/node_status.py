$def with(return_form, form_title)

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
        }); 
       
           
     
    </script>
</head>
<body>

<div class="process_body">

<div class="process_nav">
     
       <ul id="node_list">
          <li><span><a href = "/monitor/sys/node_status/lsr">LSR</a></span></li>
          <li><span><a href = "/monitor/sys/node_status/svc">SVC</a></span></li>
       </ul>
</div>

<div>
<h3>$form_title</h3>
</div>


<div class="process_content" id="svc_form">
   $if return_form:
      $return_form.render()
</div>

</div>

<script>
 
</script>

</body>
</html>
