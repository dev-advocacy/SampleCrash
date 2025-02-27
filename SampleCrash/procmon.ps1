# Registry watcher that observes specified registry value and resets it as desired.
# Might be used to fix annoying GPOs ;)

$HIVE = "HKEY_LOCAL_MACHINE"
$PATH = "SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\SampleCrash.exe"
$KEY = "CustomDumpFlags"
$GOODVALUE = 2
$EVENTKEY = "ConsentKeyChanged"

# Unregister pending events just in case the last script didn't clean up correctly
Unregister-Event -SourceIdentifier $EVENTKEY -ErrorAction SilentlyContinue

function Log([string]$text) {
    $time = get-date -Format "HH:mm:ss"
    Write-Host "$($time): $($text)"
}

# Event handler which checks the current value and fixes it
function valueChangedHandler ([object]$sender, [System.Management.EventArrivedEventArgs]$e) {

    if ($null -ne $sender) {
        Log -text "Value was changed"
    }

    $currentValue = Get-ItemPropertyValue -Path ("HKLM:\" + $PATH.Replace("\\", "\")) -Name $KEY 
    Log -text "Value is $($currentValue)"
        
    if ($currentValue -eq $GOODVALUE) {
        return
    }

    Log -text "Reset to $($GOODVALUE)"
    Set-ItemProperty -Path ("HKLM:\" + $PATH.Replace("\\", "\")) -Name $KEY -Value $GOODVALUE
}
    
try {
    # Fix the key initially
    valueChangedHandler -e $null -sender $null

    # Register the event handler to run every time the registry value is changed
    #  Register-WmiEvent -Query "Select * from RegistryValueChangeEvent where Hive='$($HIVE)' AND KeyPath='$($PATH)' AND ValueName='$($KEY)'" `
    #                    -Namespace "root\cimv2" `
    #                    -SourceIdentifier $EVENTKEY `
    #                   | Out-Null

      Register-WmiEvent -Query "Select * from RegistryValueChangeEvent where Hive='HKEY_LOCAL_MACHINE' AND KeyPath='SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\SampleCrash.exe' AND ValueName='ConsentKeyChanged'" `
                        -Namespace "root\cimv2" `
                        -SourceIdentifier "ConsentKeyChanged" `
                       | Out-Null

    # Run until cancel is requested
    $isCancelRequested = $false
    do {
        try {
            # Check if script can listen to keys
            $isKeyAvailable = [System.Console]::KeyAvailable
        }
        catch {
            # Only works if script is "owns" the command line
            $isKeyAvailable = $false
        }

        if ($isKeyAvailable) {
            # Don't immediatly cancel after Ctrl+C
            [System.Console]::TreatControlCAsInput = $true

            $keyInfo = [Console]::ReadKey($true)
        
            # If Ctrl + C was pressed
            if ($keyInfo.modifiers -eq [System.ConsoleModifiers]::Control -and $keyInfo.key -eq "C") {
                # ... signal the script to gracefully shut down
                $isCancelRequested = $true
            }
        }

    } while (-not $isCancelRequested)
}
catch {
    Log -text "Error: $($_.Exception.Message)"
}
finally {
    # Cleanup
    Log -text "Exit"
    Unregister-Event -SourceIdentifier $EVENTKEY
}