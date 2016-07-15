<?php session_start();

?>
<body style="background-image:url(images/bg.jpg)">
<?php
$connect = mysql_connect('137.189.204.243:3306' , 'newusers' , 'sy8gyrje') or die("Unable to connect to MySQL server"); 
$_SESSION[links] = $link;

mysql_select_db('db1');
$name = isset($_POST['name']) ? $_POST['name']: "";	
$_SESSION[users] = $name;
$password = isset($_POST['password']) ? $_POST['password']: "";	
$password = intval($password);

$query = "SELECT count(*) FROM t_person WHERE (Fname = '$name' AND Fage = '$password')";
$result = mysql_query($query);
$row = mysql_fetch_row($result);

if($row[0] == 0){
	echo 'Sorry, username is not correspondent with password, try it again  (Turn on caps lock?)';
	echo "<a href='login.html'>Login Again<br/></a>";
	mysql_close($link);
?>
<?php
echo 'Do not have account?  click ';
echo "<a href='register.php'>here</a>";
echo " to register<br/>";
}
else{
	echo 'Login successfully.<br/>';
	echo 'page will turn to your photo album in 5 seconds...<br/>';
	echo "<meta http-equiv=refresh content='4;url=show.php'>";
	echo "click ";
	echo "<a href='show.php'>here</a>";
	echo " if it not jump automatically";

}

?>
</body>