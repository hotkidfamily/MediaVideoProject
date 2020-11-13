set platformTartget=x86
echo [info] init debug env
set ConfigurationName=debug
xcopy ..\3rd\ffmpeg2.7.1\bin\*.dll ..\%ConfigurationName% /y /i /r /D /e 
xcopy ..\3rd\libx264\*.dll ..\%ConfigurationName% /y /i /r /D /e 

echo [info] init release env
set ConfigurationName=release
xcopy ..\3rd\ffmpeg2.7.1\bin\*.dll ..\%ConfigurationName% /y /i /r /D /e 
xcopy ..\3rd\libx264\*.dll ..\%ConfigurationName% /y /i /r /D /e 


set platformTartget=x64