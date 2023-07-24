echo "Looking for Microsoft.Cpp.Default.props..."
Get-ChildItem -Path "C:\Program Files\" -Include Microsoft.Cpp.Default.props -File -Recurse -ErrorAction SilentlyContinue
