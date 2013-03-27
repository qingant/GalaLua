$def with (is_view, form)

$var title: 管理功能配置

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <link rel="stylesheet" href="/static/content.css" type="text/css" media="screen" charset="utf-8"/>
    <script type="text/javascript" src="/static/jquery.js"></script>
    <script type="text/javascript" src="/static/base.js"></script>

</head>

<body>

<div class="content_show" style="border:1px solid white;">
$if is_view:
    <form action="/cfg/admin/edit/" method="GET">
$else:
    <form action="/cfg/admin/edit/" method="POST">
$:form.render()
</form>

</div>

<div class="admin_help" id="admin_help" style="display:none;">
   <h1> help information </h1>
</div>


    <script type="text/javascript">
        $$(function(){
            $$('#port').hover(function(){
                $$('#admin_help').show('fast');
            }, function(){
                $$('#admin_help').hide('fast');
            });
        })
    </script>

</body>
</html>
