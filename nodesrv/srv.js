let express = require('express');
const shmid = 1090584577;

let app = express();
const telemetryStateLayout = {
    speed: 'float',
    rpm: 'float',
    gear: 'int',
};
app.get('/', (req, res) => {
    if(!isWindows){
    const shm = require('shm-typed-array');
    const buffer = shm.get(shmid);
        let jsonData;
        if (!buffer) {
            jsonData = demodata;
        }else{
            const json = buffer.toJSON();
            const buffer2 = Buffer.from(json.data);
            jsonData = buffer2.toString('ascii');
            jsonData = jsonData.slice(0, -1);
        }
        res.json(JSON.parse(jsonData));
    }    
});
let demodata = '{"game":{"connected":false,"paused":false,"time":"","timeScale":0,"nextRestStopTime":"","version":"","telemetryPluginVersion":""},"truck":{"id":"","make":"","model":"","speed":0.736066,"cruiseControlSpeed":0,"cruiseControlOn":false,"odometer":0,"gear":0,"displayedGear":0,"forwardGears":0,"reverseGears":0,"shifterType":"","engineRpm":0,"engineRpmMax":0,"fuel":0,"fuelCapacity":0,"fuelAverageConsumption":0,"fuelWarningFactor":0,"fuelWarningOn":false,"wearEngine":0,"wearTransmission":0,"wearCabin":0,"wearChassis":0,"wearWheels":0,"userSteer":0,"userThrottle":0,"userBrake":0,"userClutch":0,"gameSteer":0,"gameThrottle":0,"gameBrake":0,"gameClutch":0,"shifterSlot":0,"engineOn":false,"electricOn":false,"wipersOn":false,"retarderBrake":0,"retarderStepCount":0,"parkBrakeOn":false,"motorBrakeOn":false,"brakeTemperature":0,"adblue":0,"adblueCapacity":0,"adblueAverageConsumpton":0,"adblueWarningOn":false,"airPressure":0,"airPressureWarningOn":false,"airPressureWarningValue":0,"airPressureEmergencyOn":false,"airPressureEmergencyValue":0,"oilTemperature":0,"oilPressure":0,"oilPressureWarningOn":false,"oilPressureWarningValue":0,"waterTemperature":0,"waterTemperatureWarningOn":false,"waterTemperatureWarningValue":0,"batteryVoltage":0,"batteryVoltageWarningOn":false,"batteryVoltageWarningValue":0,"lightsDashboardValue":0,"lightsDashboardOn":false,"blinkerLeftActive":false,"blinkerRightActive":false,"blinkerLeftOn":false,"blinkerRightOn":false,"lightsParkingOn":false,"lightsBeamLowOn":false,"lightsBeamHighOn":false,"lightsAuxFrontOn":false,"lightsAuxRoofOn":false,"lightsBeaconOn":false,"lightsBrakeOn":false,"lightsReverseOn":false,"placement":{"x":0,"y":0,"z":0,"heading":0,"pitch":0,"roll":0},"acceleration":{"x":0,"y":0,"z":0},"head":{"x":0,"y":0,"z":0},"cabin":{"x":0,"y":0,"z":0},"hook":{"x":0,"y":0,"z":0}}}'
let isWindows = false;
const useDemoData = false;
let jsonString;
setInterval(() => {

//remove last char because of weird bug that adds a random char
if(useDemoData || isWindows){
    jsonString = demodata;
}else{
    setJsonString();
}
try{
    let data = JSON.parse(jsonString);
    console.log(data.truck.engineRpm);
}catch(e){
    return;
}

}, 1000);

app.listen(25555, () => {
    console.log('Server is running on http://localhost:3000');
});

const game = {
    connected: true,
    paused:false
};
const data = {game};

console.log(JSON.stringify(data).length);
console.log(JSON.stringify(data));

function setJsonString() {
    // Open the shared memory block
    const shm = require('shm-typed-array');

    const buffer = shm.get(shmid);

    if (!buffer) {
        console.log('Failed to open shared memory block');
        jsonString = demodata;
    } else {
        const json = buffer.toJSON();
        const buffer2 = Buffer.from(json.data);
        jsonString = buffer2.toString('ascii');
        jsonString = jsonString.slice(0, -1);
    }
}
