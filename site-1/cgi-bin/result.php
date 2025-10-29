<?php 

echo "<!DOCTYPE html>
<html>
<head>
<title>Test PHP</title>
<style>
		body {
			font-family: \"Noto Sans Mono\", sans-serif;
			max-width: 100vw;
			margin: 0 auto;
			padding: 20px;
		}

		table {
			border-collapse: collapse;
		}

		th {
			border: none;
		}

		td {
			border: none;
		}
	</style>
</head>
<body>
	";
	
	echo "_COOKIE: ";
	print_r ($_COOKIE);
	echo "<br><br>";
	
	echo "_ENV: ";
	print_r ($_ENV);
	echo "<br><br>";
	
	echo "_FILES: ";
	print_r ($_FILES);
	echo "<br><br>";

	echo "_GET: ";
	print_r ($_GET);
	echo "<br><br>";

	echo "_POST: ";
	print_r ($_POST);
	echo "<br><br>";

	echo "_REQUEST: ";
	print_r ($_REQUEST);
	echo "<br><br>";

	echo "_SERVER: ";
	print_r ($_SERVER);
	echo "<br><br>";

	echo "_SESSION: ";
	echo ($_SESSION);
	echo "<br><br>";

	echo "HTTP_RAW_POST_DATA: ";
	echo $HTTP_RAW_POST_DATA;
	echo "<br><br>";

echo "
</body>
</html>";

?>