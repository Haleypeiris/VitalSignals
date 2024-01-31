<?php

if(isset($_GET["temperature"])) {
   $temperature = $_GET["temperature"]; // get temperature value from HTTP GET

   $servername = "localhost"; //am I supposed to change these
   $username = "Arduino";
   $password = "ArduinoGetStarted.com";
   $dbname = "db_arduino";

   // Create connection
   $conn = new mysqli($servername, $username, $password, $dbname);
   // Check connection
   if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
   }

   $sql = "INSERT INTO tbl_temp (temp_value) VALUES ($temperature)";

   if ($conn->query($sql) === TRUE) {
      echo "Detected someone ";
      echo $temperature;
      echo " metres away";
   } else {
      echo "Error: " . $sql . " => " . $conn->error;
   }

   $conn->close();
} else {
   echo "temperature is not set - fix it";
}
?>
