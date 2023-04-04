# Cases and jigs

## Case
Some designs for a cases and programming jigs have been made for this project.

The case fits the ESP32-board and can optionally house two 1.54" tags, in order to make a compact accesspoint with two additional integrated screens.

<img width="300" alt="board" src="https://user-images.githubusercontent.com/2544995/227193477-94142f07-799f-415c-bf27-3f6f0049880e.png"><img width="300" alt="board" src="https://user-images.githubusercontent.com/2544995/227194631-f6dfbbc1-66d5-473b-8230-bf1ced96766f.png">
<img width="300" alt="board" src="https://user-images.githubusercontent.com/2544995/229891370-959ecb51-72a7-4d29-8478-05a0d269046c.png">


Additionally, two other covers van be printed for this case. One blank cover (useful if you just want a flasher, no AP), and one with room for a segmented tag.

<img width="400" alt="board" src="https://user-images.githubusercontent.com/2544995/227195248-d085a118-3ec1-47a6-8a67-169c40175c10.png"><img width="400" alt="board" src="https://user-images.githubusercontent.com/2544995/229890912-bc074daf-6443-4f3d-9c76-1645eb2939e2.png">


## Jigs
The programming jigs use pogo pins to make a connection to the tag, allowing you to flash the alternative firmware without soldering wires. 

<img width="400" alt="board" src="https://user-images.githubusercontent.com/2544995/227195517-eff8949b-3ef4-45d7-853a-cd52a13faf8f.png">

The jigs here make extensive use of the boards found in the 'PCB' folder. This is due to the small pin spacing on the debug header, which doesn't lend itself well to using only a 3D-printer and some pogo pins. Especially on the segmented tag, the tolerances can be quite small. The PCB's mitigate this problem by keeping the pins aligned much better than extruded plastic could. It's possible to make jigs without those PCB's, you can use the jigs shared [here](https://github.com/atc1441/ZBS_Flasher/tree/main/jig).

<img width="400" alt="board" src="https://user-images.githubusercontent.com/2544995/227196016-894e55f6-02b3-42ff-a13e-d77532d80f33.png"><img width="400" alt="board" src="https://user-images.githubusercontent.com/2544995/227196542-117fe539-0cab-4753-8685-0db2dfbfd641.png">


Also, the pins are pretty fragile. By soldering them to an additional board at the upper side of the jig, additional strength is added. It may be necessary to trim the PCB pieces down a bit in order to fit in the jig, or through the debug-header-opening of the tag.

<img width="400" alt="board" src="https://user-images.githubusercontent.com/2544995/227197408-69baf3f8-7ff3-4c3a-a02c-8e9ee521e162.png">

The jigs use 1mm pogo pins. Don't use too much force, or you'll break them. Also, don't use your nails to coax them in; they will slip and end up under your fingernails. Ask me how I know.

The PCB under the jig can be fastened by small self-tapping screws. Attach the jig to the flasher using a 10-pin 2x5 1.27mm spacing cable. If you decide to make your own cables, don't be an idiot like me, and make sure you order 0.635mm pitch flatcable.
<img width="400" alt="board" src="https://user-images.githubusercontent.com/2544995/227199542-9e7b897e-761c-4d2a-bb1a-3d07969db9d2.png">
