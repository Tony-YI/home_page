<HTML>  
<HEAD><TITLE>Store binary data into SQL Database</TITLE></HEAD>  
<BODY> 
<?php
session_start();
$user = $_SESSION[users];
?>
<?php  

if($user == ""){
?>
<center>
<?php echo "You haven't login, please login first!<br/>click ";?>
<?php echo "<a href='login.html'>here</a>";?>
<?php echo " to login";?>
</center>
<?php
mysql_close();
}
else{
echo "login as $user<br/>";
if (isset($_POST['submit'])) {
$description = $_POST['description'];  
$data_name = $_FILES['data']['name']; 
$data_size = $_FILES['data']['size']; 
$data_type = $_FILES['data']['type'];
$data = $_FILES['data']['tmp_name']; 
 
$connect = mysql_connect('137.189.204.243:3306' , 'newusers' , 'sy8gyrje') or die("Unable to connect to MySQL server"); 
mysql_select_db( "db1") or die("Unable to select database");
$data = addslashes(fread(fopen($data, "r"), filesize($data)));

$result=MYSQL_QUERY( "INSERT INTO image_store (user,description,bin_data,filename,filesize,filetype) VALUES ('$user','$description','$data','$data_name','$data_size','$data_type')"); 
$id= mysql_insert_id();  
echo "Upload succeed!<br/> Back to album in 2 seconds...";
mysql_close();
echo "<meta http-equiv=refresh content='2;url=show.php'>";
} else { 

?>  
 <center>
<form method="post" action="upload.php" enctype="multipart/form-data">  
File Description:  
<input type="text" name="description" size="40">  
<INPUT TYPE="hidden" name="MAX_FILE_SIZE" value="1000000"> <br> 
File to upload/store in database:  
<input type="file" name="data" size="40">  
<p><input type="submit" name="submit" value="submit">  
</form>  
 </center>
<?php 
}
}
mysql_close($link);
?>  
</BODY>  
</HTML> 