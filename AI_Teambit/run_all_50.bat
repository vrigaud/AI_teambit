set AI_BOT_ROOT=%cd%
set VERSION=release
set CHROME="C:\Program Files (x86)\Google\Chrome\Application\chrome.exe"

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_050
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_050
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_050\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_051
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_051
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_051\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_052
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_052
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_052\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_053
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_053
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_053\AIBootCampResults.html