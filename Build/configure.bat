
echo [info] init debug env
set ConfigurationName=debug
xcopy ..\ffmpeg2.7.1\bin\*.dll ..\%ConfigurationName% /y /i /r /D /e 
xcopy ..\libx264\*.dll ..\%ConfigurationName% /y /i /r /D /e 
xcopy ..\VideoFilters\OrangeFilters\bin\%ConfigurationName%\x86\*.dll ..\%ConfigurationName% /y /i /r /D /e 
xcopy ..\VideoFilters\effects ..\%ConfigurationName%\effects /y /i /r /D /e 

echo [info] init release env
set ConfigurationName=release
xcopy ..\ffmpeg2.7.1\bin\*.dll ..\%ConfigurationName% /y /i /r /D /e 
xcopy ..\libx264\*.dll ..\%ConfigurationName% /y /i /r /D /e 
xcopy ..\VideoFilters\OrangeFilters\bin\%ConfigurationName%\x86\*.dll ..\%ConfigurationName% /y /i /r /D /e 
xcopy ..\VideoFilters\effects ..\%ConfigurationName%\effects /y /i /r /D /e 