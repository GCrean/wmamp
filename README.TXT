WMAMP - www.turtlehead.co.uk
Copyright 2004 James Pitts - jim@codeworker.com

WMAMP 0.3 Beta Release notes - 8 Aug 2004

-------
Preface
-------

Hello and welcome to the current release of wmamp.  Wmamp is replacement software for the Linksys WMA11B media adapter device to access Apple iTunes network shares.

There are plenty of known issues with this software, many of which I'll list below.  

This software is known to work with my setup.  If it causes irreversible damage to your wma11b then I'm really sorry, but you shouldn't have started tinkering with it in the first place.   That said, wmamp doesn't write anything permanent to the flash memory, so if it crashes ( likely ) then just reboot it.  If you don't like wmamp, just go back to the Linksys squishguava image, and forget you even read this text.

---------------
Getting Started
---------------

In this package you'll find two files that are required to give the wma11b it's brain transplant.  The first is wmamp.img, this is a cramfs filesystem with the new software.  The other is progdata.xml,  this tells the upnp loader ( linksys installed software ) all about the new boot image.  Please backup your wma11b directory (c:\program files\Linksys Wireless-B Media Adapter\) before you start replacing the default installation with these replacements.  If it doesn't work or you don't like it you can go back!

Please be aware that your wma11b should be working correctly before you start replacing bits of it.  The device must be able to boot correctly before you consider going any further.   I'm also unsure if you'll be able to change it's setting if you use the new software, so always ensure bootimg/squishguava and bin/progdirdata.xml are kept safe.

Copy the wmamp.img ( in this package ) to the bootimg directory and progdirdata.xml to the bin directory.  Close the media manager program that's running in the system tray and restart both services.  If you don't know how to do this, just rebooting will achieve the same thing.

Now turn on your wma11b.  If everything went ok, you should see a new ( basic ) start screen.
This is the iTune discovery screen.  It will show you all the iTunes servers wmamp can locate via Rendezvous on your local network.

You have to enable file sharing in iTune if you're not already to be able to access it from wmamp.  If you don't see anything on the first screen it's possibly because iTune file sharing isn't enabled.  To enable it, check the box called 'Share my music' in iTunes preferences, ok it and you should see the library appear on your telly.  Coushty eh?  ;-p

Everything else from here on is pretty easy.  I've tried to model it on the iPod.  Select an Artist, an Album and then a track.  It should then start playing.

If you don't fancy running iTunes then you could run daapd, found here http://www.deleet.de/projekte/daap/daapd/  It also runs on linux if you've got your media library there.

I have only tested with iTunes 4.6, anything else may or may not work.  I can't promise to support older versions of iTunes, but if the fix is trivial I'll try.  I'll endeavor to support the most current iTunes I can, but the release may lag behind by a few days.  Especially if Apple change the authentication method again.

At the moment this replacement software is limited to Linksys Windows bootloader software.  This is basically a hack of the intel media player reference software.  I'd really like to replace this, but the wmamp software takes precedence.

Not many of the remote control buttons are yet supported.  You can jump to the next track, pause / play, etc.  More as I get to them.

Is anyone really bothered by the lack of picture support?  Did anyone actually use this feature?

------------
Installation
------------

Please read these instructions carefully and remember to make a backup of your wma directory first.  
If you break your device you shouldn't have played with it.  [Although some argue it was broken before it left the linksys labs]

You'll find two files in this package.

./bin/ProgDirData.xml
./bootimg/wmamp.img

ProgDirData.xml is a direct replacement for the one in the 'c:\Program Files\Linksys Wireless-B Media Adapter\bin' directory.
Its location may be different depending on your installation.  Please remember to backup this file before you overwrite it.

Next copy ./bootimg/wmamp.img from this package to the 'c:\Program Files\Linksys Wireless-B Media Adapter\bootimg' directory.

Restart the two services 'Digital media adapter application loader service' and 'Digital media adapter host service'.  Shutdown the little media adapter tool in the system tray and restart it.  Alternatively just reboot your pc.  If you don't intend to run the linksys software anymore you can disable the 'Digital media adapter host service' altogether.
Restart the wma11b device and you're done.

--------------------------
Known Problems, Bugs, etc.
--------------------------

Wow, there's probably lots of these.  And many I've not discovered yet.  If you find anything quirky, crashing or weird and it's not listed below, please let me know and I'll try and fix it.

Problems.....
1) Looks a little slow.  Yes,  I really regret using 16bit mode now.  But I hate palette management.  In retrospect, if I'd used 8bit mode I would have enough memory for a second screen buffer to do flipping and have half the memory to shift around so things would be faster.  I may fix this in the future.  Another reason it looks a little slow at times is because there's still loads of debugging code present and much of the code is less than optimal.  This will eventually go away.

2) Listview selection is weird.  I ran out of time, and listviews are generally non-trivial problems to solve in the first place.  Especially one that can change as you're displaying it ( like the iTunes server selection page )..  For the time being accept it's general weirdness and I'll get to this soon.

3) Crashes.  Actually I've had very few of these.  But it's likely you'll experience a crash sooner or later.  There are leaks in the code and probably loads of glitches.  Please let me know if you manage to crash it and what you were doing at the time.

4) Volume control doesn't work.  Many of the remote buttons haven't yet been connected to anything useful.  I've only implemented the vital ones for the moment.  I'm also desperate to include page up / page down controls too, I'm getting fed up listening to only artists that start with 'A'.

Trouble Shooting...

I've left the telnet daemon running so if you are hitting problems with wmamp, telnet in and run wmamp from the command prompt and examine the output for clues.  To stop running wmamp, just do 'killall wmamp' from the prompt and restart it.  A knowledge on unix would be useful if you want to toy around with the command prompt.  If you report any bugs a log of the output would be very useful.

Future Plans...

Whoa, plenty of these.  First of all, I'd like to get wmamp stable and practical.  The interface needs improving, the listviews need scroll bars and hints to actions, keys to press etc...  I'd also like to make it faster, prettier and use less memory.  I need to add a now playing screen.

After that I'd probably add the 'sweet bits', the toys that are nice to have.  A now playing xml feed and a little web interface are possible.  It's likely I'll add web radio too.

I'd love to hear ideas and suggestions.  So please let me know at the address at the top of this document.

----------------
How You can help
----------------

How can you help?  Oh lots of ways.  Especially if you can code.  The source is available on the website (http://www.turtlehead.co.uk) so have a hack with it.  I'll happily accept bug fixes and enchancement, either by patch or diffs.

I'd be really grateful to someone who can code a upnp server for the new image so we can completely dispose of the linksys software.

Also there's documentation.  Documenting everything from the software to setting up a development environment to code for the arm processor.

I'm open to suggestions and encouragement.  Leave a message on the site ( www.turtlehead.co.uk or jim@codeworker.com )

-----------------------
Packages Used - Credits
-----------------------

This whole development wouldn't have been possible without these packages.

libmad - www.underbit.com/products/mad
freetype 2 - www.freetype.org
Rendezvous - Apple Darwin
Network icon - Found on Gentoo webpage.
Coolvetica font -  www.larabiefonts.com
Dustismo font - www.dustismo.com
iTunes Authentication -  David Hammerton - http://crazney.net/programs/itunes/authentication.html

Also see the CREDITS file.



---
eof
---
