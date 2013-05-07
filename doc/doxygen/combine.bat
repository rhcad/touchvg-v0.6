set destpath=..\..\build\all

del /Q/S  %destpath%

xcopy ..\..\core\include\geom\*.* %destpath%\geom\ /Y
xcopy ..\..\core\include\graph\*.* %destpath%\graph\ /Y
xcopy ..\..\core\include\shape\*.* %destpath%\shape\ /Y
xcopy ..\..\core\include\command\*.* %destpath%\command\ /Y
xcopy ..\..\core\src\geom\*.* %destpath%\geom\ /Y
xcopy ..\..\core\src\graph\*.* %destpath%\graph\ /Y
xcopy ..\..\core\src\shape\*.* %destpath%\shape\ /Y
xcopy ..\..\core\src\command\*.* %destpath%\command\ /Y
xcopy ..\..\core\src\json\*.* %destpath%\json\ /Y

xcopy ..\..\win32\include\graphwin\*.* %destpath%\WinCanvas\ /Y
xcopy ..\..\win32\src\graphwin\*.* %destpath%\WinCanvas\ /Y
xcopy ..\..\win32\include\view\*.* %destpath%\MfcView\ /Y
xcopy ..\..\win32\src\ShapeEditor\*.* %destpath%\MfcApp\ /Y

xcopy ..\..\Ios\Graph2d\Headers\*.h %destpath%\IosView\ /Y
xcopy ..\..\Ios\Graph2d\Sources\*.* %destpath%\IosView\ /Y

mkdir %destpath%\IosCanvas
move /Y %destpath%\IosView\Ios*.* %destpath%\IosCanvas\

xcopy ..\..\Ios\FreeDraw\Headers\*.h %destpath%\IosApp\ /Y
xcopy ..\..\Ios\FreeDraw\Sources\*.* %destpath%\IosApp\ /Y
del /Q/S  %destpath%\IosApp\Test*.*

mkdir %destpath%\AndroidView
mkdir %destpath%\AndroidJNI
xcopy ..\..\android\graph2d\jni\*.h %destpath%\AndrSwig\ /Y
xcopy ..\..\android\graph2d\jni\Gi*.cpp %destpath%\AndrSwig\ /Y
xcopy ..\..\android\graph2d\src\touchvg\jni\*.java %destpath%\AndroidJNI\ /Y
xcopy ..\..\android\graph2d\src\touchvg\view\*.java %destpath%\AndroidView\ /Y
xcopy ..\..\android\graph2d\src\touchvg\demo\Testy.java %destpath%\AndroidApp\ /Y
