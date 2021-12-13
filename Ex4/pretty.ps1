$sources = @("examples","include","src")
$sources | ForEach-Object {
  Get-ChildItem $_ -Include @("*.h","*.cpp") -Recurse | ForEach-Object {
    & "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\bin\clang-format.exe" -i $_.FullName
  }
}