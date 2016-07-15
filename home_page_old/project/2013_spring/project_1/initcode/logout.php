<?php
session_start();
unset($_SESSION[users]);
echo "Log out succeeded<br/>Back to former page in 3 seconds.<br/>";
if($_SESSION[url]!=""){
echo "<meta http-equiv=refresh content='3;url=$_SESSION[url]'>";
echo "<a href=$_SESSION[url]>Here</a>";
echo " if no response.";
$_SESSION[url]="";
}else{
echo "<meta http-equiv=refresh content='3;url=login.html'>";
echo "click ";
echo "<a href='login.html'>Here</a>";
echo " if no response.";
}
?>