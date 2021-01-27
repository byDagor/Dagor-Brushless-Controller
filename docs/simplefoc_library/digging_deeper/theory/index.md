---
layout: default
title: Theory corner
parent: Digging deeper
grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 1
permalink: /theory_corner
has_children: True
has_toc: False
---

# Theory lovers corner

This is the section intended to explain some interesting theoretical features of the Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span> implementation. For now, I have briefly written only about few topics, but the plan is to cover much more topics and much more in depth.


<div style="display:grid; width:100%">
    <div >
        <a href="voltage_torque_control">
            <h3 style="color:inherit"> <i class="fa fa-lg fa-graduation-cap" style="padding:10px"></i> Torque control using voltage</h3>
        </a>
        <p> Short explanation and justification of the torque control using voltage instead current. </p>
    </div>
    <div>
        <a href="foc_theory">
            <h3 style="color:inherit"> <i class="fa fa-lg fa-graduation-cap" style="padding:10px"></i> Field Oriented Control algorithm</h3>
        </a>
        <p> A brief overview of implemented modulation methods and the main principles of the FOC algorithms for BLDC motors.</p>
    </div>
    <div>
        <a href="low_pass_filter">
            <h3 style="color:inherit"> <i class="fa fa-lg fa-graduation-cap" style="padding:10px"></i> Low pass filter theory</h3>
        </a>
        <p> Low pass filter differential equation and implementation details.</p>
    </div>
    <div>
        <a href="pi_controller">
            <h3 style="color:inherit"> <i class="fa fa-lg fa-graduation-cap" style="padding:10px"></i> PID controller theory</h3>
        </a>
        <p> PID controller differential equation and implementation details.</p>
    </div>
<div>
