set AI_BOT_ROOT=C:\Users\maie1703\Documents\HitmenTeamAIBot
set VERSION=release
call PlayStandaloneMatch.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_020
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_020
Taskkill /IM aibot.exe /F
call PlayStandaloneMatch.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_021
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_021
Taskkill /IM aibot.exe /F
call PlayStandaloneMatch.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_022
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_022
Taskkill /IM aibot.exe /F
call PlayStandaloneMatch.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_023
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_023
Taskkill /IM aibot.exe /F
call PlayStandaloneMatch.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_030
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_030
Taskkill /IM aibot.exe /F
call PlayStandaloneMatch.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_031
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_031
Taskkill /IM aibot.exe /F
call PlayStandaloneMatch.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_032
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_032
Taskkill /IM aibot.exe /F
call PlayStandaloneMatch.bat %AI_BOT_ROOT%\build-solution-x64\%VERSION% TC_040
robocopy /E %AI_BOT_ROOT%\StandaloneMatchResults\gamelog %AI_BOT_ROOT%\StandaloneMatchResults\StandaloneMatchResults_TC_040
Taskkill /IM aibot.exe /F