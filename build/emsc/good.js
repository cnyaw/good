//
// 2017/7/12 Waync created.
//
var strUrl = location.search;
if (strUrl.indexOf("?") != -1) {
  var getSearch = strUrl.split("?");
  var getPara = getSearch[1].split("&");
  for (var i = 0; i < getPara.length; i++) {
    var param = getPara[i].split("=");
    if ("pkg" == param[0]) {
      Module.arguments = [param[1]];
    }
  }
}

function loadPkg(file, ccallName) {
  var xhr = new XMLHttpRequest();
  xhr.responseType = "arraybuffer";
  xhr.onload = function(e) {
    if (4 == xhr.readyState) {
      var arrayBuffer = xhr.response;
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
  };
  xhr.open("GET", "uploads/" + file, true);
  xhr.send(null);
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
  var s;
  if (0xffff < ch) {
    // Convert to surrogate pair.
    var h = (ch - 0x10000) / 0x400 + 0xd800;
    var l = (ch - 0x10000) % 0x400 + 0xdC00;
    s = String.fromCharCode(h) + String.fromCharCode(l);
  } else {
    s = String.fromCharCode(ch);
  }
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
