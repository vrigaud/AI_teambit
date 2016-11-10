set AI_BOT_ROOT=C:\Users\maie1703\Documents\HitmenTeamAIBot
set VERSION=release
set CHROME="C:\Program Files (x86)\Google\Chrome\Application\chrome.exe"
call PlayStandaloneMatch.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_000
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_000
%CHROME% %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_000\AIBootCampResults.html