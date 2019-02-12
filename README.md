# DUMPCIS- Dump PCMCIA Card Information Structure
`dumpcis` is a 8088-Compatible DOS program for detecting the existence of
Intel 82365SL compatible controllers on your old hardware and dumping out
the Card Information Structure (CIS) that is required to be present on any
inserted PCMCIA cards.

Why did I write this? I was asked to, and not having written DOS code in a
while, I thought it would be fun. So I agreed.

## Compilation
`dumpcis` requires the [Open Watcom](http://openwatcom.com) C Compiler 1.9
and either [GNU Make](https://www.gnu.org/software/make/) or `wmake` (provided
with the Watcom C Compiler) to build.

If using GNU Make, make sure to use a maximum of 1 job, like as follows:
`make -j1`. Unfortunately, `wcl` writes its output to temporary files and
it is possible that there will be a naming conflict if running in parallel;
`wmake` chokes on GNU Make's `.NOTPARALLEL` directive.

If using `wmake`, make sure to invoke `wmake` in Unix-compatibility mode, as
follows: `wmake -u`. Only the `all` and `wmake-clean` targets are supported if
using `wmake`.

## Usage
At present, `dumpcis` takes no parameters and is invoked by typing its name as
the DOS command line: `dumpcis.exe`.

## Theory Of Operation
The utility will scan for any existing Intel 82365SL PCMCIA controllers or
compatibles. If a controller and inserted cards are found,
`dumpcis` will use a nominally-free region (`e000:0000`) of
[Upper Memory](https://en.wikipedia.org/wiki/Upper_memory_area) to query the
PCMCIA's Attribute Memory for the CIS.

Like all good DOS utilities, `dumpcis` directly talks to the hardware, and does
not require Card Services (a BIOS-like API that _some_ PCMCIA utilies use) to
function. `dumpcis` makes an effort to leave the controller's registers
unmodified after it has completed its tasks. This works because, as far as I
know, register writes to PCMCIA controllers are idempotent.

## TODO
* Some Intel 82365SL clones, like the Vadem VG-469, contain additional features
  such as ISA Plug-n-Pray. Detection of controllers utilizing Plug-n-Pray
  should not be difficult to add, but I don't have hardware on which to test.
* Add option to dump CIS to a formatted text file or a raw dump.
* Add option to change Upper Memory Area used for scanning.
