$def with()

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/static/jquery.js"></script>
<link rel="stylesheet" href="/static/admin_reg.css" type="text/css" media="screen" charset="utf-8"/>
<script>
</script>
</head>
<body>
   <div class="reg_block"> 
      <form action=/user/admin/register method=POST>
         <table id="login">
         <tr>
            <td>User: </td>
            <td><input type=text name='user'></td>
        </tr>
        <tr>
            <td>Password: </td>
            <td><input type="password" name=passwd></td>
        </tr>
        <tr>

        </tr>       
        <tr>
            <td>User Role: </td>
            <td><select name="user_role"><option value="admin">admin</option><option value="maintain">maintain</option><option value="config">config</option></select></td>
        </tr>  
        <tr>
           <td></td>
           <td><input type=submit value=REGISTER></td>
        </tr>
        </table>
      </form>
   </div>
</body>
</html>

