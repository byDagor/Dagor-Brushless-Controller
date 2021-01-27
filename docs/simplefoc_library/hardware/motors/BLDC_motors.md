---
layout: default
title: BLDC motors
nav_order: 1
description: "Arduino Simple Field Oriented Control (FOC) library ."
permalink: /bldc_motors
parent: Motors
grand_parent: Supported Hardware
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
---

# BLDC motors 

Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> supports most of the common 3 phase BLDC motors, both 
- [Gimbal motors](#gimbal-motors) - low KV (higher resistance ~10Ω) 
- [High performance BLDC motors](#high-performance-motors) - high KV ( low resistance <1Ω ). 

## Gimbal motors

Gimbal motors will work basically with any BLDC motor driver, but since the high-performance drivers have current measurement circuits optimized for high currents you will not have any benefit of using them. Therefore low-power BLDC motor drivers will have comparable performance as the expensive high-power, high-performance drivers for gimbal motors. What is in my opinion very cool! 😃 This was one of the main motivations to start developing  <span class="simple">Simple<span class="foc">FOC</span>Shield</span>.

Some of the characteristics of Gimbal motors are:
 - High torque on low velocities
 - Very smooth operation
 - Internal resistance >10Ω
 - Currents up to 5A

Gimbal motors are very versatile and their main benefit is very smooth operation on low speeds and high torque. They can be used in may different applications everywhere from being a high-quality replacement for your stepper motor or DC servo motor to very smooth camera gimbals and many different robotics applications. One of very interesting use cases are student experiments, where BLDC motors provide a very high degree of control and dynamics, such examples are ball and plate,  inverted pendulums, balancing robots and similar. 
<blockquote class="info"> <p class="heading">EXAMPLE</p>Make sure to check out the <a href="simplefoc_pendulum">reaction wheel inverted pendulum <i class="fa fa-external-link"></i></a> developed using <span class="simple">Simple<span class="foc">FOC</span>library</span>, <span class="simple">Simple<span class="foc">FOC</span>Shield</span> and one gimbal motor. </blockquote>

Here are some examples of the different price range gimbal motors that have been successfully tested with this library.

Examples | Description | Specifications | Link | Price 
---- | ---- | ---- | ---- | ----
[<img src="extras/Images/mot.jpg" style="height:100px">](https://www.ebay.com/itm/iPower-Gimbal-Brushless-Motor-GBM4108H-120T-for-5N-7N-GH2-ILDC-Aerial-photo-FPV/254541115855?hash=item3b43d531cf:g:q94AAOSwPcVVo571) | IPower GBM4198H-120T |  - 12N14P <br> - 98g  <br> - 11.4Ω <br> - 45x25mm| [Ebay](https://www.ebay.com/itm/iPower-Gimbal-Brushless-Motor-GBM4108H-120T-for-5N-7N-GH2-ILDC-Aerial-photo-FPV/252025852824?hash=item3aade95398:g:q94AAOSwPcVVo571:rk:2:pf:1&frcectupt=true) | 25$
 [<img src="extras/Images/mot2.jpg" style="height:100px">](https://www.ebay.com/itm/Brushless-Gimbal-Motor-BGM4108-130HS-for-DYS-BLG3SN-DSLR-Camera-Mount-DIY/281372437636?epid=1239081107&hash=item41831aac84:g:K3kAAOSwVFlT20du) | BGM4108-130HS |  - 24N22P <br> - 93g  <br> - 17Ω <br> - 46x25mm| [Ebay](https://www.ebay.com/itm/Brushless-Gimbal-Motor-BGM4108-130HS-for-DYS-BLG3SN-DSLR-Camera-Mount-DIY/281372437636?epid=1239081107&hash=item41831aac84:g:K3kAAOSwVFlT20du) | 30$
 [<img src="extras/Images/mot3.jpg" style="height:100px">](https://www.ebay.com/itm/Alloy-2208-90KV-Gimbal-Brushless-Motor-for-Gopro3-RC-Drone-Camera-100-200g/223195701385?hash=item33f7802089:g:cjUAAOSw1iVbyccJ) | 2208 90KV Gimbal motor |  - 12N14P <br> - 39g  <br> - ?Ω <br> - 29x25mm| [Ebay](https://www.ebay.com/itm/Alloy-2208-90KV-Gimbal-Brushless-Motor-for-Gopro3-RC-Drone-Camera-100-200g/223195701385?hash=item33f7802089:g:cjUAAOSw1iVbyccJ) | 15$
 [<img src="extras/Images/bigger.jpg" style="height:100px">](https://www.onedrone.com/store/ipower-gbm5108-120t-gimbal-motor.html) | GBM5108-120T |  - 24N22P <br> - 175g  <br> - 12.6Ω <br> - 60x24mm| [Onedrone](https://www.onedrone.com/store/ipower-gbm5108-120t-gimbal-motor.html) | 90$
 [<img src="extras/Images/big.jpg" style="height:100px">](https://fr.aliexpress.com/item/32483131130.html?spm=a2g0o.productlist.0.0.6ddd749fFd3u9E&algo_pvid=a67f2ec1-5341-4f97-ba3e-720e24f6c4fb&algo_expid=a67f2ec1-5341-4f97-ba3e-720e24f6c4fb-10&btsid=0b0a187915885172220541390e7eed&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) | GBM8017-120T | - 24N22P <br> - 318g  <br> - 14.7Ω <br> - 90x13mm| [Aliexpress](https://fr.aliexpress.com/item/32483131130.html?spm=a2g0o.productlist.0.0.6ddd749fFd3u9E&algo_pvid=a67f2ec1-5341-4f97-ba3e-720e24f6c4fb&algo_expid=a67f2ec1-5341-4f97-ba3e-720e24f6c4fb-10&btsid=0b0a187915885172220541390e7eed&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_) | 150$


## High-performance motors
Gimbal motors are just a subset of all the BLDC motors there is. As suggested in previous chapters, when using high-torque ( currents > 5A), low-resistance (~1Ω) BLDC motors such as drone motors make sure your BLDC driver can support the currents necessary. <span class="simple">Simple<span class="foc">FOC</span>library</span> has been tested with several high performance BLDC drivers ([supported BLDC drivers list](drivers)). 

For example, if we limit ourselves to the [Aliexpress DRV8302 board](https://bit.ly/2BZZ5fG) we will be searching for motors with the peak current lower than 27A and continuous current of 15A. Here are some motors that can fit in that category:

Examples | Description | Specifications | Link | Price 
---- | ---- | ---- | ---- | ----
[<img src="extras/Images/n2830.png" style="height:100px">](https://ebay.to/2OTy7tk) | N2830 1000KV |  - 7.4-11.1V <br> - (max)20A  <br> - 0.104Ω <br> - 28 x 28mm| [Ebay](https://ebay.to/2OTy7tk) | 10$
[<img src="extras/Images/c2216.png" style="height:100px">](https://ebay.to/2ZZTT4S) | C2216 880KV |  - 7-18V <br> - (max)22A  <br> - 0.108Ω <br> - 28 x 34mm| [Ebay](https://ebay.to/2ZZTT4S) | 20$
  [<img src="extras/Images/ml4114.png" style="height:100px">](https://amzn.to/3f38b9p) | GARTT ML4114 330KV |  - 36V <br> - (max)25A  <br> - 0.1082Ω <br> - 40 x 20mm| [Amazon](https://amzn.to/3f38b9p) | 40$
 [<img src="extras/Images/jk42.png" style="height:100px">](https://amzn.to/3hB7h5r) | WJN-Motor JK42BL |  - 24V <br> - (max)15A  <br> - 0.8Ω <br> - 42 x 61mm| [Amazon](https://amzn.to/3hB7h5r) | 60$
 [<img src="extras/Images/mad5008.png" style="height:100px">](https://amzn.to/2OWwgE3) | MAD5008-240KV |  - 36V <br> - (max)22A  <br> - 0.08Ω <br> - 56 x 25mm|  [Amazon](https://amzn.to/2OWwgE3) | 60$