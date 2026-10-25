# Inputs

## lidar
Lidar will be used in the top of the feeder to detect food level in the hopper
### distance
Type: float32
Unit: Meters

## motor_sensor
This will be the ammeter/voltmeter attached to the motor and supply
### voltage
Type: float32
Unit: Volts
### current
Type: float32
Unit: Amps

## battery
This will be a ADC attached to the feeders backup batteries through a voltage divider
### raw_voltage
Type: float32
Unit: Volts
### divider_ratio
Type: float32
Unit: ratio of the voltage divider that you need to use to get the actual voltage from the raw_voltage
### battery_warning_level
Type: float32
Unit: Volts
The battery level to send an alert at

## feeder_limit_switch
### switch
Type: bool
Unit: bool
Currently do not know how many state changes to feeder cycles this will be.
### activations_per_feeding
Type: uint8
Unit: Number

## gross_scale
### raw_weight
Type: int32
Unit: grams
### calibration_data
Type: array[pair[int32,int32]]
### empty_weight
Type: int32
Unit: grams
This parameter is the weight of the unit without any food
### full_weight
Type: int32
Unit: grams
This parameter is the weight of the unit completely full of food

## clock
### seconds
### minutes
### hours

## date
### month
### day
### year

# Outputs

## Feedings
### number
Type: uint32
Number of feedings since last reset
### datetime_of_last_feeding
Type: datetime
### feedings_drift
Type: datetime delta
How much the feedings drift
### feeding_duration
Type: uint32
Unit: ms
This will keep track of how long the motor stays on for each feeding.
### feeding_ammount_dispensed
Type: int32
Unit: grams


## Weight
### weight_of_food_left
Type: int32
Unit: grams
### percentage_full
Type: int32
Unit: grams
### days_of_food_left
Type: int32
Unit: days

## motor_sensor
This is to keep track of the current and voltage used for feedings over time
### voltage_of_feeding
Type: float32
Unit: Volts
### current_of_feeding
Type: float32
Unit: Amps

## battery
### voltage
Type: float32
Unit: Volts
### percentage
Type: float32
Unit: Percentage

## seven_seg_display
Type: char[4]

## feather_rgb_led
### R
Type: uint8
### G
Type: uint8
### B
Type: uint8
