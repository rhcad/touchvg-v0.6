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

xcopy ..\..\iPad\GraphView\Headers\*.h %destpath%\GraphView.ios\ /Y
xcopy ..\..\iPad\GraphView\Sources\*.* %destpath%\GraphView.ios\ /Y

xcopy ..\..\iPad\iPadDraw\Headers\*.h %destpath%\iPadApp\ /Y
xcopy ..\..\iPad\iPadDraw\Sources\*.* %destpath%\iPadApp\ /Y
