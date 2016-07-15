<html>
<HEAD><TITLE>Photo bar</TITLE></HEAD>  
<body>
<?php
session_start();
$user = $_SESSION[users];
if($user != ""){
echo "login as $user ";
echo "<a href='logout.php'>Logout</a>"; 
}
?> 
<form action="login.php" method="POST">
UserName: <input type="text" name="name" />
Password: <input type="Password" name="password"  />
<input type="submit" value="Log-in"/>
</form>
<?php
echo "<a href='register.php'>New Account<center><a href='upload.php'>Upload a new photo</a></center>";
?>
</body>
</html>