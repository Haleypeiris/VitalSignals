<?php require_once('../../private/initialize.php'); ?>
<?php include(SHARED_PATH . '/staff_header.php'); ?>

<?php
//echo url_for('/staff/insert_values.php?measure='. $_GET["measure"]);
if(isset($_GET["yAcc"])) {
   $yAcc = $_GET["yAcc"]; // get temperature value from HTTP GET
   //echo $yAcc;
   $bed_status = $_GET["bed_status"];
   $yAccString = strval($_GET["yAccString"]);
   $yAccText = mysqli_real_escape_string($db, $yAccString);
   //$yAccString = mysqli_real_escape_string($db, $_GET["yAccString"]);
   echo $yAccString;
   /*$distance = $_GET["distance"];
   if $distance>0{ //maybe make this a jupyter thing?

   }*/
   $patient_id = 1; //CHANGE BACK TO GISELLE BC JUST TESTING ACCELERATION RN FOR GRAPHING
   $sql = "INSERT INTO acceleration (patient_id, y_acc, status, time_stamp, acc_array) VALUES ($patient_id, $yAcc, $bed_status, NOW(), $yAccString)";
   //$sql = "INSERT INTO giselle20 (patient_id, y_acc, x_acc, z_acc, time_stamp, acc_array) VALUES ($patient_id, $yAcc, $rr, 1290.00, NOW(), $yAccString)";
   //$sql = "INSERT INTO radar_sensor (patient_id, heart_rate, respiratory_rate, bed_status, time_stamp) VALUES ($patient_id, $hr, $rr, $distance, NOW())";

   //only relevant to see php page online
   
   if (mysqli_query($db,$sql) === TRUE) { //this is what runs that sql query insert btw
      //echo "Value from radar: " . $measure;
      echo "Current heart rate: ";
      $query = "SELECT * FROM radar_sensor ORDER BY id DESC LIMIT 1";
      $result = mysqli_query($db, $query); 
      $row = mysqli_fetch_array($result); //array
      echo $row['heart_rate']; //displays only heart rate; do print_r($row) to see whole row I think
      echo " bpm" . "<br />";
      echo "Timestamp: " . date("Y-m-d h:i:sa"). "<br />";
   } else {
      echo "Error: " . $sql . " => " . $db->error;
   }
   
} else {
   echo "measurement is not set - fix it";
}
?>
<!--<?php include(SHARED_PATH . '/staff_footer.php'); ?>-->
