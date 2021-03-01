// 
// HAPPluginKNXMemory.hpp
// Homekit
//
//  Created on: 16.01.2021
//      Author: michael
//

#ifndef HAPPLUGINKNXMEMORY_HPP_
#define HAPPLUGINKNXMEMORY_HPP_

#define HAP_PLUGIN_KNX_MAX_DEVICES              15

// ==============================================================================================
//  Parameter Offsets
// ==============================================================================================
#define ETS_HK_CHANNEL_PARAMETER_SIZE           44

// Service Name
#define ETS_HK_OFFSET_SERVICE_TYPE              1    // 1    // 8 Bits, Bit 7-0
#define ETS_HK_OFFSET_SERVICE_NAME              2    // 2    // 40 Bytes

// Fakegato / Schedule
#define ETS_HK_OFFSET_ENABLE_FAKEGATO           42   // 19   // 1 Bit, Bit 7
#define ETS_HK_OFFSET_ENABLE_SCHEDULE           42   // 19   // 1 Bit, Bit 6

// Weather
#define ETS_HK_OFFSET_HAS_TEMPERATURE_SENSOR    43   // 43   // 1 Bit, Bit 7
#define ETS_HK_OFFSET_HAS_HUMIDITY_SENSOR       43   // 43   // 1 Bit, Bit 6
#define ETS_HK_OFFSET_HAS_AIRPRESSURE_SENSOR    43   // 43   // 1 Bit, Bit 5
// Humidity DPT
#define ETS_HK_OFFSET_HUMIDITY_DPT_TYPE         44   // 8 Bits, Bit 7-0


// ==============================================================================================
//  Communication Objects Offset
// ==============================================================================================
#define ETS_HK_CHANNEL_COMOBJECT_SIZE           7

// Weather
#define ETS_HK_KO_OFFSET_REMOTE_TEMPERATURE     1    //     ETS_HK_KO_CHANNEL_XXX_OFFSET
#define ETS_HK_KO_OFFSET_REMOTE_HUMIDITY        2    //     (ETS_HK_KO_CHANNEL_XXX_OFFSET + 1)
#define ETS_HK_KO_OFFSET_REMOTE_AIRPRESSURE     3    //     (ETS_HK_KO_CHANNEL_XXX_OFFSET + 2)

// Switch
#define ETS_HK_KO_OFFSET_SWITCH_READ            4    //     (ETS_HK_KO_CHANNEL_XXX_OFFSET + 3)
#define ETS_HK_KO_OFFSET_SWITCH_WRITE           5    //     (ETS_HK_KO_CHANNEL_XXX_OFFSET + 4)

// Outlet
#define ETS_HK_KO_OFFSET_OUTLET_CURRENT         6    //     (ETS_HK_KO_CHANNEL_XXX_OFFSET + 5)
#define ETS_HK_KO_OFFSET_OUTLET_ACT_ENERGY      7    //     (ETS_HK_KO_CHANNEL_XXX_OFFSET + 6)


#endif /* HAPPLUGINKNXMEMORY_HPP_ */