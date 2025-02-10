# Arduino Nano Low Power Sleep Mode Test

Tiny sketch to put the Arduino Nano into sleep mode, after blinking its
LED a few times, so you can test the quiescent current of the module.
This can be run with the bare module connected to a programmer with
nothing else for the test needed, other than the ability to measure the
quiescent current the module is drawing from the USB. A good test to run
on a module before wiring it into a project. :smirk:

I discovered that not all Arduino modules on AliExpress are made equal.
Some have genuine Atmel chips, have some Atmel clones and others, like
the ones I got stuck with, which have cloned AT328p and an unmarked
`CH340`/`FT230` like USB driver clone chips that draw 3.8mA all on their
own, whether the USB cable is connected or not. I say 'like' because
these don't match the pin-out of either chips and being totally unmarked
makes it harder to get find their datasheet.

This is what motivated me to do down a rabbit hole for a few days trying
to figure out how I can make my module go to sleep and use less than a
100µA of current, 80-90µA actually, as I was able to do it on my other
project with the Arduino Pro Mini clone.

Contrary to some forums where the issue was discussed and heatly
debated, the issue boiled down to the unmarked USB driver chip and not
the cloned AT328p controller chip. I know. I replaced the controller
chip on one module, with an official Atmel 328p chip and the current
draw did not budge. However, when I disabled the power to the USB driver
chip, current draw in sleep mode went down to 25µA (according to my
trusty Fluke 189).

## Real Project µA Sleep Mode

### Hardware Mods Required

Some hardware changes will need to be made to the module. The first two
you will need to do on the Nano module whether it is an official one or
a clone:

1. Power LED needs to be disabled by removing it or its 1k resistor. It
   draws about 3-4mA.

   I find removing the resistor is easier to come back from. First they
   are harder to destroy than LEDs, and much more abundant to remove
   from other modules.
2. Remove the L3117 LDO regulator on the under side of the module. It
   has a quiescent current draw of 5-8mA.
3. If you are unlucky enough to have gotten the `CH340` quasimodo clone,
   then things will get more ugly.
   1. You will need to modify the module to isolate the USB driver chip
      from the 5V supply by desoldering and bending up its two `GND`
      pins up from their pads.
   2. You will need to cut a trace connecting the USB 5V directly to +5V
      of the module. Color me surprised when I found out there is no
      Schottky diode isolating the module power from USB, if it is
      externally powered.
   3. Add a Schottky diode from the USB power side to the +5V power
      side, to allow the board to be powered externally or from the USB.
   4. To use USB port for communications and programming when it is
      connected but disable the USB driver chip when it is not, you will
      need to:
      1. Desolder a resistor that connects to the now isolated USB line
         and solder it to the now +5V isolated from USB power trace.
         Without changing the pull-up source for the resistor, it will
         pull the USB power in line to 2V through a 4.7k resistor. This
         will either turn on the MOSFET all the time, if you use a high
         value resistor to ground the gate pin, or will barely turn it
         on so that the USB driver is not working but drawing additional
         200, 300 or 500 µA of current when in sleep mode. This kept me
         up for two days trying to figure out why am I seeing this extra
         current draw when the USB driver is disabled.
      2. Add an N-MOSFET and a resistor which will switch the
         disconnected `GND` pins of the USB driver, to ground when
         drawing power from the USB. This will allow the module to work
         as before if the USB is connected, but will shut off the USB
         driver glutton when powered externally, like from rechargeable
         batteries.

### Project Dependent Software Mods

In a real project the `power_down()` function should do the following
having before sleep, in addition to what it is doing now:

1. Change all inputs not used to wake up the CPU on state change from
   `INPUT_PULLUP` to `INPUT`. This ensures that if any of these are
   active during sleep they will not cause an extra 500 or 330 µA
   through each resistor, at 5V or 3.3V respectively.

2. Disable any modules in the project which can be disabled to reduce
   power drain.

3. Enable level change interrupts on all inputs allowed to wake up the
   CPU.

It will have to reverse the above on wake up:

1. Disable level change interrupts on all inputs enabled before going to
   sleep.
2. Restore `INPUT_PULLUP` on all inputs disable above.
3. Go into a loop waiting for all inputs which can wake up the CPU to go
   inactive. This last step is optional but a good idea to prevent a
   button press, which was used to wake up the CPU from being
   interpreted as a regular button press after the `power_down()`
   function returns.

### Required Tools

Unless you have done SMT re-work and already familiar with the setup,
you will need to have the following:

1. Something to measure the low current in sleep mode (µA range), to
   verify that mods worked and how well.

   I used my Fluke 189 on the µA setting to do it with accuracy, but a
   more convenient way to do it is to use the
   [FNIRSI® FNB58 USB Fast Charge Tester](https://www.fnirsi.com/products/fnb58),
   which shows USB current draw to 10µA resolution and is within 25µA to
   75µA of the Fluke. It is a feature packed USB tester and more
   affordable than the Fluke 289 modernized replacement for the 189
   (search for the 289 and if you don't get sticker shock, I am looking
   to be adopted.) I got my FNB58 on Amazon for CDN$75 in Nov 2024, at
   the time of writing (Feb2025, after the precipitous drop of CDN vs
   the USD) it is CDN$68 on the FNIRSI website and CDN$53 on AliExpress
   with free shipping.

2. Temperature controlled soldering iron with tips for SMT rework. I
   tried to work with my PACE soldering/desoldering/hot-air stations and
   found that I had no tips sized for the tiny, microscopic really, SMT
   components. The 1/32" tip which served me fine for years, felt like a
   golf club when working with modern SMT. I looked into getting
   appropriate soldering tips for the PACE but didn't feel like spending
   CDN$120 on an 8-piece set or buying individually a few that I needed
   at CDN$28 a pop.

   After trying to make do with my existing equipment, I caved in and
   got a
   [FNIRSI HS-02B](https://www.fnirsi.com/products/hs-02?variant=46042249232602),
   on AliExpress for less that it would cost me to get tips for my PACE.
   I wouldn't be able to do this work without the upgrade.

   It is so fast to heat up that I have it configured to go to stand-by
   (at 100C) in 30 seconds of being idle. Comes out of sleep when I pick
   it up and heats up to 360C in about a second, before I am ready to
   use it.

   It heats up from dead cold 18C to 360C in less 3 seconds, USB-C
   powered, 100W Max or configurable to lower for use with a lower
   wattage USB supply, weighs nothing, has fine tips that do not get in
   the way and are swapped by just pulling them out and inserting.

   Finally, a fully loaded set is under CDN$120 on the official site, on
   AliExpress and you can find it on special there down to CDN$105. My
   PACE soldering station cost me $500 and desoldering/hot-air rework
   another $900, in 25-years ago dollars. They work great and are
   reliable, but if I was getting my electronics hobby workshop together
   today, I would not want to fork out that kind of money.

3. Unless you are young and eagle-eyed, a bi-focal magnifier to allow
   you to see what you are doing. A digital microscope screen will work
   but not having depth perception will make it a lot harder to
   coordinate your movement at small scale, without fair bit of
   practice.

4. Quality fine tweezers that will not slip/snap around the tiny
   components flicking them somewhere in the room, never to be found.

5. Soldering supplies: fine (0.015" or 0.4 mm) flux core solder, flux
   paste in a syringe for SMT, 2 or 1 mm wide solder wick.

