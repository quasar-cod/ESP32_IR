<?php
  include 'database.php';
  if (!empty($_POST)) {
    $command = $_POST['command'];
    $myObj = (object)array();
    $pdo = Database::connect();
    $sql = 'SELECT c.*, a.activity as last_activity FROM esp32_command c
    inner join esp32_activity a on a.board=c.board 
    WHERE command = "' . $command . '" and a.activity in ("UP","DOWN","OFF") order by a.id desc limit 1';
    $q = $pdo->prepare($sql);
    $q->execute();
    $row = $q->fetch();
    $myObj->board = $row['board'];
    $myObj->activity = $row['activity'];
    $myObj->last_activity = $row['last_activity'];
    $myJSON = json_encode($myObj);
    echo $myJSON;
    //echo $sql;
    Database::disconnect();
  }
?>