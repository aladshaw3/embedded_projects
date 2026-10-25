import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID, STATE_CLASS_MEASUREMENT, UNIT_METER, ICON_ARROW_EXPAND_VERTICAL

DEPENDENCIES = ["i2c"]

# Define the namespace
vl53l4cd_ns = cg.esphome_ns.namespace("vl53l4cd")

# Link to the C++ class.
# Because the namespace is "vl53l4cd", ESPHome will automatically generate '#include "vl53l4cd.h"' during compile.
VL53L4CDComponent = vl53l4cd_ns.class_(
    "VL53L4CDComponent", cg.PollingComponent, i2c.I2CDevice, sensor.Sensor
)

# Define the YAML schema
CONFIG_SCHEMA = sensor.sensor_schema(
    VL53L4CDComponent,
    unit_of_measurement=UNIT_METER,
    icon=ICON_ARROW_EXPAND_VERTICAL,
    accuracy_decimals=3,
    state_class=STATE_CLASS_MEASUREMENT,
).extend(cv.polling_component_schema("250ms")).extend(i2c.i2c_device_schema(0x29))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    await i2c.register_i2c_device(var, config)
