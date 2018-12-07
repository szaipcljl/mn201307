byobu

tips
====
Install byobu on the server, then work on it

Help
====
man byobu

Name
====
byobu - wrapper script for seeding a user's byobu configuration and
       launching a text based window manager (either screen or tmux)

SYNOPSIS
========
   byobu [options]
   byobu-screen [screen options]
   byobu-tmux [tmux options]
   Options to byobu are simply passed through screen(1) or tmux(1).

Keybindings
===========
F2 - Create a new window
F3 - Move to previous window
F4 - Move to next window
F5 - Reload profile
F6 - Detach from this session

Using Sessions
==============
if you have more than one session open, Byobu will ask you which session you
want to use with a prompt.

Start a new session by pressing CTRL+SHIFT+F2, then use ALT+UP and ALT+DOWN to
move backwards and forwards through your open sessions.

press CTRL+D to exit Byobu and close all of your sessions.

If you instead want to detach your session, there are three useful ways to do
this.

Pressing F6 cleanly detaches your current session and logs you out of the SSH
connection. It does not close Byobu, so the next time you connect to the server,
the current session will be restored. This is one of the most useful features of
Byobu; you can leave commands running and documents open while disconnecting
safely.

If you wish to detach the current session but maintain an SSH connection to the
server, you can use Shift-F6. This will detach Byobu (but not close it), and
leave you in an active SSH connection to the server. You can relaunch Byobu at
any time using the byobu command.

Next, consider a scenario where you are logged into Byobu from multiple
locations. While this is often quite a useful feature to take advantage of, it
can cause problems if, for example, one of the connections has a much smaller
window size (because Byobu will resize itself to match the smallest window). In
this case, you can use ALT+F6, which will detach all other connections and leave
the current one active. This ensures only the current connection is active in
Byobu, and will resize the window if required.

To recap:
---------

CTRL+SHIFT+F2 will create a new session.

ALT+UP and ALT+DOWN` will scroll through your sessions.

F6 will detach your current Byobu session.

SHIFT+F6 will detach (but not close) Byobu, and will maintain
your SSH connection to the server. You can get back to Byobu
with the byobu command.

ALT+F6 will detach all connections to Byobu except for the
current one.

