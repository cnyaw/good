set JAVA_HOME=C:\Program Files\Java\jdk1.6.0_03

call "c:\android-ndk-r9d\ndk-build" -C %~dp0\jni
@if ERRORLEVEL 1 goto Exit

call "C:\adt-bundle-windows-x86-20140321\sdk\tools\android" update project --path %~dp0
@if ERRORLEVEL 1 goto Exit

call "C:\adt-bundle-windows-x86-20140321\eclipse\plugins\org.apache.ant_1.8.4.v201303080030\bin\ant" -f %~dp0build.xml clean
@if ERRORLEVEL 1 goto Exit

call "C:\adt-bundle-windows-x86-20140321\eclipse\plugins\org.apache.ant_1.8.4.v201303080030\bin\ant" -f %~dp0build.xml debug
@if ERRORLEVEL 1 goto Exit

pause

:Exit
EXIT /B
