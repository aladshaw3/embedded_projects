import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, binary_sensor, light
from esphome.const import CONF_ID

cat_feeder_logic_ns = cg.esphome_ns.namespace("cat_feeder_logic")
CatFeederLogic = cat_feeder_logic_ns.class_("CatFeederLogic", cg.Component)

CONF_DISTANCE_SENSOR = "distance_sensor"
CONF_BUS_VOLTAGE_SENSOR = "bus_voltage_sensor"
CONF_CURRENT_SENSOR = "current_sensor"
CONF_POWER_SENSOR = "power_sensor"
CONF_ANALOG_SENSOR = "analog_sensor"
CONF_DIGITAL_SENSOR = "digital_sensor"
CONF_DISPLAY = "display"
CONF_STATUS_LIGHT = "status_light"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CatFeederLogic),
    cv.Required(CONF_DISTANCE_SENSOR): cv.use_id(sensor.Sensor),
    cv.Required(CONF_BUS_VOLTAGE_SENSOR): cv.use_id(sensor.Sensor),
    cv.Required(CONF_CURRENT_SENSOR): cv.use_id(sensor.Sensor),
    cv.Required(CONF_POWER_SENSOR): cv.use_id(sensor.Sensor),
    cv.Required(CONF_ANALOG_SENSOR): cv.use_id(sensor.Sensor),
    cv.Required(CONF_DIGITAL_SENSOR): cv.use_id(binary_sensor.BinarySensor),
    cv.Required(CONF_DISPLAY): cv.use_id(cg.Component),  # Accepts any Component, including HT16K33AlphaDisplay
    cv.Required(CONF_STATUS_LIGHT): cv.use_id(light.LightState),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    dist = await cg.get_variable(config[CONF_DISTANCE_SENSOR])
    cg.add(var.set_distance_sensor(dist))

    v_bus = await cg.get_variable(config[CONF_BUS_VOLTAGE_SENSOR])
    cg.add(var.set_bus_voltage_sensor(v_bus))

    curr = await cg.get_variable(config[CONF_CURRENT_SENSOR])
    cg.add(var.set_current_sensor(curr))

    pwr = await cg.get_variable(config[CONF_POWER_SENSOR])
    cg.add(var.set_power_sensor(pwr))

    ana = await cg.get_variable(config[CONF_ANALOG_SENSOR])
    cg.add(var.set_analog_sensor(ana))

    dig = await cg.get_variable(config[CONF_DIGITAL_SENSOR])
    cg.add(var.set_digital_sensor(dig))

    disp = await cg.get_variable(config[CONF_DISPLAY])
    cg.add(var.set_display(disp))

    lit = await cg.get_variable(config[CONF_STATUS_LIGHT])
    cg.add(var.set_status_light(lit))