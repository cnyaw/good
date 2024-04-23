<?php
/*
  2017/7/16 Waync created.
*/
$target_dir = "uploads/";
$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
$uploadOk = 1;
// Check file size
if ($_FILES["fileToUpload"]["size"] > 2000000) {
  echo "Sorry, your file is too large.";
  $uploadOk = 0;
}

// Allow certain file formats
$fileExt = pathinfo($target_file, PATHINFO_EXTENSION);
if ("good" != $fileExt && "zip" != $fileExt) {
  echo "Sorry, only good package (*.good,*.zip) files are allowed.";
  $uploadOk = 0;
}

// Check if $uploadOk is set to 0 by an error
if ($uploadOk == 0) {
  echo "Sorry, your file was not uploaded.";
// if everything is ok, try to upload file
} else {
  if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
    echo "The file ". basename( $_FILES["fileToUpload"]["name"]). " has been uploaded.";
  } else {
    echo "Sorry, there was an error uploading your file.";
  }
}

echo "<meta http-equiv='refresh' content='2;index.php'>";
?>
