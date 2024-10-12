<img src="app-icon.png" alt="icon" width="75" height="75">

# Goosey

Goosey is a cribbage AI. It is full-featured, well-tested, and plays a strong game (although I have no practical way to quantify that). However, I believe there is ample room for improvement, so in that sense, it is still a work in progress. 

### License

The source code for this app has been released under the [MIT License](LICENSE).

### Copyright

© 2022 Stephen E. Bensley

## Building from Source

The app was developed with [Xcode](https://developer.apple.com/xcode/), which is freely available from Apple. After installing Xcode and cloning the repo, open the Xcode [project](Goosey.xcodeproj) at the root of the repo. Check out the most recent tag to ensure a stable build.

The code is entirely standards-compliant C++17, so it should be easy to compile on other platforms.

### Dependencies

None.

### Targets

The Xcode project has the following targets:

- BoardStrategy
- CardPlayStrategy
- DiscardStrategy
- GameModel
- Players
- CopyDataFiles
- gen_file:
- play_match:
- run_tests: Unit tests implemented using [Catch2](https://github.com/catchorg/Catch2)
