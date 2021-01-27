---
layout: default
title: v2.0.2
description: "Arduino SimpleFOCShield board fabrication"
parent: Board Fabrication
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span>
nav_order: 2
permalink: /arduino_simplefoc_shield_fabrication_v2
---
# Fabrication guide for <span class="simple">Simple<span class="foc">FOC</span>Shield</span> <small><i>v2.0.2</i></small> 
Here is a quick guide how to fabricate the Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span> [version <i class="fa fa-tag"></i>V2.0.2](https://github.com/simplefoc/Arduino-SimpleFOCShield/releases) 


<p align="">
<img src="https://raw.githubusercontent.com/simplefoc/Arduino-SimpleFOCShield/v2.0.2/images/top.png"  class="width30"><img src="https://raw.githubusercontent.com/simplefoc/Arduino-SimpleFOCShield/v2.0.2/images/bottom.png"  class="width30">
</p>

## Board releases
To check the release timeline, click [here](https://github.com/simplefoc/Arduino-SimpleFOCShield/releases) 

Version  | link | Release date | Comment
----- | ----- | ---- | ----
*Simple**FOC**Shield* v2.0 |[release v2.0](https://github.com/simplefoc/Arduino-SimpleFOCShield/releases/tag/v2.0) | 01/21 | - 3A in-line current sensing <br>- 5V regulator <br>- new pinout for hardware config 
*Simple**FOC**Shield* v2.0.1 |[release v2.0.1](https://github.com/simplefoc/Arduino-SimpleFOCShield/releases/tag/v2.0.1) | 01/21 | - reduced via size <br> - configurable range
*Simple**FOC**Shield* v2.0.2 |[release v2.0.2](https://github.com/simplefoc/Arduino-SimpleFOCShield/releases) | 01/21 | replaced 7805(connected to 5V) with 7808 (connected to VIN) to be compatible with stm32 Nucleo-64




[<i class="fa fa-github"></i> Github](https://github.com/simplefoc/Arduino-SimpleFOCShield) repository contains: 
- <b><i class="fa fa-file"></i> Schematics</b> - PDF file
- <b><i class="fa fa-file"></i> Gerber</b> - Gerber, NC Drill & Assembly files
- <b><i class="fa fa-file"></i> Bill Of Materials (BOM)</b> - Excel sheet
- <b><i class="fa fa-file"></i> 3D model</b> -  3D PDF & STEP file
- <b><i class="fa fa-file"></i> PCB design project</b> - Altium Designer 2019


## Fabrication procedure and cost
The price of the fabrication will highly depend of the number of boards and the companies you decide to use both as PCB manufacturers and the component suppliers. Here are two nice articles explaining how to approach the problem choosing the PCB manufacturer form [electronics-notes.com](https://www.electronics-notes.com/articles/constructional_techniques/printed-circuit-board-pcb/how-to-choose-right-best-pcb-manufacturer.php) and component supplier from [pcbonline.com](https://www.pcbonline.com/blog/How_to_Choose_a_PCB_Component_Supplier_165.html). 
But in most of cases it will always boil down to the price :)

## PCB fabrication + Self assembly
Even though most of the components are big smd components and the board is intentionally made to me solderable by hand the curent sense amplifiers are going to be a bit more difficult. But if you are interested in this way of producing this board here is a quick guide how to make it.

### PCB fabrication
Download the [<i class="fa fa-file"></i> gerber files](https://github.com/simplefoc/Arduino-SimpleFOCShield) and provide them to your PCB manufacturer of choice. So far I have used [seeedstrudio](https://www.seeedstudio.com/) and [JLCPCB](https://jlcpcb.com/). The price of 10 PCBs is around 5\$ but the shipping was around 20\$. 

Quantity | SeedStudio |  JLCPCB
---- | ---- | ---- | ---
 5 | 5€ | 2€
 10 | 5€ |2€
 30 | 30€ | 15€
 50 | 50€ | 30€
 
<i class="fa fa-info-circle"></i> <i><small>On top of these prices you will have to pay shipping of approximately  20€.</small></i>

### Component purchase
Here is a complete list of all the components you will need to make the board:
<table>
      <thead>
         <tr>
            <th>Component</th>
            <th>Description</th>
            <th>Quantity</th>
            <th>Manufacturer Number</th>
         </tr>
      </thead>
      <tbody>
         <tr>
            <td colspan="4"><b>L6234 circuit:</b> <a href="https://www.st.com/resource/en/datasheet/l6234.pdf"><i class="fa fa-file"></i> Datasheet</a></td>
         </tr>
         <tr>
            <td>C1</td>
            <td>0.1 uF</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/KEMET/C1206C104M5RECAUTO?qs=MLItCLRbWswBKiY20DF1SA%3D%3D">C1206C104M5RECAUTO</a></td>
         </tr>
         <tr>
            <td>C2</td>
            <td>0.01 uF</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/KEMET/C1206C103J3RACAUTO?qs=%2Fha2pyFaduhAFP6oO4LLeYMkrC9QNia0EjiZTqcgzLScln%252BPiND5Ww%3D%3D">C1206C103J3RAUTO</a></td>
         </tr>
         <tr>
            <td>C3</td>
            <td>1 uF</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/KEMET/C1206X105K3RAC3316?qs=%2Fha2pyFaduilEz%252BiJtRzoz0gb0S3v4m%252B2vm5WoIZPYxGhbTceT8iyu5uY%252BnsPWGD">C1206X105K3RAC3316</a></td>
         </tr>
         <tr>
            <td>C4</td>
            <td>0.22 uF</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/KEMET/C1206C224J5RECAUTO7210?qs=%2Fha2pyFaduiFNVbEFQqG8g760vwSal6p%252BrMckdrZBQmtOlARWq3l2WWJv5HhNnqv">C1206C224J5RECAUTO7210</a></td>
         </tr>
         <tr>
            <td>CAP1</td>
            <td>100 uF (Electrolytic)</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/Panasonic/EEE-HAV101XAP?qs=%2Fha2pyFadujAo14cOabh4%2FHGWJclSBJVoXpO6qVRwLQTQ6LscWsHQA%3D%3D">EEEHAV101XAP</a></td>
         </tr>
         <tr>
            <td>BAT1</td>
            <td>Schottky diodes</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/Toshiba/TBAT54SLM?qs=kdd6aVn74hyQL5%252Beb9w%252BHw%3D%3D">TBAT54S,LM</a></td>
         </tr>
         <tr>
            <td>L6234</td>
            <td>Motor driver chip</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/STMicroelectronics/L6234PD?qs=lgHKUCmDFtgFRXXnpwFpNg%3D%3D">L6234PD</a></td>
         </tr>
         <tr>
            <td colspan="4"><b>Pull Up and Pull Down resistors</b></td>
         </tr>
         <tr>
            <td>PULL_SCL, PULL_SDA, R2</td>
            <td>4.7kΩ </td>
            <td>3</td>
            <td><a href="https://www.mouser.fr/ProductDetail/603-RC1210FR-074K7L">603-RC1210FR-074K7L</a></td>
         </tr>
         <tr>
            <td>PULL_A, PULL_B, PULL_I</td>
            <td>3kΩ </td>
            <td>3</td>
            <td><a href="https://www.mouser.fr/ProductDetail/KOA-Speer/RN73H2ETTD3001F50?qs=%2Fha2pyFadugz1PN4m8q5QBmmNYUlMOzQI3k%2FT%252B8vFrn5l%2FvB8B97FQ%3D%3D">RN73H2ETTD3001F50</a></td>
         </tr>
         <tr>
            <td colspan="4"><b>LED circuit</b></td>
         </tr>
         <tr>
            <td>D1</td>
            <td>Red LED</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/Wurth-Elektronik/156120RS75300?qs=%2Fha2pyFaduhtSsTKzZu8BG2kEWNH5l3iOIVGi20HkjmxMeBY4VpJSw%3D%3D">156120RS75300</a></td>
         </tr>
         <tr>
            <td>R1</td>
            <td>620Ω </td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/KOA-Speer/RN73H2ETTD6200F50?qs=%2Fha2pyFadugz1PN4m8q5QKhCzpicGijTcn6N2kk6lgXhcYi6JSLlrg%3D%3D">RN73H2ETTD6200F50</a></td>
         </tr>
         <tr>
            <td colspan="4"><b>Linear regulator circuit</b></td>
         </tr>
         <tr>
            <td>L1</td>
            <td>L7808</td>
            <td>1</td>
            <td><a href="https://eu.mouser.com/ProductDetail/STMicroelectronics/L7808ABD2T-TR/?qs=%2Fha2pyFaduhJaWWi9Q0Ux5qBhxdStDSDVBKjt6TWzIYi2UHCV3ncDg%3D%3D">L7808ABD2T-TR</a></td>
         </tr>
         <tr>
             <td>CAP2</td>
            <td>10 uF (Electrolytic)</td>
            <td>1</td>
            <td><a href="https://eu.mouser.com/ProductDetail/Lelon/VZH-100M1HTR-0606/?qs=%2Fha2pyFadug9fpMd9zS1WVuLY1XOmrV1BVzFD1joM%252BDTbPmM%252BxSOHg%3D%3D">VZH100M1HTR-0606</a></td>
         </tr>
         <tr>
            <td colspan="4"><b>Curent sense circuit</b></td>
         </tr>
         <tr>
            <td>CBY1</td>
            <td>0.1 uF</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/KEMET/C1206C224J5RECAUTO7210?qs=%2Fha2pyFaduiFNVbEFQqG8g760vwSal6p%252BrMckdrZBQmtOlARWq3l2WWJv5HhNnqv">C1206C224J5RECAUTO7210</a></td>
         </tr>
         <tr>
             <td>INA1, INA2</td>
            <td>INA240A2 current sense amp</td>
            <td>2</td>
            <td><a href="https://eu.mouser.com/ProductDetail/Texas-Instruments/INA240A2QPWRQ1/?qs=Bho%2FbeBaDEy8f96mmgMD%2Fw%3D%3D">INA240A2</a></td>
         </tr>
         <tr>
            <td>RCS1, RCS2</td>
            <td>0.01Ohm 2512</td>
            <td>2</td>
            <td><a href="https://eu.mouser.com/ProductDetail/Vishay-Dale/WSLT2512R0100FEA/?qs=eFWhpKjIuGgySlBeZZzAWQ%3D%3D">WSLT2512R0100FEA</a></td>
         </tr>
         <tr>
            <td colspan="4"><b>Terminal connectors</b></td>
         </tr>
         <tr>
            <td>TB_M1</td>
            <td>3 pin, 5mm</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/CUI-Devices/TB002-500-03BE?qs=%2Fha2pyFadujMo%2F8XIx7GL3VaKbn4rpnI4huWO6RUre2577fclJuWwA%3D%3D">TB002-500-03BE</a></td>
         </tr>
         <tr>
            <td>TB_PWR</td>
            <td>2 pin, 5mm</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/CUI-Devices/TB002-500-02BE?qs=%2Fha2pyFadujMo%2F8XIx7GL%2F8B4TM%252BUPJvcyODkgPPYDPGTjOBZNS5pw%3D%3D">TB002-500-02BE</a></td>
         </tr>
         <tr>
            <td colspan="4"><b>Header connectors</b></td>
         </tr>
         <tr>
            <td>P1, P4</td>
            <td>8 pin, female, long</td>
            <td>2</td>
            <td><a href="https://www.mouser.fr/ProductDetail/872-920-0086-01">872-920-0086-01</a></td>
         </tr>
         <tr>
            <td> P2</td>
            <td>10 pin, female, long</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/872-920-0087-01">872-920-0087-01</a></td>
         </tr>
         <tr>
            <td>P3</td>
            <td>6  pin, female, long</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/Gravitech/6fx1L-254mm?qs=%2Fha2pyFadugTMKIzmATdF3ycHTdv4fz%2FLeD9aI6nqeEU9o9FRZ5XDw%3D%3D">6fx1L-254mm</a></td>
         </tr>
         <tr>
            <td>P_ENC</td>
            <td> 5 pin, male/female</td>
            <td>1</td>
            <td><a href="https://www.mouser.fr/ProductDetail/649-1012937990501BLF ">649-1012937990501BLF</a></td>
         </tr>
      </tbody>
   </table>

The same list ( a bit optimized for PCB manufacturers ) you can download on link [<i class="fa fa-file"></i> Bill of Materials](https://github.com/simplefoc/Arduino-SimpleFOCShield). In my case I have used [Mouser](https://www.mouser.com/) as my component supplier but the components of this board should be easy to find with any other supplier. The price of the components was around 10\$ per board and the shipping was again around 20\$ (for orders under 50\$).    
<blockquote class="warning"> <p class="heading">Note</p>
The components with the manufacturers numbers I have chosen are not really set in stone, feel free to optimize them both in terms of prices and values if necessary. </blockquote>

### Board assembly and soldering
Once when you have all the components and the PCBs the fun begins. All the components are relatively easy to solder. All the capacitors are `1206`, all the resistors are `1210`, the connectors are all through-hole and there is no real small SMD components. Therefore with some patience and you should have no problems soldering this board with a regular soldering iron and a bit of time. Therefore take a coffee and one deep breath and you are ready for 20 minutes of fun. The only non-fun part are two current sense amplifier INA240, but even they are not soooooo small 😊 .

<blockquote class="info"> <p class="heading">Note</p>
The board itself already has the descriptor names of the components on it so it should be easy to determine which component goes where. There is also an assembly drawing in the Pick and Place folder of the <a href="https://github.com/simplefoc/Arduino-SimpleFOCShield"><i class="fa fa-file"></i> gerber files</a> if you prefer to have a printed version. 
</blockquote>


## PCB fabrication and assembly service - **preferred choice** 📢
This option is a lot simpler and produces much better results. It is also even cheaper for larger quantities > 50pcs.
To order the boards, download the [<i class="fa fa-file"></i> Gerber files](https://github.com/simplefoc/Arduino-SimpleFOCShield) and [<i class="fa fa-file"></i> Bill of Materials](https://github.com/simplefoc/Arduino-SimpleFOCShield) and supply it to your manufacturer of choice. In my case, I'm using the [JLCPCB](https://jlcpcb.com/).

<iframe class="youtube"  src="https://www.youtube.com/embed/sax_9sUgBuk" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## Fully assembled versions
Order your own fully tested and assembled Arduino <span class="simple">Simple<span class="foc">FOC</span>Shield</span>  from our [shop](https://simplefoc.com/simplefoc_shield_product).

