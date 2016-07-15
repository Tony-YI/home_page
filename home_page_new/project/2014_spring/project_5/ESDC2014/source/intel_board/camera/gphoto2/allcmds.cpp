Home :: News :: Projects :: Documentation :: Developers :: Mailing lists :: Download :: Feedback :: About Manufacturers :: Links

User's manual :: API :: PTP/IP :: FAQ :: Remote control

gphoto2
Prev 	The gPhoto2 Reference (the man pages)	 Next
Name

gphoto2 — command-line gphoto2 client

Synopsis

gphoto2 [--debug] [--debug-logfile FILENAME] [[-q] | [--quiet]] [[-v] | [--verbose]] [[-h] | [--help]]
[--hook-script FILENAME]
[--list-cameras] [--list-ports] [--stdout] [--stdout-size]
[--auto-detect] [--port PATH] [--speed SPEED] [--camera MODEL]
[--filename FILENAME]
[--usbid USBIDS]
[[-a] | [--abilities]]
[[--folder FOLDER] | [-f FOLDER]] [[[-R] | [--recurse]] | [--no-recurse]]
[[-l] | [--list-folders]]
[[-L] | [--list-files]] [[-m NAME] | [--mkdir NAME]]
[[-r NAME] | [--rmdir NAME]] [[-n] | [--num-files]]
[[-p RANGE] | [--get-file RANGE]] [[-P] | [--get-all-files]]
[[-t RANGE] | [--get-thumbnail RANGE]]
[[-T] | [--get-all-thumbnails]]
[--get-raw-data RANGE] [--get-all-raw-data]
[--get-audio-data RANGE] [--get-all-audio-data]
[--force-overwrite]
[--new]
[[-d RANGE] | [--delete-file RANGE]] [[-D] | [--delete-all-files]]
[[-u FILENAME] | [--upload-file FILENAME]] [--config]
[--list-config]
[--get-config CONFIGENTRY] [--set-config CONFIGENTRY=CONFIGVALUE] [--set-config-index CONFIGENTRY=CONFIGINDEX] [--set-config-value CONFIGENTRY=CONFIGVALUE]
[--capture-preview]
[[-F COUNT] | [--frames COUNT]] [[-I SECONDS] | [--interval SECONDS]]
[--reset-interval]
[--capture-image] [--trigger-capture] [--capture-movie] [--capture-sound]
[--capture-tethered SECONDS or COUNT]
[--wait-event SECONDS or COUNT] [--wait-event-and-download SECONDS or COUNT]
[--show-info RANGE]
[--show-exif RANGE]
[--storage-info] [--summary] [--manual] [--about] [--shell]

Description

libgphoto2(3) is a cross-platform digital camera library, and gphoto2(1) is a command-line client for it.

Where an option takes a RANGE of files, thumbnails, or other data, they are numbered beginning at 1. A range is a comma-separated list of numbers or spans (“first-last”). Ranges are XOR (exclusive or), so that “1-5,3,7” is equivalent to “1,2,4,5,7”.

--debug
Turn on debugging output. Debugging output is written to stderr by default, or to the filename given to the --debug-logfile option.

--debug-logfile FILENAME (since 2.3.0)
The logfile to write the debugging info to, if --debug is given.

--hook-script FILENAME (new after 2.3.0)
Execute the hook script FILENAME every time a certain event happens within gphoto2. The hook script reads the environment variable ACTION. It must ignore ACTION values unknown to it.

ACTION=init
gphoto2 has just been initialized. If the hook script returns with a non-zero exit code now, gphoto2 will abort.

ACTION=start
gphoto2 has just finished parsing the command line and is about to start executing the commands given on the command line.

ACTION=download
gphoto2 has just downloaded a file to the computer, storing it in the file indicated by the environment variable ARGUMENT.

ACTION=stop
gphoto2 is about to finish. Do your final cleanups here.

All other environment variables are passed to the hook script unchanged. You can make use of that to pass data to the hook script.

Hook script may be specified in the ~/.gphoto/settings file as gphoto2=hook-script=filename.

-q, --quiet
Quiet output (default=verbose).

-v, --version
Display version and exit.

-h, --help
Display a short usage message.

--list-cameras
List supported camera models.

--list-ports
List supported port devices.

--stdout
Send file to stdout.

--stdout-size
Print filesize before data.

--auto-detect
List auto-detected cameras and the ports to which they are connected.

--port PATH
Specify port device. The --list-ports prints a list of valid, usable ports. In case of multiple USB cameras, the --auto-detect shows you the specific port each camera is connected to.

--speed SPEED
Specify serial transfer speed.

--camera MODEL
Specify camera model. The --list-cameras option prints a list of all explicitly supported cameras.

Most model names contain spaces: remember to enclose the name in quotes so that the shell knows it is one parameter. For example: --camera "Kodak DC240".

Note that if you specify --camera, you must also specify --port. Otherwise the --camera option will be silently ignored.

--filename FILENAME
When downloading files from the camera, specify the file name or file name pattern to use when storing the downloaded file on the local disk. When uploading a file to the camera, specify the filename to store the uploaded file as on the camera.

The --filename option accepts %a, %A, %b, %B, %d, %H, %k, %I, %l, %j, %m, %M, %S, %y, %%, (see date(1)) and, in addition, %n for the number, %C for the filename suffix, %f for the filename without suffix, %F for the foldername, %: for the complete filename in lowercase.

Note that %: is still in alpha stage, and the actual character or syntax may still be changed. E.g. it might be possible to use %#f and %#C for lower case versions, and %^f and %^C for upper case versions.

%n is the only conversion specifier to accept a padding character and width: %03n will pad with zeros to width 3 (e.g. print the number 7 as “007”). Leaving out the padding character (e.g. %3n) will use an implementation specific default padding character which may or may not be suitable for use in file names.

Default value for this option can be specified in the ~/.gphoto/settings file as gphoto2=filename=value.

--usbid USBIDS
(Expert only) Override USB IDs. USBIDSmust be of the form DetectedVendorID:DetectedProductID=TreatAsVendorID:TreatAsProductID to treat any USB device detected as DetectedVendorID:DetectedProductID as TreatAsVendorID:TreatAsProductID instead. All the VendorIDs and ProductIDs should be hexadecimal numbers beginning in C notation, i.e. beginning with '0x'.

Example: --usbid 0x4a9:0x306b=0x4a9:0x306c

-a, --abilities
Display camera abilities.

-f, --folder FOLDER
Specify camera folder (default="/").

-R, --recurse
Recursion (default for download).

--no-recurse
No recursion (default for deletion).

-l, --list-folders
List folders in folder.

-L, --list-files
List files in folder.

-m, --mkdir NAME
Create a directory.

-r, --rmdir NAME
Remove a directory.

-n, --num-files
Display number of files.

-p, --get-file RANGE
Get files given in range.

-P, --get-all-files
Get all files from folder.

-t, --get-thumbnail RANGE
Get thumbnails given in range.

-T, --get-all-thumbnails
Get all thumbnails from folder.

--get-raw-data RANGE
Get raw data given in range.

--get-all-raw-data
Get all raw data from folder.

--get-audio-data RANGE
Get audio data given in range.

--get-all-audio-data
Get all audio data from folder.

--force-overwrite
Overwrite files without asking.

--skip-existing
Skip files if they exist already on the local directory.

--new
Only get not already downloaded files. This option depends on camera support of flagging already downloaded images and is not available for all drivers.

-d, --delete-file RANGE
Delete files given in range.

-D, --delete-all-files
Delete all files in folder (defaults to --no-recurse).

-u, --upload-file FILENAME
Upload a file to camera.

--capture-preview
Capture a quick preview.

-F COUNT, --frames COUNT
Number of frames to capture in one run. Default is infinite number of frames.

-I SECONDS, --interval SECONDS
Time between capture of multiple frames.

(Since 2.4) If SIGUSR1 signal is received, a picture is taken immediately without waiting for the end of the current interval period (see the section called “Signals”). A value of -1 will let gphoto2 wait forever, i.e. until a signal arrives. See also --reset-interval (since 2.4).

--reset-interval (since 2.4)
Setting this option will reset the time interval to the value given by the -I|--interval option when a SIGUSR1 signal is received in time-lapse mode.

--capture-image
Capture an image and keep it on the camera.

--trigger-capture
Trigger the capture an image and return. If you want to get the image downloaded, see --wait-event-and-download

--capture-image-and-download
Capture an image and download it immediately to the computer.

--keep
When doing --capture-image-and-download or interval capture, this option will keep the images on the memory card of the camera.

--no-keep
When doing --capture-image-and-download or interval capture, this option will not keep the images on the memory card of the camera after downloading them during capture. (default)

--capture-movie
Capture a movie. If the camera supports previews, this will capture a stream of previews (motion-jpeg) as fast as the camera can.

--capture-sound
Capture an audio clip. (No driver supports this at this time.)

--capture-tethered
Lets gphoto2 wait for notifications from the camera that an object was added. This is useful for tethered capture, where pressing the shutter on the camera immediately transfer the image to the machine for processing.

Together with the --hook-script to immediately postprocess or display the images this can help a studio workflow.

This option requires support in the driver and by the camera, currently newer Canon EOS and Nikon DSC are known to work.

--wait-event SECONDS or COUNT, --wait-event-and-download SECONDS or COUNT
Lets gphoto2 wait for notifications from the camera for various things. This is useful for seeing what the camera does and waiting for objects to be added. The objects are kept on camera in the “--wait-event” version, with “--wait-event-and-download” they are downloaded.

--wait-event-and-download is equivalent to --capture-tethered.

The time to wait can be either specified as full seconds with a "s" suffix, or as a number of events (just a number). If nothing happens, a timeout is generated after 1 second, so a “wait-event=5” will take at most 5 seconds. A “--wait-event=5s” will take exactly 5 second.

If no argument is given, the wait time is 1 million events (basically forever).

In the download variant this can be used together with the --hook-script to immediately postprocess or display the images this can help a studio workflow.

This option requires support in the driver and by the camera, currently newer Canon EOS and Nikon DSC are known to work.

--show-info RANGE
Show info.

--list-config
List all configuration entries.

--get-config CONFIGENTRY
Get the specified configuration entry.

This command will list the type, the current value and also the available options of this configuration value.

--set-config CONFIGENTRY=CONFIGVALUE
Set the specified configuration entry. For lists of choices of values this setting first looks up CONFIGVALUE as value and then as index into the choice list. Since this is not fully clear, you can use --set-config-index or --set-config-value to be more clear what is searched for.

Look at the output of --get-config to see what values are possible to set here.

--set-config-index CONFIGENTRY=CONFIGINDEX
Set the specified configuration entry by specifying the index into the list of choices for the configuration value. This of course only works for configuration settings that offer list of choices.

Look at the output of --get-config to see what indices are possible to set here.

--set-config-value CONFIGENTRY=CONFIGVALUE
Set the specified configuration entry by specifying its new value. For lists of choices the value is looked up and set.

Look at the output of --get-config to see what indices are possible to set here.

--storage-info
Display information about the camera's storage media.

--summary
Summary of camera status.

--manual
Camera driver manual.

--about
About the camera driver.

--shell
Start the gphoto2 shell, an interactive environment. See SHELL MODE for a detailed description.

Shell Mode

The following commands are available:

cd
Change to a directory on the camera.

ls
List the contents of the current directory on the camera.

lcd
Change to a directory on the local machine.

get
Download the file to the current directory.

get-thumbnail
Download the thumbnail to the current directory.

get-raw
Download raw data to the current directory.

show-info
Show information.

delete
Delete a file or directory.

mkdir DIRECTORY
Creates a directory named “DIRECTORY”.

rmdir DIRECTORY
Removes a directory named “DIRECTORY”.

show-exif
Show EXIF information (only if compiled with EXIF support).

capture-image
Captures a single image and keeps it on the camera.

capture-image-and-download
Captures a single image and downloads it from the camera.

list-config
Lists all configuration values.

get-config NAME
Gets the configuration specified by “NAME”.

set-config NAME=VALUE
Sets the configuration specified by “NAME” to “VALUE”.

set-config-value NAME=VALUE
Sets the configuration specified by “NAME” to “VALUE”.

set-config-index NAME=VALUE
Sets the configuration specified by “NAME” to the “INDEX” into the list of choices. Works only for Menu or Radio button entries.

wait-event COUNT or SECONDS
Waits for events from the camera for the specified time in SECONDS (if suffixed with s) or the COUNT of events from the camera, where every seconds a timeout event happens. Newly added images are kept on the camera.

Default is 1 event.

wait-event-and-download COUNT or SECONDS, capture-tethered COUNT or SECONDS
Waits for events from the camera for the specified time in SECONDS (if suffixed with s) or the COUNT of events from the camera, where every seconds a timeout event happens. Newly added images are downloaded from the camera.

Default is 1 event.

help, ?
Displays command usage.

exit, quit, q
Exit the gphoto2 shell.

Environment variables

CAMLIBS
If set, defines the directory where the libgphoto2 library looks for its camera drivers (camlibs). You only need to set this on OS/2 systems and broken/test installations.

IOLIBS
If set, defines the directory where the libgphoto2_port library looks for its I/O drivers (iolibs). You only need to set this on OS/2 systems and broken/test installations.

LD_DEBUG
Set this to all to receive lots of debug information regarding library loading on ld based systems.

USB_DEBUG
If set, defines the numeric debug level with which the libusb library will print messages. In order to get some debug output, set it to 1.

Signals

SIGUSR1 (since 2.4)
In time-lapse capture mode, receiving a SIGUSR1 signal makes gphoto2 take a picture immediately.

If the --reset-interval option is given, the time counter is reset to the value given by the -I|--interval option. Note that the camera will need some time (from 50ms to a few seconds) to actually capture the image.

See also

libgphoto2(3), The gPhoto2 Manual, The gphoto.org website, Digital Camera Support for UNIX, Linux and BSD

Examples

gphoto2 --list-ports
Shows what kinds of ports (USB and serial) you have.

gphoto2 --auto-detect
Shows what camera(s) you have connected.

gphoto2 --list-files
List files on camera.

gphoto2 --get-file 7-13
Get files number 7 through 13 from the list output by gphoto2 --list-files.

gphoto2 --capture-image --interval 60 --hook-script /usr/share/doc/gphoto2/test-hook.sh
Capture one image every 60 seconds from now to eternity. The example hook script will be called after each captured image has been stored on the computer.

To track down errors, you can add the --debug parameter to the gphoto2 command line and, if dealing with USB problems, setting the environment variable USB_DEBUG=1.

Prev 	Up	 Next
The gPhoto2 Reference (the man pages) 	Home	 libgphoto2
