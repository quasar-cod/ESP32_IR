<?php
  require 'database.php';
  if (!empty($_POST)) {
    date_default_timezone_set("Europe/Rome"); // Look here for your timezone : https://www.php.net/manual/en/timezones.php
    $tm = date("H:i:s");
    $dt = date("Y-m-d");
    $board = $_POST['board'];
    $activity = $_POST['activity'];
    $pdo = Database::connect();
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "INSERT INTO esp32_activity (board,activity) values(?, ?)";
		$q = $pdo->prepare($sql);
		$q->execute(array($board,$activity));

    $myObj = (object)array();
    $myObj->board =$board;
    $myObj->activity = $activity;
    $myJSON = json_encode($myObj);  
    echo $myJSON;
    // echo $sql;
    Database::disconnect();
  }
?>