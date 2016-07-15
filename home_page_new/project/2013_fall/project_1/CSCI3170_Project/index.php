<html>
<head>
<title>CSCI3170 Project</title>

<body>


<form name="form1" action="index.php" method="GET">
<p>Keyword<input type="input" name="keyword"></p>
<p><input type="checkbox" name="part_name"> By Part Name</p>
<p><input type="checkbox" name="category_name"> By Category Name</p>
<p><input type="checkbox" name="manufacturer_name"> By Manufacturer Name</p>
<p><input type="submit" value="Search"></p>
</form>


<?php
//showing error
error_reporting(E_ALL);
ini_set("display_errors",1);

//define a connection string
/*$dbstr="(DESCRIPTION=
	(ADDRESS_LIST=
		(ADDRESS=
			(PROTOCLOL=TCP)
			(HOST=db12.cse.cuhk.edu.hk)
			(PORT=1521)
		)
	)
	(CONNECT_DATA=
		//(SERVER=DEDICATED)
		(SERVER_NAME=db12.cse.cuhk.edu.hk)
	)
)";*/
$dbstr='137.189.88.200/db12.cse.cuhk.edu.hk';

//establish the connection
$conn=oci_connect("c056", "csci3170", $dbstr);//for demo
//$conn=oci_connect("c057", "csci3170", $dbstr);//for test
if(!$conn)
{
	echo "<script type='text/javascript'>alert('ERROR: Cannot establish the connection!')</script>";
}

//get values from the user
$keyword=isset($_GET['keyword'])?$_GET['keyword']:"";
$part_name=isset($_GET['part_name'])?$_GET['part_name']:"";
$part_name=isset($_GET['category_name'])?$_GET['category_name']:"";
$part_name=isset($_GET['manufacturer_name'])?$_GET['manufacturer_name']:"";
if($keyword=="" && isset($_GET['keyword']))
{
	echo "<script type='text/javascript'>alert('ERROR: Search key must be specified!')</script>";
	
}
if(!isset($_GET['part_name']) && !isset($_GET['category_name']) && !isset($_GET['manufacturer_name']) && isset($_GET['keyword']))
{
	echo "<script type='text/javascript'>alert('ERROR: Search option must be specified!')</script>";
}


//process SQL statements
$sql_part_1="SELECT p.pid, p.name AS p_name, c.name AS c_name, m.name AS m_name, p.available_quantity FROM category c, manufacturer m, part p WHERE p.category_id=c.cid AND p.manufacturer_id=m.mid";
$sql=$sql_part_1;

if(isset($_GET['part_name']) && isset($_GET['category_name']) && isset($_GET['manufacturer_name']))//By Part Name, Category Name, Manufacturer Name
{
	$sql_part_2=" AND (p.name LIKE '%$keyword%' OR c.name LIKE '%$keyword%' OR m.name LIKE '%$keyword%')";
	$sql=$sql.$sql_part_2;
}
if(isset($_GET['part_name']) && isset($_GET['category_name']) && !isset($_GET['manufacturer_name']))//By Pary Name, Category Name
{
	$sql_part_3=" AND (p.name LIKE '%$keyword%' OR c.name LIKE '%$keyword%')";
	$sql=$sql.$sql_part_3;
}
if(isset($_GET['part_name']) && !isset($_GET['category_name']) && isset($_GET['manufacturer_name']))//By Part Name, Manufacturer Name
{
	$sql_part_4=" AND (p.name LIKE '%$keyword%' OR m.name LIKE '%$keyword%')";
	$sql=$sql.$sql_part_4;
}
if(!isset($_GET['part_name']) && isset($_GET['category_name']) && isset($_GET['manufacturer_name']))//By Category Name, Manufacturer Name
{
	$sql_part_5=" AND (c.name LIKE '%$keyword%' OR m.name LIKE '%$keyword%')";
	$sql=$sql.$sql_part_5;
}
if(isset($_GET['part_name']) && !isset($_GET['category_name']) && !isset($_GET['manufacturer_name']))//By Part Name
{
	$sql_part_6=" AND p.name LIKE '%$keyword%'";
	$sql=$sql.$sql_part_6;
}
if(!isset($_GET['part_name']) && isset($_GET['category_name']) && !isset($_GET['manufacturer_name']))//By Category Name
{
	$sql_part_7=" AND c.name LIKE '%$keyword%'";
	$sql=$sql.$sql_part_7;
}
if(!isset($_GET['part_name']) && !isset($_GET['category_name']) && isset($_GET['manufacturer_name']))//By Manufacturer Name
{
	$sql_part_8=" AND m.name LIKE '%$keyword%'";
	$sql=$sql.$sql_part_8;
}

$sql_part_9=" AND p.available_quantity>0";
$sql=$sql.$sql_part_9;

$sql_part_10=" ORDER BY p.pid ASC";
$sql=$sql.$sql_part_10;

/*
$sql_part_1="SELECT p.p_name, p.p_budget FROM project p WHERE p.p_budget>0";
$sql=$sql_part_1;

if(isset($_GET['part_name']))//By Part Name
{
	$sql_part_2=" AND p.p_name LIKE '%$keyword%'";
	$sql=$sql.$sql_part_2;
}
if(isset($_GET['category_name']))//By Category Name
{
	$sql_part_3=" AND p.p_name LIKE '%$keyword%'";
	$sql=$sql.$sql_part_3;
}
if(isset($_GET['manufacturer_name']))//By Manufacturer Name
{
	$sql_part_4=" AND p.p_name LIKE '%$keyword%'";
	$sql=$sql.$sql_part_4;
}
*/


if(($keyword!="") && (isset($_GET['part_name']) || isset($_GET['category_name']) || isset($_GET['manufacturer_name'])))
{
	$stid=oci_parse($conn,$sql);
	oci_execute($stid);

	//display the result
	echo "
	<table>
	<tr>
	<th>Part ID </th>
	<th>Part Name </th>
	<th>Category </th>
	<th>Manufacturer </th>
	<th>Avaliable Quantity</th>
	</tr>
	<tr>";
	while($row=oci_fetch_object($stid))
	{
		
		echo "<tr><td>$row->PID</td><td>$row->P_NAME</td><td>$row->C_NAME</td><td>$row->M_NAME</td><td>$row->AVAILABLE_QUANTITY</td></tr>";
	}
	echo "</tr></table>";
}



//close the connection
oci_close($conn);
?>


</body>
</html>