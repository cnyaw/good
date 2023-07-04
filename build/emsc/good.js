//
// 2017/7/12 Waync created.
//

var Module = {
  canvas: (function() {return document.getElementById('canvas');})()
};

var strUrl = location.search;
if (strUrl.indexOf("?") != -1) {
  var getSearch = strUrl.split("?");
  var getPara = getSearch[1].split("&");
  for (var i = 0; i < getPara.length; i++) {
    var param = getPara[i].split("=");
    if ("pkg" == param[0]) {
      arguments = [param[1]];
    }
  }
}

function stringToArrayBuffer(str) {
  var arrbuf = new ArrayBuffer(str.length);
  var bufView = new Uint8Array(arrbuf);
  for (var i = 0; i < str.length; i++) {
    bufView[i] = str.charCodeAt(i);
  }
  return arrbuf;
}

function loadPkg(file, ccallName) {
  var xhr = new XMLHttpRequest();
  xhr.overrideMimeType('text/plain; charset=x-user-defined');
  xhr.open('GET', "uploads/" + file, false);
  xhr.send(null);
  if (200 === xhr.status) {
    var arrayBuffer = stringToArrayBuffer(xhr.response);
    if (arrayBuffer) {
      var bytes = new Uint8Array(arrayBuffer);
      var buf = Module._malloc(bytes.length);
      Module.HEAPU8.set(bytes, buf);
      Module.ccall(ccallName, 'number', ['number', 'number'], [buf, bytes.length]);
      Module._free(buf);
    }
  } else {
    Module.print("loadpkg " + file + " fail");
  }
}

function u32chToChar(ch) {
  if (0xffff < ch) {
    // Convert to surrogate pair.
    var h = (ch - 0x10000) / 0x400 + 0xd800;
    var l = (ch - 0x10000) % 0x400 + 0xdC00;
    return String.fromCharCode(h) + String.fromCharCode(l);
  } else {
    return String.fromCharCode(ch);
  }
}

function loadImageFromChar(size, ch, bAntiAlias) {
  var canvas = document.createElement('canvas');
  canvas.width = 2 * size;
  canvas.height = 2 * size;
  var ctx = canvas.getContext('2d');
  ctx.fillStyle = 'rgba(0,0,0,0)';
  ctx.fillRect(0, 0, canvas.width, canvas.height);
  ctx.fillStyle = 'white';
  ctx.font = size + 'px Arial';
  ctx.textBaseline = 'top';
  var s = u32chToChar(ch);
  var m = ctx.measureText(s);
  var w = m.width, h = size;
  ctx.fillText(s, 0, 2);
  var imgd = ctx.getImageData(0, 0, w, h);
  var pix = imgd.data;
  var bytes = new Uint8Array(pix);
  var buf = Module._malloc(bytes.length);
  Module.HEAPU8.set(bytes, buf);
  Module.ccall('cLoadImageFromChar', 'number', ['number', 'number', 'number', 'number'], [w, h, buf, bytes.length]);
  Module._free(buf);
}

var keyStates = [0,0,0,0,0,0,0,0]; // up/down/left/right/enter/esc/z/x.

function getKeyStates() {
  var mask = [1,2,4,8,16,512,32,64]; // up/down/left/right/enter/esc/z/x.
  var ks = 0;
  for (var i = 0; i < keyStates.length; i++) {
    if (0 != keyStates[i]) {
      ks += mask[i];
    }
  }
  return ks;
}

function keyDown(e) {
  updateKeyState(e, 1);
}

function keyUp(e) {
  updateKeyState(e, 0);
}

function updateKeyState(e, s) {
  switch (e.code)
  {
  case 'ArrowUp':
    keyStates[0] = s;
    break;
  case 'ArrowDown':
    keyStates[1] = s;
    break;
  case 'ArrowLeft':
    keyStates[2] = s;
    break;
  case 'ArrowRight':
    keyStates[3] = s;
    break;
  case 'Enter':
    keyStates[4] = s;
    break;
  case 'Escape':
    keyStates[5] = s;
    break;
  case 'KeyZ':
    keyStates[6] = s;
    break;
  case 'KeyX':
    keyStates[7] = s;
    break;
  default:
    return;
  }
  var keys = getKeyStates();
  Module.ccall('cSetKeyStates', 'number', ['number'], [keys]);
}

window.onkeydown = keyDown;
window.onkeyup = keyUp;
