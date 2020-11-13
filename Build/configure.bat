set platformTarget=x86
set ConfigurationName=Debug
set destDir=..\Output\%platformTarget%\%ConfigurationName%\

echo [info] init %platformTartget% %ConfigurationName% env

xcopy ..\3rd\ffmpeg2.7.1\bin\*.dll  %destDir% /y /i /r /D
xcopy ..\3rd\libx264\libx264-146.dll*   %destDir% /y /i /r /D

echo [info] init release env
set platformTartget=x86
set ConfigurationName=Release
set destDir=..\Output\%platformTarget%\%ConfigurationName%\

xcopy ..\3rd\ffmpeg2.7.1\bin\*.dll  %destDir% /y /i /r /D
xcopy ..\3rd\libx264\libx264-146.dll*  %destDir% /y /i /r /D
