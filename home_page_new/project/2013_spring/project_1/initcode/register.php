<?php
session_start();
$user = $_SESSION[users];
if($user != ""){
	echo "Please log-out before register another account.<br/>";
	$_SESSION[url] = 'register.php';
	echo "<a href='logout.php'>Logout</a>";
}
else{
?>
<form action="register_implementation.php" method="POST">
UserName: <input type="text" name="name" />
<br/>
Password: <input type="Password" name="password"  />
<br/>
<input type="submit" value="Register"/>
</form>
<?php
}
?>