# MPQExtractor

## Summary

A command-line tool to extract files from MPQ archives (used by Blizzard games).

Works on MacOS X and Linux.

## Compilation

Requires <a href="http://www.cmake.org/">cmake</a> to build:

    $ mkdir build
    $ cd build
    $ cmake <path/to/the/source/of/MPQExtractor>
    $ make

The executable will be put in build/bin/


## Usage

See `MPQExtractor --help` for more details. All the examples below are shown using the World of Warcraft 4.3 files, as of Jan 2012.

Retrieve the list of files in a MPQ archive:

    $ MPQExtractor -l list.txt /path/to/art.MPQ
    $ cat list.txt
    art-md5.lst
    Cameras\Abyssal_Maw_CameraFly_01.M2
    Cameras\Abyssal_Maw_CameraFly_0100.skin
    Cameras\FlyBy_Maelstrom.M2
    ...


Search all the *.M2 files in a MPQ archive:

    $ MPQExtractor -s *.M2 /path/to/art.MPQ
    Opening 'path/to/art.MPQ'...

    Searching for '*.M2'...

    Found files:
      - PARTICLES\LoginFX.m2
      - Character\Worgen\Male\WorgenMale.M2
      - Character\Worgen\Female\WorgenFemale.M2
      - Character\Goblin\Female\GoblinFemale.M2
    ....


Extract a specific file from a MPQ archive:

    $ mkdir out
    $ MPQExtractor -e "Character\Worgen\Male\WorgenMale.M2" -o out /path/to/art.MPQ
    Opening '/path/to/art.MPQ'...

    Extracting files...

    $ ls out/
    WorgenMale.M2


Extract some specific files from a MPQ archive, preserving the path hierarchy found inside the MPQ archive:

    $ mkdir out
    $ MPQExtractor -e "Character\Worgen\Male\WorgenMale*" -f -o out /path/to/art.MPQ
    Opening '/path/to/art.MPQ'...

    Searching for 'Character\Worgen\Male\WorgenMale*'...

    Found files:
      - Character\Worgen\Male\WorgenMale.M2
      - Character\Worgen\Male\WorgenMaleSkin00_03.blp
      - Character\Worgen\Male\WorgenMaleSkin00_01.blp
      - Character\Worgen\Male\WorgenMaleSkin00_05.blp
      - Character\Worgen\Male\WorgenMaleNakedPelvisSkin00_05.blp
    ...

    Extracting files...

    $ ls out/Character/Worgen/Male/
    WorgenMale.M2
    WorgenMale00.skin
    WorgenMale0060-00.anim
    WorgenMale0060-01.anim
    WorgenMale0061-00.anim
    WorgenMale0062-00.anim
    WorgenMale0064-00.anim
    WorgenMale0065-00.anim
    WorgenMale0066-00.anim
    WorgenMale0067-00.anim
    WorgenMale0068-00.anim
    WorgenMale0069-00.anim
    WorgenMale0069-01.anim
    ...


Apply some patches before extracting a specific file from a MPQ archive:

    $ mkdir out
    $ MPQExtractor -p /patches/wow-update-*.MPQ  \
                   /patches/wow-update-base-1*.MPQ  \
                   --prefix base
                   -e "Character\Worgen\Male\WorgenMale.M2"  \
                   -o out /path/to/art.MPQ
    Opening '/path/to/art.MPQ'...
    Applying patch '/patches/wow-update-13164.MPQ'...
    Applying patch '/patches/wow-update-13205.MPQ'...
    Applying patch '/patches/wow-update-13287.MPQ'...
    Applying patch '/patches/wow-update-13329.MPQ'...
    Applying patch '/patches/wow-update-13596.MPQ'...
    Applying patch '/patches/wow-update-13623.MPQ'...
    Applying patch '/patches/wow-update-base-13914.MPQ'...
    Applying patch '/patches/wow-update-base-14007.MPQ'...
    Applying patch '/patches/wow-update-base-14333.MPQ'...
    Applying patch '/patches/wow-update-base-13914.MPQ'...
    Applying patch '/patches/wow-update-base-14007.MPQ'...
    Applying patch '/patches/wow-update-base-14333.MPQ'...

    Extracting files...

    $ ls out/
    WorgenMale.M2

Apply patches with different bases, extract files in lowercase:

    $ mkdir out
    $ MPQExtractor -p /patches/wow-update-13164.MPQ,base  \
                    /patches/wow-update-13205.MPQ,base  \
                    /patches/wow-update-base-13914.MPQ
                    -e "World\Minimaps\*" -f -c -o out  \
                    /path/to/art.MPQ
    Opening '/path/to/art.MPQ'...
    Applying patch '/patches/wow-update-13164.MPQ' (prefix 'base')...
    Applying patch '/patches/wow-update-13205.MPQ' (prefix 'base')...
    Applying patch '/patches/wow-update-base-13914.MPQ' (no prefix)...

    Extracting files...

    $ ls out/
    world/


## Dependencies

The repository/package contain all the necessary files, no need to install anything.

The following libraries are used (and are part of the distribution):

* StormLib (http://www.zezula.net/en/mpq/stormlib.html), no license, free to use
* SimpleOpt 3.4 (http://code.jellycan.com/simpleopt/), MIT License


## License

MPQExtractor is is made available under the MIT License. The text of the license is in the file 'LICENSE'.

Under the MIT License you may use MPQExtractor for any purpose you wish, without warranty, and modify it if you require, subject to one condition:

>   "The above copyright notice and this permission notice shall be included in
>   all copies or substantial portions of the Software."

In practice this means that whenever you distribute your application, whether as binary or as source code, you must include somewhere in your distribution the
text in the file 'LICENSE'. This might be in the printed documentation, as a file on delivered media, or even on the credits / acknowledgements of the
runtime application itself; any of those would satisfy the requirement. 

Even if the license doesn't require it, please consider to contribute your modifications back to the community.
