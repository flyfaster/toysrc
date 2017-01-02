cd /d %~dp0

call npm install

set MSBuildExtensionsPath=C:\Program Files (x86)\MSBuild\Microsoft.Cpp\v4.0
set MSBuildToolsVersion=v140
set VCTargetsPath=C:\Program Files (x86)\MSBuild\Microsoft.Cpp\v4.0\v140

del node_modules\nslog\build\Release\nslog.node
node_modules\.bin\electron-rebuild.cmd
