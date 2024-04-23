<?php
/*
  2017/7/16 Waync created.
*/
echo '<!DOCTYPE html>';
echo '<html>';
echo '<head><meta charset="utf-8"><meta name="viewport" content="width=device-width"></head>';
echo '<body>';
echo '<form action="upload.php" method="post" enctype="multipart/form-data">';
  echo 'Select good package (*.good,*.zip) to upload:';
  echo '<input type="file" name="fileToUpload" id="fileToUpload">';
  echo '<input type="submit" value="Upload Package" name="submit">';
echo '</form>';
echo '<hr/>';
// List uploaded files of [uploads] folder.
echo "<table style='text-align:left'>";
echo '<tr><th>Name</th><th>Last modified</th><th>Size</th><th></th><th></th></tr>';
$dir    = './uploads';
$files = scandir($dir);
foreach($files as $file) {
  if (!is_dir("$dir/$file")) {
    echo '<tr>';
    echo '<td>' . "<a href='$dir/$file'>" . $file . '</a></td>';
    echo '<td>' . date("Y-m-d H:i", filemtime("$dir/$file")) . '</td>';
    echo '<td>' . filesize_formatted(filesize("$dir/$file")) . '</td>';
    echo "<td><a href='play.html?pkg=$file'>Play</a></td>";
    echo "<td><a href='edit.html?pkg=$file'>Edit</a></td>";
    echo '</tr>';
  }
}
echo '</table>';
echo '</body>';
echo '</html>';

function filesize_formatted($size)
{
  $units = array( 'B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB');
  $power = $size > 0 ? floor(log($size, 1024)) : 0;
  return number_format($size / pow(1024, $power), 2, '.', ',') . $units[$power];
}
?>
