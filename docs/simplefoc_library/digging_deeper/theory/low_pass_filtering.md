---
layout: default
title: Low Pass Filter
parent: Theory corner
grand_parent: Digging deeper
grand_grand_parent: Arduino <span class="simple">Simple<span class="foc">FOC</span>library</span>
description: "Arduino Simple Field Oriented Control (FOC) library ."
nav_order: 4
permalink: /low_pass_filter
---

# Low-Pass velocity filter theory
Transfer function of the Low pass filter is controller is:

<p><img src="./extras/Images/cont_LPF.png" /></p>
In it discrete form it becomes:

<p><img src="./extras/Images/dis_LPF.png" /></p>

where <i>v<sub>f</sub>(k)</i> is filtered velocity value in moment <i>k</i>, <i>v(k)</i> is the measured velocity in the moment <i>k</i>, <i>T<sub>f</sub></i> is the filter time constant and <i>T<sub>s</sub></i> is the sampling time (or time in between executions of the equation).
This low pass filter can be also written in the form:

<p><img src="./extras/Images/LPF_alpha.png" /></p>

where:

<p><img src="./extras/Images/alpha.png" /></p>

This makes it a bit more clear what the time constant `Tf` of the Low pass filter stands for. If your sample time is around 1millisecond (for arduino UNO this can be taken as an average) then setting the
`Tf` value to `Tf = 0.01` will result in:

```cpp
alpha = 0.01/(0.01 + 0.001) = 0.91
```

Which means that your actual velocity measurement <i>v</i> will influence the filtered value <i>v<sub>f</sub><i> with the coefficient `1-alpha = 0.09` which is going to smooth the velocity values considerably (maybe even too much, depends of the application).


## Implementation details

Low pass filtering function implemented in the <span class="simple">Simple<span class="foc">FOC</span>library</span> as a class called `LowPassFilter`. 
This class receives the time constant in the constructor:
```cpp
LowPassFilter filter = LowPassFilter(0.001); // Tf = 1ms
```
The filtering function is implemented as follows:
```cpp
// low pass filtering function
float LowPassFilter::operator(float input){
  unsigned long timestamp = _micros();
  float dt = (timestamp - timestamp_prev)*1e-6f;
  // quick fix for strange cases (micros overflow)
  if (dt < 0.0f || dt > 0.5f) dt = 1e-3f;

  // calculate the filtering 
  float alpha = Tf/(Tf + dt);
  float y = alpha*y_prev + (1.0f - alpha)*x;

  // save the variables
  y_prev = y;
  timestamp_prev = timestamp;
  return y;
}
```
You can use it in code just by calling:
```cpp
float signal_filtered = filter(signal);
```
And you can change the filtering constant at any time with line:
```cpp
filter.Tf = 0.01; // changed to 10ms
```
This low pass filter is implemented in the motor class and its time constant can be changed by calling:
```cpp
motor.LPF_velocity.Tf = 0.01;// to set it to 10ms
```

