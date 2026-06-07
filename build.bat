@REM
@REM
@REM
@REM
mkdir build
cd build

@REM
cmake ..
@if ERRORLEVEL 1 goto onError

@REM
cmake --build .
@if ERRORLEVEL 1 goto onError

goto onSuccess

:onError
@echo An error occured!
:onSuccess
cd ..
