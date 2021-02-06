//
// HAPPrint.hpp
// Homekit
//
//  Created on: 08.05.2018
//      Author: michael
//

#ifndef HAPPRINT_HPP_
#define HAPPRINT_HPP_


#include "HAPPrintBuffered.hpp"
#include "HAPPrintChunked.hpp"
#include "HAPPrintEncrypted.hpp"

#define HAP_PRINT_PLAIN_BUFFER_SIZE				1024

#define HAP_PRINT_CHUNKED_BUFFER_SIZE			1031    // 1024 + 7 Encoding Reserve

// Do not change !!!
//      + 	2 AAD
//      + 	7 ENCODING RESERVE
//      + 1024 PAYLOAD 
//      +	7 ENCODING RESERVE
//      +   16 TAG
//   =====  1056
#define HAP_PRINT_ENCRYPTED_BUFFER_SIZE		  	1056	// Do not change !!!

#endif /* HAPPRINT_HPP_ */