#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "HAPSRP.hpp"

#define USERNAME "alice"
#define PASSWORD "password123"

int main(){

	HAPSRP* _hapsrp = new HAPSRP();
	HAPSRP::SRPSession* session = _hapsrp->newSession(SRP_SHA512, HAPSRP::SRP_NG_3072, NULL,NULL);


	delete _hapsrp;

}