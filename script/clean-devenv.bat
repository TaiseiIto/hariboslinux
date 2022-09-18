@ECHO OFF

REM Usage: $ clean_devenv.bat <docker command> <image name> <container name>
REM This program removes docker image "hariboslinux" and container "hariboslinux".
REM This program should be called from "../Makefile" with command "make clean-devenv".

SET ERROR=FALSE
IF "%1" == "" SET ERROR=TRUE
IF "%2" == "" SET ERROR=TRUE
IF "%3" == "" SET ERROR=TRUE
IF %ERROR%==TRUE (
ECHO There should be 3 arguments.
EXIT /B -1
)

SET DOCKER=%1
SET IMAGE=%2
SET CONTAINER=%3

FOR /F "USEBACKQ DELIMS=" %%I IN (`%DOCKER% ps -a`) DO (
	ECHO %%I | FIND /I "%CONTAINER%" > NUL
	IF NOT ERRORLEVEL 1 %DOCKER% rm %CONTAINER%
)
FOR /F "USEBACKQ DELIMS=" %%I IN (`%DOCKER% images`) DO (
	ECHO %%I | FIND /I "%IMAGE%" > NUL
	IF NOT ERRORLEVEL 1 %DOCKER% rmi %IMAGE%
)

