set AI_BOT_ROOT=%cd%
set VERSION=release
set CHROME="C:\Program Files (x86)\Google\Chrome\Application\chrome.exe"

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_010
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_010
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_010\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_011
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_011
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_011\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_012
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_012
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_012\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_013
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_013
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_013\AIBootCampResults.html