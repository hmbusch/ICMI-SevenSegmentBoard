# ICMI Seven-Segment Board

* Author          : Hendrik Busch
* Current version : v1.1
* License         : Creative Commons Attribution-NonCommercial-
                    ShareAlike 3.0 Germany License.

## What is it?

The Seven-Segment Board offers up to 8 seven-segment digits with a
control circuit made up of several cheap and easy to obtain
components.

## Wait, there are dedicated ICs for that...

Yes, there are, e.g. the [MAX7219][1]. The point of this project is 
not the create an equal or better board, but to learn all aspects
required to design, manufacture and program such a board. If you 
need a hasslefree seven-segment-board, then this project is probably
not for you.

## Initial Note
> [!IMPORTANT]
> WARNING: The current board (v1.1) still has a design flaw, that I
> built into the initial design because I didn't really have a clue
> what I was doing (and still didn't have one when I made the v1.1
> version recently, many years later).
>
> The common anodes of the digits ideally should be switched by a
> p-channel MOSFET, but since I didn't have one available, I used
> an n-channel MOSFET and put the load on the drain. What I didn't
> realise until much later: the LED on the drain creates a voltage
> drop (about ~1.6V usually) and the MOSFET cannot reach the saturation
> zone with only a small voltage difference between the dropped voltage
> and drain. So it isn't fully conducting and thus limits the current
> well below 10mA, rendering the display very dim.
>
> It's not possible to simply replace the n-channel with a p-channel
> MOSFET as those are on when their gate pin is low - which is the 
> opposite to how the switchting currently works. The 4017 decade counter
> that iterates the digits on the anode side, pull the current output
> pin high, so it would switch the p-channel MOSFET off.
>
> A future revision will use load switches (which is basically a
> combination of the required p-channel MOSFET, but driven by an 
> n-channel MOSFET to achieve the same switching logic as on the
> low side).

## Seven-segment LED circuit

The board uses dual common anode seven-segment displays, meaning that
each component feature two digits with seven segments each. Each digit
has its own anode that is common to all segments in that digit (thus
common anode) and each segment has its own cathode, resulting in two 
anodes and 14 cathodes per component. Each segment cathode of a
digit is connected to all the other cathodes for this segment on
the other digits.
The LED circuit therefore features 8 anodes (one for each digit) and
7 cathodes (one for each segment).

### Carrier Boards

I added carrier board for several types of dual-digit seven segment 
displays in my possession the make it easier to use them in breadboarding
and experimental circuits. There currently are carrier boards for:

- Kingbright DA-04/DC-04
- Kingbright DA-52/DC-52
- Oasistek TOD-4201
- RFT VQE21/VQE22 (yes, the 30 year old ones)

Especially the first and the third carrier will also fit different
seven segment displays currently available for purchase.

Please note that the supply pins are always routed to pins 1 & 2,
regardless of the polarity of the display. Common anode ones will
have the anodes connected to these pins (and cathodes to the rest),
common cathode ones have their cathodes connected to 1 & 2.

Keep that in mind before mixing and matching different displays.

## Switching circuit

The switching circuit uses a minimum of I/O 5 pins (excluding power).

Two are needed for a 74HC4017 decade counter that is responsible for
sourcing power to the digits, one at a time. Using a software timer,
the decade counter switches through each digit so fast, that the
human eye cannot distinguish between the single digits being turned
on and off but sees a steady display with all eight digits lit (this
is called the POV (persistence-of-vision) effect. Exploiting this
effect, the software and circuit only have to worry about displaying
a single digit at a time and switching between digits.

In order to control what each digit is actually displaying, a shift
register (74HC595) is used to set the state of each segment. As the
shift register is not made to sink current, it is connected to an
ULN2803A darlington array. If one of the shift register pins is HIGH,
the corresponding switch in the darlington array sinks the incoming
LED current on that pin, effectively lighting the corresponding 
segment.

The software must ensure that the digit switching and the segment
configuration are in sync, otherwise the display content may show
as distorted of shifted.

## Version History

- v1.1
  - DO NOT USE, SEE NOTE AT TOP OF DOCUMENT
  - The CLEAR pin of the shift register is now connected to VCC and 
    not to GND as before. Pulling it to GND in v1.0 resulted in a
	  continous purge of any data loaded into the register and thus
	  rendering the display unusable.
  - added some connection possibilities for capacitors


- v1.0
  - DO NOT USE, SEE NOTE AT TOP OF DOCUMENT
  - Initial revision

[1]: https://www.analog.com/en/products/MAX7219.html