param(
  [Parameter(Mandatory = $true)][string] $Runner,
  [Parameter(Mandatory = $true)][string] $Driver,
  [Parameter(Mandatory = $true)][string] $WorkDir,
  [Parameter(Mandatory = $true)][string] $Fixture,
  [Parameter(Mandatory = $true)][string] $Simple,
  [Parameter(Mandatory = $true)][string] $Bios
)

$ErrorActionPreference = 'Stop'
$shareName = 'muhtari-paths-' + [Guid]::NewGuid().ToString('N').Substring(0, 12)
$identity = [Security.Principal.WindowsIdentity]::GetCurrent().Name
$computer = $env:COMPUTERNAME
$uncRoot = "\\?\UNC\$computer\$shareName"
$probe = "$uncRoot\smb-preflight-$PID.bin"
$baseline = [System.IO.Path]::Combine($WorkDir, 'windows-paths-baseline.state')
$created = $false
$primaryFailure = $null
$cleanupFailures = [System.Collections.Generic.List[string]]::new()

try {
  if ([string]::IsNullOrWhiteSpace($computer) -or [string]::IsNullOrWhiteSpace($identity)) {
    throw 'Could not determine the current Windows computer and job identity.'
  }
  if (-not (Test-Path -LiteralPath $WorkDir -PathType Container)) {
    throw "Test work directory does not exist: $WorkDir"
  }
  $existing = Get-SmbShare -Name $shareName -ErrorAction SilentlyContinue
  if ($null -ne $existing) {
    throw "Refusing to alter pre-existing SMB share '$shareName'."
  }

  New-SmbShare -Name $shareName -Path $WorkDir -FullAccess $identity -ErrorAction Stop |
    Out-Null
  $created = $true

  $expected = [byte[]](0x4D, 0x55, 0x48, 0x54, 0x41, 0x52, 0x49)
  [System.IO.File]::WriteAllBytes($probe, $expected)
  $actual = [System.IO.File]::ReadAllBytes($probe)
  $same = $actual.Length -eq $expected.Length
  for ($index = 0; $same -and $index -lt $expected.Length; $index++) {
    $same = $actual[$index] -eq $expected[$index]
  }
  if (-not $same) {
    throw 'Loopback SMB preflight bytes did not round-trip.'
  }
  [System.IO.File]::Delete($probe)
  if ([System.IO.File]::Exists($probe)) {
    throw 'Loopback SMB preflight file could not be removed.'
  }

  & $Driver $Runner $WorkDir $Fixture $Simple $Bios 'drive'
  if ($LASTEXITCODE -ne 0) {
    throw "Drive namespace process test failed with exit $LASTEXITCODE."
  }
  & $Driver $Runner $WorkDir $Fixture $Simple $Bios 'unc' $uncRoot
  if ($LASTEXITCODE -ne 0) {
    throw "Extended UNC process test failed with exit $LASTEXITCODE."
  }
}
catch {
  $primaryFailure = $_.Exception.Message
}
finally {
  try {
    if ([System.IO.File]::Exists($baseline)) {
      [System.IO.File]::Delete($baseline)
    }
    if ([System.IO.File]::Exists($baseline)) {
      $cleanupFailures.Add('Ordinary-path baseline state remains after cleanup.')
    }
  }
  catch {
    $cleanupFailures.Add("Could not remove ordinary-path baseline state: $($_.Exception.Message)")
  }
  try {
    if ([System.IO.File]::Exists($probe)) {
      [System.IO.File]::Delete($probe)
    }
    if ([System.IO.File]::Exists($probe)) {
      $cleanupFailures.Add('Loopback SMB preflight file remains after cleanup.')
    }
  }
  catch {
    $cleanupFailures.Add("Could not remove loopback SMB preflight file: $($_.Exception.Message)")
  }
  if ($created) {
    try {
      Remove-SmbShare -Name $shareName -Force -Confirm:$false -ErrorAction Stop | Out-Null
    }
    catch {
      $cleanupFailures.Add("Could not remove SMB share '$shareName': $($_.Exception.Message)")
    }
    try {
      $remaining = Get-SmbShare -Name $shareName -ErrorAction SilentlyContinue
      if ($null -ne $remaining) {
        $cleanupFailures.Add("SMB share '$shareName' remains after cleanup.")
      }
    }
    catch {
      $cleanupFailures.Add("Could not verify removal of SMB share '$shareName': $($_.Exception.Message)")
    }
  }
}

if ($null -ne $primaryFailure) {
  [Console]::Error.WriteLine("Primary Windows path test failure: $primaryFailure")
}
foreach ($failure in $cleanupFailures) {
  [Console]::Error.WriteLine("Windows path test cleanup failure: $failure")
}
if ($null -ne $primaryFailure -or $cleanupFailures.Count -ne 0) {
  exit 1
}
