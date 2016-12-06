set AI_BOT_ROOT=%cd%
set VERSION=release
set CHROME="C:\Program Files (x86)\Google\Chrome\Application\chrome.exe"

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_000
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_000
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_000\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_001
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_001
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_001\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_002
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_002
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_002\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_003
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_003
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_003\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_004
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_004
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_004\AIBootCampResults.html

call %AI_BOT_ROOT%\PlayStandaloneMatchRelease.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_005
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_005
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_005\AIBootCampResults.html