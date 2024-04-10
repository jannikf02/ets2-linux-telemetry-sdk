/**
 * @brief Simple logger.
 *
 * Writes the output into file inside the current directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <iostream>
#include <cstring> 
#include <sstream>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>

// SDK
#include "./include/scssdk_telemetry.h"
#include "./include/eurotrucks2/scssdk_eut2.h"
#include "./include/eurotrucks2/scssdk_telemetry_eut2.h"
#include "./include/amtrucks/scssdk_ats.h"
#include "./include/amtrucks/scssdk_telemetry_ats.h"

#define UNUSED(x)

#define speedMultiplicatorToKmh 3.571428571

/**
 * @brief Function writting message to the game internal log.
 */
scs_log_t game_log = NULL;

/**
 * @brief Tracking of paused state of the game.
 */
bool output_paused = true;

struct Game {
    bool connected;
    bool paused;
    std::string time;
    int timeScale;
    std::string nextRestStopTime;
    std::string version;
    std::string telemetryPluginVersion;
};

struct Placement {
    double x;
    double y;
    double z;
    double heading;
    double pitch;
    double roll;
};

struct Acceleration {
    double x;
    double y;
    double z;
};

struct Head {
    double x;
    double y;
    double z;
};

struct Cabin {
    double x;
    double y;
    double z;
};

struct Hook {
    double x;
    double y;
    double z;
};

struct Truck {
    std::string id;
    std::string make;
    std::string model;
    double speed;
    double cruiseControlSpeed;
    bool cruiseControlOn;
    double odometer;
    int gear;
    int displayedGear;
    int forwardGears;
    int reverseGears;
    std::string shifterType;
    double engineRpm;
    double engineRpmMax;
    double fuel;
    double fuelCapacity;
    double fuelAverageConsumption;
    double fuelWarningFactor;
    bool fuelWarningOn;
    double wearEngine;
    double wearTransmission;
    double wearCabin;
    double wearChassis;
    double wearWheels;
    double userSteer;
    double userThrottle;
    double userBrake;
    double userClutch;
    double gameSteer;
    double gameThrottle;
    double gameBrake;
    double gameClutch;
    int shifterSlot;
    bool engineOn;
    bool electricOn;
    bool wipersOn;
    double retarderBrake;
    int retarderStepCount;
    bool parkBrakeOn;
    bool motorBrakeOn;
    double brakeTemperature;
    double adblue;
    double adblueCapacity;
    double adblueAverageConsumpton;
    bool adblueWarningOn;
    double airPressure;
    bool airPressureWarningOn;
    double airPressureWarningValue;
    bool airPressureEmergencyOn;
    double airPressureEmergencyValue;
    double oilTemperature;
    double oilPressure;
    bool oilPressureWarningOn;
    double oilPressureWarningValue;
    double waterTemperature;
    bool waterTemperatureWarningOn;
    double waterTemperatureWarningValue;
    double batteryVoltage;
    bool batteryVoltageWarningOn;
    double batteryVoltageWarningValue;
    int lightsDashboardValue;
    bool lightsDashboardOn;
    bool blinkerLeftActive;
    bool blinkerRightActive;
    bool blinkerLeftOn;
    bool blinkerRightOn;
    bool lightsParkingOn;
    bool lightsBeamLowOn;
    bool lightsBeamHighOn;
    bool lightsAuxFrontOn;
    bool lightsAuxRoofOn;
    bool lightsBeaconOn;
    bool lightsBrakeOn;
    bool lightsReverseOn;
    Placement placement;
    Acceleration acceleration;
    Head head;
    Cabin cabin;
    Hook hook;
};

struct Trailer {
    bool attached;
    std::string id;
    std::string name;
    double mass;
    double wear;
    Placement placement;
};

struct Job {
    int income;
    std::string deadlineTime;
    std::string remainingTime;
    std::string sourceCity;
    std::string sourceCompany;
    std::string destinationCity;
    std::string destinationCompany;
};

struct Navigation {
    std::string estimatedTime;
    int estimatedDistance;
    int speedLimit;
};

struct GameState {
    Game game;
    Truck truck;
    Trailer trailer;
    Job job;
    Navigation navigation;
} game_state_s;

struct GameState *game_state = &game_state_s;

std::string convertToJson(const GameState& gameState) {
std::ostringstream oss;
	oss << "{";
	oss << "\"game\":{";
	oss << "\"connected\":" << (gameState.game.connected ? "true" : "false") << ",";
	oss << "\"paused\":" << (gameState.game.paused ? "true" : "false") << ",";
	oss << "\"time\":\"" << gameState.game.time << "\",";
	oss << "\"timeScale\":" << gameState.game.timeScale << ",";
	oss << "\"nextRestStopTime\":\"" << gameState.game.nextRestStopTime << "\",";
	oss << "\"version\":\"" << gameState.game.version << "\",";
	oss << "\"telemetryPluginVersion\":\"" << gameState.game.telemetryPluginVersion << "\"";
	oss << "},";
	oss << "\"truck\":{";
	oss << "\"id\":\"" << gameState.truck.id << "\",";
	oss << "\"make\":\"" << gameState.truck.make << "\",";
	oss << "\"model\":\"" << gameState.truck.model << "\",";
	oss << "\"speed\":" << gameState.truck.speed << ",";
	oss << "\"cruiseControlSpeed\":" << gameState.truck.cruiseControlSpeed*(speedMultiplicatorToKmh) << ",";
	oss << "\"cruiseControlOn\":" << (gameState.truck.cruiseControlSpeed > 0 ? "true" : "false") << ",";
	oss << "\"odometer\":" << gameState.truck.odometer << ",";
	oss << "\"gear\":" << gameState.truck.gear << ",";
	oss << "\"displayedGear\":" << gameState.truck.displayedGear << ",";
	oss << "\"forwardGears\":" << gameState.truck.forwardGears << ",";
	oss << "\"reverseGears\":" << gameState.truck.reverseGears << ",";
	oss << "\"shifterType\":\"" << gameState.truck.shifterType << "\",";
	oss << "\"engineRpm\":" << gameState.truck.engineRpm << ",";
	oss << "\"engineRpmMax\":" << gameState.truck.engineRpmMax << ",";
	oss << "\"fuel\":" << gameState.truck.fuel << ",";
	oss << "\"fuelCapacity\":" << gameState.truck.fuelCapacity << ",";
	oss << "\"fuelAverageConsumption\":" << gameState.truck.fuelAverageConsumption << ",";
	oss << "\"fuelWarningFactor\":" << gameState.truck.fuelWarningFactor << ",";
	oss << "\"fuelWarningOn\":" << (gameState.truck.fuelWarningOn ? "true" : "false") << ",";
	oss << "\"wearEngine\":" << gameState.truck.wearEngine << ",";
	oss << "\"wearTransmission\":" << gameState.truck.wearTransmission << ",";
	oss << "\"wearCabin\":" << gameState.truck.wearCabin << ",";
	oss << "\"wearChassis\":" << gameState.truck.wearChassis << ",";
	oss << "\"wearWheels\":" << gameState.truck.wearWheels << ",";
	oss << "\"userSteer\":" << gameState.truck.userSteer << ","; 
	oss << "\"userThrottle\":" << gameState.truck.userThrottle << ",";
	oss << "\"userBrake\":" << gameState.truck.userBrake << ",";
	oss << "\"userClutch\":" << gameState.truck.userClutch << ",";
	oss << "\"gameSteer\":" << gameState.truck.gameSteer << ",";
	oss << "\"gameThrottle\":" << gameState.truck.gameThrottle << ",";
	oss << "\"gameBrake\":" << gameState.truck.gameBrake << ",";
	oss << "\"gameClutch\":" << gameState.truck.gameClutch << ",";
	oss << "\"shifterSlot\":" << gameState.truck.shifterSlot << ",";
	oss << "\"engineOn\":" << (gameState.truck.engineOn ? "true" : "false") << ",";
	oss << "\"electricOn\":" << (gameState.truck.electricOn ? "true" : "false") << ",";
	oss << "\"wipersOn\":" << (gameState.truck.wipersOn ? "true" : "false") << ",";
	oss << "\"retarderBrake\":" << gameState.truck.retarderBrake << ",";
	oss << "\"retarderStepCount\":" << gameState.truck.retarderStepCount << ",";
	oss << "\"parkBrakeOn\":" << (gameState.truck.parkBrakeOn ? "true" : "false") << ",";
	oss << "\"motorBrakeOn\":" << (gameState.truck.motorBrakeOn ? "true" : "false") << ",";
	oss << "\"brakeTemperature\":" << gameState.truck.brakeTemperature << ",";
	oss << "\"adblue\":" << gameState.truck.adblue << ",";
	oss << "\"adblueCapacity\":" << gameState.truck.adblueCapacity << ",";
	oss << "\"adblueAverageConsumpton\":" << gameState.truck.adblueAverageConsumpton << ",";
	oss << "\"adblueWarningOn\":" << (gameState.truck.adblueWarningOn ? "true" : "false") << ",";
	oss << "\"airPressure\":" << gameState.truck.airPressure << ",";
	oss << "\"airPressureWarningOn\":" << (gameState.truck.airPressureWarningOn ? "true" : "false") << ",";
	oss << "\"airPressureWarningValue\":" << gameState.truck.airPressureWarningValue << ",";
	oss << "\"airPressureEmergencyOn\":" << (gameState.truck.airPressureEmergencyOn ? "true" : "false") << ",";
	oss << "\"airPressureEmergencyValue\":" << gameState.truck.airPressureEmergencyValue << ",";
	oss << "\"oilTemperature\":" << gameState.truck.oilTemperature << ",";
	oss << "\"oilPressure\":" << gameState.truck.oilPressure << ",";
	oss << "\"oilPressureWarningOn\":" << (gameState.truck.oilPressureWarningOn ? "true" : "false") << ",";
	oss << "\"oilPressureWarningValue\":" << gameState.truck.oilPressureWarningValue << ",";
	oss << "\"waterTemperature\":" << gameState.truck.waterTemperature << ",";
	oss << "\"waterTemperatureWarningOn\":" << (gameState.truck.waterTemperatureWarningOn ? "true" : "false") << ",";
	oss << "\"waterTemperatureWarningValue\":" << gameState.truck.waterTemperatureWarningValue << ",";
	oss << "\"batteryVoltage\":" << gameState.truck.batteryVoltage << ",";
	oss << "\"batteryVoltageWarningOn\":" << (gameState.truck.batteryVoltageWarningOn ? "true" : "false") << ",";
	oss << "\"batteryVoltageWarningValue\":" << gameState.truck.batteryVoltageWarningValue << ",";
	oss << "\"lightsDashboardValue\":" << gameState.truck.lightsDashboardValue << ",";
	oss << "\"lightsDashboardOn\":" << (gameState.truck.lightsDashboardOn ? "true" : "false") << ",";
	oss << "\"blinkerLeftActive\":" << (gameState.truck.blinkerLeftActive ? "true" : "false") << ",";
	oss << "\"blinkerRightActive\":" << (gameState.truck.blinkerRightActive ? "true" : "false") << ",";
	oss << "\"blinkerLeftOn\":" << (gameState.truck.blinkerLeftOn ? "true" : "false") << ",";
	oss << "\"blinkerRightOn\":" << (gameState.truck.blinkerRightOn ? "true" : "false") << ",";
	oss << "\"lightsParkingOn\":" << (gameState.truck.lightsParkingOn ? "true" : "false") << ",";
	oss << "\"lightsBeamLowOn\":" << (gameState.truck.lightsBeamLowOn ? "true" : "false") << ",";
	oss << "\"lightsBeamHighOn\":" << (gameState.truck.lightsBeamHighOn ? "true" : "false") << ",";
	oss << "\"lightsAuxFrontOn\":" << (gameState.truck.lightsAuxFrontOn ? "true" : "false") << ",";
	oss << "\"lightsAuxRoofOn\":" << (gameState.truck.lightsAuxRoofOn ? "true" : "false") << ",";
	oss << "\"lightsBeaconOn\":" << (gameState.truck.lightsBeaconOn ? "true" : "false") << ",";
	oss << "\"lightsBrakeOn\":" << (gameState.truck.lightsBrakeOn ? "true" : "false") << ",";
	oss << "\"lightsReverseOn\":" << (gameState.truck.lightsReverseOn ? "true" : "false") << ",";
	oss << "\"placement\":{";
	oss << "\"x\":" << gameState.truck.placement.x << ",";
	oss << "\"y\":" << gameState.truck.placement.y << ",";
	oss << "\"z\":" << gameState.truck.placement.z << ",";
	oss << "\"heading\":" << gameState.truck.placement.heading << ",";
	oss << "\"pitch\":" << gameState.truck.placement.pitch << ",";
	oss << "\"roll\":" << gameState.truck.placement.roll;
	oss << "},";  
	oss << "\"acceleration\":{";
	oss << "\"x\":" << gameState.truck.acceleration.x << ",";
	oss << "\"y\":" << gameState.truck.acceleration.y << ",";
	oss << "\"z\":" << gameState.truck.acceleration.z;
	oss << "},";
	oss << "\"head\":{";
	oss << "\"x\":" << gameState.truck.head.x << ",";
	oss << "\"y\":" << gameState.truck.head.y << ",";
	oss << "\"z\":" << gameState.truck.head.z;
	oss << "},";
	oss << "\"cabin\":{";
	oss << "\"x\":" << gameState.truck.cabin.x << ",";
	oss << "\"y\":" << gameState.truck.cabin.y << ",";
	oss << "\"z\":" << gameState.truck.cabin.z;
	oss << "},";
	oss << "\"hook\":{";
	oss << "\"x\":" << gameState.truck.hook.x << ",";
	oss << "\"y\":" << gameState.truck.hook.y << ",";
	oss << "\"z\":" << gameState.truck.hook.z;
	oss << "}";
	oss << "},";
    oss << "\"trailer\":{";
    oss << "\"attached\":" << (gameState.trailer.attached ? "true" : "false") << ",";
    oss << "\"id\":\"" << gameState.trailer.id << "\",";
    oss << "\"name\":\"" << gameState.trailer.name << "\",";
    oss << "\"mass\":" << gameState.trailer.mass << ",";
    oss << "\"wear\":" << gameState.trailer.wear << ",";
    oss << "\"placement\":{";
    oss << "\"x\":" << gameState.trailer.placement.x << ",";
    oss << "\"y\":" << gameState.trailer.placement.y << ",";
    oss << "\"z\":" << gameState.trailer.placement.z << ",";
    oss << "\"heading\":" << gameState.trailer.placement.heading << ",";
    oss << "\"pitch\":" << gameState.trailer.placement.pitch << ",";
    oss << "\"roll\":" << gameState.trailer.placement.roll;
    oss << "}";
    oss << "},";
    oss << "\"job\":{";
    oss << "\"income\":" << gameState.job.income << ",";
    oss << "\"deadlineTime\":\"" << gameState.job.deadlineTime << "\",";
    oss << "\"remainingTime\":\"" << gameState.job.remainingTime << "\",";
    oss << "\"sourceCity\":\"" << gameState.job.sourceCity << "\",";
    oss << "\"sourceCompany\":\"" << gameState.job.sourceCompany << "\",";
    oss << "\"destinationCity\":\"" << gameState.job.destinationCity << "\",";
    oss << "\"destinationCompany\":\"" << gameState.job.destinationCompany << "\"";
    oss << "},";
    oss << "\"navigation\":{";
    oss << "\"estimatedTime\":\"" << gameState.navigation.estimatedTime << "\",";
    oss << "\"estimatedDistance\":" << gameState.navigation.estimatedDistance << ",";
    oss << "\"speedLimit\":" << gameState.navigation.speedLimit;
    oss << "}";
	oss << "}";
	
    return oss.str();
}

// Function to handle the GET request
void handleGETRequest(int clientSocket) {
    
    // Simulate fetching data from a database or external source
    std::string responseData = convertToJson(*game_state);

    // Construct the HTTP response
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Type: application/json\r\n";
    responseStream << "Content-Length: " << responseData.length() << "\r\n";
    responseStream << "\r\n";
    responseStream << responseData;

    

    // Send the response
    std::string httpResponse = responseStream.str();
    send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
}


void runRESTServer() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t sinSize = sizeof(struct sockaddr_in);

    // Socket erstellen
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Serveradresse konfigurieren
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000);
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Socket an Adresse und Port binden
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Auf eingehende Verbindungen hören
    listen(serverSocket, 5);
    
    std::cout << "Server is running on localhost:8000\n";

    while (true) {
        // Eingehende Verbindung akzeptieren
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &sinSize);
 
        // GET-Anfrage in einem separaten Thread behandeln
        std::thread requestThread(handleGETRequest, clientSocket);
        requestThread.detach(); // Thread abkoppeln, um ihn unabhängig auszuführen
    }

    // Server-Socket schließen (nicht im Beispiel erreicht)
    close(serverSocket);
}

void log(std::string message){
    //Concat message with [JANNIK]
    std::string newMessage = "[JANNIK] " + message;
    game_log(SCS_LOG_TYPE_message, newMessage.c_str());
}

int oldMemorySize = 0;
//2MegaBytes
int memorySize = 2048*100;

std::string keys = "JANNIK";

SCSAPI_VOID telemetry_pause(const scs_event_t UNUSED(event), const void *const UNUSED(event_info), const scs_context_t UNUSED(context))
{
    output_paused = !output_paused;
}

SCSAPI_VOID telemetry_frame_start(const scs_event_t UNUSED(event), const void *const UNUSED(event_info), const scs_context_t UNUSED(context)){
    if(output_paused){
        return;
    }
}

SCSAPI_VOID telemetry_frame_end(const scs_event_t UNUSED(event), const void *const UNUSED(event_info), const scs_context_t UNUSED(context)){
    if(output_paused){
        return;
    }
	std::string data = convertToJson(*game_state);
}

SCSAPI_VOID telemetry_store_speed(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
	game_state->truck.speed = (value->value_float.value)*(speedMultiplicatorToKmh);
    if(game_state->truck.speed < 0){ 
        game_state->truck.speed = -1 * game_state->truck.speed;
    }
}
SCSAPI_VOID telemetry_store_rpm(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        log("Not a float");
        return;
    }
	game_state->truck.engineRpm = value->value_float.value;
} 

SCSAPI_VOID telemetry_store_electric_enabled(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.electricOn = value->value_bool.value;
} 

SCSAPI_VOID telemetry_store_engine_enabled(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.engineOn = value->value_bool.value;
} 

SCSAPI_VOID telemetry_store_wipers(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.wipersOn = value->value_bool.value;
} 

SCSAPI_VOID telemetry_store_retarder_brake(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
	game_state->truck.retarderBrake = value->value_float.value;
} 

SCSAPI_VOID telemetry_store_park_brake(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.parkBrakeOn = value->value_bool.value;
} 

SCSAPI_VOID telemetry_store_motor_brake(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.motorBrakeOn = value->value_bool.value;
} 

SCSAPI_VOID telemetry_store_brake_air_pressure(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
	game_state->truck.airPressure = value->value_float.value;
} 

SCSAPI_VOID telemetry_store_adblue(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
	game_state->truck.adblue = value->value_float.value;
} 

SCSAPI_VOID telemetry_store_fuel(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
	game_state->truck.fuel = value->value_float.value;
} 

SCSAPI_VOID telemetry_store_oil_pressure(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
	game_state->truck.oilPressure = value->value_float.value;
} 

SCSAPI_VOID telemetry_store_oil_temperature(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
	game_state->truck.oilTemperature = value->value_float.value;
} 

SCSAPI_VOID telemetry_store_water_temperature(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
	game_state->truck.waterTemperature = value->value_float.value;
} 

SCSAPI_VOID telemetry_store_battery_voltage(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
	game_state->truck.batteryVoltage = value->value_float.value;
}

SCSAPI_VOID telemetry_store_lights_dashboard(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_u32){
        return;
    }
	game_state->truck.lightsDashboardValue = value->value_u32.value;
}

SCSAPI_VOID telemetry_store_lights_dashboard_on(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.lightsDashboardOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_blinker_left_active(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.blinkerLeftActive = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_blinker_right_active(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.blinkerRightActive = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_blinker_left_on(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.blinkerLeftOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_blinker_right_on(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.blinkerRightOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_lights_parking(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.lightsParkingOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_lights_beam_low(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.lightsBeamLowOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_lights_beam_high(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.lightsBeamHighOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_lights_aux_front(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.lightsAuxFrontOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_lights_aux_roof(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.lightsAuxRoofOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_lights_beacon(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.lightsBeaconOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_lights_brake_on(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.lightsBrakeOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_lights_reverse(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
	game_state->truck.lightsReverseOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_world_placement(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_dplacement){
        return;
    }

    game_state->truck.placement.x = value->value_dplacement.position.x;
    game_state->truck.placement.y = value->value_dplacement.position.y;
    game_state->truck.placement.z = value->value_dplacement.position.z;
    game_state->truck.placement.heading = value->value_dplacement.orientation.heading;
    game_state->truck.placement.pitch = value->value_dplacement.orientation.pitch;
    game_state->truck.placement.roll = value->value_dplacement.orientation.roll;
}

SCSAPI_VOID telemetry_store_local_linear_velocity(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_fvector){
        return;
    }

    game_state->truck.acceleration.x = value->value_fvector.x;
    game_state->truck.acceleration.y = value->value_fvector.y;
    game_state->truck.acceleration.z = value->value_fvector.z;
}

SCSAPI_VOID telemetry_store_local_angular_velocity(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_fvector){
        return;
    }

    game_state->truck.acceleration.x = value->value_fvector.x;
    game_state->truck.acceleration.y = value->value_fvector.y;
    game_state->truck.acceleration.z = value->value_fvector.z;
}

SCSAPI_VOID telemetry_store_head(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_fvector){
        return;
    }

    game_state->truck.head.x = value->value_fvector.x;
    game_state->truck.head.y = value->value_fvector.y;
    game_state->truck.head.z = value->value_fvector.z;
}

SCSAPI_VOID telemetry_store_cabin(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_fvector){
        return;
    }

    game_state->truck.cabin.x = value->value_fvector.x;
    game_state->truck.cabin.y = value->value_fvector.y;
    game_state->truck.cabin.z = value->value_fvector.z;
}

SCSAPI_VOID telemetry_store_hook(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_fvector){
        return;
    }

    game_state->truck.hook.x = value->value_fvector.x;
    game_state->truck.hook.y = value->value_fvector.y;
    game_state->truck.hook.z = value->value_fvector.z;
}

SCSAPI_VOID telemetry_store_job_income(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_u32){
        return;
    }

    game_state->job.income = value->value_u32.value;
}

SCSAPI_VOID telemetry_store_job_deadline_time(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_string){
        return;
    }

    game_state->job.deadlineTime = value->value_string.value;
}

SCSAPI_VOID telemetry_store_job_remaining_time(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_string){
        return;
    }

    game_state->job.remainingTime = value->value_string.value;
}

SCSAPI_VOID telemetry_store_job_source_city(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_string){
        return;
    }

    game_state->job.sourceCity = value->value_string.value;
}

SCSAPI_VOID telemetry_store_job_source_company(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_string){
        return;
    }

    game_state->job.sourceCompany = value->value_string.value;
}

SCSAPI_VOID telemetry_store_job_destination_city(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_string){
        return;
    }

    game_state->job.destinationCity = value->value_string.value;
}

SCSAPI_VOID telemetry_store_job_destination_company(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_string){
        return;
    }

    game_state->job.destinationCompany = value->value_string.value;
}

SCSAPI_VOID telemetry_store_navigation_estimated_time(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_string){
        return;
    }

    game_state->navigation.estimatedTime = value->value_string.value;
}

SCSAPI_VOID telemetry_store_navigation_estimated_distance(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->navigation.estimatedDistance = value->value_float.value;
}

SCSAPI_VOID telemetry_store_trailer_attached(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->trailer.attached = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_trailer_id(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_string){
        return;
    }

    game_state->trailer.id = value->value_string.value;
}

SCSAPI_VOID telemetry_store_trailer_name(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_string){
        return;
    }

    game_state->trailer.name = value->value_string.value;
}

SCSAPI_VOID telemetry_store_trailer_mass(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->trailer.mass = value->value_float.value;
}

SCSAPI_VOID telemetry_store_trailer_wear(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->trailer.wear = value->value_float.value;
}

SCSAPI_VOID telemetry_store_trailer_placement(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_dplacement){
        return;
    }

    game_state->trailer.placement.x = value->value_dplacement.position.x;
    game_state->trailer.placement.y = value->value_dplacement.position.y;
    game_state->trailer.placement.z = value->value_dplacement.position.z;
    game_state->trailer.placement.heading = value->value_dplacement.orientation.heading;
    game_state->trailer.placement.pitch = value->value_dplacement.orientation.pitch;
    game_state->trailer.placement.roll = value->value_dplacement.orientation.roll;
}

SCSAPI_VOID telemetry_store_air_pressure_warning(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.airPressureWarningOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_air_pressure_warning_value(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.airPressureWarningValue = value->value_float.value;
}

SCSAPI_VOID telemetry_store_air_pressure_emergency_warning(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.airPressureEmergencyOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_air_pressure_emergency_warning_value(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.airPressureEmergencyValue = value->value_float.value;
}

SCSAPI_VOID telemetry_store_fuel_warning(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.fuelWarningOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_fuel_warning_value(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.fuelWarningOn = value->value_float.value;
}

SCSAPI_VOID telemetry_store_adblue_warning_on(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.adblueWarningOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_adblue_warning_value(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.adblueWarningOn = value->value_float.value;
}

SCSAPI_VOID telemetry_store_battery_voltage_warning_on(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.batteryVoltageWarningOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_battery_voltage_warning_value(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.batteryVoltageWarningValue = value->value_float.value;
}


SCSAPI_VOID telemetry_store_local_linear_acceleration(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_fvector){
        return;
    }

    game_state->truck.acceleration.x = value->value_fvector.x;
    game_state->truck.acceleration.y = value->value_fvector.y;
    game_state->truck.acceleration.z = value->value_fvector.z;
}

SCSAPI_VOID telemetry_store_local_angular_acceleration(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_fvector){
        return;
    }

    game_state->truck.acceleration.x = value->value_fvector.x;
    game_state->truck.acceleration.y = value->value_fvector.y;
    game_state->truck.acceleration.z = value->value_fvector.z;
}

SCSAPI_VOID telemetry_store_engine_gear(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_s32){
        return;
    }

    game_state->truck.gear = value->value_s32.value;
}

SCSAPI_VOID telemetry_store_displayed_gear(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_s32){
        return;
    }

    game_state->truck.displayedGear = value->value_s32.value;
}

SCSAPI_VOID telemetry_store_input_steering(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.userSteer = value->value_float.value;
}

SCSAPI_VOID telemetry_store_input_throttle(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.userThrottle = value->value_float.value;
}

SCSAPI_VOID telemetry_store_input_brake(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.userBrake = value->value_float.value;
}

SCSAPI_VOID telemetry_store_input_clutch(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.userClutch = value->value_float.value;
}

SCSAPI_VOID telemetry_store_effective_steering(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.gameSteer = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_effective_throttle(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.gameThrottle = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_effective_brake(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.gameBrake = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_effective_clutch(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.gameClutch = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_cruise_control_speed(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.cruiseControlSpeed = value->value_float.value;
}

SCSAPI_VOID telemetry_store_cruise_control(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.cruiseControlSpeed = value->value_float.value;
}


SCSAPI_VOID telemetry_store_wipers_on(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.wipersOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_hshifter_slot(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_u32){
        return;
    }

    game_state->truck.shifterSlot = value->value_u32.value;
}
SCSAPI_VOID telemetry_store_parking_brake(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.parkBrakeOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_retarder_level(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_u32){
        return;
    }

    game_state->truck.retarderStepCount = value->value_u32.value;
}

SCSAPI_VOID telemetry_store_brake_air_pressure_warning(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.airPressureWarningOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_brake_air_pressure_emergency(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.airPressureEmergencyOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_brake_temperature(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.brakeTemperature = value->value_float.value;
}

SCSAPI_VOID telemetry_store_fuel_average_consumption(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.fuelAverageConsumption = value->value_float.value;
}

SCSAPI_VOID telemetry_store_adblue_warning(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.adblueWarningOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_adblue_average_consumption(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }

    game_state->truck.adblueAverageConsumpton = value->value_float.value;
}

SCSAPI_VOID telemetry_store_oil_pressure_warning(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.oilPressureWarningOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_water_temperature_warning(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.waterTemperatureWarningOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_battery_voltage_warning(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }

    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }

    game_state->truck.batteryVoltageWarningOn = value->value_bool.value;
}

//blinkers

SCSAPI_VOID telemetry_store_left_blinker(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.blinkerLeftOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_right_blinker(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.blinkerRightOn = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_light_lblinker(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.blinkerLeftActive = value->value_bool.value;
}

SCSAPI_VOID telemetry_store_light_rblinker(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.blinkerRightActive = value->value_bool.value;
}

//telemetry_store_light_parking
SCSAPI_VOID telemetry_store_light_parking(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.lightsParkingOn = value->value_bool.value;
}

//telemetry_store_light_low_beam
SCSAPI_VOID telemetry_store_light_low_beam(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.lightsBeamLowOn = value->value_bool.value;
}

//telemetry_store_light_high_beam

SCSAPI_VOID telemetry_store_light_high_beam(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.lightsBeamHighOn = value->value_bool.value;
}

//telemetry_store_light_aux_front

SCSAPI_VOID telemetry_store_light_aux_front(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.lightsAuxFrontOn = value->value_bool.value;
}

//telemetry_store_light_aux_roof
SCSAPI_VOID telemetry_store_light_aux_roof(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.lightsAuxRoofOn = value->value_bool.value;
}

//telemetry_store_light_beacon
SCSAPI_VOID telemetry_store_light_beacon(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.lightsBeaconOn = value->value_bool.value;
}

//telemetry_store_light_brake
SCSAPI_VOID telemetry_store_light_brake(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.lightsBrakeOn = value->value_bool.value;
}

//telemetry_store_light_reverse
SCSAPI_VOID telemetry_store_light_reverse(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_bool){
        return;
    }
    game_state->truck.lightsReverseOn = value->value_bool.value;
}

//telemetry_store_wear_engine
SCSAPI_VOID telemetry_store_wear_engine(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
    game_state->truck.wearEngine = value->value_float.value;
}

//telemetry_store_wear_transmission
SCSAPI_VOID telemetry_store_wear_transmission(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
    game_state->truck.wearTransmission = value->value_float.value;
}

//telemetry_store_wear_cabin
SCSAPI_VOID telemetry_store_wear_cabin(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
    game_state->truck.wearCabin = value->value_float.value;
}

//telemetry_store_wear_chassis
SCSAPI_VOID telemetry_store_wear_chassis(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
    game_state->truck.wearChassis = value->value_float.value;
}

//telemetry_store_wear_wheels
SCSAPI_VOID telemetry_store_wear_wheels(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
    game_state->truck.wearWheels = value->value_float.value;
}

//telemetry_store_odometer
SCSAPI_VOID telemetry_store_odometer(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_u64){
        return;
    }
    game_state->truck.odometer = value->value_u64.value;
}

//telemetry_store_navigation_speed_limit
SCSAPI_VOID telemetry_store_navigation_speed_limit(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
    game_state->navigation.speedLimit = value->value_float.value*(speedMultiplicatorToKmh);
}

//telemetry_store_navigation_distance
SCSAPI_VOID telemetry_store_navigation_distance(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_float){
        return;
    }
    game_state->navigation.estimatedDistance = value->value_float.value;
}

//telemetry_store_navigation_time
SCSAPI_VOID telemetry_store_navigation_time(scs_string_t name, scs_u32_t index, const scs_value_t *value, scs_context_t context){
    if(output_paused){
        return;
    }
    if(value->type != SCS_VALUE_TYPE_string){
        return;
    }
    game_state->navigation.estimatedTime = value->value_string.value;
}






/**
 * @brief Telemetry API initialization function.
 *
 * See scssdk_telemetry.h
 */
SCSAPI_RESULT scs_telemetry_init(const scs_u32_t version, const scs_telemetry_init_params_t *const params)
{
	// We currently support only one version.

	if (version != SCS_TELEMETRY_VERSION_1_01) {
		return SCS_RESULT_unsupported;
	}

	const scs_telemetry_init_params_v101_t *const version_params = static_cast<const scs_telemetry_init_params_v101_t *>(params);

	if (strcmp(version_params->common.game_id, SCS_GAME_ID_EUT2) == 0) {

		// Below the minimum version there might be some missing features (only minor change) or
		// incompatible values (major change).

		const scs_u32_t MINIMAL_VERSION = SCS_TELEMETRY_EUT2_GAME_VERSION_1_00;
		if (version_params->common.game_version < MINIMAL_VERSION) {
			game_log(SCS_LOG_TYPE_warning,"WARNING: Too old version of the game, some features might behave incorrectly");
		}

		// Future versions are fine as long the major version is not changed.

		const scs_u32_t IMPLEMENTED_VERSION = SCS_TELEMETRY_EUT2_GAME_VERSION_CURRENT;
		if (SCS_GET_MAJOR_VERSION(version_params->common.game_version) > SCS_GET_MAJOR_VERSION(IMPLEMENTED_VERSION)) {
			game_log(SCS_LOG_TYPE_warning,"WARNING: Too new major version of the game, some features might behave incorrectly");
		}
	}
	else if (strcmp(version_params->common.game_id, SCS_GAME_ID_ATS) == 0) {

		// Below the minimum version there might be some missing features (only minor change) or
		// incompatible values (major change).

		const scs_u32_t MINIMAL_VERSION = SCS_TELEMETRY_ATS_GAME_VERSION_1_00;
		if (version_params->common.game_version < MINIMAL_VERSION) {
			game_log(SCS_LOG_TYPE_warning,"WARNING: Too old version of the game, some features might behave incorrectly");
		}

		// Future versions are fine as long the major version is not changed.

		const scs_u32_t IMPLEMENTED_VERSION = SCS_TELEMETRY_ATS_GAME_VERSION_CURRENT;
		if (SCS_GET_MAJOR_VERSION(version_params->common.game_version) > SCS_GET_MAJOR_VERSION(IMPLEMENTED_VERSION)) {
			game_log(SCS_LOG_TYPE_warning,"WARNING: Too new major version of the game, some features might behave incorrectly");
		}
	}
	else {
		game_log(SCS_LOG_TYPE_warning,"WARNING: Unsupported game, some features or values might behave incorrectly");
	}

	// Register for events. Note that failure to register those basic events
	// likely indicates invalid usage of the api or some critical problem. As the
	// example requires all of them, we can not continue if the registration fails.

	const bool events_registered =
		(version_params->register_for_event(SCS_TELEMETRY_EVENT_frame_start, telemetry_frame_start, NULL) == SCS_RESULT_ok) &&
		(version_params->register_for_event(SCS_TELEMETRY_EVENT_frame_end, telemetry_frame_end, NULL) == SCS_RESULT_ok) &&
		(version_params->register_for_event(SCS_TELEMETRY_EVENT_paused, telemetry_pause, NULL) == SCS_RESULT_ok) &&
		(version_params->register_for_event(SCS_TELEMETRY_EVENT_started, telemetry_pause, NULL) == SCS_RESULT_ok)
	;
	if (! events_registered) {

		// Registrations created by unsuccessfull initialization are
		// cleared automatically so we can simply exit.

		version_params->common.log(SCS_LOG_TYPE_error, "Unable to register event callbacks");
		return SCS_RESULT_generic_error;
	}

	// Register for the configuration info. As this example only prints the retrieved
	// data, it can operate even if that fails.

	//version_params->register_for_event(SCS_TELEMETRY_EVENT_configuration, telemetry_configuration, NULL);

	// Register for gameplay events.

	//version_params->register_for_event(SCS_TELEMETRY_EVENT_gameplay, telemetry_gameplay_event, NULL);

	// Register for channels. The channel might be missing if the game does not support
	// it (SCS_RESULT_not_found) or if does not support the requested type
	// (SCS_RESULT_unsupported_type). For purpose of this example we ignore the failues
	// so the unsupported channels will remain at theirs default value.

version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_world_placement, SCS_U32_NIL, SCS_VALUE_TYPE_dplacement, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_world_placement, &game_state_s.truck.placement);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_local_linear_velocity, SCS_U32_NIL, SCS_VALUE_TYPE_fvector, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_local_linear_velocity, &game_state_s.truck.acceleration);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_local_angular_velocity, SCS_U32_NIL, SCS_VALUE_TYPE_fvector, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_local_angular_velocity, &game_state_s.truck.acceleration);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_local_linear_acceleration, SCS_U32_NIL, SCS_VALUE_TYPE_fvector, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_local_linear_acceleration, &game_state_s.truck.acceleration);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_local_angular_acceleration, SCS_U32_NIL, SCS_VALUE_TYPE_fvector, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_local_angular_acceleration, &game_state_s.truck.acceleration);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_speed, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_speed, &game_state_s.truck.speed);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_engine_rpm, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_rpm, &game_state_s.truck.engineRpm);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_engine_gear, SCS_U32_NIL, SCS_VALUE_TYPE_s32, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_engine_gear, &game_state_s.truck.gear);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_displayed_gear, SCS_U32_NIL, SCS_VALUE_TYPE_s32, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_displayed_gear, &game_state_s.truck.displayedGear);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_input_steering, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_input_steering, &game_state_s.truck.userSteer);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_input_throttle, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_input_throttle, &game_state_s.truck.userThrottle);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_input_brake, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_input_brake, &game_state_s.truck.userBrake);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_input_clutch, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_input_clutch, &game_state_s.truck.userClutch);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_effective_steering, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_effective_steering, &game_state_s.truck.gameSteer);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_effective_throttle, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_effective_throttle, &game_state_s.truck.gameThrottle);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_effective_brake, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_effective_brake, &game_state_s.truck.gameBrake);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_effective_clutch, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_effective_clutch, &game_state_s.truck.gameClutch);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_cruise_control, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_cruise_control, &game_state_s.truck.cruiseControlSpeed);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_hshifter_slot, SCS_U32_NIL, SCS_VALUE_TYPE_u32, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_hshifter_slot, &game_state_s.truck.shifterSlot);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_parking_brake, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_parking_brake, &game_state_s.truck.parkBrakeOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_motor_brake, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_motor_brake, &game_state_s.truck.motorBrakeOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_retarder_level, SCS_U32_NIL, SCS_VALUE_TYPE_u32, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_retarder_level, &game_state_s.truck.retarderStepCount);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_brake_air_pressure, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_brake_air_pressure, &game_state_s.truck.airPressure);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_brake_air_pressure_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_brake_air_pressure_warning, &game_state_s.truck.airPressureWarningOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_brake_air_pressure_emergency, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_brake_air_pressure_emergency, &game_state_s.truck.airPressureEmergencyOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_brake_temperature, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_brake_temperature, &game_state_s.truck.brakeTemperature);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_fuel, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_fuel, &game_state_s.truck.fuel);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_fuel_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_fuel_warning, &game_state_s.truck.fuelWarningOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_fuel_average_consumption, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_fuel_average_consumption, &game_state_s.truck.fuelAverageConsumption);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_adblue, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_adblue, &game_state_s.truck.adblue);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_adblue_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_adblue_warning, &game_state_s.truck.adblueWarningOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_adblue_average_consumption, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_adblue_average_consumption, &game_state_s.truck.adblueAverageConsumpton);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_oil_pressure, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_oil_pressure, &game_state_s.truck.oilPressure);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_oil_pressure_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_oil_pressure_warning, &game_state_s.truck.oilPressureWarningOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_oil_temperature, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_oil_temperature, &game_state_s.truck.oilTemperature);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_water_temperature, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_water_temperature, &game_state_s.truck.waterTemperature);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_water_temperature_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_water_temperature_warning, &game_state_s.truck.waterTemperatureWarningOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_battery_voltage, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_battery_voltage, &game_state_s.truck.batteryVoltage);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_battery_voltage_warning, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_battery_voltage_warning, &game_state_s.truck.batteryVoltageWarningOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_electric_enabled, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_electric_enabled, &game_state_s.truck.electricOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_engine_enabled, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_engine_enabled, &game_state_s.truck.engineOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_lblinker, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_left_blinker, &game_state_s.truck.blinkerLeftOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_rblinker, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_right_blinker, &game_state_s.truck.blinkerRightOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_light_lblinker, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_light_lblinker, &game_state_s.truck.blinkerLeftActive);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_light_rblinker, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_light_rblinker, &game_state_s.truck.blinkerRightActive);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_light_parking, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_light_parking, &game_state_s.truck.lightsParkingOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_light_low_beam, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_light_low_beam, &game_state_s.truck.lightsBeamLowOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_light_high_beam, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_light_high_beam, &game_state_s.truck.lightsBeamHighOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_light_aux_front, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_light_aux_front, &game_state_s.truck.lightsAuxFrontOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_light_aux_roof, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_light_aux_roof, &game_state_s.truck.lightsAuxRoofOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_light_beacon, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_light_beacon, &game_state_s.truck.lightsBeaconOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_light_brake, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_light_brake, &game_state_s.truck.lightsBrakeOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_light_reverse, SCS_U32_NIL, SCS_VALUE_TYPE_bool, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_light_reverse, &game_state_s.truck.lightsReverseOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_wipers, SCS_U32_NIL, SCS_VALUE_TYPE_u32, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_wipers, &game_state_s.truck.wipersOn);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_wear_engine, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_wear_engine, &game_state_s.truck.wearEngine);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_wear_transmission, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_wear_transmission, &game_state_s.truck.wearTransmission);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_wear_cabin, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_wear_cabin, &game_state_s.truck.wearCabin);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_wear_chassis, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_wear_chassis, &game_state_s.truck.wearChassis);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_wear_wheels, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_wear_wheels, &game_state_s.truck.wearWheels);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_odometer, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_odometer, &game_state_s.truck.odometer);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_navigation_distance, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_navigation_distance, &game_state_s.navigation.estimatedDistance);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_navigation_time, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_navigation_time, &game_state_s.navigation.estimatedTime);
version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_navigation_speed_limit, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_navigation_speed_limit, &game_state_s.navigation.speedLimit);



 

	//version_params->register_for_channel(SCS_TELEMETRY_TRUCK_CHANNEL_engine_gear, SCS_U32_NIL, SCS_VALUE_TYPE_s32, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_s32, &telemetry.gear);

	// Remember the function we will use for logging.

	game_log = version_params->common.log;
	game_log(SCS_LOG_TYPE_message, "Initializing telemetry log example");

	// Set the structure with defaults.

	// Initially the game is paused.

	output_paused = true;

    /**
     * SHARED MEMORY STUFF
    */
	game_state->truck.speed = 0;
	game_state->truck.engineRpm = 0; 
	game_state->truck.gear = 0;
    game_state->game.connected = true;
    game_state->game.paused = true;
    std::thread restServerThread(runRESTServer);
    restServerThread.detach(); // Detach the thread to let it run independently
	return SCS_RESULT_ok;
}



/**
 * @brief Telemetry API deinitialization function.
 *
 * See scssdk_telemetry.h
 */
SCSAPI_VOID scs_telemetry_shutdown(void)
{
	// Any cleanup needed. The registrations will be removed automatically
	// so there is no need to do that manually.

	game_log = NULL; 
}

