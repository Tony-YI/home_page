<?php  
session_start();
$user = $_SESSION[users];
if(isset($_GET['id'])) {  
$id = $_GET['id'];
$connect = mysql_connect('137.189.204.243:3306' , 'newusers' , 'sy8gyrje') or die("Unable to connect to MySQL server"); 
mysql_select_db( "db1") or die("Unable to select database");
$query = "select bin_data,filetype from image_store where id=$id"; 
$result = @MYSQL_QUERY($query); 
$data = @MYSQL_RESULT($result,0, "bin_data");  
$type = @MYSQL_RESULT($result,0, "filetype"); 
Header( "Content-type: $type");  
echo $data;  
} 
mysql_close($link);
?>