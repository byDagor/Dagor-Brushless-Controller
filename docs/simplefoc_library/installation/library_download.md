---
layout: default
title: Library Installation
parent: Installation
nav_order: 1
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /library_download
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# Installing <span class="simple">Simple<span class="foc">FOC</span>library</span>

## <i class="fa fa-desktop"></i> Arduino Library Manager installation
The simplest way to get hold of the library is directly through `Arduino IDE` and *Arduino Library Manager*.
<p><img src="extras/Images/alm.gif" class="width80"></p>  
- Open Arduino IDE and start *Arduino Library Manager* by clicking: `Tools > Manage Libraries...`.
- Search for *"Simple FOC"* library and install the [latest version <i class="fa fa-tag"></i>.](https://github.com/simplefoc/Arduino-FOC/releases)
- Reopen Arduino IDE and you should have the library examples in `File > Examples > Simple FOC`.

## <i class="fa fa-github"></i> Github installation
If you prefer downloading the library directly from the [github repository](https://github.com/simplefoc/Arduino-FOC). 

### <i class="fa fa-download"></i> ZIP download 
- Go to the [<i class="fa fa-code-fork"></i> main](https://github.com/simplefoc/Arduino-FOC) branch
- Click first on `Clone or Download > Download ZIP`. 
- Unzip it and place it in `Arduino Libraries` folder. Windows: `Documents > Arduino > libraries`.  
- Reopen Arduino IDE and you should have the library examples in `File > Examples > Simple FOC`.

### <i class="fa fa-terminal"></i> Clone using terminal 
- Open terminal and run
```sh  
cd #arduino libraries folder
git clone https://github.com/simplefoc/Arduino-FOC.git
```
- Reopen Arduino IDE and you should have the library examples in `File > Examples > Simple FOC`.
