<html>
<!-- <?php //require_once('../../private/initialize.php');  ?> -->

<?php $page_title = 'Vital Signs'; ?>

<!-- COMMENT OUT IF YOU DON'T HAVE THE HEADER-->
<?php include('private/staff_header.php'); ?>

<?php

if(isset($_GET["temperature"])) {
   $temperature = $_GET["temperature"]; // get temperature value from HTTP GET

   $servername = "localhost";
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

    // Currently not recognizing isset(get(temp))
   if ($conn->query($sql) === TRUE) {
      echo "<p>New record created successfully\n\n</p>";
      echo $temperature;
      echo " degC";
   } else {
      echo "Error: " . $sql . " => " . $conn->error;
   }

   $conn->close();
} else {
   echo "temperature is not set";
} // fyi if any of this was outside the <?php ? > thing then could be html
// php only gives sends output to the browser whereas I think can show all of html stuff
?> 

</html>

<!-- COMMENT OUT IF YOU DON'T HAVE THE FOOTER-->
<div id="content"> </div> <!-- page break -->
<?php include('private/staff_footer.php'); ?>