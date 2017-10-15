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
$imageFileType = pathinfo($target_file,PATHINFO_EXTENSION);
if($imageFileType != "good") {
  echo "Sorry, only good package (*.good) files are allowed.";
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

if (headers_sent()) {
  die("Redirect failed. Please click on this link: <a href='./index.php'>Home</a>");
}
else{
  exit(header("refresh:2;Location: index.php"));
}
?>
