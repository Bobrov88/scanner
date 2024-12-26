
$env:CONAN_OLD_conanenv_PATH=$env:PATH

foreach ($line in Get-Content "/home/project/scanner/build_centos_x86/environment.ps1.env") {
    $var,$value = $line -split '=',2
    $value_expanded = $ExecutionContext.InvokeCommand.ExpandString($value)
    Set-Item env:\$var -Value "$value_expanded"
}

function global:_old_conan_conanenv_prompt {""}
$function:_old_conan_conanenv_prompt = $function:prompt
function global:prompt {
    write-host "(conanenv) " -nonewline; & $function:_old_conan_conanenv_prompt
}