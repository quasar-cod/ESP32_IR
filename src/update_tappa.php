<?php
  require 'database.php';
  if (!empty($_POST)) {
    date_default_timezone_set("Europe/Rome"); // Look here for your timezone : https://www.php.net/manual/en/timezones.php
    $board = $_POST['board'];
    $command = $_POST['command'];
    $pdo = Database::connect();
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "INSERT INTO esp32_command (board,command) values(?, ?)";
		$q = $pdo->prepare($sql);
		$q->execute(array($board,$command));

    $myObj = (object)array();
    $myObj->board =$board;
    $myObj->command = $command;
    $myJSON = json_encode($myObj);  
    echo $myJSON;
    // echo $sql;
    Database::disconnect();
  }
?>
