var libraires =[
    "SimpleFOC.h",
    "PciManager.h",
    "PciListenerImp.h",
    "Encoder.h",
    "FOCutils.h",
    "BLDCMotor.h",
    "StepperMotor.h",
    "HallSensor.h",
    "MagneticSensor.h",
    "MagneticSensorSPI.h",
    "MagneticSensorI2C.h",
    "MagneticSensorAnalog.h",
    "MySensor.h",
    "Sensor.h"
]

var defines =[
    "DEF_POWER_SUPPLY",
    "DEF_PID_VEL_P",
    "DEF_PID_VEL_I",
    "DEF_PID_VEL_D",
    "DEF_P_ANGLE_P",
    "DEF_PID_VEL_U_RAMP",
    "DEF_P_ANGLE_VEL_LIM",
    "DEF_INDEX_SEARCH_TARGET_VELOCITY",
    "DEF_VOLTAGE_SENSOR_ALIGN",
    "DEF_VEL_FILTER_Tf",
    "INH_A",
    "INH_B",
    "INH_C",
    "INL_A",
    "INL_B",
    "INL_C",
    "EN_GATE ",
    "M_PWM",
    "M_OC",
    "OC_ADJ",
    "SPI_MODE0"
]

var classNames = [
    "BLDCMotor",
    "StepperMotor",
    "BLDCDriver3PWM",
    "BLDCDriver6PWM",
    "StepperDriver4PWM",
    "Encoder",
    "MagneticSensor",
    "MagneticSensorSPI",
    "MagneticSensorI2C",
    "MagneticSensorAnalog",
    "HallSensor",
    "PciListenerImp",
    "PciManager",
    "Serial",
    "MySensor",
    "Wire",
    "SPIClass",
    "LowPassFilter",
    "PIDController"
];

var classProps = [
    "PID_velocity",
    "P_angle",
    "LPF_velocity",
    "P",
    "I",
    "Tf",
    "D",
    "output_ramp",
    "quadrature",
    "pullup",
    "voltage_limit",
    "voltage_power_supply",
    "index_search_velocity",
    "controller",
    "velocity_limit"
];

var funcNames = [
    "init",
    "initFOC",
    "enableInterrupts",
    "handleA",
    "handleB",
    "handleIndex",
    "handleC",
    "registerListener",
    "linkSensor",
    "linkDriver",
    "useMonitoring",
    "monitor",
    "print",
    "monitor_port",
    "println",
    "getVelocity",
    "getAngle",
    "loopFOC",
    "move",
    "constrainAngle",
    "controllerLQR",
    "sign",
    "shaftVelocity",
    "initRelativeZero",
    "initAbsoluteZero",
    "needsAbsoluteZeroSearch",
    "hasAbsoluteZero",
    "command",
    "setPhaseVoltage",
    "_delay",
    "_micros",
    "shaftAngle",
    "absoluteZeroAlign",
    "electricAngle",
    "alignSensor",
    "normalizeAngle",
    "_sin",
    "_cos",
    "setPwm",
    "positionP",
    "velocityPID",
    "controllerPID",
    "serialReceiveUserCommand",
    "disable",
    "pinMode",
    "digitalWrite",
    "constrain"

];
var structNames = [
    "Pullup",
    "Quadrature",
    "ControlType",
    "FOCModulationType",
    "MagneticSensorSPIConfig_s",
    "MagneticSensorI2CConfig_s"
];
var structProps = [
    "EXTERN",
    "INTERN",
    "ENABLE",
    "DISABLE",
    "angle",
    "velocity",
    "angle_openloop",
    "velocity_openloop",
    "voltage",
    "SpaceVectorPWM",
    "SinePWM",
    "Trapesoid_120",
    "Trapesoid_150"
];
jtd.onReady(function(){
    document.querySelectorAll('.n').forEach(function(e) {
        if(classNames.indexOf(e.innerHTML) >= 0 ){
            e.classList.remove("n");
            e.classList.add("className");
        } else if(funcNames.indexOf(e.innerHTML) >= 0 ){
            e.classList.remove("n");
            e.classList.add("fcnName");
        } else if(structNames.indexOf(e.innerHTML) >= 0 ){
            e.classList.remove("n");
            e.classList.add("structName");
        } else if(structProps.indexOf(e.innerHTML) >= 0 ){
            e.classList.remove("n");
            e.classList.add("structProp");
        } else if(classProps.indexOf(e.innerHTML) >= 0 ){
            e.classList.remove("n");
            e.classList.add("classProps");
        }
    });

    //include style
    document.querySelectorAll('.cp').forEach(function(e) {
        var str = e.innerHTML;

        // show libraries
        libraires.forEach(function(lib){
            str = str.replace( lib ,"<span class='incLib'>" +lib + "</span>" );
        }); 

        // show defines
        defines.forEach(function(def){
            str = str.replace( def ,"<span class='kt'>" +def + "</span>" );
        }); 
        
        
        // enable comments & defines
        a = str.split('\n');
        a.forEach((element,index) => {
            element = element.replace( /(\/\/)/ ,"<span class='c1'> //" ) + "</span>";
            a[index] = element.replace( "#define" ,"<span class='k'>#define </span> " );
        });

        str = a.join('\n');

        e.innerHTML = str;
    });
});