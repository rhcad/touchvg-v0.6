set destpath=..\..\build\all

del /Q/S  %destpath%

xcopy ..\..\core\include\geom\*.* %destpath%\geom\ /Y
xcopy ..\..\core\include\graph\*.* %destpath%\graph\ /Y
xcopy ..\..\core\include\shape\*.* %destpath%\shape\ /Y
xcopy ..\..\core\src\geom\*.* %destpath%\geom\ /Y
xcopy ..\..\core\src\graph\*.* %destpath%\graph\ /Y
xcopy ..\..\core\src\shape\*.* %destpath%\shape\ /Y

xcopy ..\..\win32\include\graphwin\*.* %destpath%\graphwin\ /Y
xcopy ..\..\win32\src\graphwin\*.* %destpath%\graphwin\ /Y
xcopy ..\..\win32\src\SimpView\*.* %destpath%\MfcApp\ /Y

mkdir %destpath%\GraphView.win
move /Y %destpath%\MfcApp\BaseView.* %destpath%\GraphView.win\
move /Y %destpath%\MfcApp\Step*View.* %destpath%\GraphView.win\
del  /Q %destpath%\MfcApp\StdAfx.* %destpath%\MfcApp\resource.h

xcopy ..\..\ios\Graph2d\Headers\*.h %destpath%\GraphView.ios\ /Y
xcopy ..\..\ios\Graph2d\Sources\*.* %destpath%\GraphView.ios\ /Y

mkdir %destpath%\CanvasIos
move /Y %destpath%\GraphView.ios\ios*.* %destpath%\CanvasIos\

xcopy ..\..\ios\FreeDraw\Headers\*.h %destpath%\iosApp\ /Y
xcopy ..\..\ios\FreeDraw\Sources\*.* %destpath%\iosApp\ /Y

mkdir %destpath%\AndroidView
xcopy ..\..\android\skiaview\jni\*.* %destpath%\AndroidView\ /Y
