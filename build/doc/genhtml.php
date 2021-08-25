<?php
//
// genhtml.php
// Auto generate goodapi.html from goodapi.json and goodapi_template.html.
//
// 2021/8/25 Waync created.
//
$html_template_file_name = 'goodapi_template.html';
$output_html_file_name = 'goodapi.html';
$api_doc_json_file_name = 'goodapi.json';

$html_lines = file($html_template_file_name, FILE_SKIP_EMPTY_LINES);
$json_lines = file($api_doc_json_file_name, FILE_SKIP_EMPTY_LINES);

$out_lines = '';

$insert_tag = '@@@good_api_json@@@';
for ($i = 0; $i < count($html_lines); $i++) {
  $line = $html_lines[$i];
  if (0 === strpos($line, $insert_tag)) {
    for ($j = 0; $j < count($json_lines); $j++) {
      $line = $json_lines[$j];
      $pos = strpos($line, "\r");
      if ($pos) {
        $line = substr_replace($line, '\\', $pos, 0);
      }
      $out_lines = $out_lines . $line;
    }
  } else {
    $out_lines = $out_lines . $line;
  }
}

file_put_contents($output_html_file_name, $out_lines);
