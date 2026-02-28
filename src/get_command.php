<?php
  include 'database.php';
  if (!empty($_POST)) {
    $command = $_POST['command'];
    $myObj = (object)array();
    $pdo = Database::connect();
    $sql = 'SELECT c.board, c.activity, a.activity as last_activity FROM esp32_command c
    inner join esp32_activity a on a.board=c.board 
    WHERE command = "' . $command . '" and a.activity in ("UP","DOWN","OFF") order by a.id desc limit 1';
    $q = $pdo->prepare($sql);
    $q->execute();
    $row = $q->fetch();

    if ($row === false || $row === null) {
      // no matching command found, try to insert a new record
      try {
        // activity column is required in the table; insert an empty value
        $insertSql = 'INSERT INTO esp32_command (command, activity, board) VALUES (?, ?, ?)';
        $insertStmt = $pdo->prepare($insertSql);
        $ok = $insertStmt->execute(array($command, '', ''));
        if ($ok) {
          $myObj->messaggio = "non_riconosciuto";
          $myObj->created = true;
        } else {
          $myObj->messaggio = "insert_failed";
          $myObj->created = false;
        }
      } catch (PDOException $e) {
        $myObj->messaggio = "error";
        $myObj->error = $e->getMessage();
        $myObj->created = false;
      }
    } else {
        $myObj->board = $row['board'];
        $myObj->activity = $row['activity'];
        $myObj->last_activity = $row['last_activity'];
    }

    $myJSON = json_encode($myObj);
    echo $myJSON;
    //echo $sql;
    Database::disconnect();
  }
?>