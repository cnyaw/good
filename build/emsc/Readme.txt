* Added following line to emscripten generated good.html as below

        Module.setStatus = function(text) {
          if (text) Module.printErr('[post-exception status] ' + text);
        };
      };
    </script>
->> <script language="JavaScript" src="impl.js"></script> <----  HERE!!!
    <script>

        (function() {
          var memoryInitializer = 'good.html.mem';
          if (typeof Module['locateFile'] === 'function') {
            memoryInitializer = Module['locateFile'](memoryInitializer);
