SET ARC_PATH="..\..\..\Arc-2.0"

xcopy /s /y %ARC_PATH%\Debug\ArcCore.lib lib\ArcCore.lib
xcopy /s /y %ARC_PATH%\Debug\ArcNet.lib lib\ArcNet.lib

xcopy /s /y %ARC_PATH%\Modules\ArcCore\Arc\*.h include\Arc\
xcopy /s /y %ARC_PATH%\Modules\ArcNet\Arc\*.h include\Arc\