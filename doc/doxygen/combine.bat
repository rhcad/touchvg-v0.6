set destpath=..\..\build\touchdraw

xcopy ..\..\core\include\geom\*.* %destpath%\geom\ /Y
xcopy ..\..\core\include\graph\*.* %destpath%\graph\ /Y
xcopy ..\..\core\include\shape\*.* %destpath%\shape\ /Y
xcopy ..\..\core\src\geom\*.* %destpath%\geom\ /Y
xcopy ..\..\core\src\graph\*.* %destpath%\graph\ /Y
xcopy ..\..\core\src\shape\*.* %destpath%\shape\ /Y

xcopy ..\..\win32\include\graphwin\*.* %destpath%\graphwin\ /Y
xcopy ..\..\win32\src\graphwin\*.* %destpath%\graphwin\ /Y
xcopy ..\..\win32\src\SimpView\*.* %destpath%\MfcApp\ /Y

xcopy ..\..\iPad\Graph2d\Headers\*ios.h %destpath%\graphios\ /Y
xcopy ..\..\iPad\Graph2d\Sources\*.cpp %destpath%\graphios\ /Y

xcopy ..\..\iPad\GraphView\Headers\*.h %destpath%\viewios\ /Y
xcopy ..\..\iPad\GraphView\Sources\*.* %destpath%\viewios\ /Y

xcopy ..\..\iPad\iPadDraw\Headers\*.h %destpath%\viewios\ /Y
xcopy ..\..\iPad\iPadDraw\Sources\*.* %destpath%\iPadApp\ /Y
