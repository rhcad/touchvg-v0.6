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

mkdir %destpath%\graph2d.win
move /Y %destpath%\MfcApp\BaseView.* %destpath%\graph2d.win\
move /Y %destpath%\MfcApp\Step*View.* %destpath%\graph2d.win\
del  /Q %destpath%\MfcApp\StdAfx.* %destpath%\MfcApp\resource.h

xcopy ..\..\iPad\graph2d\Headers\*.h %destpath%\graph2d.ios\ /Y
xcopy ..\..\iPad\graph2d\Sources\*.* %destpath%\graph2d.ios\ /Y

mkdir %destpath%\graphios
move /Y %destpath%\graph2d.ios\ios*.* %destpath%\graphios\

xcopy ..\..\iPad\iPadDraw\Headers\*.h %destpath%\iPadApp\ /Y
xcopy ..\..\iPad\iPadDraw\Sources\*.* %destpath%\iPadApp\ /Y
