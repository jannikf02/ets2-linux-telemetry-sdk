let express = require('express');
const shmid = 1090584577;

let app = express();
const telemetryStateLayout = {
    speed: 'float',
    rpm: 'float',
    gear: 'int',
};

// Enable CORS for all routes
app.use(function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "http://127.0.0.1:25555");
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    next();
});

let demodata = '{"game":{"connected":true,"paused":true,"time":"","timeScale":0,"nextRestStopTime":"","version":"","telemetryPluginVersion":""},"truck":{"id":"","make":"","model":"","speed":66.7714,"cruiseControlSpeed":0,"cruiseControlOn":false,"odometer":0,"gear":0,"displayedGear":0,"forwardGears":0,"reverseGears":0,"shifterType":"","engineRpm":1185.85,"engineRpmMax":0,"fuel":0,"fuelCapacity":0,"fuelAverageConsumption":0,"fuelWarningFactor":0,"fuelWarningOn":false,"wearEngine":0,"wearTransmission":0,"wearCabin":0,"wearChassis":0,"wearWheels":0,"userSteer":0,"userThrottle":0,"userBrake":0,"userClutch":0,"gameSteer":0,"gameThrottle":0,"gameBrake":0,"gameClutch":0,"shifterSlot":0,"engineOn":false,"electricOn":false,"wipersOn":false,"retarderBrake":0,"retarderStepCount":0,"parkBrakeOn":false,"motorBrakeOn":false,"brakeTemperature":0,"adblue":0,"adblueCapacity":0,"adblueAverageConsumpton":0,"adblueWarningOn":false,"airPressure":0,"airPressureWarningOn":false,"airPressureWarningValue":0,"airPressureEmergencyOn":false,"airPressureEmergencyValue":0,"oilTemperature":0,"oilPressure":0,"oilPressureWarningOn":false,"oilPressureWarningValue":0,"waterTemperature":0,"waterTemperatureWarningOn":false,"waterTemperatureWarningValue":0,"batteryVoltage":0,"batteryVoltageWarningOn":false,"batteryVoltageWarningValue":0,"lightsDashboardValue":0,"lightsDashboardOn":false,"blinkerLeftActive":false,"blinkerRightActive":false,"blinkerLeftOn":false,"blinkerRightOn":false,"lightsParkingOn":false,"lightsBeamLowOn":false,"lightsBeamHighOn":false,"lightsAuxFrontOn":false,"lightsAuxRoofOn":false,"lightsBeaconOn":false,"lightsBrakeOn":false,"lightsReverseOn":false,"placement":{"x":0,"y":0,"z":0,"heading":0,"pitch":0,"roll":0},"acceleration":{"x":0,"y":0,"z":0},"head":{"x":0,"y":0,"z":0},"cabin":{"x":0,"y":0,"z":0},"hook":{"x":0,"y":0,"z":0}},"trailer":{"attached":false,"id":"","name":"","mass":0,"wear":0,"placement":{"x":0,"y":0,"z":0,"heading":0,"pitch":0,"roll":0}},"job":{"income":0,"deadlineTime":"","remainingTime":"","sourceCity":"","sourceCompany":"","destinationCity":"","destinationCompany":""},"navigation":{"estimatedTime":"","estimatedDistance":0,"speedLimit":0}}'
let oldData = demodata;
let errorCount = 0;
let maxReached = false;

app.get('/api/ets2/telemetry', (req, res) => {
    if(maxReached){
        errorCount--;
        if(errorCount <= 0){
            maxReached = false;
        }
        res.json(JSON.parse(oldData));
        return;
    }
    if(!isWindows){
    const shm = require('shm-typed-array');
    let buffer;
    try{
    buffer = shm.get(shmid); 
    }
    catch(e){
        console.log("Buffer get Error?")
    }
        let jsonData;
        if (!buffer) {
            jsonData = oldData; 
            console.log("BUFFER ERROR")            
            if(errorCount < 10000){
                errorCount++;
            }else{
                maxReached = true;
            }
        }else{
            errorCount = 0;
            const json = buffer.toJSON();
            const buffer2 = Buffer.from(json.data);
            jsonData = buffer2.toString('ascii');
            console.log(jsonData.length);
            oldData = jsonData
        }
        try{
            //console.log(jsonData);
            console.log(cutStringAtLastClosingCurlyBrace(jsonData));
            const parsed = JSON.parse(cutStringAtLastClosingCurlyBrace(jsonData));
            res.json(parsed); 
        }catch(e){
            //console.log(e)
            res.json(JSON.parse(demodata));
        }
    }else{
        res.json(JSON.parse(demodata));
	}
});
let isWindows = false;
const useDemoData = false;
let jsonString;
const debug = false;

setInterval(() => {
if(debug){
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
}
}, 1000);

const port = 8000;

app.listen(port, () => {
    console.log('Server is running on http://localhost:'+port);
});

const game = {
    connected: true,
    paused:false
};
const data = {game};

console.log(JSON.stringify(data).length);
console.log(JSON.stringify(data));

function cutStringAtLastClosingCurlyBrace(str) {
    return str.substring(0, str.lastIndexOf('}') + 1);
}

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
