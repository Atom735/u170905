IF "%LOCAL_SETTINGS%" == "1" GOTO settings_accepted
CALL WIN_Settings.bat
:settings_accepted
CALL "%TOOL_GIT%" add --all
CALL "%TOOL_GIT%" commit -a -m %1
CALL "%TOOL_GIT%" push
PAUSE
