<?php
//
// gendoc.php
// Auto generate goodapi.json from script.h for goodapi.html.
//
// 2021/8/25 Waync created.
//

$script_src_file_name = '../../src/good/rt/script.h';
$api_doc_json_file_name = 'goodapi.json';
$lines = file($script_src_file_name, FILE_SKIP_EMPTY_LINES);

ob_start();

echo '{"modules":[';

$first_module = true;
$first_api = true;

for ($i = 0; $i < count($lines); $i++) {
  $line = $lines[$i];
  $name = '';
  if (isModule($line, $name)) {
    parseModule($lines, $i, $name);
  }
}

echo ']}';

$echo_str = ob_get_contents();

ob_end_clean();

file_put_contents($api_doc_json_file_name, $echo_str);

function isApi($line) {
  $pos = 0;
  return isTag($line, '[API]', $pos);
}

function isApiEnd($line) {
  $pos = 0;
  return isTag($line, '[/API]', $pos);
}

function isComment($line, &$pos) {
  $pos = strpos($line, '//');
  return FALSE !== $pos;
}

function isModule($line, &$name) {
  $pos = 0;
  $tag = '[Module]';
  if (isTag($line, $tag, $pos)) {
    $name = trimSpaces(substr($line, $pos + strlen($tag) + 1));
    return $pos;
  }
  return FALSE;
}

function isTag($line, $tag, &$pos) {
  $pos = 0;
  if (isComment($line, $pos)) {
    $pos = strpos($line, $tag, $pos);
    return FALSE !== $pos;
  }
  return FALSE;
}

function parseApi($lines, &$i) {
  global $first_api;
  if ($first_api) {
    $first_api = false;
  } else {
    echo ',';
  }
  echo '{';
  for ($i++; $i < count($lines); $i++) {
    $line = $lines[$i];
    if (isApiEnd($line) || !isComment($line, $pos)) {
      break;
    }
    echo substr($line, $pos + 3);
  }
  echo '}';
}

function parseModule($lines, &$i, &$name) {
  global $first_module, $first_api;
  if ($first_module) {
    $first_module = false;
  } else {
    echo ',';
  }
  $first_api = true;
  echo '{"name":"' . $name . '",';
  echo '"api":[';
  for ($i++; $i < count($lines); $i++) {
    $line = $lines[$i];
    if (isApi($line)) {
      parseApi($lines, $i);
    } else if (isModule($line, $name)) {
      $i -= 1;
      break;
    }
  }
  echo ']}';
}

function trimSpaces($str) {
  return str_replace(array("\n", "\r", "\t", "　", " "), '', strip_tags($str));
}
