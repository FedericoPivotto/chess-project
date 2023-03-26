cmake -S . -B .\build -G "MinGW Makefiles"

cd build

cmake --build .
if %errorlevel% neq 0 exit /b %errorlevel%

.\scacchiera cc
::.\scacchiera pc
.\replay f log.txt out.txt
::.\replay v log.txt