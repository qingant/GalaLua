$def with(title)

$var title: $title
<html>
   <head>
      <title>$title</title>
      <script type="text/javascript", src="/static/ajax.js"></script>
      <script type="text/javascript", src="/static/jquery.js"></script>
     <link rel="stylesheet" href="/static/basic.css" type="text/css" media="screen" charset="utf-8"/>
   </head>
   <body>
      <div class="login">
      <form id="login_form" name = "login_form" method="post" action="/user/admin/check">
      <p>User Name:</p><input id = "usrname" name = "usrname" type="text"></input><br />
      <p>Password:</p><input id = "passwd" name = "passwd" type = "password"></input><br />
      <input id = "loginbutton", type = "submit", name = "button" value="submit"></input>
      </form>
      </div>
   </body>
<html>
