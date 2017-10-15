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
