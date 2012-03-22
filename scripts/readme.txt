This directory contains scripts aiding with releasing new Copy Handler versions.

Scripts requirements:
- ActivePerl (for tagging debug symbols with source server information)
- Subversion client (command line) with svnversion script
- Debugging tools for Windows (symstore)
- 7zip (packaging)
- InnoSetup + Preprocessor pack

Notes:
1. Those requirements does not include the dependencies required by the build process, which must also be met.
2. All tools must be available in %PATH
3. Scripts should be called from the directory where they are placed.