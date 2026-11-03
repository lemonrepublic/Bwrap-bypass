====================================================================
Flatpak Bubblewrap (bwrap) Bypass for Termux / PRoot
​This workaround provides a dynamic library hook (LD_PRELOAD) that
intercepts execve calls targeting bwrap (Bubblewrap). Because PRoot
cannot create unprivileged user namespaces (CLONE_NEWUSER), standard
Flatpak isolation fails. This hook strips out bwrap sandbox arguments
and executes the inner target application directly.
​PREREQUISITES
​Ensure you have basic build tools installed inside your PRoot container
(Debian/Ubuntu/Arch):
​Debian / Ubuntu:
apt update && apt install gcc make -y
​Arch Linux:
pacman -S gcc make --noconfirm
​INSTALLATION & COMPILATION
​Save your bwrap_bypass.c code to a local file inside the container.
​Compile the library with the -fPIC and -ldl flags:
​gcc -shared -fPIC bwrap_bypass.c -o /usr/lib/libbwrap_bypass.so -ldl
​USAGE
​Method 1: Single Command Execution
Preload the shared library right before calling flatpak run:
​LD_PRELOAD=/usr/lib/libbwrap_bypass.so flatpak run <APP_ID>
​Example:
LD_PRELOAD=/usr/lib/libbwrap_bypass.so flatpak run org.chromium.Chromium
​Method 2: Shell Alias (Recommended)
Add a custom alias or wrapper function in your ~/.bashrc or ~/.zshrc:
​alias flatpak-run='LD_PRELOAD=/usr/lib/libbwrap_bypass.so flatpak run'
​Reload your shell config:
source ~/.bashrc
​Now launch any app using:
flatpak-run org.chromium.Chromium
​HOW IT WORKS
​flatpak attempts to launch an application by passing its binary path
and mount arguments to /usr/bin/bwrap.
​LD_PRELOAD intercepts the execve system call before bwrap is called.
​The hook scans the argument list for the '--' delimiter, which separates
bwrap's namespace flags from the actual application binary.
​It bypasses bwrap completely and executes the underlying app binary
using real_execve.
​TROUBLESHOOTING & NOTES
​X11 / Display Errors: Since bwrap is bypassed, make sure your $DISPLAY
variable is set correctly and termux-x11 or a VNC server is active
before launching graphical applications.
​Library Missing Error: If you compile the .so file in a non-standard
path, make sure to provide the full absolute path in LD_PRELOAD.