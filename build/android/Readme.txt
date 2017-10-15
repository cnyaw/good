How to build good
----------------------------------------
* cwd to c:/android-ndk-r9d
* cwd to d:/vs.net/good/build/android/jni
* execute c:/ndk-build to build JNI so
* launch ADT/eclipse and build android proj

* To fix lua bulid error: patch lau/llex.c:
  static void trydecpoint (LexState *ls, SemInfo *seminfo) {
  #ifndef _android_
    struct lconv *cv = localeconv();
  #endif
    char old = ls->decpoint;
  #ifdef _android_
    ls->decpoint = '.';
  #else
    ls->decpoint = (cv ? cv->decimal_point[0] : '.');
  #endif


How to export libgood.jar
----------------------------------------
* In Eclipse, right click on goodJNIActivity project.
* Click on Export...
* Select Java/JAR file
* Select need source files.
* Select export destination to ./libgood.jar.
* Click finish!


How to use libgood with new project
----------------------------------------
* Copy whole folder libs to new project folder
* Copy build_goodapp.bat to new project folder
* Import and create new android proj in eclipse
  # Command line: $(sdk_dir)/sdk/tools/android.bat update project -p . -t android-10
* First build in eclipse
* Run build_goodapp.bat
