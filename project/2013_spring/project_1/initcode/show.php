<HTML>
<BODY>
<?php
session_start();
$user = $_SESSION[users];
echo "login as $user ";
echo "<br/>You can ";
echo "<a href='logout.php'>Logout</a>";
?>

<?php  
if($user == ""){
?>
<center>
<?php echo "You haven't login, please login first!<br/>click ";?>
<?php echo "<a href='login.html'>here</a>";?>
<?php echo " to login";?>
</center>
</center>
<?php
mysql_close();
}
else{
$connect = mysql_connect('137.189.204.243:3306' , 'newusers' , 'sy8gyrje') or die("Unable to connect to MySQL server"); 
mysql_select_db( "db1") or die("Unable to select database"); 
$result=mysql_query("SELECT * FROM image_store where user = '$user'") or die("Can't Perform Query");
echo "<a href='http://appsrv.cse.cuhk.edu.hk/~sqhong2/upload.php'>Upload a new photo</a>";
While ($row=mysql_fetch_object($result)){
?>
<center>
<?php echo  "<img src=\"getdata.php?id=".$row->id."\"> <br>";?>
</center>
<?php
}
}
mysql_close($link);
?>

</BODY>
</HTML>