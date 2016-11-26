set AI_BOT_ROOT=%cd%
set VERSION=release
set CHROME="C:\Program Files (x86)\Google\Chrome\Application\chrome.exe"

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_030
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_030
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_030\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_031
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_031
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_031\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_032
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_032
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_032\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_040
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_040
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_040\AIBootCampResults.html