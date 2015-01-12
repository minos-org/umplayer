## Umplayer

[Umplayer](https://github.com/minos-org/umplayer/) is a free media player with built-in codecs that can play virtually all video and audio formats. This is a custom + freeze version, refer to [sourceforge](http://www.umplayer.com/) or [smplayer](http://smplayer.sourceforge.net/) for alternatives.

<p align="center">
<img src="https://raw.githubusercontent.com/minos-org/umplayer/master/umplayer.jpg" alt="umplayer"/>
</p>

## Quick start

### On Ubuntu

   ```
   $ sudo add-apt-repository ppa:minos-archive/main
   $ sudo apt-get update && sudo apt-get install umplayer
   ```

### On other Linux distributions

1. Type `make`

2. Type `sudo make install`

## Differences

* Remove youtube (broken anyway) and shoutcast (there are better programs for listening online radio)
* Remove background refreshing (faster and prettier)
* Remove links to faq, social networks and original web pages (abandoned)
* Remove annoying facebook and initial language messages
* Remove floating control widget (ugly and unnecessary)
* Change default Ctrl+F -> Ctrl+O for openning files
* Quit by pressing 'q', just as in mplayer{,2}
* Use a modified black theme by default
* Add an optional umplayer-dbg package
