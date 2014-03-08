<?php

$link = mysql_connect('137.189.204.243:3306' , 'newusers' , 'sy8gyrje') or die("Unable to connect to MySQL server"); 

mysql_select_db('db1');
$name = isset($_POST['name']) ? $_POST['name']: "";	
$password = isset($_POST['password']) ? $_POST['password']: "";	
$password = intval($password);
if($name == "" || $password == ""){
echo "Invalid information. Check if you have finish all the blank.<br/>";
echo "Back to register page in 2 seconds...";
echo "<meta http-equiv=refresh content='3;url=register.php'>";
}
else{
$query = "SELECT count(*) from t_person WHERE Fname = '$name'";
$result = mysql_query($query);
$row = mysql_fetch_row($result);
if($row[0] != 0){
	echo 'Sorry, username has been used, use another one please.<br/>';
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
else{
	$query = "INSERT INTO t_person (Fname,Fage) VALUES ('$name','$password')";
	$result = mysql_query($query);
	$affect = mysql_affected_rows();
	if($affect == 0){
		echo 'Register failed, try again.';
		echo "<meta http-equiv=refresh content='1;url=register.php'>";
	}
	else{
		echo 'Register succeeded! Please log in.';
		echo "<meta http-equiv=refresh content='2;url=login.html'>";
	}
}
	
mysql_close($link);
}
?>