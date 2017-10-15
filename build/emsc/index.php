<?php
/*
  2017/7/16 Waync created.
*/
if (isset($_GET['p'])) {
  // Redirect to play specified package.
  $file = $_GET['p'];
  header('Location: good.html?pkg=' . $file);
  exit;
} else {
  // Upload file form.
  echo '<!DOCTYPE html>';
  echo '<html>';
  echo '<body>';
  echo '<form action="upload.php" method="post" enctype="multipart/form-data">';
    echo 'Select good package (*.good) to upload:';
    echo '<input type="file" name="fileToUpload" id="fileToUpload">';
    echo '<input type="submit" value="Upload Package" name="submit">';
  echo '</form>';
  echo '<hr/>';
  // List uploaded files of [uploads] folder.
  echo "<table style='text-align:left'>";
  echo '<tr><th>Last Mod</th><th>File Size</th><th>Pkg Name</th><th></th></tr>';
  $dir    = './uploads';
  $files = scandir($dir);
  foreach($files as $file) {
    if (!is_dir("$dir/$file")) {
      echo '<tr>';
      echo '<td>' . date("F d Y H:i:s", filemtime("$dir/$file")) . '</td>';
      echo '<td>' . filesize_formatted(filesize("$dir/$file")) . '</td>';
      echo '<td>' . "<a href='$dir/$file'>" . $file . '</a></td>';
      echo "<td><a href='index.php?p=$file'>Play</a></td>";
      echo '</tr>';
    }
  }
  echo '</table>';
  echo '</body>';
  echo '</html>';
}

function filesize_formatted($size)
{
  $units = array( 'B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB');
  $power = $size > 0 ? floor(log($size, 1024)) : 0;
  return number_format($size / pow(1024, $power), 2, '.', ',') . $units[$power];
}
?>
