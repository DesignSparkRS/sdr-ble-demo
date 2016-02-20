// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <iostream>
#include <thread>
#include <chrono>

/***********************************************************************
 * |PothosDoc BTLE Sensor Monitor
 *
 * The BTLE sensor monitor processes decoded BTLE packets for sensor data.
 * The monitor compares the sensor data to programmable thresholds
 * and emits a state change which can be used to control and upstream block.
 *
 * <h2>Input messages</h2>
 *
 * The sensor monitor expects dictionary-style messages from the BTLE decoder block.
 * The "Service Data UUID16" key will be checked for the specified 16-bit UUID.
 * Then the sensor value will be extracted from the "Service Data" key.
 * The service data is treated as a string that can be parsed as an IEEE float.
 *
 * <h2>Output signals</h2>
 *
 * <string>state</strong>
 * When the activation state changes (and at regular intervals),
 * the monitor block emits the "state" signal with a string value.
 * The value can be "ACTIVE", "INACTIVE", or "ALARM".
 * The alarm will be occur if a BTLE packet has not been seen for more
 * than the specified idle period.
 *
 * <string>value</strong>
 * At regular intervals the monitor will emit the last seen sensor value
 * from the "value" signal. The value will be a floating point number
 *
 * <string>active</strong>
 * Emit the activation state (true or false) over the "active" signal.
 * When the alarm has activated, the active state will always be false.
 *
 * |category /Control
 * |keywords bluetooth sensor monitor control
 *
 * |param serviceUUID[Service UUID] A 16-bit UUID that identifies the service we are interested in.
 * The UUID is a string containing 4 hex characters.
 * Use the same UUID that the BTLE advertisement application was programmed with.
 * |default "EA06"
 * |widget StringEntry()
 *
 * |param activationLevel[Activation Level] The activation threshold.
 * The monitor enters the active state when the sensor value rises above this threshold.
 * |default 0
 *
 * |param deactivationLevel[Detivation Level] The deactivation threshold.
 * The monitor enters the inactive state when the sensor value falls below this threshold.
 * |default 0
 *
 * |param alarmTimeout[Alarm Timeout] The allowed idle timeout before the alarm is raised.
 * Alarm is raised if a decoded packet is not seen for more than idle timeout.
 * |default 10
 * |units seconds
 *
 * |factory /btle/btle_sensor_monitor()
 * |setter setServiceUUID(serviceUUID)
 * |setter setActivationLevel(activationLevel)
 * |setter setDeactivationLevel(deactivationLevel)
 * |setter setAlarmTimeout(alarmTimeout)
 **********************************************************************/
class BTLESensorMonitor : public Pothos::Block
{
public:
    BTLESensorMonitor(void):
        _activationLevel(0),
        _deactivationLevel(0)
    {
        this->setupInput(0); //input messages from decoder
        this->registerSignal("state");
        this->registerSignal("value");
        this->registerSignal("active");
        this->registerSlot("triggerReport");
        this->registerCall(this, POTHOS_FCN_TUPLE(BTLESensorMonitor, setServiceUUID));
        this->registerCall(this, POTHOS_FCN_TUPLE(BTLESensorMonitor, setActivationLevel));
        this->registerCall(this, POTHOS_FCN_TUPLE(BTLESensorMonitor, setDeactivationLevel));
        this->registerCall(this, POTHOS_FCN_TUPLE(BTLESensorMonitor, setAlarmTimeout));
        this->registerCall(this, POTHOS_FCN_TUPLE(BTLESensorMonitor, triggerReport));
    }

    static Block *make(void)
    {
        return new BTLESensorMonitor();
    }

    void setServiceUUID(const std::string &uuid)
    {
        _uuid = uuid;
    }

    void setActivationLevel(const double level)
    {
        _activationLevel = level;
    }

    void setDeactivationLevel(const double level)
    {
        _deactivationLevel = level;
    }

    void setAlarmTimeout(const double timeout)
    {
        _alarmTimeout = std::chrono::milliseconds(long(timeout*1000));
    }

    void triggerReport(void)
    {
        const bool isAlarm = std::chrono::high_resolution_clock::now() > _lastSensorTime + _alarmTimeout;
        if (isAlarm) _isActive = false; //disable when alarm has been set
        this->callVoid("state", isAlarm?"ALARM":(_isActive?"ACTIVE":"INACTIVE"));
        this->callVoid("value", _lastSensorValue);
        this->callVoid("active", _isActive);
    }

    void activate(void)
    {
        _isActive = false;
        _lastSensorValue = 0;
    }

    void work(void)
    {
        auto inPort = this->input(0);
        if (inPort->hasMessage())
        {
            const auto msg = inPort->popMessage();
            this->processSensorData(msg);
        }
    }

private:
    void processSensorData(const Pothos::Object &msg)
    {
        if (not msg.canConvert(typeid(Pothos::ObjectKwargs))) return;
        auto data = msg.convert<Pothos::ObjectKwargs>();
        if (data.count("Service Data UUID16") == 0) return;
        if (data.count("Service Data") == 0) return;

        //get keyword values as strings
        const auto sensorDataStr = data.at("Service Data").convert<std::string>();
        const auto remoteUUIDstr = data.at("Service Data UUID16").convert<std::string>();

        //compare uuid
        const auto myUUID = std::stoul(_uuid, nullptr, 16);
        const auto remoteUUID = std::stoul(remoteUUIDstr, nullptr, 16);
        if (myUUID != remoteUUID) return;

        //extract sensor value
        _lastSensorValue = std::stod(sensorDataStr);
        if (
            (not _isActive and _lastSensorValue > _activationLevel) or
            (_isActive and _lastSensorValue < _deactivationLevel))
        {
            _isActive = not _isActive;
            this->triggerReport();
        }
        _lastSensorTime = std::chrono::high_resolution_clock::now();
    }

    //state
    bool _isActive;
    double _lastSensorValue;
    std::chrono::high_resolution_clock::time_point _lastSensorTime;

    //config
    std::string _uuid;
    double _activationLevel;
    double _deactivationLevel;
    std::chrono::high_resolution_clock::duration _alarmTimeout;
};

static Pothos::BlockRegistry registerBTLESensorMonitor(
    "/btle/btle_sensor_monitor", &BTLESensorMonitor::make);
