# Introduction #

Commands that you can use. Most require quotations around the argument. Change your currently active talking channel with /ircact


# /ircsay #
Say stuff in your currently active channel. You can use color codes like in normal JA, but they currently won't show up for other users.
Syntax: /ircsay "Hi, guys!"

# /ircquit #
Quit IRC
**Known Bugs** : Can cause glitches when /ircleave is used beforehand.
Syntax: /ircquit "Goodbye, everyone!"

# /ircjoin #
Join a channel.
Syntax (one channel): /ircjoin #JA-OtherChannel
Suntax (passworded): /ircjoin "#JA-Passworded password"
Syntax (multiple at once): /ircjoin "#JA-OtherChannel,#JA-AnotherChannel"
_todo_ : connect to more than one server at a time

# /ircleave #
Leave a specified channel.
Syntax: /ircleave #JA-OtherChannel

# /ircconnect #
Connect to an irc server.
_todo_ : connect to more than one server at a time

# /me #
Action. Good for roleplaying
Syntax: /me "explains how to use the /me command"
**Known Bugs** : Won't look correct on your client. But shows up fine to other people.

# /ircstats #
I think it's only used for IRC server admins. Not sure.

# /irclist #
Lists all users in a channel, or all channels if no argument.
Syntax: /irclist #JA-Global

# /ircname #
Change your nickname. **This is required if you are in #JA-Global (is an enforced rule)**